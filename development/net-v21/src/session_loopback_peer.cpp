// A no-game peer running the production session, EndState and Steam stand-in. Engine/state/shot
// functions are stubs; this is not a simulated battle, a Steam client, or a physics test.
#include "common.h"
#include "steam.h"
#include "netlog.h"
#include "render_guard.h"
#include "protocol.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>

void session_test_disable_udp(bool);
bool session_test_peer_proposed();
bool session_test_peer_over();
bool session_test_end_pending();
bool session_test_end_left();
bool session_test_udp_open();
unsigned session_test_hello_rejected();
void session_test_state_dispatch(bool tactical);
void session_test_shot_dispatch(bool tactical);
bool session_test_transform_dispatch();
bool session_test_new_force_offer();
bool session_test_force_fragments();
bool session_test_force_recovery();
bool session_test_legacy_end();
void session_test_motion_begin();
void session_test_motion_push(const UnitRec &);
u32 session_test_motion_updates();
u32 session_test_motion_skipped();
size_t session_test_motion_cached();
void session_test_motion_end();

char g_program[32] = "tacgame";
char g_out_dir[MAX_PATH] = "C:\\mius-no-game\\out";
void mp_log(const char *fmt, ...) { va_list a; va_start(a, fmt); vprintf(fmt, a); va_end(a); putchar('\n'); fflush(stdout); }
HWND main_window() { return nullptr; }
bool readable(const void *p, size_t n) {
    MEMORY_BASIC_INFORMATION m = {};
    if (!p || !n || !VirtualQuery(p, &m, sizeof m) || m.State != MEM_COMMIT || (m.Protect & (PAGE_NOACCESS | PAGE_GUARD))) return false;
    uintptr_t at = (uintptr_t)p, base = (uintptr_t)m.BaseAddress;
    return at >= base && at - base <= m.RegionSize && n <= m.RegionSize - (at - base);
}
void netlog_packet(const char *, const unsigned char *, int, unsigned int, unsigned short, unsigned short) {}
void netlog_frame() {}
void netlog_count(const char *, long long) {}
void netlog_mark(const char *, ...) {}
void netlog_state(int, unsigned, int, unsigned) {}
void render_guard_install() {}
void render_guard_report(DWORD) {}
static int tactical_resets, legacy_resets, tactical_forgets, tactical_packets, legacy_packets;
void statesync_reset() { ++tactical_resets; }
void statesync_legacy_reset() { ++legacy_resets; }
void statesync_forget_peer() { ++tactical_forgets; }
void statesync_apply(ObjMan *) {}
void statesync_owner_frame(ObjMan *, DWORD, bool (*)(Object *), void (*)(const void *, int)) {}
void statesync_on_packet(const unsigned char *, int, void (*)(const void *, int), bool) { ++tactical_packets; }
void statesync_legacy_on_packet(const unsigned char *, int, void (*)(const void *, int), bool) { ++legacy_packets; }
void statesync_report() {}
bool xlate_ready() { return false; }
void xlate_frame(ObjMan *) {}
static unsigned shot_steps, trench_steps, tactical_shot_packets, legacy_shot_packets;
void shots_install(ObjMan *) {}
void shots_sim_step(ObjMan *) { ++shot_steps; }
void shots_frame(void (*)(const void *, int)) {}
void shots_on_packet(const unsigned char *, int, void (*)(const void *, int)) { ++tactical_shot_packets; }
void shots_on_packet_legacy(const unsigned char *, int, void (*)(const void *, int)) { ++legacy_shot_packets; }
void shots_reset() {}
void shots_report() {}
void trench_install() {}
void trench_sim_step(ObjMan *) { ++trench_steps; }
void trench_report() {}
static Object *find_nothing(ObjMan *, Dep *) { return nullptr; }

static bool motion_test()
{
    alignas(8) unsigned char memory[0x300] = {}, manager_bytes[0x110] = {};
    Object *unit = (Object *)memory;
    unit->iface = "i_techn"; strcpy_s(unit->full, "i_techn:unit_7b");
    Object *units[] = { unit };
    *(Object ***)(manager_bytes + 0xf0) = units; *(u32 *)(manager_bytes + 0xfc) = 1;
    ObjMan *manager = (ObjMan *)manager_bytes;
    UnitRec r = {}; r.id = 0x7b; r.kind = 1; r.pos[0] = 10; r.lin[0] = 1;
    r.rot[0] = r.rot[4] = r.rot[8] = 1;
    session_test_motion_begin();
    session_test_motion_push(r);
    session_sim_step(manager, 0);
    bool ok = *(float *)(memory + UNIT_POS) == 10 && session_test_motion_updates() == 1 &&
              shot_steps == 1 && trench_steps == 1;
    for (int i = 0; i < 20; i++) session_sim_step(manager, 0);
    ok = ok && *(float *)(memory + UNIT_POS) == 10 && session_test_motion_updates() == 1 &&
         session_test_motion_skipped() == 20 && shot_steps == 21 && trench_steps == 21;
    r.pos[0] = 20; session_test_motion_push(r); session_sim_step(manager, 0);
    ok = ok && *(float *)(memory + UNIT_POS) == 20 && session_test_motion_updates() == 2;
    session_sim_step(manager, 16);
    ok = ok && session_test_motion_updates() == 3 && shot_steps == 23 && trench_steps == 23;
    *(u32 *)(manager_bytes + 0xfc) = 0;
    session_sim_step(manager, 0);
    ok = ok && session_test_motion_cached() == 0 && shot_steps == 24 && trench_steps == 24;
    session_test_motion_end();
    shot_steps = trench_steps = 0;
    return ok;
}

int main(int argc, char **argv)
{
    if (argc != 4 || (strcmp(argv[1], "host") && strcmp(argv[1], "join"))) return 2;
    // Exercise the actual session dispatch in both modes before starting the tactical pair.
    session_test_state_dispatch(false);
    if (legacy_resets != 1 || legacy_packets != 1 || tactical_resets || tactical_forgets || tactical_packets) {
        puts("FAIL: non-tactical state dispatch"); return 1;
    }
    session_test_state_dispatch(true);
    if (legacy_resets != 1 || legacy_packets != 1 || tactical_resets != 1 || tactical_forgets != 1 || tactical_packets != 1) {
        puts("FAIL: tactical state dispatch"); return 1;
    }
    session_test_shot_dispatch(false);
    session_test_shot_dispatch(true);
    if (legacy_shot_packets != 1 || tactical_shot_packets != 1) { puts("FAIL: shot dispatch by mode"); return 1; }
    if (!session_test_legacy_end()) { puts("FAIL: non-tactical end packet behavior"); return 1; }
    if (!session_test_transform_dispatch()) { puts("FAIL: transform dispatch by mode"); return 1; }
    if (!session_test_new_force_offer()) { puts("FAIL: changed force offer"); return 1; }
    if (!session_test_force_fragments()) { puts("FAIL: force fragment lengths"); return 1; }
    if (!session_test_force_recovery()) { puts("FAIL: force backup recovery"); return 1; }
    if (!motion_test()) { puts("FAIL: zero-ms simulation and remote transform processing"); return 1; }
    puts("state, force, zero-ms motion and cache tests passed");
    bool host = !strcmp(argv[1], "host");
    SetEnvironmentVariableA("MP_ROLE", argv[1]);
    SetEnvironmentVariableA("MP_HOST_MODE", "skirmish");
    SetEnvironmentVariableA("MP_STEAM", nullptr);
    SetEnvironmentVariableA("MP_STEAM_FAKE", argv[2]);
    char peer[80]; sprintf_s(peer, "steam:%s", argv[3]);
    SetEnvironmentVariableA("MP_PEER", peer);
    SetEnvironmentVariableA("MP_TEST_MAXTIME", nullptr);
    session_test_disable_udp(true); // Steam stand-in still has its separate loopback socket
    alignas(8) unsigned char manager[0x1800] = {};
    void *vt[16] = {};
    vt[9] = (void *)find_nothing;
    ((ObjMan *)manager)->vtbl = vt;
    session_start((ObjMan *)manager);
    session_note_phase(2);
    if (session_test_udp_open() || !steam_ready()) return 3;

    DWORD start = GetTickCount(), mutual_at = 0, withdrew_at = 0, ended_at = 0;
    bool offered = false, saw_offer = false, saw_withdrawal = false, saw_peer_over = false, intruder_sent = false;
    while (GetTickCount() - start < 14000) {
        DWORD now = GetTickCount();
        session_frame((ObjMan *)manager);
        if (session_connected() && !offered) { session_toggle_ceasefire(); offered = true; }
        if (host && offered && !intruder_sent) {
            // A third Steam-stand-in identity sends a syntactically valid HELLO while the real
            // peer is active; the production session must keep its peer and its pending state.
            SOCKET intruder = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            sockaddr_in target = {}; target.sin_family = AF_INET;
            target.sin_addr.s_addr = htonl(INADDR_LOOPBACK); target.sin_port = htons((unsigned short)atoi(argv[2]));
            Hello hello = {}; hello.h = make_head(PKT_HELLO);
            memcpy(hello.proto, k_protocol, sizeof k_protocol); hello.version = k_version;
            strcpy_s(hello.name, "intruder");
            int sent = sendto(intruder, (const char *)&hello, sizeof hello, 0, (sockaddr *)&target, sizeof target);
            closesocket(intruder);
            if (sent != sizeof hello) return 4;
            intruder_sent = true;
        }
        if (session_test_peer_proposed()) { saw_offer = true; if (!mutual_at) mutual_at = now; }
        if (!host && saw_offer && !session_test_peer_proposed()) saw_withdrawal = true;
        if (host && mutual_at && !withdrew_at && now - mutual_at >= 750) {
            session_toggle_ceasefire(); withdrew_at = now;
        }
        if (host && withdrew_at && !ended_at && now - withdrew_at >= 1000) {
            session_note_phase(3); ended_at = now;
        }
        if (!host && session_test_peer_over()) saw_peer_over = true;
        if (!host && saw_peer_over && session_test_end_pending()) {
            bool ok = offered && saw_offer && saw_withdrawal && !session_test_end_left() && !session_test_udp_open();
            printf("SESSION_RESULT join ok=%d connected=%d offer=%d withdrawal=%d peer_over=%d pending=%d left=%d udp=%d\n",
                   (int)ok, (int)session_connected(), (int)saw_offer, (int)saw_withdrawal, (int)saw_peer_over,
                   (int)session_test_end_pending(), (int)session_test_end_left(), (int)session_test_udp_open());
            session_stop(); steam_shutdown();
            return ok ? 0 : 1;
        }
        if (host && ended_at && now - ended_at >= 6000) {
            bool ok = offered && saw_offer && session_test_end_left() && !session_test_udp_open() &&
                      intruder_sent && session_test_hello_rejected() > 0;
            printf("SESSION_RESULT host ok=%d offer=%d withdrew=%d left=%d udp=%d rejected=%u\n",
                   (int)ok, (int)saw_offer, (int)(withdrew_at != 0), (int)session_test_end_left(),
                   (int)session_test_udp_open(), session_test_hello_rejected());
            session_stop(); steam_shutdown();
            return ok ? 0 : 1;
        }
        Sleep(10);
    }
    printf("SESSION_RESULT %s TIMEOUT connected=%d offer=%d peer_over=%d\n", argv[1],
           (int)session_connected(), (int)saw_offer, (int)saw_peer_over);
    session_stop(); steam_shutdown();
    return 1;
}

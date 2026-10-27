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
void statesync_reset() {}
void statesync_forget_peer() {}
void statesync_apply(ObjMan *) {}
void statesync_owner_frame(ObjMan *, DWORD, bool (*)(Object *), void (*)(const void *, int)) {}
void statesync_on_packet(const unsigned char *, int, void (*)(const void *, int), bool) {}
void statesync_report() {}
bool xlate_ready() { return false; }
void xlate_frame(ObjMan *) {}
void shots_install(ObjMan *) {}
void shots_sim_step(ObjMan *) {}
void shots_frame(void (*)(const void *, int)) {}
void shots_on_packet(const unsigned char *, int, void (*)(const void *, int)) {}
void shots_reset() {}
void shots_report() {}
void trench_install() {}
void trench_sim_step(ObjMan *) {}
void trench_report() {}
static Object *find_nothing(ObjMan *, Dep *) { return nullptr; }

int main(int argc, char **argv)
{
    if (argc != 4 || (strcmp(argv[1], "host") && strcmp(argv[1], "join"))) return 2;
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

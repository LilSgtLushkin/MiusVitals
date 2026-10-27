// A no-game peer running the production session, EndState and Steam stand-in. Engine/state/shot
// functions are stubs; this is not a simulated battle, a Steam client, or a physics test.
#include "common.h"
#include "steam.h"
#include "netlog.h"
#include "render_guard.h"
#include "protocol.h"
#include "joiner.h"
#include "opsave_fixture.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>

void session_test_disable_udp(bool);
void session_test_disable_upnp(bool);
void session_test_lobby_image();
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
void session_test_map_enable(bool);
unsigned session_test_map_dropped();
unsigned session_test_map_duplicates();
unsigned session_test_map_stale();
bool session_test_map_seed(const std::vector<unsigned char> &, bool);
int session_test_map_host_phase();
bool session_test_map_peer_loaded();
bool session_test_map_input();
bool session_test_map_finish();
bool session_test_map_download_ready();
bool session_test_map_adopt_download(std::vector<unsigned char> *);
bool session_test_map_collecting();
bool session_test_map_offer(const std::vector<unsigned char> &);
bool session_test_map_received();
bool session_test_map_commit(bool, std::vector<unsigned char> *);
bool session_test_map_acked();
void session_test_map_disconnect();
void session_test_map_old_hello();

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

// Test the production frame order: session_frame() runs before Joiner::poll() in the real menu.
// Exercise an operation-menu host and, separately, a tactical host serving a synthetic image.
static int lobby_test(bool host, bool steam, bool menu_only, const char *own_port, const char *peer_port)
{
    // The host signals only after its production session has sent WELCOME. This forces the reply
    // to be waiting before the joiner's next session_frame, making the old receive-order bug deterministic.
    char event_name[96]; sprintf_s(event_name, "Local\\netv21_%s_welcome_%s", steam ? "steam" : "ip", host ? own_port : peer_port);
    HANDLE welcome_sent = CreateEventA(nullptr, TRUE, FALSE, event_name);
    sprintf_s(event_name, "Local\\netv21_%s_download_%s", steam ? "steam" : "ip", host ? own_port : peer_port);
    HANDLE download_done = CreateEventA(nullptr, TRUE, FALSE, event_name);
    sprintf_s(event_name, "Local\\netv21_%s_ready_%s", steam ? "steam" : "ip", host ? own_port : peer_port);
    HANDLE host_ready = CreateEventA(nullptr, TRUE, FALSE, event_name);
    sprintf_s(event_name, "Local\\netv21_%s_ready_seen_%s", steam ? "steam" : "ip", host ? own_port : peer_port);
    HANDLE ready_seen = CreateEventA(nullptr, TRUE, FALSE, event_name);
    if (!welcome_sent || !download_done || !host_ready || !ready_seen) return 2;
    strcpy_s(g_program, host && !menu_only ? "tacgame" : "opgame");
    SetEnvironmentVariableA("MP_ROLE", host ? "host" : nullptr);
    SetEnvironmentVariableA("MP_HOST_MODE", menu_only ? "operation" : "skirmish");
    SetEnvironmentVariableA("MP_MAP", nullptr); // menu only; never simulate the M5 map
    SetEnvironmentVariableA("MP_STEAM", steam ? nullptr : "0");
    SetEnvironmentVariableA("MP_STEAM_FAKE", steam ? own_port : nullptr);
    SetEnvironmentVariableA("MP_PORT", host && !steam ? own_port : nullptr);
    SetEnvironmentVariableA("MP_TEST_MAXTIME", nullptr);
    session_test_disable_udp(steam); // fake Steam only; IP uses the production UDP socket
    session_test_disable_upnp(true); // do not touch a real router in the IP test
    alignas(8) unsigned char manager[0x1800] = {};
    void *vt[16] = {};
    vt[9] = (void *)find_nothing;
    ((ObjMan *)manager)->vtbl = vt;
    session_start((ObjMan *)manager);
    if ((steam && (!steam_ready() || session_test_udp_open())) ||
        (!steam && host && !session_test_udp_open())) return 3;
    if (host) {
        if (!menu_only) session_test_lobby_image(); // synthetic payload; no game save or process launch
        SetEvent(host_ready);                     // the client must not send before the host has bound its socket
    } else if (WaitForSingleObject(host_ready, 5000) != WAIT_OBJECT_0) return 14;
    Joiner lobby;
    if (!host) {
        if (steam) {
            if (lobby.start("steam:42garbage") || steam_lobby_receiving()) return 4;
            if (!lobby.start("127.0.0.1:1943") || steam_lobby_receiving()) return 5;
            lobby.cancel();
        }
        char address[80];
        if (steam) sprintf_s(address, "steam:%s", peer_port);
        else sprintf_s(address, "127.0.0.1:%s", peer_port);
        if (steam) {
            Joiner map_compatible;
            if (!map_compatible.start(address, false) || steam_lobby_receiving()) return 9;
            map_compatible.cancel();
            Joiner cancelled;
            if (!cancelled.start(address) || !steam_lobby_receiving()) return 10;
            cancelled.cancel();
            if (steam_lobby_receiving()) return 11;
            Joiner failed;
            if (!failed.start(address) || !steam_lobby_receiving()) return 12;
            failed.fail("synthetic failure");
            if (steam_lobby_receiving()) return 13;
        }
        if (!lobby.start(address) || steam_lobby_receiving() != steam) return 6;
        if (steam) {
            Joiner competing;
            if (competing.start(address) || !steam_lobby_receiving()) return 7;
            competing.cancel();
            if (!steam_lobby_receiving()) return 8; // failed competitor cannot release our claim
        }
        lobby.last_hello = GetTickCount() - 500; // initial HELLO immediately, even just after boot
    }
    const char *result = menu_only ? (steam ? "STEAM_MENU_RESULT" : "IP_MENU_RESULT") :
                                      (steam ? "LOBBY_RESULT" : "IP_LOBBY_RESULT");
    DWORD start = GetTickCount(), connected_at = 0;
    while (GetTickCount() - start < 8000) {
        session_frame((ObjMan *)manager);
        if (host && session_connected() && !connected_at) {
            connected_at = GetTickCount();
            SetEvent(welcome_sent);
        }
        if (host && connected_at && WaitForSingleObject(download_done, 0) == WAIT_OBJECT_0) {
            bool ready = false;
            char peer_name[24];
            if (!menu_only || (session_peer(peer_name, sizeof peer_name, &ready) && ready)) {
                SetEvent(ready_seen);
                printf("%s host ok=1%s\n", result, menu_only ? " ready=1" : ""); session_stop(); steam_shutdown();
                CloseHandle(welcome_sent); CloseHandle(download_done); CloseHandle(host_ready); CloseHandle(ready_seen); return 0;
            }
        }
        if (!host) {
            lobby.poll(); // sends HELLO
            if (WaitForSingleObject(welcome_sent, 5000) != WAIT_OBJECT_0) break;
            bool packet_waiting;
            if (steam) packet_waiting = steam_test_wait_packet(2000);
            else {
                fd_set ready; FD_ZERO(&ready); FD_SET(lobby.sock, &ready);
                timeval timeout = { 2, 0 };
                packet_waiting = select(0, &ready, nullptr, nullptr, &timeout) > 0;
            }
            if (!packet_waiting) break;
            session_frame((ObjMan *)manager); // WELCOME is already waiting: this must not consume it
            lobby.poll();
            if (lobby.state == Joiner::WAITING_FOR_BATTLE && lobby.host_name[0]) {
                bool reserved = steam_lobby_receiving() == steam;
                if (menu_only) {
                    bool waiting = !lobby.host_in_battle && lobby.image.empty() && lobby.host_mode == HOST_MODE_OPERATION;
                    lobby.lobby_ready = true;
                    lobby.last_hello = GetTickCount() - 500;
                    SetEvent(download_done); // host must observe READY before acknowledging the test
                    bool seen = false;
                    while (GetTickCount() - start < 7000 && lobby.state == Joiner::WAITING_FOR_BATTLE) {
                        session_frame((ObjMan *)manager);
                        lobby.poll();
                        if (WaitForSingleObject(ready_seen, 0) == WAIT_OBJECT_0) { seen = true; break; }
                        Sleep(10);
                    }
                    lobby.cancel();
                    bool released = !steam_lobby_receiving();
                    bool ok = reserved && released && waiting && seen;
                    printf("%s join ok=%d reserved=%d released=%d ordered=1 waiting=%d ready=%d\n",
                           result, (int)ok, (int)reserved, (int)released, (int)waiting, (int)seen);
                    session_stop(); steam_shutdown(); CloseHandle(welcome_sent); CloseHandle(download_done);
                    CloseHandle(host_ready); CloseHandle(ready_seen);
                    return ok ? 0 : 1;
                }
                lobby.lobby_ready = true;
                while (lobby.state != Joiner::DONE && lobby.state != Joiner::FAILED && GetTickCount() - start < 7000) {
                    session_frame((ObjMan *)manager); // preserve the real ordering throughout download
                    lobby.poll();
                    Sleep(10);
                }
                bool image_ok = lobby.state == Joiner::DONE && lobby.image.size() == k_chunk * 2 + 117;
                for (size_t i = 0; image_ok && i < lobby.image.size(); ++i)
                    image_ok = lobby.image[i] == (unsigned char)((i * 37u + 11u) & 255u);
                bool released = !steam_lobby_receiving(); // DONE closes the Joiner and releases its queue
                bool ok = reserved && released && image_ok;
                if (ok) SetEvent(download_done);
                printf("%s join ok=%d reserved=%d released=%d ordered=1 image=%d\n", result, (int)ok, (int)reserved, (int)released, (int)image_ok);
                lobby.cancel(); session_stop(); steam_shutdown(); CloseHandle(welcome_sent); CloseHandle(download_done); CloseHandle(host_ready); CloseHandle(ready_seen);
                return ok ? 0 : 1;
            }
            break; // fail immediately: do not allow a later retransmit to hide the ordering bug
        }
        Sleep(10);
    }
    printf("%s %s ok=0 state=%d error=%s\n", result, host ? "host" : "join", (int)lobby.state, lobby.error);
    lobby.cancel(); session_stop(); steam_shutdown(); CloseHandle(welcome_sent); CloseHandle(download_done); CloseHandle(host_ready); CloseHandle(ready_seen);
    return 1;
}

// End-to-end M5 packet flow through the real session + Joiner. Only game save/load and engine
// turn execution are substituted by fixture operations; no claim is made about the real game.
static int ordered_map_test(bool host, bool steam, const char *own_port, const char *peer_port)
{
    char event_name[96];
    sprintf_s(event_name, "Local\\netv21_m5_%s_%s_ready", steam ? "steam" : "ip", host ? own_port : peer_port);
    HANDLE host_ready = CreateEventA(nullptr, TRUE, FALSE, event_name);
    sprintf_s(event_name, "Local\\netv21_m5_%s_%s_done", steam ? "steam" : "ip", host ? own_port : peer_port);
    HANDLE guest_done = CreateEventA(nullptr, TRUE, FALSE, event_name);
    if (!host_ready || !guest_done) return 2;
    strcpy_s(g_program, "opgame");
    SetEnvironmentVariableA("MP_ROLE", host ? "host" : nullptr);
    SetEnvironmentVariableA("MP_HOST_MODE", "operation");
    SetEnvironmentVariableA("MP_MAP", nullptr);
    SetEnvironmentVariableA("MP_MAP_EPOCH", "101"); SetEnvironmentVariableA("MP_MAP_REV", "1");
    SetEnvironmentVariableA("MP_STEAM", steam ? nullptr : "0");
    SetEnvironmentVariableA("MP_STEAM_FAKE", steam ? own_port : nullptr);
    SetEnvironmentVariableA("MP_PORT", host && !steam ? own_port : nullptr);
    session_test_disable_udp(steam); session_test_disable_upnp(true);
    alignas(8) unsigned char manager[0x1800] = {};
    void *vt[16] = {}; vt[9] = (void *)find_nothing; ((ObjMan *)manager)->vtbl = vt;
    session_start((ObjMan *)manager);
    session_test_map_enable(true); // inject losses/duplicates/one stale revision in the test build
    if ((steam && (!steam_ready() || session_test_udp_open())) || (!steam && host && !session_test_udp_open())) return 3;
    const char *result = steam ? "M5_STEAM_RESULT" : "M5_IP_RESULT";
    if (host) {
        if (!session_test_map_seed(opsave_fixture::host(false), false)) return 4;
        SetEvent(host_ready);
    } else {
        if (WaitForSingleObject(host_ready, 5000) != WAIT_OBJECT_0) return 5;
        char address[80];
        if (steam) sprintf_s(address, "steam:%s", peer_port);
        else sprintf_s(address, "127.0.0.1:%s", peer_port);
        Joiner lobby; lobby.lobby_ready = true;
        if (!lobby.start(address)) return 6;
        lobby.last_hello = GetTickCount() - 500;
        DWORD started = GetTickCount();
        while (GetTickCount() - started < 10000 && lobby.state != Joiner::DONE && lobby.state != Joiner::FAILED) {
            session_frame((ObjMan *)manager); lobby.poll(); Sleep(10);
        }
        if (lobby.state != Joiner::DONE || !lobby.want_map || lobby.map_epoch != 101 || lobby.map_revision != 1 ||
            !opsave_checked::validate(lobby.image)) {
            printf("%s join ok=0 initial state=%d error=%s\n",result,(int)lobby.state,lobby.error); return 7;
        }
        // Simulate the opgame process that would be started by launch_map; this one keeps no engine.
        session_stop();
        SetEnvironmentVariableA("MP_ROLE", "join"); SetEnvironmentVariableA("MP_MAP", "1");
        SetEnvironmentVariableA("MP_PEER", address); SetEnvironmentVariableA("MP_MAP_EPOCH", "101");
        SetEnvironmentVariableA("MP_MAP_REV", "1");
        session_start((ObjMan *)manager); session_test_map_enable(true);
    }
    bool host_finished = false, adopted = false, guest_finished = false, offered = false;
    bool disconnected_checked = false;
    bool failed_commit_seen = false, committed = false, old_hello_safe = false, reconnected = false;
    DWORD failure_at = 0, started = GetTickCount();
    std::vector<unsigned char> live, combined;
    while (GetTickCount() - started < 22000) {
        DWORD now = GetTickCount();
        session_frame((ObjMan *)manager);
        if (host && !host_finished && session_test_map_peer_loaded()) {
            if (!disconnected_checked) {
                session_test_map_disconnect();
                if (session_test_map_input() || session_test_map_finish()) return 16;
                disconnected_checked = true;
                continue; // reconnect before the host can finish its orders
            }
            if (!session_test_map_input() || !session_test_map_finish() || session_test_map_input()) return 8;
            if (!session_test_map_seed(opsave_fixture::host(true), true)) return 9;
            session_test_map_old_hello();
            old_hello_safe = session_test_map_host_phase() == (int)operation_turn::Phase::handoff && !session_test_map_input();
            if (!old_hello_safe) return 10;
            session_test_map_disconnect(); // same peer must reconnect without resetting phase
            host_finished = true;
        }
        if (host && host_finished && session_connected()) reconnected = true;
        if (!host && !adopted && session_test_map_download_ready()) {
            if (!session_test_map_adopt_download(&live) || opsave::rows_of(live,"user_orders") != 1 || session_test_map_input()) return 11;
            adopted = true;
        }
        if (!host && adopted && !guest_finished && session_test_map_input()) {
            if (!disconnected_checked) {
                session_test_map_disconnect();
                if (session_test_map_input() || session_test_map_finish()) return 17;
                disconnected_checked = true;
                continue; // the guest must reconnect before finishing its orders
            }
            if (!opsave_fixture::guest_order(&live) || !session_test_map_finish() || session_test_map_input()) return 12;
            guest_finished = true;
        }
        if (!host && guest_finished && !offered && session_test_map_collecting()) {
            if (!session_test_map_offer(live)) return 13;
            offered = true;
        }
        if (host && session_test_map_received() && !failed_commit_seen) {
            if (session_test_map_commit(false, &combined) || session_test_map_host_phase() != (int)operation_turn::Phase::collect ||
                session_test_map_input()) return 14;
            failed_commit_seen = true; failure_at = now;
        }
        if (host && failed_commit_seen && !committed && now - failure_at >= 1500) {
            if (!session_test_map_commit(true, &combined) || opsave::rows_of(combined,"user_orders") != 2 ||
                opsave::u32_at(combined,opsave_fixture::field(combined,"p_guest","px")) != 7) return 15;
            committed = true;
        }
        if (!host && session_test_map_acked()) {
            bool ok = disconnected_checked && adopted && guest_finished && offered && !session_test_map_input() &&
                      session_test_map_dropped() && session_test_map_duplicates() && session_test_map_stale();
            printf("%s join ok=%d adopted=%d finished=%d offer=%d drop=%u duplicate=%u stale=%u\n",result,(int)ok,
                   (int)adopted,(int)guest_finished,(int)offered,session_test_map_dropped(),
                   session_test_map_duplicates(),session_test_map_stale());
            SetEvent(guest_done); session_stop(); steam_shutdown(); CloseHandle(host_ready); CloseHandle(guest_done);
            return ok ? 0 : 1;
        }
        if (host && committed && WaitForSingleObject(guest_done,0) == WAIT_OBJECT_0) {
            bool ok = disconnected_checked && host_finished && old_hello_safe && reconnected && failed_commit_seen &&
                      session_test_map_dropped() && session_test_map_duplicates() && session_test_map_stale();
            printf("%s host ok=%d stale_hello=%d reconnect=%d io_failure=%d drop=%u duplicate=%u stale=%u\n",result,(int)ok,
                   (int)old_hello_safe,(int)reconnected,(int)failed_commit_seen,session_test_map_dropped(),
                   session_test_map_duplicates(),session_test_map_stale());
            session_stop(); steam_shutdown(); CloseHandle(host_ready); CloseHandle(guest_done);
            return ok ? 0 : 1;
        }
        Sleep(10);
    }
    printf("%s %s ok=0 timeout connected=%d phase=%d adopted=%d offered=%d commit=%d ack=%d\n", result,
           host ? "host" : "join", (int)session_connected(),session_test_map_host_phase(),(int)adopted,(int)offered,
           (int)committed,(int)session_test_map_acked());
    session_stop(); steam_shutdown(); CloseHandle(host_ready); CloseHandle(guest_done);
    return 1;
}

int main(int argc, char **argv)
{
    if (argc != 4) return 2;
    if (!strcmp(argv[1], "m5-steam-host") || !strcmp(argv[1], "m5-steam-join"))
        return ordered_map_test(!strcmp(argv[1], "m5-steam-host"), true, argv[2], argv[3]);
    if (!strcmp(argv[1], "m5-ip-host") || !strcmp(argv[1], "m5-ip-join"))
        return ordered_map_test(!strcmp(argv[1], "m5-ip-host"), false, argv[2], argv[3]);
    if (!strcmp(argv[1], "lobby-host") || !strcmp(argv[1], "lobby-join"))
        return lobby_test(!strcmp(argv[1], "lobby-host"), true, false, argv[2], argv[3]);
    if (!strcmp(argv[1], "ip-lobby-host") || !strcmp(argv[1], "ip-lobby-join"))
        return lobby_test(!strcmp(argv[1], "ip-lobby-host"), false, false, argv[2], argv[3]);
    if (!strcmp(argv[1], "steam-menu-host") || !strcmp(argv[1], "steam-menu-join"))
        return lobby_test(!strcmp(argv[1], "steam-menu-host"), true, true, argv[2], argv[3]);
    if (!strcmp(argv[1], "ip-menu-host") || !strcmp(argv[1], "ip-menu-join"))
        return lobby_test(!strcmp(argv[1], "ip-menu-host"), false, true, argv[2], argv[3]);
    if (strcmp(argv[1], "host") && strcmp(argv[1], "join")) return 2;
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

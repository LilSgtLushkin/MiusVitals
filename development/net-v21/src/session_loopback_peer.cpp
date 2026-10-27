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
bool session_test_host_battle(const std::vector<unsigned char> &);
bool session_test_host_battle_notice();
void session_test_spawn_battle_lobby();
u32 session_test_source_hash();
void session_test_fresh_tactical();
bool session_test_map_control();
u32 session_test_map_out_hash();
bool session_test_map_no_guest_upload();
bool session_test_map_peer_loaded();
bool session_test_map_input();
bool session_test_map_download_ready();
bool session_test_map_adopt_download(std::vector<unsigned char> *);
void session_test_map_disconnect();
void session_test_map_leave_screen();
void session_test_map_old_hello();
void session_test_map_forbidden_packets();

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

// End-to-end spectator packet flow through the real session + Joiner. Only game save/load and engine
// turn execution are substituted by fixture operations; no claim is made about the real game.
static int spectator_map_test(bool host, bool steam, const char *own_port, const char *peer_port)
{
    char event_name[96];
    auto make_event = [&](const char *label) -> HANDLE {
        sprintf_s(event_name, "Local\\netv21_spectator_%s_%s_%s", steam ? "steam" : "ip", host ? own_port : peer_port, label);
        return CreateEventA(nullptr, TRUE, FALSE, event_name);
    };
    HANDLE host_ready = make_event("ready"), receiving = make_event("receiving"), guest_done = make_event("done");
    if (!host_ready || !receiving || !guest_done) return 2;
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
    session_start((ObjMan *)manager); session_test_map_enable(true);
    if (!session_test_map_control() || (steam && (!steam_ready() || session_test_udp_open())) ||
        (!steam && host && !session_test_udp_open())) return 3;
    const char *result = steam ? "SPECTATOR_STEAM_RESULT" : "SPECTATOR_IP_RESULT";
    std::vector<unsigned char> pending = opsave_fixture::host(true);
    opsave_fixture::put(pending, opsave_fixture::field(pending, "op_stream", "turn_phase"), 2);
    std::vector<unsigned char> final = pending;
    opsave_fixture::put(final, opsave_fixture::field(final, "p_host", "px"), 9);
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
        bool announced = false; DWORD started = GetTickCount();
        while (GetTickCount() - started < 12000 && lobby.state != Joiner::DONE && lobby.state != Joiner::FAILED) {
            session_frame((ObjMan *)manager); lobby.poll();
            if (!announced && lobby.state == Joiner::RECEIVING && lobby.got) { SetEvent(receiving); announced = true; }
            Sleep(10);
        }
        if (!announced || lobby.state != Joiner::DONE || !lobby.want_map || lobby.map_epoch != 103 ||
            lobby.map_revision != 3 || lobby.image != pending) {
            printf("%s join ok=0 supersede state=%d epoch=%u rev=%u error=%s\n", result, (int)lobby.state,
                   lobby.map_epoch, lobby.map_revision, lobby.error); return 7;
        }
        std::vector<unsigned char> local = lobby.image;
        if (!opsave_fixture::guest_order(&local) || local == lobby.image) return 8;
        session_stop();
        SetEnvironmentVariableA("MP_ROLE", "join"); SetEnvironmentVariableA("MP_MAP", "1");
        SetEnvironmentVariableA("MP_PEER", address); SetEnvironmentVariableA("MP_MAP_EPOCH", "103");
        SetEnvironmentVariableA("MP_MAP_REV", "3");
        session_start((ObjMan *)manager); session_test_map_enable(true);
        session_test_map_disconnect();
        if (!session_test_map_input() || !session_test_map_control()) return 9;
        DWORD start = GetTickCount();
        while (GetTickCount() - start < 18000) {
            session_frame((ObjMan *)manager);
            if (session_test_map_download_ready()) {
                std::vector<unsigned char> loaded;
                bool ok = session_test_map_adopt_download(&loaded) && loaded == final && loaded != local &&
                          session_test_map_input() && session_test_map_control();
                session_test_map_leave_screen(); session_frame((ObjMan *)manager);
                bool menu = !session_map_mode() && session_role() == ROLE_NONE && !session_connected();
                ok = ok && menu;
                session_stop();
                strcpy_s(g_program, "tacgame"); SetEnvironmentVariableA("MP_ROLE", "join");
                SetEnvironmentVariableA("MP_MAP", "1"); session_start((ObjMan *)manager);
                bool offline = session_role() == ROLE_NONE && !session_connected();
                ok = ok && offline;
                printf("%s join ok=%d supersede=%d local_only=1 reload=%d menu=%d local_battle_offline=%d\n",
                       result, (int)ok, (int)announced, (int)(loaded == final), (int)menu, (int)offline);
                SetEvent(guest_done); session_stop(); steam_shutdown();
                CloseHandle(host_ready); CloseHandle(receiving); CloseHandle(guest_done);
                return ok ? 0 : 1;
            }
            Sleep(10);
        }
        printf("%s join ok=0 timeout connected=%d\n", result, (int)session_connected()); return 10;
    }
    bool superseded = false, disconnected = false, updated = false, malicious_ignored = false;
    DWORD start = GetTickCount();
    while (GetTickCount() - start < 22000) {
        session_frame((ObjMan *)manager);
        if (!superseded && WaitForSingleObject(receiving, 0) == WAIT_OBJECT_0) {
            if (!session_test_map_seed(pending, true) || !session_test_map_input()) return 11;
            superseded = true;
        }
        if (superseded && !disconnected && session_test_map_peer_loaded()) {
            session_test_map_disconnect();
            if (!session_test_map_input()) return 12;
            disconnected = true;
        } else if (disconnected && !updated && session_test_map_peer_loaded()) {
            if (!session_test_map_seed(final, true)) return 13;
            unsigned hash = session_test_map_out_hash();
            session_test_map_old_hello(); // forged obsolete turn/guest upload cannot change the host
            unsigned state_before = legacy_packets, shot_before = legacy_shot_packets;
            session_test_map_forbidden_packets();
            malicious_ignored = session_test_map_no_guest_upload() && session_test_map_out_hash() == hash && session_test_map_input() &&
                                legacy_packets == state_before && legacy_shot_packets == shot_before;
            updated = true;
        }
        if (updated && WaitForSingleObject(guest_done, 0) == WAIT_OBJECT_0) {
            bool ok = superseded && disconnected && malicious_ignored && session_test_map_dropped() &&
                      session_test_map_duplicates() && session_test_map_stale();
            printf("%s host ok=%d supersede=%d reconnect=%d guest_ignored=%d drop=%u duplicate=%u stale=%u\n",
                   result, (int)ok, (int)superseded, (int)disconnected, (int)malicious_ignored,
                   session_test_map_dropped(), session_test_map_duplicates(), session_test_map_stale());
            session_stop(); steam_shutdown(); CloseHandle(host_ready); CloseHandle(receiving); CloseHandle(guest_done);
            return ok ? 0 : 1;
        }
        Sleep(10);
    }
    printf("%s host ok=0 timeout supersede=%d disconnected=%d updated=%d\n", result, (int)superseded, (int)disconnected, (int)updated);
    session_stop(); steam_shutdown(); return 1;
}

// Both actual CreateProcess paths spawn this harmless starter. Read its environment verdict
// and wait for it to exit before reusing the filename or deleting the isolated directory.
static bool starter_checked(const char *root)
{
    char path[MAX_PATH]; sprintf_s(path, "%s\\starter_fixture_result.txt", root);
    bool ok = false; unsigned long pid = 0; DWORD start = GetTickCount();
    while (GetTickCount() - start < 5000) {
        FILE *f = nullptr;
        if (!fopen_s(&f, path, "rb") && f) { ok = fscanf_s(f, "ok:%lu", &pid) == 1; fclose(f); break; }
        Sleep(10);
    }
    if (pid) {
        HANDLE child = OpenProcess(SYNCHRONIZE, FALSE, pid);
        if (child) { ok = ok && WaitForSingleObject(child, 5000) == WAIT_OBJECT_0; CloseHandle(child); }
    }
    DeleteFileA(path);
    return ok;
}

// Real network sessions, map->lobby and tactical launchers; synthetic AZCF and harmless child.
// UI and game engine are not running: the test opens the lobby and resumes a fresh tactical
// session in-process after checking the exact environments of both actual child processes.
static int host_battle_transition(bool host, bool steam, const char *our_port, const char *peer_port)
{
    SetEnvironmentVariableA("MP_STEAM", steam ? nullptr : "0");
    SetEnvironmentVariableA("MP_STEAM_FAKE", steam ? our_port : nullptr);
    SetEnvironmentVariableA("MP_PORT", host ? our_port : "0");
    SetEnvironmentVariableA("MP_HOST_MODE", "operation");
    SetEnvironmentVariableA("MP_ROLE", host ? "host" : nullptr);
    SetEnvironmentVariableA("MP_MAP", nullptr); SetEnvironmentVariableA("MP_MAP_EPOCH", "101");
    SetEnvironmentVariableA("MP_MAP_REV", "1"); SetEnvironmentVariableA("MP_VERSUS", nullptr);
    session_test_disable_upnp(true); session_test_disable_udp(steam);
    session_test_map_enable(false);
    char address[80];
    if (steam) sprintf_s(address, "steam:%s", peer_port);
    else sprintf_s(address, "127.0.0.1:%s", peer_port);
    const std::vector<unsigned char> map = opsave_fixture::host(false), battle = opsave_fixture::battle();
    char event_name[128];
    sprintf_s(event_name, "Local\\netv21_battle_ready_%s_%s", steam ? "steam" : "ip", host ? our_port : peer_port);
    HANDLE ready = CreateEventA(nullptr, TRUE, FALSE, event_name);
    sprintf_s(event_name, "Local\\netv21_battle_done_%s_%s", steam ? "steam" : "ip", host ? our_port : peer_port);
    HANDLE done = CreateEventA(nullptr, TRUE, FALSE, event_name);
    if (!ready || !done) return 2;
    alignas(8) unsigned char manager[0x110] = {};
    void *vtable[15] = {}; vtable[9] = (void *)&find_nothing; *(void ***)manager = vtable;
    strcpy_s(g_program, "opgame"); session_start((ObjMan *)manager);
    if (host) {
        if (!session_test_map_seed(map, false)) return 3;
        SetEvent(ready);
        bool map_seen = false, tactical_started = false, controls = false, connected = false, finished = false;
        DWORD start = GetTickCount();
        while (GetTickCount() - start < 26000) {
            session_frame((ObjMan *)manager);
            if (!tactical_started && session_test_map_peer_loaded()) {
                map_seen = true; session_stop();
                strcpy_s(g_program, "tacgame"); session_start((ObjMan *)manager);
                tactical_started = session_test_host_battle(battle);
                if (!tactical_started) break;
            }
            if (tactical_started && session_connected()) {
                connected = true;
                Object mine = {}, theirs = {};
                mine.iface = theirs.iface = "i_human";
                strcpy_s(mine.full, "i_human:hum_00000100"); strcpy_s(theirs.full, "i_human:hum_00000200");
                controls = session_versus() && session_owns(&mine) && !session_owns(&theirs) &&
                           session_owns_squad(0x100) && !session_owns_squad(0x200);
            }
            if (WaitForSingleObject(done, 0) == WAIT_OBJECT_0) { finished = true; break; }
            Sleep(10);
        }
        bool ok = map_seen && tactical_started && connected && controls && finished;
        printf("BATTLE_%s_RESULT host ok=%d map=%d launched=%d connected=%d controls=%d guest_done=%d\n",
               steam ? "STEAM" : "IP", (int)ok, (int)map_seen, (int)tactical_started,
               (int)connected, (int)controls, (int)finished);
        session_stop(); steam_shutdown(); CloseHandle(ready); CloseHandle(done); return ok ? 0 : 1;
    }
    if (WaitForSingleObject(ready, 5000) != WAIT_OBJECT_0) return 4;
    Joiner map_lobby; map_lobby.lobby_ready = true;
    if (!map_lobby.start(address)) return 5;
    map_lobby.last_hello = GetTickCount() - 500;
    DWORD start = GetTickCount();
    while (GetTickCount() - start < 10000 && map_lobby.state != Joiner::DONE && map_lobby.state != Joiner::FAILED) {
        session_frame((ObjMan *)manager); map_lobby.poll(); Sleep(10);
    }
    if (map_lobby.state != Joiner::DONE || !map_lobby.want_map || map_lobby.image != map ||
        map_lobby.map_epoch != 102 || map_lobby.map_revision != 2) return 6;
    char temp[MAX_PATH], root[MAX_PATH], src[MAX_PATH], dst[MAX_PATH], path[MAX_PATH];
    if (!GetTempPathA(sizeof temp, temp)) return 11;
    sprintf_s(root, "%snetv21_host_battle_%lu", temp, GetCurrentProcessId());
    if (!CreateDirectoryA(root, nullptr)) return 12;
    const char *dirs[] = { "data", "data\\common", "data\\common\\shared", "data\\common\\shared\\config" };
    for (const char *d : dirs) { sprintf_s(path, "%s\\%s", root, d); if (!CreateDirectoryA(path, nullptr)) return 13; }
    if (!GetModuleFileNameA(nullptr, src, sizeof src)) return 14;
    char *slash = strrchr(src, '\\'); if (!slash) return 15;
    strcpy_s(slash + 1, sizeof src - (slash + 1 - src), "starter_fixture.exe");
    sprintf_s(dst, "%s\\starter_x64.exe", root);
    if (!CopyFileA(src, dst, FALSE)) return 16;
    sprintf_s(g_out_dir, "%s\\out", root);
    SetEnvironmentVariableA("MP_ROLE", "join"); SetEnvironmentVariableA("MP_MAP", "1");
    SetEnvironmentVariableA("MP_MAP_EPOCH", "102"); SetEnvironmentVariableA("MP_MAP_REV", "2");
    SetEnvironmentVariableA("MP_PEER", address);
    session_test_spawn_battle_lobby(); session_start((ObjMan *)manager);
    bool notice = false;
    start = GetTickCount();
    while (GetTickCount() - start < 10000) {
        session_frame((ObjMan *)manager);
        if (session_test_host_battle_notice()) { notice = true; break; }
        Sleep(10);
    }
    if (!notice) return 7;
    bool lobby_child = starter_checked(root);
    if (!lobby_child) return 18;
    // The fixture tested CreateProcess and environment. Simulate the exited map process and
    // the game's autojoin UI: this fixture is not the engine and must not invoke ExitProcess.
    session_test_map_leave_screen(); session_frame((ObjMan *)manager);
    if (session_map_mode() || session_role() != ROLE_NONE) return 8;
    Joiner lobby; lobby.lobby_ready = true;
    if (!lobby.start(address)) return 9;
    lobby.last_hello = GetTickCount() - 500;
    start = GetTickCount();
    while (GetTickCount() - start < 10000 && lobby.state != Joiner::DONE && lobby.state != Joiner::FAILED) {
        session_frame((ObjMan *)manager); lobby.poll(); Sleep(10);
    }
    bool downloaded = lobby.state == Joiner::DONE && !lobby.want_map && lobby.host_mode == HOST_MODE_OPERATION &&
                      lobby.versus && lobby.image == battle;
    if (!downloaded) return 10;

    // Launch the real Joiner into a harmless starter, and then use its actual mirrored file to
    // model the new tactical process. Starting with MP_MAP=1 exercises its explicit removal.
    SetEnvironmentVariableA("MP_MAP", "1");
    bool launched = lobby.launch(root);
    if (!launched) { printf("launcher: %s\n", lobby.error); return 17; }
    bool child = starter_checked(root);
    // Start with the same environment the child fixture verified. Reset in-memory perspective to
    // model a genuinely fresh process and check the source hash after load_battle_description().
    SetEnvironmentVariableA("MP_ROLE", "join"); SetEnvironmentVariableA("MP_MAP", nullptr);
    SetEnvironmentVariableA("MP_PEER", address); SetEnvironmentVariableA("MP_VERSUS", "1");
    sprintf_s(g_out_dir, "%s\\out", root); strcpy_s(g_program, "tacgame");
    session_test_fresh_tactical(); session_start((ObjMan *)manager);
    bool hash = session_test_source_hash() == fnv(battle.data(), battle.size());
    Object mine = {}, theirs = {};
    mine.iface = theirs.iface = "i_human";
    strcpy_s(mine.full, "i_human:hum_00000200"); strcpy_s(theirs.full, "i_human:hum_00000100");
    bool controls = session_role() == ROLE_JOIN && session_versus() && session_owns(&mine) && !session_owns(&theirs) &&
                    session_owns_squad(0x200) && !session_owns_squad(0x100);
    bool connected = false;
    start = GetTickCount();
    while (GetTickCount() - start < 10000) {
        session_frame((ObjMan *)manager);
        if (session_connected()) { connected = true; break; }
        Sleep(10);
    }
    bool ok = notice && lobby_child && downloaded && launched && child && hash && controls && connected;
    printf("BATTLE_%s_RESULT join ok=%d notice=%d lobby_env=%d image=%d launch=%d battle_env=%d hash=%d controls=%d connected=%d\n",
           steam ? "STEAM" : "IP", (int)ok, (int)notice, (int)lobby_child, (int)downloaded, (int)launched, (int)child,
           (int)hash, (int)controls, (int)connected);
    // Give the host enough time to receive the final tactical HELLO before stopping.
    start = GetTickCount();
    while (GetTickCount() - start < 500) { session_frame((ObjMan *)manager); Sleep(10); }
    SetEvent(done); session_stop(); steam_shutdown();
    DeleteFileA(path); DeleteFileA(dst);
    sprintf_s(path, "%s\\%s", root, k_image_path); DeleteFileA(path);
    for (int i = 3; i >= 0; --i) { sprintf_s(path, "%s\\%s", root, dirs[i]); RemoveDirectoryA(path); }
    RemoveDirectoryA(root); CloseHandle(ready); CloseHandle(done); return ok ? 0 : 1;
}

int main(int argc, char **argv)
{
    if (argc != 4) return 2;
    if (!strcmp(argv[1], "battle-steam-host") || !strcmp(argv[1], "battle-steam-join"))
        return host_battle_transition(!strcmp(argv[1], "battle-steam-host"), true, argv[2], argv[3]);
    if (!strcmp(argv[1], "battle-ip-host") || !strcmp(argv[1], "battle-ip-join"))
        return host_battle_transition(!strcmp(argv[1], "battle-ip-host"), false, argv[2], argv[3]);
    if (!strcmp(argv[1], "spectator-steam-host") || !strcmp(argv[1], "spectator-steam-join"))
        return spectator_map_test(!strcmp(argv[1], "spectator-steam-host"), true, argv[2], argv[3]);
    if (!strcmp(argv[1], "spectator-ip-host") || !strcmp(argv[1], "spectator-ip-join"))
        return spectator_map_test(!strcmp(argv[1], "spectator-ip-host"), false, argv[2], argv[3]);
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

    DWORD start = GetTickCount(), mutual_at = 0, withdrew_at = 0, ended_at = 0, disconnected_at = 0;
    bool offered = false, saw_offer = false, saw_withdrawal = false, saw_peer_over = false, intruder_sent = false;
    while (GetTickCount() - start < 24000) {
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
        if (host && mutual_at && !withdrew_at && now - mutual_at >= 6500) {
            if (!session_test_end_pending() || session_test_end_left()) return 5;
            session_toggle_ceasefire(); withdrew_at = now;
        }
        if (host && withdrew_at && !ended_at && now - withdrew_at >= 1000) {
            session_note_phase(3); ended_at = now;
        }
        if (!host && session_test_peer_over()) saw_peer_over = true;
        if (!host && saw_peer_over && session_test_end_pending()) {
            if (!session_connected() && !disconnected_at) disconnected_at = now;
            if (disconnected_at && now - disconnected_at >= 4500) {
                char role[32] = {}; GetEnvironmentVariableA("MP_ROLE", role, sizeof role);
                bool ok = offered && saw_offer && saw_withdrawal && !session_test_end_left() &&
                          !session_test_udp_open() && !strcmp(role, "join") && session_role() == ROLE_JOIN;
                printf("SESSION_RESULT join ok=%d disconnected=1 role_kept=%d offer=%d withdrawal=%d peer_over=%d pending=%d left=%d udp=%d\n",
                       (int)ok, (int)(!strcmp(role, "join")), (int)saw_offer, (int)saw_withdrawal, (int)saw_peer_over,
                       (int)session_test_end_pending(), (int)session_test_end_left(), (int)session_test_udp_open());
                session_stop(); steam_shutdown();
                return ok ? 0 : 1;
            }
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

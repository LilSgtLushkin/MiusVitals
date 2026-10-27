// Network session: one host, one joining player, UDP.
//
// The role comes from the in-game multiplayer menu (ui.cpp) or, for scripts and tests, from the environment:
//   MP_ROLE=host | join     MP_PEER=<address>[:port]     MP_PORT=<port>
//   MP_HOST_MODE=skirmish | operation                     MP_PREFER=GER | USSR (side the joining player wants)
// The environment is also how the role travels between the game's two programs: the operational program
// starts the tactical one as a child process, which inherits it.
//
// The host runs the battle. While it is in its menus it only tells callers to wait. Once a battle runs it
// serves the battle description, the battle clock and phase, and the state of the units it owns.
// Ownership: in a versus game each player's game owns the units that player commands and the other game
// follows them; when both look from the host's side the host owns everything. For owned units a game sends
// transforms about 15 times a second (this file) and the full saved state when it changes (statesync.cpp).
#include "common.h"
#include "env_value.h"
#include "tactical_mode.h"
#include "transform_write.h"
#include "transform_inbox.h"
#include "transform_send.h"
#include "transform_gc.h"
#include "peer_guard.h"
#include "transport_policy.h"
#include "render_guard.h"
#include "netlog.h"
#include "steam.h"
#include "protocol.h"
#include "ceasefire.h"
#include "image.h"
#include "force.h"
#include "battle_file.h"
#include "atomic_file.h"
#include "retry_gate.h"
#include "detour.h"
#include "hooks.h"
#include "opsave.h"
#include "opsave_checked.h"
#include "spectator_snapshot.h"
#include "spectator_policy.h"
#include "operation_turn.h"
#include "image_chunks.h"
#include "map_file_windows.h"
#include "joiner.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <natupnp.h>
#include <oleauto.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <atomic>

static const int k_units_per_packet = 14;       // 24 + 14 * 80 = 1144 bytes, below any sane MTU

static int g_role;
static int g_host_mode;
static bool g_tactical, g_versus;
static unsigned g_prefer_side;
static SOCKET g_sock = INVALID_SOCKET;
static sockaddr_in g_peer;
static bool g_have_peer, g_connected, g_bind_pending, g_upnp_started;
static unsigned short g_port = k_default_port;
static DWORD g_last_rx, g_last_hello, g_last_snapshot, g_last_report, g_last_bind_try;
static char g_root[MAX_PATH], g_local_ip[64] = "?", g_external_ip[64], g_status[320];
// lobby: who is playing, and whether the two players have exchanged forces (skirmish only)
static char g_my_name[24] = "Player", g_peer_name[24], g_image_file[MAX_PATH];
static bool g_peer_lobby_ready, g_swap, g_peer_swap_known;
static unsigned char g_peer_swap_req;
// The host alone owns the operation. A guest receives an unmodified, checked snapshot and
// may navigate its local copy; it never uploads orders or participates in host resolution.
static char g_profile_id[64] = "cloud";
static std::vector<unsigned char> g_map_out, g_map_in, g_map_have;
static std::vector<DWORD> g_map_asked;
static u32 g_map_out_hash, g_map_in_hash, g_map_got, g_map_turns;
static char g_map_file[96], g_map_in_file[96], g_map_note[120];
static bool g_map_given, g_map_mode, g_peer_on_map, g_map_hooks_ready;
static bool g_map_refresh_pending, g_guest_seen_map;
static DWORD g_share_after, g_map_seen_since, g_guest_left_since;
static DWORD g_map_reload_at, g_map_battle_at;
static u32 g_map_epoch, g_map_base_epoch, g_map_in_epoch, g_map_in_turns;
static u32 g_map_revision, g_map_base_revision, g_map_in_revision;
static operation_turn::Client g_map_client; // identity/order tracking only, never grants input ownership
// The worker may be called outside the session callback; marshal notifications to its thread.
static std::atomic<unsigned> g_host_turn_events{0};
typedef void (*NextTurn)(void *, u64, const char *);
typedef void (*TurnWorker)(void *, char, void *, void *);
static std::atomic<NextTurn> g_next_turn(nullptr);
static std::atomic<TurnWorker> g_turn_worker(nullptr);
static std::atomic<bool> g_watch_host_worker(false), g_block_guest_turn(false);

// battle description: served by the host, read by both for sides and unit ownership
static std::vector<unsigned char> g_image;
static u32 g_image_hash, g_user_side, g_enemy_side;
// The description as the host has it: this game's own copy, turned back round when this game mirrored it.
static u32 g_source_hash;
static std::unordered_set<u64> g_my_units;          // versus: the units this game's player commands
static std::unordered_set<u64> g_position_only;     // units whose saved state is not exchanged

// unit transforms
static std::unordered_map<u64, transforms::Sent> g_sent;            // what was last sent per owned unit
static std::unordered_map<u64, Object *> g_units;       // local unit per id
static u32 g_units_count_seen;
static transforms::Inbox g_pending;                     // tactical: latest arrival per unit before the next simulation step
static std::vector<UnitRec> g_pending_legacy;           // non-tactical: preserve the old unfiltered receive path
// The last two transforms received per followed unit, with the time each arrived: the unit is shown
// between them, a little in the past (see session_sim_step).
struct Snap { UnitRec rec; double at; };
struct Held { Snap a, b; int have = 0; bool dirty = false; };
static std::unordered_map<u64, Held> g_held;
static float g_host_speed = 1, g_host_cur_speed = 1;
static u32 g_host_sim_ms;
static float g_clock_factor = 1;
static unsigned char g_host_paused;
static bool g_have_host_clock;
static u32 g_peer_sim_ms, g_peer_phase;    // host: the joining game's battle clock and phase, and when they were heard
static DWORD g_peer_clock_at, g_waiting_since;
static bool g_clock_adopted;
static u32 g_snapshot, g_stat_sent, g_stat_applied, g_stat_elided, g_stat_coalesced, g_stat_bad_transforms;
static u32 g_stat_zero_steps, g_stat_zero_skipped, g_stat_cache_pruned;
static u32 g_stat_packets, g_stat_rx_bytes, g_stat_tx_bytes, g_stat_snaps;
#ifdef NETV21_TEST_HOOKS
static unsigned g_test_hello_rejected;
static bool g_test_map_no_game, g_test_map_faults, g_test_map_on_screen;
static unsigned g_test_map_dropped, g_test_map_duplicates, g_test_map_stale;
static std::set<u64> g_test_map_dropped_pieces;
unsigned session_test_hello_rejected() { return g_test_hello_rejected; }
void session_test_map_enable(bool faults) { g_test_map_no_game = g_test_map_on_screen = true; g_map_hooks_ready = true; g_test_map_faults = faults; }
void session_test_map_leave_screen() { g_test_map_on_screen = false; g_guest_seen_map = true; g_guest_left_since = GetTickCount() - 2100; }
unsigned session_test_map_dropped() { return g_test_map_dropped; }
unsigned session_test_map_duplicates() { return g_test_map_duplicates; }
unsigned session_test_map_stale() { return g_test_map_stale; }
#endif
static double g_stat_err_sum;
static float g_stat_err_max;
static u64 g_sim_ms;
static u32 g_phase, g_host_phase;          // battle phase here / on the host
static DWORD g_last_phase_step;
static DWORD g_host_lost;                  // joining game: when the host went away (0 = it has not)
// Both players have to press Start to leave deployment, and again to leave the initial orders.
static u32 g_my_ready, g_peer_ready;       // the phase each player has asked to move on to (1 or 2)

// M3: how far the battle's end has gone (see the "end of a battle" section below).
static ceasefire::State g_end;
static DWORD g_end_last_tx, g_end_key_was;
// The host is starting the battle again with both forces: the joining game must not take the silence that
// follows for the host leaving.
static bool g_restart_coming, g_restart_say;
static DWORD g_restart_until;

// M4: the two players' forces (see the "two players' forces" section below).
static std::vector<unsigned char> g_force_out, g_force_in, g_force_have;
static std::vector<DWORD> g_force_asked;
static u32 g_force_out_hash, g_force_in_hash, g_force_got;
static bool g_force_published, g_force_applied, g_force_noted, g_force_recovery_blocked;
static DWORD g_force_last_ask;
static RetryGate g_force_retry; // do not reparse and log a permanently invalid offer every rendered frame
// The joining game downloading a description the host has started its battle with.
static std::vector<unsigned char> g_desc_in, g_desc_have;
static std::vector<DWORD> g_desc_asked;
static u32 g_desc_hash, g_desc_got;
static DWORD g_desc_last_ask, g_desc_since;
static int g_desc_tries;
static bool g_desc_dead;
typedef u64 (*StartAction)(Object *, u64, u64, u64);
typedef void (*InputAction)(Object *, u64, const char *);
typedef void (*BoxResult)(Object *, int, u64, u64);
typedef void (*TimerStep)(char *, float, u32);
static StartAction g_start_action;         // the engine's own functions, reached past our detours
static InputAction g_input_action;
static BoxResult g_box_result;
static TimerStep g_timer_step;
typedef void (*SetSpeed)(char *, float);
typedef void (*SetPause)(char *, unsigned char);
static SetSpeed g_set_speed;               // the timer's own setters, replaced on the timer object of a joining game
static SetPause g_set_pause;
static double g_sim_step = 0.02;           // multiplayer battles simulate in steps of this many seconds (MP_SIM_HZ)
static void force_publish(void);           // M4: this player's force, offered to the other game (defined below)

int session_role(void) { return g_role; }
int session_host_mode(void) { return g_host_mode; }
bool session_connected(void) { return g_connected; }
bool session_versus(void) { return g_versus; }
const char *session_local_ip(void) { return g_local_ip; }
unsigned short session_port(void) { return g_port; }
// load measurement for the 5 s report
static u32 g_prof_frames, g_prof_steps, g_prof_step_ms;
static double g_prof_save, g_prof_load;
static double seconds_now(void)
{
    static LARGE_INTEGER f;
    LARGE_INTEGER t;
    if (!f.QuadPart) QueryPerformanceFrequency(&f);
    QueryPerformanceCounter(&t);
    return (double)t.QuadPart / f.QuadPart;
}

void session_note_sim_ms(u32 ms)
{
    if (g_phase == 2) g_sim_ms += ms;      // battle time
    g_prof_steps += ms != 0;
    g_prof_step_ms += ms;
}

void session_note_phase(u32 phase)
{
    static const char *names[] = { "deployment", "initial orders", "battle", "battle over" };
    g_phase = phase;
    mp_log("phase: %s", phase < 4 ? names[phase] : "?");
}

const char *session_my_name(void) { return g_my_name; }
bool session_swap(void) { return g_swap; }

void session_toggle_swap(void)
{
    g_swap = !g_swap;
    SetEnvironmentVariableA("MP_SWAP", g_swap ? "1" : "0");
    mp_log("lobby: sides swapped - the host commands the %s force of the battle setup", g_swap ? "enemy" : "own");
}

// The other player, if one is connected. ready: pressed READY in the lobby.
bool session_peer(char *name, size_t size, bool *ready)
{
    if (!g_connected) return false;
    strcpy_s(name, size, g_peer_name[0] ? g_peer_name : "player");
    if (ready) *ready = g_peer_lobby_ready;
    return true;
}

// One line for the on-screen banner; empty when this game is not in multiplayer. Always the same shape:
//     MULTIPLAYER  -  <who is playing>  -  <what is going on>
const char *session_status(void)
{
    g_status[0] = 0;
    if (g_map_mode) {
        const char *host = g_peer_name[0] ? g_peer_name : "the host";
        char what[200];
        if (g_map_note[0]) sprintf_s(what, "%s", g_map_note);
        else if (g_map_battle_at) sprintf_s(what, "%s started a battle - entering the lobby", host);
        else if (g_map_reload_at || g_map_client.pending_epoch) sprintf_s(what, "host update arriving; local changes are not sent");
        else if (!g_connected) sprintf_s(what, "connecting to %s; local changes are not sent", host);
        else if (!g_map_hooks_ready) sprintf_s(what, "spectator: turn control unverified; do not advance locally");
        else sprintf_s(what, "spectator: %s controls the map; local changes are not sent", host);
        sprintf_s(g_status, "MULTIPLAYER  -  %s on the operational map  -  %s", g_my_name, what);
        return g_status;
    }
    if (g_role == ROLE_NONE || (g_role == ROLE_JOIN && !g_tactical)) return g_status;
    const char *peer = g_peer_name[0] ? g_peer_name : "the other player";
    char who[140], state[150];
    if (!g_tactical) {
        sprintf_s(who, "%s is hosting %s", g_my_name, g_host_mode == HOST_MODE_OPERATION ? "an operation" :
                  g_host_mode == HOST_MODE_MISSION ? "a custom mission" : "a skirmish");
    } else if (g_connected || g_peer_name[0]) {
        char mine[16], theirs[16];
        strcpy_s(mine, image::side_name(g_user_side));
        strcpy_s(theirs, image::side_name(g_enemy_side));
        if (g_versus || g_role == ROLE_HOST) sprintf_s(who, "%s (%s)  vs  %s (%s)", g_my_name, mine, peer, theirs);
        else sprintf_s(who, "%s watching %s (%s)", g_my_name, peer, mine);
    } else {
        sprintf_s(who, "%s (%s)", g_my_name, image::side_name(g_user_side));
    }

    const char *what = g_phase == 0 ? "deployment" : "the initial orders";
    bool mine = g_my_ready > g_phase, theirs = g_peer_ready > g_phase;
    if (g_role == ROLE_HOST && (g_bind_pending || g_sock == INVALID_SOCKET) &&
        !(g_connected && g_peer.sin_family == STEAM_FAMILY))
        sprintf_s(state, "UDP port %u is busy, retrying", g_port);
    else if (g_role == ROLE_JOIN && g_host_lost)
        sprintf_s(state, "%s disconnected; use the game's own controls", peer);
    else if (!g_connected && g_role == ROLE_HOST)
        sprintf_s(state, "waiting for a player at %s:%u", g_local_ip, g_port);
    else if (!g_connected)
        sprintf_s(state, "connecting to the host");
    else if (!g_tactical && g_map_note[0])
        sprintf_s(state, "%s", g_map_note);
    else if (!g_tactical && g_map_refresh_pending)
        sprintf_s(state, "your turn is yours; preparing the next spectator snapshot");
    else if (!g_tactical && g_map_given)
        sprintf_s(state, "host controls the map; %s follows snapshots", peer);
    else if (!g_tactical)
        sprintf_s(state, "%s has joined and is %s  (F9: lobby)", peer, g_peer_lobby_ready ? "ready" : "not ready yet");
    else if (g_end.pending)
        sprintf_s(state, "agreement noted: finish through the game's own battle controls");
    else if (g_end.deadline) {
        DWORD now = GetTickCount();
        DWORD remaining = ceasefire::State::due(now, g_end.deadline) ? 0 : g_end.deadline - now;
        sprintf_s(state, "ceasefire agreement in %u s - use the game to finish", (unsigned)((remaining + 999) / 1000));
    } else if (g_end.proposed)
        sprintf_s(state, "you have proposed a ceasefire - F9 takes it back");
    else if (g_end.peer_proposed)
        sprintf_s(state, "%s proposes a ceasefire - F9 accepts", peer);
    else if (g_end.peer_over)
        sprintf_s(state, "%s reached results - finish this battle in the game", peer);
    else if (g_phase < 2 && g_start_action && mine && !theirs)
        sprintf_s(state, "waiting for %s to finish %s", peer, what);
    else if (g_phase < 2 && g_start_action && theirs && !mine)
        sprintf_s(state, "%s has finished %s, press Start when ready", peer, what);
    else if (g_phase < 2 && g_start_action && !mine)
        sprintf_s(state, "%s ends when both players press Start", g_phase == 0 ? "deployment" : "initial orders");
    else if (g_phase < 2)
        sprintf_s(state, "connected, starting");
    else if (g_phase == 2 && g_waiting_since && GetTickCount() - g_waiting_since > 1500)
        sprintf_s(state, "waiting for %s's game to catch up", peer);
    else if (g_phase == 2 && g_role == ROLE_JOIN && g_clock_factor > 1.5f)
        sprintf_s(state, "catching up with %s's game", peer);
    else if (g_phase == 2)
        sprintf_s(state, "connected");
    else
        sprintf_s(state, "battle over");
    sprintf_s(g_status, "MULTIPLAYER  -  %s  -  %s", who, state);
    return g_status;
}

// One send path for both transports, exactly the seam v20 has: the address says where the bytes go,
// and a Steam peer (family 0x5354) is handed to Steam instead of to the socket.  Everything above
// this line - HELLO, the image transfer, the state and shot packets - is transport-agnostic.
static bool send_raw(const sockaddr_in &to, const void *data, int size)
{
    if (to.sin_family == STEAM_FAMILY) return steam_send(steam_peer_id(&to), data, size) != 0;
    return sendto(g_sock, (const char *)data, size, 0, (const sockaddr *)&to, sizeof to) == size;
}
static void send_to(const sockaddr_in &to, const void *data, int size)
{
#ifdef NETV21_TEST_HOOKS
    if (g_test_map_faults && size >= (int)(sizeof(ImageData) - k_chunk) && size <= (int)sizeof(ImageData)) {
        const ImageData *d = (const ImageData *)data;
        if (d->h.type == PKT_IMAGE_DATA && d->h.flags == 1) {
            u64 key = ((u64)d->map_revision << 32) | d->index;
            if (!(d->index & 1) && g_test_map_dropped_pieces.insert(key).second) { ++g_test_map_dropped; return; }
            if (d->map_revision > 1 && !g_test_map_stale) {
                ImageData stale = {};
                memcpy(&stale, data, (size_t)size);
                --stale.map_revision;
                if (send_raw(to, &stale, size)) ++g_test_map_stale;
            }
            if (send_raw(to, data, size)) ++g_test_map_duplicates;
        }
    }
#endif
    if (!send_raw(to, data, size)) return;
    g_stat_tx_bytes += size;
    // Every datagram out through the counter, whichever transport carries it: HELLO, the image pieces and
    // the state stream used to go round it, and then the two ends' totals could never be compared.
    netlog_packet("tx", (const unsigned char *)data, size, ntohl(to.sin_addr.s_addr), ntohs(to.sin_port),
                  to.sin_family);
}

static void send_peer(const void *data, int size)
{
    if (transport_policy::send_peer(g_tactical, g_sock != INVALID_SOCKET, g_have_peer, g_peer.sin_family == STEAM_FAMILY))
        send_to(g_peer, data, size);
}

// Full-state replication. MP_SYNC_STATE=0 switches it off (positions only), =1 forces it on.
// In operation battles it is off unless forced: there a unit's saved state refers to the formations of the
// game that saved it, and loading it into the other game crashes that game's unit update (seen in every
// operation battle tried; skirmishes and missions, with one group per side, are not affected).
static bool sync_state_enabled(void)
{
    static int forced = -2;
    if (forced == -2) {
        int value;
        forced = env_integer("MP_SYNC_STATE", &value) ? (value != 0) : -1;
    }
    // A tactical operation can be launched directly, without its parent's MP_HOST_MODE.
    // Even if some other code calls it a skirmish, avoid loading its incompatible full states.
    static const bool direct_operation = tactical_mode::operation_battle(GetCommandLineA());
    return forced >= 0 ? forced != 0 : tactical_mode::default_full_state(g_host_mode, g_tactical && direct_operation);
}

static bool unit_key(Object *o, u64 *key, UnitRec *rec)
{
    unsigned char kind = !strcmp(o->iface, "i_techn") ? 1 : !strcmp(o->iface, "i_human") ? 2 : 0;
    if (!kind) return false;
    const char *us = strrchr(o->full, '_');
    if (!us) return false;
    u32 id = strtoul(us + 1, 0, 16);
    *key = ((u64)kind << 32) | id;
    if (rec) { rec->id = id; rec->kind = kind; }
    return true;
}

// Some vehicle models do not survive having another game's saved state loaded into them (the game
// crashes in its renderer shortly afterwards): supply trucks, seen with opel_blitz_36 in operation battles.
// They are followed by position only. MP_STATE_SKIP lists the model-name fragments (comma separated).
static void find_position_only_units(void)
{
    char list[256] = "opel,zis5,zis_5,gaz_aa,gaz_mm,truck", configured[sizeof list];
    if (env_text("MP_STATE_SKIP", configured, sizeof configured)) strcpy_s(list, configured);
    g_position_only.clear();
    std::unordered_map<u64, std::string> models = image::unit_models(g_image);
    for (auto &kv : models) {
        char *ctx = 0, copy[256];
        strcpy_s(copy, list);
        for (char *tok = strtok_s(copy, ", ", &ctx); tok; tok = strtok_s(0, ", ", &ctx))
            if (kv.second.find(tok) != std::string::npos) { g_position_only.insert(kv.first); break; }
    }
    if (!g_position_only.empty()) mp_log("net: %zu units are followed by position only (%s)", g_position_only.size(), list);
}

bool session_state_wanted(Object *o)
{
    u64 key;
    if (g_position_only.empty() || strcmp(o->iface, "i_techn") || !o->full) return true;
    const char *us = strrchr(o->full, '_');
    if (!us) return true;
    key = (1ull << 32) | strtoul(us + 1, 0, 16);
    return g_position_only.count(key) == 0;
}

// Which game an object's state comes from.
static bool owns_object(Object *o)
{
    u64 key;
    if (!g_versus || !unit_key(o, &key, 0)) return g_role == ROLE_HOST;
    return g_my_units.count(key) != 0;
}

bool session_owns(Object *o) { return owns_object(o); }
u32 session_phase(void) { return g_phase; }

// A squad is its units: a unit's id is its squad's id plus its place in it (the low five bits).
bool session_owns_squad(u32 id)
{
    if (!g_versus) return g_role == ROLE_HOST;
    static std::unordered_set<u32> squads;
    static size_t built_from;
    if (built_from != g_my_units.size()) {
        squads.clear();
        for (u64 key : g_my_units) squads.insert((u32)key & 0x7fffffe0);
        built_from = g_my_units.size();
    }
    return squads.count(id & 0x7fffffe0) != 0;
}
bool session_unit_key(Object *o, u64 *key) { return unit_key(o, key, 0); }
u64 session_battle_ms(void) { return g_sim_ms; }

// ---------------------------------------------------------------- battle description
static bool read_file(const char *path, std::vector<unsigned char> &out)
{
    FILE *f = 0;
    if (fopen_s(&f, path, "rb") || !f) return false;
    fseek(f, 0, SEEK_END);
    long n = ftell(f);
    fseek(f, 0, SEEK_SET);
    out.resize(n > 0 ? n : 0);
    size_t got = n > 0 ? fread(out.data(), 1, n, f) : 0;
    fclose(f);
    return got == out.size() && n > 0;
}

// M4 changes a battle description only after a checked backup and marker exist. Both marker and
// image are written to a same-directory temporary and renamed over the target, never truncated in
// place. Keep the first Windows failure code even if cleanup also fails.
struct BattleFileContext { DWORD error; };
static bool battle_copy(void *context, const char *from, const char *to)
{
    if (CopyFileA(from, to, FALSE)) return true;
    BattleFileContext *c = (BattleFileContext *)context;
    if (!c->error) c->error = GetLastError();
    return false;
}
static bool battle_write(void *context, const char *to, const void *bytes, size_t size)
{
    if (atomic_file::replace(to, bytes, size)) return true;
    BattleFileContext *c = (BattleFileContext *)context;
    if (!c->error) c->error = GetLastError();
    return false;
}
static bool battle_remove(void *context, const char *path)
{
    if (DeleteFileA(path)) return true;
    BattleFileContext *c = (BattleFileContext *)context;
    if (!c->error) c->error = GetLastError();
    return false;
}
static bool battle_install(const char *path, const std::vector<unsigned char> &current,
                           const std::vector<unsigned char> &replacement)
{
    if (g_force_recovery_blocked) {
        mp_log("lobby: an earlier battle backup needs recovery; no new description will be installed");
        return false;
    }
    std::vector<unsigned char> on_disk;
    if (!read_file(path, on_disk) || on_disk != current) {
        mp_log("lobby: battle description changed on disk or cannot be read; it was not overwritten");
        return false;
    }
    BattleFileContext context = {};
    battle_file::Io io = { &context, battle_copy, battle_write, battle_remove };
    u32 hash = fnv(replacement.data(), replacement.size());
    battle_file::Result result = battle_file::install(io, path, replacement, hash);
    if (result != battle_file::Result::ok) {
        mp_log("lobby: battle description was not replaced: %s (Windows error %lu)", battle_file::reason(result), context.error);
        return false;
    }
    return true;
}
static bool battle_restore_backup(const char *path, std::vector<unsigned char> *restored = nullptr)
{
    const std::string backup = std::string(path) + ".before_mp";
    const std::string marker = std::string(path) + ".mp_combined";
    std::vector<unsigned char> old;
    if (!read_file(backup.c_str(), old)) {
        mp_log("lobby: recovery copy %s cannot be read", backup.c_str());
        return false;
    }
    if (!atomic_file::replace(path, old.data(), old.size())) {
        mp_log("lobby: cannot restore the battle description from %s (Windows error %lu)", backup.c_str(), GetLastError());
        return false;
    }
    if (!DeleteFileA(marker.c_str()) && GetLastError() != ERROR_FILE_NOT_FOUND)
        mp_log("lobby: restored the description but could not remove its marker (Windows error %lu)", GetLastError());
    if (restored) restored->swap(old);
    return true;
}

// A description that a two-player battle was started from carries the other player's force as well. The next
// battle must start from this player's own - otherwise the previous opponent's squads would be fought again -
// so what went in last time is remembered beside the file, and the copy taken before it is put back when the
// file has not been set up again since. (A new Skirmish setup by the player changes the file, and then the
// remembered hash no longer matches: nothing is restored.)
static bool force_restore_leftover(char *path, bool honor_applied = true)
{
    int value;
    // The battle that was just started again on purpose carries both forces: it is no leftover.
    if (honor_applied && env_integer("MP_FORCE_APPLIED", &value) && value) return true;
    const std::string sidecar = std::string(path) + ".mp_combined";
    const std::string backup = std::string(path) + ".before_mp";
    if (GetFileAttributesA(sidecar.c_str()) == INVALID_FILE_ATTRIBUTES) {
        DWORD error = GetLastError();
        if (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND) return true;
        mp_log("lobby: cannot inspect recovery marker %s (Windows error %lu)", sidecar.c_str(), error);
        return false;
    }
    FILE *f = 0;
    if (fopen_s(&f, sidecar.c_str(), "rb") || !f) {
        mp_log("lobby: cannot read recovery marker %s", sidecar.c_str());
        return false;
    }
    unsigned was = 0;
    size_t got = fread(&was, 1, sizeof was, f);
    fclose(f);
    std::vector<unsigned char> now;
    if (got != sizeof was || !read_file(path, now)) {
        mp_log("lobby: recovery marker or battle description could not be read; backup retained at %s", backup.c_str());
        return false;
    }
    if (fnv(now.data(), now.size()) != was) {
        // The player has set a battle up since: the remembered pair is stale. Do not replace the
        // backup while a failed marker deletion would make the old transaction ambiguous.
        if (!DeleteFileA(sidecar.c_str())) {
            mp_log("lobby: cannot remove stale recovery marker (Windows error %lu); backup retained", GetLastError());
            return false;
        }
        DeleteFileA(backup.c_str());
        return true;
    }
    if (!battle_restore_backup(path)) {
        mp_log("lobby: the description still carries the last battle's other force; the previous one could not be restored");
        return false;
    }
    mp_log("lobby: the battle description still carried the last two-player battle's other force - the previous one is back");
    return true;
}
#ifdef NETV21_TEST_HOOKS
bool session_test_force_recovery()
{
    char dir[MAX_PATH] = {}, path[MAX_PATH] = {};
    DWORD n = GetTempPathA(MAX_PATH, dir);
    if (!n || n >= MAX_PATH || !GetTempFileNameA(dir, "mfr", 0, path)) return false;
    const std::string backup = std::string(path) + ".before_mp", marker = std::string(path) + ".mp_combined";
    const unsigned char original[] = {1, 2, 3}, combined[] = {4, 5, 6};
    u32 hash = fnv(combined, sizeof combined);
    bool ok = atomic_file::replace(path, combined, sizeof combined) &&
              atomic_file::replace(backup.c_str(), original, sizeof original) &&
              atomic_file::replace(marker.c_str(), &hash, sizeof hash);
    HANDLE locked = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (locked == INVALID_HANDLE_VALUE) ok = false;
    else {
        ok = !force_restore_leftover(path, false) && ok;
        std::vector<unsigned char> saved;
        ok = read_file(backup.c_str(), saved) && saved == std::vector<unsigned char>(original, original + sizeof original) && ok;
        CloseHandle(locked);
    }
    ok = force_restore_leftover(path, false) && ok;
    std::vector<unsigned char> restored;
    ok = read_file(path, restored) && restored == std::vector<unsigned char>(original, original + sizeof original) &&
         GetFileAttributesA(marker.c_str()) == INVALID_FILE_ATTRIBUTES && ok;
    ok = atomic_file::replace(marker.c_str(), original, 1) && !force_restore_leftover(path, false) && ok;
    DeleteFileA(path); DeleteFileA(backup.c_str()); DeleteFileA(marker.c_str());
    return ok;
}
#endif

static void load_battle_description(void)
{
    // the description this battle was started from: second item on the command line
    char path[MAX_PATH], rel[MAX_PATH];
    strcpy_s(rel, k_image_path);
    if (const char *comma = strchr(GetCommandLineA(), ',')) {
        const char *p = comma + 1;
        while (*p == ' ') p++;
        size_t n = strcspn(p, ",");
        if (n && n < sizeof rel) { memcpy(rel, p, n); rel[n] = 0; }
    }
    sprintf_s(path, "%s\\%s", g_root, rel);
    strcpy_s(g_image_file, path);
    g_force_recovery_blocked = !force_restore_leftover(path);
    if (g_force_recovery_blocked) mp_log("lobby: force exchange and side swap are disabled until the previous backup is recovered");
    if (!read_file(path, g_image)) { mp_log("net: cannot read the battle description %s", path); return; }
    g_image_hash = fnv(g_image.data(), g_image.size());
    // The host serves its description as it is; a joining game mirrors the copy it was given, so the hash of
    // *the host's* file is the hash of this one turned back round. Comparing the wrong one made two games
    // that are playing the same battle look as if their descriptions differed.
    g_source_hash = g_image_hash;
    if (g_versus) {
        std::vector<unsigned char> back = g_image;
        if (image::mirror(back)) g_source_hash = fnv(back.data(), back.size());
    }
    image::sides(g_image, &g_user_side, &g_enemy_side);
    // In this game's own description the squads tagged USER are the ones its player commands.
    g_my_units = image::units_of(g_image, image::k_user);
    find_position_only_units();
    mp_log("net: battle description %zu bytes (hash %08x), this player commands %s (%zu units)", g_image.size(), g_image_hash,
           image::side_name(g_user_side), g_my_units.size());
    force_publish();
}

// ---------------------------------------------------------------- versus: no computer commander
// In a versus game both forces have a human commander. The engine's AI commander (i_aicolonel) would keep
// giving orders to the "enemy" force of each game, fighting the other player's orders, so its simulation
// step is switched off in this process: the first instruction of its handler becomes a return.
// A host keeps it off from the start of its battle, so the opponent has no computer-made plan when the other
// player takes command; it comes back if the battle begins without a second player, or with one on the host's side.
static void update_ai_commander(void)
{
    static const unsigned char sig[] = { 0x40, 0x57, 0x48, 0x83, 0xec, 0x30, 0x48, 0x8b, 0xf9 };
    static int state = -1;          // -1 unknown, 0 active, 1 off, 2 unknown game build
    static unsigned char *fn;
    if (!g_tactical || g_role == ROLE_NONE || state == 2) return;
    bool off = g_versus || (g_role == ROLE_HOST && !g_connected && g_phase < 2);
    if (state == (int)off) return;
    if (!fn) {
        if (!GetModuleHandleA("adv_game.codelib")) return;
        const HookSig where[] = { { 0xbec30, sig, sizeof sig } };
        fn = (unsigned char *)hook_resolve_checked("adv_game.codelib", where, 1, "AI commander");
    }
    if (!fn || !readable(fn, sizeof sig) || (*fn != 0x40 && *fn != 0xc3) || memcmp(fn + 1, sig + 1, sizeof sig - 1)) {
        state = 2;
        mp_log("net: AI commander bytes do not match, its code is left unchanged");
        return;
    }
    DWORD old;
    if (!VirtualProtect(fn, 1, PAGE_EXECUTE_READWRITE, &old)) return;
    *fn = off ? 0xc3 : 0x40;
    VirtualProtect(fn, 1, old, &old);
    FlushInstructionCache(GetCurrentProcess(), fn, 1);
    state = off;
    mp_log("net: AI commander switched %s", off ? "off" : "on");
}

// Tactical and non-tactical receivers are deliberately separate: M5 keeps the pre-hardening
// state implementation, even when a peer sends it a state packet outside a battle.
static void reset_state_for_mode()
{
    if (g_tactical) { statesync_reset(); statesync_forget_peer(); }
    else statesync_legacy_reset();
}
static void state_packet_for_mode(const unsigned char *data, int size, void (*send)(const void *, int), bool versus)
{
    if (g_tactical) statesync_on_packet(data, size, send, versus);
    else statesync_legacy_on_packet(data, size, send, versus);
}
static void shot_packet_for_mode(const unsigned char *data, int size, void (*send)(const void *, int))
{
    if (g_tactical) shots_on_packet(data, size, send);
    else shots_on_packet_legacy(data, size, send);
}
static void queue_transforms_for_mode(const UnitRec *records, unsigned count)
{
    if (!g_tactical) {
        g_pending_legacy.insert(g_pending_legacy.end(), records, records + count); // original non-tactical path
        return;
    }
    for (unsigned i = 0; i < count; i++) {
        transforms::Put result = g_pending.put(records[i]);
        if (result == transforms::Put::replaced) g_stat_coalesced++;
        else if (result == transforms::Put::invalid || result == transforms::Put::full) g_stat_bad_transforms++;
    }
}
#ifdef NETV21_TEST_HOOKS
void session_test_state_dispatch(bool tactical)
{
    bool previous = g_tactical;
    g_tactical = tactical;
    reset_state_for_mode();
    state_packet_for_mode(nullptr, 0, nullptr, false); // the test substitutes both receivers
    g_tactical = previous;
}
void session_test_shot_dispatch(bool tactical)
{
    bool previous = g_tactical;
    g_tactical = tactical;
    shot_packet_for_mode(nullptr, 0, nullptr); // the test substitutes both receivers
    g_tactical = previous;
}
bool session_test_transform_dispatch()
{
    bool previous = g_tactical;
    u32 previous_coalesced = g_stat_coalesced, previous_bad = g_stat_bad_transforms;
    UnitRec records[3] = {};
    records[0].kind = records[1].kind = 1;
    records[0].id = records[1].id = 42; // same unit twice, third record is invalid
    g_tactical = false;
    g_pending_legacy.clear();
    queue_transforms_for_mode(records, 3);
    bool legacy_ok = g_pending_legacy.size() == 3;
    g_tactical = true;
    g_pending.clear();
    queue_transforms_for_mode(records, 3);
    bool tactical_ok = g_pending.records().size() == 1;
    g_pending.clear(); g_pending_legacy.clear();
    g_stat_coalesced = previous_coalesced; g_stat_bad_transforms = previous_bad;
    g_tactical = previous;
    return legacy_ok && tactical_ok;
}
#endif

static void set_versus(bool versus)
{
    if (versus == g_versus) return;
    g_versus = versus;
    mp_log("net: %s", versus ? "versus: each game owns its own player's units" : "same side: the host owns every unit");
    g_sent.clear();
    reset_state_for_mode();
}

// ---------------------------------------------------------------- addresses, UPnP (host)
static void find_local_ip(void)
{
    // the address the default route uses; connect() on a UDP socket sends nothing
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
    SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    sockaddr_in probe = {};
    probe.sin_family = AF_INET;
    probe.sin_port = htons(53);
    inet_pton(AF_INET, "8.8.8.8", &probe.sin_addr);
    if (connect(s, (sockaddr *)&probe, sizeof probe) == 0) {
        sockaddr_in me;
        int n = sizeof me;
        if (getsockname(s, (sockaddr *)&me, &n) == 0) inet_ntop(AF_INET, &me.sin_addr, g_local_ip, sizeof g_local_ip);
    }
    closesocket(s);
}

static DWORD WINAPI upnp_thread(LPVOID)
{
    if (FAILED(CoInitializeEx(0, COINIT_MULTITHREADED))) return 0;
    IUPnPNAT *nat = 0;
    IStaticPortMappingCollection *maps = 0;
    HRESULT hr = CoCreateInstance(__uuidof(UPnPNAT), 0, CLSCTX_ALL, __uuidof(IUPnPNAT), (void **)&nat);
    if (SUCCEEDED(hr)) hr = nat->get_StaticPortMappingCollection(&maps);
    if (FAILED(hr) || !maps) {
        mp_log("net: UPnP not available on this network (0x%08lx); internet play needs UDP %u forwarded by hand", hr, g_port);
    } else {
        wchar_t wip[64];
        MultiByteToWideChar(CP_ACP, 0, g_local_ip, -1, wip, 64);
        BSTR proto = SysAllocString(L"UDP"), client = SysAllocString(wip), desc = SysAllocString(L"Graviteam Tactics multiplayer");
        IStaticPortMapping *m = 0;
        hr = maps->Add(g_port, proto, g_port, client, VARIANT_TRUE, desc, &m);
        if (SUCCEEDED(hr)) {
            BSTR ext = 0;
            if (m && SUCCEEDED(m->get_ExternalIPAddress(&ext)) && ext) {
                WideCharToMultiByte(CP_ACP, 0, ext, -1, g_external_ip, sizeof g_external_ip, 0, 0);
                SysFreeString(ext);
            }
            mp_log("net: UPnP mapped UDP %u (external address %s)", g_port, g_external_ip[0] ? g_external_ip : "unknown");
        } else {
            mp_log("net: UPnP mapping refused (0x%08lx); relying on a manual port forward of UDP %u", hr, g_port);
        }
        if (m) m->Release();
        SysFreeString(proto); SysFreeString(client); SysFreeString(desc);
        maps->Release();
    }
    if (nat) nat->Release();
    CoUninitialize();
    return 0;
}

// ---------------------------------------------------------------- start / stop
#ifdef NETV21_TEST_HOOKS
static bool g_test_no_udp, g_test_no_upnp;
void session_test_disable_udp(bool value) { g_test_no_udp = value; }
void session_test_disable_upnp(bool value) { g_test_no_upnp = value; }
void session_test_lobby_image()
{
    g_image.resize(k_chunk * 2 + 117);
    for (size_t i = 0; i < g_image.size(); ++i) g_image[i] = (unsigned char)((i * 37u + 11u) & 255u);
    g_image_hash = fnv(g_image.data(), g_image.size());
}
#endif
static bool open_socket(unsigned short port)
{
#ifdef NETV21_TEST_HOOKS
    if (g_test_no_udp) { g_sock = INVALID_SOCKET; return false; } // test both tactical and menu Steam without IP
#endif
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
    g_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    u_long nonblocking = 1;
    ioctlsocket(g_sock, FIONBIO, &nonblocking);
    DWORD off = 0, ret = 0;
    WSAIoctl(g_sock, _WSAIOW(IOC_VENDOR, 12) /* SIO_UDP_CONNRESET */, &off, sizeof off, 0, 0, &ret, 0, 0);
    int buf = 1 << 21;
    setsockopt(g_sock, SOL_SOCKET, SO_RCVBUF, (char *)&buf, sizeof buf);
    setsockopt(g_sock, SOL_SOCKET, SO_SNDBUF, (char *)&buf, sizeof buf);
    sockaddr_in me = {};
    me.sin_family = AF_INET;
    me.sin_addr.s_addr = INADDR_ANY;
    me.sin_port = htons(port);
    if (bind(g_sock, (sockaddr *)&me, sizeof me)) {
        closesocket(g_sock);
        g_sock = INVALID_SOCKET;
        return false;
    }
    return true;
}

static void host_listen(void)
{
    // The operational program hands the port over to the tactical one; for a moment both may exist.
    g_last_bind_try = GetTickCount();
    if (!open_socket(g_port)) {
        if (!g_bind_pending) mp_log("net: UDP port %u is still in use, retrying", g_port);
        g_bind_pending = true;
        return;
    }
    g_bind_pending = false;
    mp_log("net: hosting %son %s UDP %u", g_tactical ? "battle " : "", g_local_ip, g_port);
#ifdef NETV21_TEST_HOOKS
    if (g_test_no_upnp) return; // no real router changes in the two-process loopback test
#endif
    if (!g_upnp_started) {
        g_upnp_started = true;
        CloseHandle(CreateThread(0, 0, upnp_thread, 0, 0, 0));
    }
}

// Become the host now (multiplayer menu). mode: HOST_MODE_SKIRMISH or HOST_MODE_OPERATION.
void session_host(int mode)
{
    session_stop();
    g_role = ROLE_HOST;
    g_host_mode = mode;
    g_watch_host_worker.store(!g_tactical && mode == HOST_MODE_OPERATION, std::memory_order_release);
    g_map_given = g_map_refresh_pending = g_peer_on_map = false;
    g_map_out.clear(); g_host_turn_events.store(0);
    g_map_epoch = (GetTickCount() & 0x00ffffffu) | 1u;
    g_map_revision = 1;
    SetEnvironmentVariableA("MP_MAP_ACK", nullptr); SetEnvironmentVariableA("MP_MAP_ACK_EPOCH", nullptr);
    SetEnvironmentVariableA("MP_MAP_ACK_REV", nullptr);
    char map_identity[16];
    sprintf_s(map_identity, "%u", g_map_epoch); SetEnvironmentVariableA("MP_MAP_EPOCH", map_identity);
    sprintf_s(map_identity, "%u", g_map_revision); SetEnvironmentVariableA("MP_MAP_REV", map_identity);
    SetEnvironmentVariableA("MP_ROLE", "host");
    SetEnvironmentVariableA("MP_HOST_MODE", mode == HOST_MODE_OPERATION ? "operation" : mode == HOST_MODE_MISSION ? "mission" : "skirmish");
    g_swap = false;
    SetEnvironmentVariableA("MP_SWAP", "0");
    SetEnvironmentVariableA("MP_PEER_NAME", 0);
    g_peer_name[0] = 0;
    find_local_ip();
    host_listen();
    // The mode is not an operation slot id, so advertise slot 0 rather than a fabricated id.
    // Steam Join Game is preferred when available; the UDP listener remains the IP fallback.
    if (steam_ready()) steam_broadcast(true, 0);
}

// Back to single player (multiplayer menu).
void session_single_player(void)
{
    if (g_role == ROLE_HOST && steam_ready()) steam_broadcast(false, 0);
    session_stop();
    g_role = ROLE_NONE;
    SetEnvironmentVariableA("MP_ROLE", 0);
    SetEnvironmentVariableA("MP_PEER", 0);
    SetEnvironmentVariableA("MP_VERSUS", 0);
    SetEnvironmentVariableA("MP_PREFER", 0);
}

// ---------------------------------------------------------------- lobby decisions carried into the battle
static void read_profile_name(void)
{
    char path[MAX_PATH], text[512] = "";
    sprintf_s(path, "%s\\users\\profiles\\cur_profile.engcfg", g_root);
    FILE *f = 0;
    if (fopen_s(&f, path, "rb") || !f) return;
    text[fread(text, 1, sizeof text - 1, f)] = 0;
    fclose(f);
    const char *p = strstr(text, "cur_profile");
    if (p && (p = strchr(p, '='))) {
        for (p++; *p == ' ' || *p == '\t'; p++) {}
        size_t k = 0;
        while (k < sizeof g_my_name - 1 && (isalnum((unsigned char)p[k]) || p[k] == '_' || p[k] == '-')) k++;
        if (k) { memcpy(g_my_name, p, k); g_my_name[k] = 0; }
    }
    p = strstr(text, "cur_id");
    if (p && (p = strchr(p, '='))) {
        for (p++; *p == ' ' || *p == '\t'; p++) {}
        size_t k = strcspn(p, ";\r\n");
        if (k && k < sizeof g_profile_id) { memcpy(g_profile_id, p, k); g_profile_id[k] = 0; }
    }
}

// Skirmish or mission, sides swapped in the lobby: the host plays the force its battle setup calls the enemy. The
// battle description is turned round on disk and this battle is started again from it; everything else
// (what the joining game downloads, who owns which unit) then follows from the description as usual.
static void host_swap_sides_if_asked(void)
{
    char v[8];
    if (g_host_mode == HOST_MODE_OPERATION || !g_swap || g_image.empty() || g_force_recovery_blocked) return; // skirmishes and missions
    if (GetEnvironmentVariableA("MP_SWAPPED", v, sizeof v)) return;         // this is the restarted battle
    std::vector<unsigned char> turned = g_image;
    if (!image::mirror(turned)) { mp_log("lobby: this battle cannot be swapped, playing it as set up"); return; }
    // Never truncate the battle which the running process loaded. Save a separate backup before
    // replacing it, and refuse to overwrite an unresolved backup from a previous failed restart.
    std::vector<unsigned char> disk;
    if (!read_file(g_image_file, disk) || disk != g_image) {
        mp_log("lobby: battle description changed on disk; sides are not swapped");
        return;
    }
    const std::string backup = std::string(g_image_file) + ".before_swap";
    if (!CopyFileA(g_image_file, backup.c_str(), TRUE)) {
        mp_log("lobby: cannot save %s (%lu); sides are not swapped", backup.c_str(), GetLastError());
        return;
    }
    if (!atomic_file::replace(g_image_file, turned.data(), turned.size())) {
        if (read_file(g_image_file, disk) && disk == g_image) DeleteFileA(backup.c_str());
        mp_log("lobby: cannot replace the battle description, playing it as set up");
        return;
    }
    SetEnvironmentVariableA("MP_SWAPPED", "1");
    std::string cmd = GetCommandLineA();
    STARTUPINFOA si = { sizeof si };
    PROCESS_INFORMATION pi;
    if (CreateProcessA(0, &cmd[0], 0, 0, FALSE, 0, 0, g_root, &si, &pi)) {
        DeleteFileA(backup.c_str());
        mp_log("lobby: sides swapped, restarting the battle from the turned description");
        ExitProcess(0);
    }
    DWORD error = GetLastError();
    if (atomic_file::replace(g_image_file, g_image.data(), g_image.size())) DeleteFileA(backup.c_str());
    else mp_log("lobby: could not restore the original battle description; backup retained at %s", backup.c_str());
    mp_log("lobby: could not restart the battle (%lu), playing it as set up", error);
    SetEnvironmentVariableA("MP_SWAPPED", 0);
}

// Operation: the engine saves the operation at the moment it starts a battle. That save (every operation
// save of the profile, with the terrain damage files) is copied to users\mp_saves\<host>_vs_<player>, so
// the host keeps the state "after the move, before the battle" whatever happens to either game afterwards.
static int copy_matching(const char *from, const char *to, const char *pattern)
{
    char find[MAX_PATH], src[MAX_PATH], dst[MAX_PATH];
    WIN32_FIND_DATAA fd;
    int n = 0;
    sprintf_s(find, "%s\\%s", from, pattern);
    HANDLE h = FindFirstFileA(find, &fd);
    if (h == INVALID_HANDLE_VALUE) return 0;
    do {
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
        sprintf_s(src, "%s\\%s", from, fd.cFileName);
        sprintf_s(dst, "%s\\%s", to, fd.cFileName);
        if (CopyFileA(src, dst, FALSE)) n++;
    } while (FindNextFileA(h, &fd));
    FindClose(h);
    return n;
}

static void snapshot_operation_saves(void)
{
    if (g_host_mode != HOST_MODE_OPERATION) return;
    char id[64] = "cloud", peer[24] = "nobody", from[MAX_PATH], to[MAX_PATH];
    const char *p = GetCommandLineA();
    for (int item = 0; item < 3 && p; item++) p = strchr(p, ',') ? strchr(p, ',') + 1 : 0;      // 4th item: profile id
    if (p) {
        while (*p == ' ') p++;
        size_t n = strcspn(p, ",");
        if (n && n < sizeof id) { memcpy(id, p, n); id[n] = 0; }
    }
    char configured[sizeof peer];
    if (env_text("MP_PEER_NAME", configured, sizeof configured)) strcpy_s(peer, configured);
    sprintf_s(from, "%s\\users\\profiles\\%s", g_root, id);
    sprintf_s(to, "%s\\users\\mp_saves", g_root);
    CreateDirectoryA(to, 0);
    sprintf_s(to, "%s\\users\\mp_saves\\%s_vs_%s", g_root, g_my_name, peer);
    CreateDirectoryA(to, 0);
    int n = copy_matching(from, to, "*.opsave") + copy_matching(from, to, "*.opsave_") + copy_matching(from, to, "*.damsav2");
    mp_log("lobby: operation saved before the battle: %d files in users\\mp_saves\\%s_vs_%s", n, g_my_name, peer);
}

// ---------------------------------------------------------------- host-authoritative operation snapshots
// The host acts normally and publishes its checked, unmodified saves. The guest only receives and
// loads them; it never uploads map state. Local navigation remains available and the verified
// next-turn control is intercepted, but other engine input paths still require real-game testing.
static void *menu_controller_of(ObjMan *om)
{
    Object *og = engine_find(om, "i_opgame", "opgame");
    if (!og) return 0;
    void *c = *(void **)((char *)og + 0x100);
    return readable(c, 0x1400) ? c : 0;
}

// The operation object while its map is on screen: the controller is in operation mode (+0x1200 == 0; it
// is 4 in the menus) and holds the operation at +0x1d8. The screen number (+0x1258) is 0 when the program
// was started straight on the operation (as after a battle) and stays 2, the Operations screen's, when
// the player opened the operation from that screen.
static void *operation_on_screen(ObjMan *om)
{
    if (g_tactical) return 0;
    char *c = (char *)menu_controller_of(om);
    if (!c || (*(int *)(c + 0x1258) != 0 && *(int *)(c + 0x1258) != 2) || *(int *)(c + 0x1200) != 0) return 0;
    void *op = *(void **)(c + 0x1d8);
    return readable(op, 0x100) ? op : 0;
}

static bool map_ready_for_hello(ObjMan *om)
{
#ifdef NETV21_TEST_HOOKS
    if (g_test_map_no_game) return g_test_map_on_screen; // the no-game peer simulates screen transitions
#endif
    return operation_on_screen(om) != nullptr;
}
bool session_on_op_map(ObjMan *om) { return operation_on_screen(om) != 0; }
bool session_map_mode(void) { return g_map_mode; }
// Do not globally swallow mouse/keyboard: that also prevented Esc and the stock exit menu.
// Local spectator actions never go to the host; the next-turn control is independently gated.
bool session_map_blocking(void) { return false; }

static void profile_path(char *out, size_t size, const char *file)
{
    sprintf_s(out, size, "%s\\users\\profiles\\%s\\%s", g_root, g_profile_id, file);
}

// Has the game write the operation that is on screen and returns the file's contents and name.
static bool save_operation(ObjMan *om, std::vector<unsigned char> &out, char *file, size_t file_size)
{
    static const unsigned char sig[] = { 0x48, 0x89, 0x5c, 0x24, 0x08, 0x48, 0x89, 0x7c, 0x24, 0x10, 0x55, 0x48, 0x8d, 0xac, 0x24, 0x10, 0xff, 0xff, 0xff };
    static const HookSig where[] = { { 0xd6080, sig, sizeof sig } };
    static void *save = (void *)1;              // 1: not looked for yet, 0: not in this build
    void *op = operation_on_screen(om);
    HMODULE m = GetModuleHandleA("adv_opgame.codelib");
    if (!op || !m || !opsave_checked::safe_profile_id(g_profile_id)) return false;
    if (save == (void *)1) save = hook_resolve("adv_opgame.codelib", where, 1, "saving the operation");
    if (!save) { mp_log("map: unknown game build, the operation cannot be saved from here"); return false; }
    FILETIME before;
    GetSystemTimeAsFileTime(&before);
    ((void (*)(void *))save)(op);
    // the save it has just written: the newest operation save of the profile
    char find[MAX_PATH], path[MAX_PATH];
    WIN32_FIND_DATAA fd, best = {};
    profile_path(find, sizeof find, "*.opsave");
    HANDLE h = FindFirstFileA(find, &fd);
    if (h == INVALID_HANDLE_VALUE) return false;
    do {
        if (CompareFileTime(&fd.ftLastWriteTime, &best.ftLastWriteTime) > 0) best = fd;
    } while (FindNextFileA(h, &fd));
    FindClose(h);
    ULARGE_INTEGER a, b;
    a.LowPart = best.ftLastWriteTime.dwLowDateTime; a.HighPart = best.ftLastWriteTime.dwHighDateTime;
    b.LowPart = before.dwLowDateTime; b.HighPart = before.dwHighDateTime;
    if (!best.cFileName[0] || a.QuadPart + 20000000ull < b.QuadPart) { mp_log("map: the game did not write the operation"); return false; }
    if (!opsave::safe_file_name(best.cFileName)) return false;
    profile_path(path, sizeof path, best.cFileName);
    std::string why;
    if (!map_file_windows::read(nullptr, path, &out) || !opsave_checked::validate(out, &why) ||
        opsave::operation_id(best.cFileName, opsave::operation_name(out)).empty()) {
        mp_log("map: the saved operation failed validation: %s", why.c_str());
        return false;
    }
    strcpy_s(file, file_size, best.cFileName);
    return true;
}

// Publish the exact host save; never mirror ownership or merge a guest save. A refreshed
// identity is made visible only after the complete checked snapshot exists.
static void map_moved_on(void);
static bool adopt_host_snapshot(std::vector<unsigned char> &save, const char *file, bool refresh)
{
    if (!operation_turn::valid_epoch(g_map_epoch) || !g_map_revision || !opsave_checked::validate(save)) return false;
    if (refresh) map_moved_on();
    g_map_turns = opsave::turns(save);
    strcpy_s(g_map_file, file);
    g_map_out.swap(save);
    g_map_out_hash = fnv(g_map_out.data(), g_map_out.size());
    g_map_given = true; g_map_refresh_pending = false;
    g_map_note[0] = 0;
    mp_log("map: host snapshot %u/%u offered (%s, %zu bytes, %u frames, phase %d); no guest orders",
           g_map_epoch, g_map_revision, g_map_file, g_map_out.size(), g_map_turns, opsave::turn_phase(g_map_out));
    return true;
}

static bool publish_host_map(ObjMan *om)
{
    if (!g_connected || !operation_turn::valid_epoch(g_map_epoch) || !g_map_revision) return false;
    std::vector<unsigned char> save;
    char file[96];
    if (!save_operation(om, save, file, sizeof file)) { strcpy_s(g_map_note, "host save unavailable; your turn is not blocked"); return false; }
    // Phase 0 requires a newer frame; phase 2 means battles pending. Both are only
    // serialized evidence, not proof of the real game-thread lifecycle.
    if (!spectator_snapshot::publishable(save, g_map_refresh_pending, g_map_turns, g_map_out)) {
        mp_log("map: save not yet a changed stable spectator snapshot (phase %d); will retry", opsave::turn_phase(save));
        return false;
    }
    return adopt_host_snapshot(save, file, g_map_given || g_map_refresh_pending);
}

static bool start_program(const char *arguments)
{
    char cmd[1024];
    if (arguments && *arguments) sprintf_s(cmd, "\"%s\\starter_x64.exe\" %s", g_root, arguments);
    else sprintf_s(cmd, "\"%s\\starter_x64.exe\"", g_root);
    STARTUPINFOA si = { sizeof si };
    PROCESS_INFORMATION pi;
    if (!CreateProcessA(0, cmd, 0, 0, FALSE, 0, 0, g_root, &si, &pi)) {
        mp_log("map: could not start '%s' (%lu)", cmd, GetLastError());
        return false;
    }
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return true;
}

// Return through the in-game lobby, which owns the real game's Steam context and uses this
// same wire protocol. Do not invoke an old standalone mp_join.exe during a map-to-battle transition.
static void joiner_back_to_lobby(void)
{
    char peer[128] = "";
    GetEnvironmentVariableA("MP_PEER", peer, sizeof peer);
    if (!peer[0]) { strcpy_s(g_map_note, "host address missing; cannot enter battle"); return; }
    map_file_windows::Environment env;
    if (!env.set("MP_AUTOJOIN", peer) || !env.set("MP_ROLE", nullptr) || !env.set("MP_MAP", nullptr) ||
        !env.set("MP_MAP_EPOCH", nullptr) || !env.set("MP_MAP_REV", nullptr)) {
        mp_log("map: could not prepare lobby environment (%lu); keeping the current map", GetLastError());
        strcpy_s(g_map_note, "cannot prepare the multiplayer lobby; retrying");
        g_map_battle_at = GetTickCount() + 10000;
        return;
    }
    if (!start_program(nullptr)) {
        strcpy_s(g_map_note, "cannot open the multiplayer lobby; retrying");
        g_map_battle_at = GetTickCount() + 10000;
        return;
    }
    if (!env.restore()) mp_log("map: could not fully restore the parent process environment");
    mp_log("map: returning through the in-game lobby of %s", peer);
    session_stop();
    ExitProcess(0);
}

// Replace and launch transactionally. Keep this process/connection alive on failure so no map or
// pending orders are silently discarded; the backup and recovery marker explain a failed rollback.
static void joiner_reload_map(void)
{
    Joiner j;
    GetEnvironmentVariableA("MP_PEER", j.address, sizeof j.address);
    j.image = g_map_in;
    j.image_hash = g_map_in_hash;
    strcpy_s(j.map_file, g_map_in_file);
    j.map_turns = g_map_in_turns;
    j.map_epoch = g_map_in_epoch;
    j.map_revision = g_map_in_revision;
    mp_log("map: snapshot %u/%u arrived (%zu bytes); reloading safely", j.map_epoch, j.map_revision, g_map_in.size());
    if (j.launch_map(g_root)) {
        mp_log("map: new map started; retained previous save as %s", j.map_backup.c_str());
        session_stop();
        ExitProcess(0);
    }
    mp_log("map: reload failed: %s; current game and any recovery files retained", j.error);
    strcpy_s(g_map_note, "cannot reload updated map; retrying, see log");
    g_map_reload_at = GetTickCount() + 10000;
}

static void joiner_into_battle(void)
{
    mp_log("map: host started a battle; entering via the in-game lobby");
    joiner_back_to_lobby();
}

// The current build's actual next-turn control is +0x369e0, not the older +0x36b40.
// It alone is gated on the spectator; the host always calls stock code. In particular, do not
// suppress the general worker on the guest: it has many unrelated callers, including UI transitions.

static void map_moved_on(void)
{
    char v[16];
    g_map_epoch = (g_map_epoch + 1) & operation_turn::epoch_mask;
    if (!g_map_epoch) g_map_epoch = 1;
    g_map_revision = operation_turn::next_revision(g_map_revision);
    sprintf_s(v, "%u", g_map_epoch); SetEnvironmentVariableA("MP_MAP_EPOCH", v);
    sprintf_s(v, "%u", g_map_revision); SetEnvironmentVariableA("MP_MAP_REV", v);
}

static void turn_worker_hook(void *ctrl, char a, void *b, void *c)
{
    TurnWorker original = g_turn_worker.load(std::memory_order_acquire);
    if (!original) return;
    original(ctrl, a, b, c); // always stock, even if peer is disconnected
    if (g_watch_host_worker.load(std::memory_order_acquire) && !a && !b && !c)
        g_host_turn_events.fetch_add(1, std::memory_order_relaxed);
}
static void next_turn_hook(void *ctrl, u64 b, const char *event)
{
    if (g_block_guest_turn.load(std::memory_order_acquire)) {
        if (readable(event, 0x10) && *(const float *)(event + 8) < 0)
            mp_log("map: spectator cannot advance a local turn; host controls the operation");
        return;
    }
    NextTurn original = g_next_turn.load(std::memory_order_acquire);
    if (original) original(ctrl, b, event);
}
static void install_map_detours(void)
{
    // Whole position-independent instructions; full signatures disambiguate similar prologues.
    static const unsigned char next_sig[] = { 0x40,0x53,0x48,0x83,0xec,0x20,0x0f,0x57,0xc0,0x48,0x8b,0xd9,0x41,0x0f,0x2f,0x40,0x08,0x0f,0x86,0x99,0x00,0x00,0x00 };
    static const unsigned char worker_sig[] = { 0x48,0x8b,0xc4,0x44,0x88,0x40,0x18,0x88,0x50,0x10,0x55,0x56,0x57,0x41,0x56 };
    static const HookSig next_where[] = { { 0x369e0, next_sig, sizeof next_sig } };
    static const HookSig worker_where[] = { { 0x76510, worker_sig, sizeof worker_sig } };
    static bool next_done, worker_done;
    bool host = g_role == ROLE_HOST && !g_tactical && g_host_mode == HOST_MODE_OPERATION;
    if ((!g_map_mode && !host) || !GetModuleHandleA("adv_opgame.codelib")) return;
    if (g_map_mode) {
        if (next_done) { g_map_hooks_ready = g_next_turn.load() != nullptr; return; }
        next_done = true;
        char *next = hook_resolve_checked("adv_opgame.codelib", next_where, 1, "spectator next turn control");
        g_map_hooks_ready = next && install_published_detour(next, 17, next_turn_hook, g_next_turn);
        mp_log("map: spectator next-turn guard %s; local changes never sent", g_map_hooks_ready ? "installed" : "unavailable: do not advance locally");
    } else {
        if (worker_done) { g_map_hooks_ready = g_turn_worker.load() != nullptr; return; }
        worker_done = true;
        char *worker = hook_resolve_checked("adv_opgame.codelib", worker_where, 1, "operation update worker");
        g_map_hooks_ready = worker && install_published_detour(worker, sizeof worker_sig, turn_worker_hook, g_turn_worker);
        mp_log("map: host worker observation %s; host is never blocked", g_map_hooks_ready ? "installed" : "unavailable; automatic map refresh unverified");
    }
}

// Test commands are non-invasive except the explicit host test, which uses the same verified worker.
static bool g_end_turn_asked, g_test_save_asked;
void session_test_next_turn(void) { g_end_turn_asked = true; }
void session_test_save(void) { g_test_save_asked = true; }
void session_test_new_map(void)
{
    if (g_role == ROLE_HOST && !g_tactical) {
        g_host_turn_events.fetch_add(1, std::memory_order_relaxed);
        mp_log("test: operation update requested");
    }
}
static void host_map_frame(ObjMan *om, DWORD now)
{
    if (g_role != ROLE_HOST || g_tactical || g_host_mode != HOST_MODE_OPERATION) return;
    if (g_host_turn_events.exchange(0, std::memory_order_relaxed)) {
        g_map_refresh_pending = true;
        g_share_after = now + 12000;
        mp_log("map: host operation worker ran; waiting for a stable post-turn save");
    }
    if (!operation_on_screen(om)) { g_map_seen_since = 0; return; }
    if (!g_map_seen_since) g_map_seen_since = now;
    bool settled = now - g_map_seen_since > 4000;
    if (g_test_save_asked) {
        g_test_save_asked = false;
        std::vector<unsigned char> save; char file[96] = "";
        bool ok = save_operation(om, save, file, sizeof file);
        mp_log("test: operation %s (%s, %zu bytes, turn phase %d, %u orders)", ok ? "saved" : "not saved", file, save.size(),
               ok ? opsave::turn_phase(save) : -1, ok ? opsave::rows_of(save, "user_orders") : 0);
    }
    if (g_end_turn_asked) {
        g_end_turn_asked = false;
        void *c = menu_controller_of(om);
        if (c && g_turn_worker) turn_worker_hook(c, 0, 0, 0);
        else mp_log("test: turn worker unavailable");
    }
    if (settled && g_connected && g_peer_lobby_ready && (!g_map_given || g_map_refresh_pending) &&
        (!g_share_after || (LONG)(now - g_share_after) >= 0)) {
        if (!publish_host_map(om)) g_share_after = now + 15000;
        else g_share_after = 0;
    }
}

// asks the other game for the pieces of the map that are still missing
static void request_map_pieces(DWORD now)
{
    if (g_map_in.empty() || g_map_got == g_map_have.size()) return;
    // 24 pieces per 60 ms at most: the frame runs a hundred times a second and a request per frame is
    // 2400 datagrams a second with nothing to show for it.
    static DWORD last;
    if (now - last < 60) return;
    last = now;
    unsigned sent = 0;
    for (unsigned i = 0; i < g_map_have.size() && sent < 24; i++) {
        if (g_map_have[i] || now - g_map_asked[i] < 300) continue;
        ImageRequest r = { make_head(PKT_IMAGE_REQUEST), i };
        r.h.flags = 1;
        r.map_epoch = g_map_in_epoch; r.map_revision = g_map_in_revision;
        send_to(g_peer, &r, sizeof r);
        g_map_asked[i] = now;
        sent++;
    }
}

void session_start(ObjMan *)
{
    char v[128];
    // Command line first: a friend's Join Game puts +gtmf_join=steam:<id> there, and the menu reads
    // that request, so the layer has to be up before the first frame.
    steam_install();
    mp_log("net: wire protocol %u (%s)", k_version, k_wire_tag);
    g_tactical = !strcmp(g_program, "tacgame");
    g_watch_host_worker.store(false, std::memory_order_release);
    g_block_guest_turn.store(false, std::memory_order_release);
    strcpy_s(g_root, g_out_dir);
    if (char *s = strrchr(g_root, '\\')) *s = 0;
    read_profile_name();
    int value;
    if (g_tactical) {
        if (env_integer("MP_SWAP", &value)) g_swap = value != 0;
    } else if (GetEnvironmentVariableA("MP_SWAP", v, sizeof v)) g_swap = atoi(v) != 0;
    if (!g_tactical) SetEnvironmentVariableA("MP_SWAPPED", 0);
    g_map_mode = !g_tactical && GetEnvironmentVariableA("MP_MAP", v, sizeof v) && atoi(v);
    // M5 ordered-turn flags are deliberately not resumed under spectator protocol 15.
    SetEnvironmentVariableA("MP_ENDTURN", nullptr); SetEnvironmentVariableA("MP_TURNDONE", nullptr);
    SetEnvironmentVariableA("MP_TURN_BASE", nullptr);
    if (GetEnvironmentVariableA("MP_MAP_EPOCH", v, sizeof v)) g_map_epoch = g_map_base_epoch = strtoul(v, 0, 10);
    if (GetEnvironmentVariableA("MP_MAP_REV", v, sizeof v)) g_map_revision = g_map_base_revision = strtoul(v, 0, 10);
    if (g_map_mode && !g_map_client.begin(g_map_base_epoch, g_map_base_revision))
        mp_log("map: missing or invalid snapshot identity; updates cannot be applied");
    if (g_tactical) {
        if (env_integer("MP_PORT", &value) && value >= 0 && value <= 65535) g_port = (unsigned short)value;
    } else if (GetEnvironmentVariableA("MP_PORT", v, sizeof v)) g_port = (unsigned short)atoi(v);
    bool host_mode_set = g_tactical ? env_text("MP_HOST_MODE", v, sizeof v) : GetEnvironmentVariableA("MP_HOST_MODE", v, sizeof v) != 0;
    if (host_mode_set)
        g_host_mode = !_stricmp(v, "operation") ? HOST_MODE_OPERATION : !_stricmp(v, "mission") ? HOST_MODE_MISSION : HOST_MODE_SKIRMISH;
    else if (g_tactical && tactical_mode::operation_battle(GetCommandLineA())) {
        g_host_mode = HOST_MODE_OPERATION;
        mp_log("net: direct tactical operation detected; using operation safety defaults");
    }
    bool prefer_set = g_tactical ? env_text("MP_PREFER", v, sizeof v) : GetEnvironmentVariableA("MP_PREFER", v, sizeof v) != 0;
    if (prefer_set) { char code[5] = { 0 }; strncpy_s(code, v, 4); memcpy(&g_prefer_side, code, 4); }
    bool role_set = g_tactical ? env_text("MP_ROLE", v, sizeof v) : GetEnvironmentVariableA("MP_ROLE", v, sizeof v) != 0;
    if (!role_set) return;
    g_role = !_stricmp(v, "host") ? ROLE_HOST : !_stricmp(v, "join") ? ROLE_JOIN : ROLE_NONE;
    int inherited_map = 0;
    if (spectator_policy::local_battle_offline(g_tactical, g_role == ROLE_JOIN,
                                               env_integer("MP_MAP", &inherited_map) && inherited_map != 0)) {
        // A battle started from the guest's local copy is NOT the host's battle. Do not
        // connect or send tactical commands. Authorized entry goes through Joiner::launch,
        // which explicitly clears MP_MAP before starting the host's battle.
        mp_log("map: locally started spectator battle is offline; only the host may start the shared battle");
        g_role = ROLE_NONE;
        SetEnvironmentVariableA("MP_ROLE", nullptr); SetEnvironmentVariableA("MP_PEER", nullptr);
        SetEnvironmentVariableA("MP_MAP", nullptr); SetEnvironmentVariableA("MP_AUTOJOIN", nullptr);
        return;
    }
    g_block_guest_turn.store(g_map_mode && g_role == ROLE_JOIN, std::memory_order_release);
    g_watch_host_worker.store(g_role == ROLE_HOST && !g_tactical && g_host_mode == HOST_MODE_OPERATION, std::memory_order_release);
    if (g_role != ROLE_NONE && g_tactical) load_battle_description();
    if (g_role == ROLE_HOST && g_tactical) {
        host_swap_sides_if_asked();
        snapshot_operation_saves();
    }
    if (g_role == ROLE_HOST) {
        // a battle changes the operation: the map the other player had before it is of the past
        if (!operation_turn::valid_epoch(g_map_epoch)) g_map_epoch = (GetTickCount() & 0xffff) | 0x10000;
        if (!g_map_revision) g_map_revision = 1;
        if (g_tactical) map_moved_on();
        else {
            map_moved_on(); // a fresh operation process must not reuse a previous offered identity
            sprintf_s(v, "%u", g_map_epoch); SetEnvironmentVariableA("MP_MAP_EPOCH", v);
            sprintf_s(v, "%u", g_map_revision); SetEnvironmentVariableA("MP_MAP_REV", v);
        }
    }
    // a joining game is told by its launcher whether its description is the mirrored one
    if (g_role == ROLE_JOIN && g_tactical && env_integer("MP_VERSUS", &value)) set_versus(value != 0);
    if (g_role == ROLE_HOST) {
        if (!g_host_mode) g_host_mode = HOST_MODE_SKIRMISH;
        find_local_ip();
        host_listen();
        if (steam_ready()) steam_broadcast(true, 0);
    } else if (g_role == ROLE_JOIN && (g_tactical || g_map_mode)) {
        char peer[128] = "127.0.0.1", configured[sizeof peer];
        if (g_tactical) {
            if (env_text("MP_PEER", configured, sizeof configured)) strcpy_s(peer, configured);
        } else GetEnvironmentVariableA("MP_PEER", peer, sizeof peer);
        g_peer = {};
        if (!_strnicmp(peer, "steam:", 6)) {
            // A friend's game, reached through Steam: the id is all the address there is.
            if (!steam_peer_make(_strtoui64(peer + 6, 0, 10), &g_peer)) {
                mp_log("net: '%s' is not a Steam address", peer);
                return;
            }
            bool udp_open = open_socket(0);
            // Keep the Steam address even if the unrelated UDP fallback is unavailable. Steam may
            // finish initializing on a later frame; no IP packet is sent to a Steam address.
            g_have_peer = true;
            mp_log("net: joining %s through Steam%s", peer, udp_open ? "" : " (UDP fallback unavailable)");
            return;
        }
        unsigned short port = g_port;
        if (char *colon = strchr(peer, ':')) { *colon = 0; port = (unsigned short)atoi(colon + 1); }
        g_peer.sin_family = AF_INET;
        g_peer.sin_port = htons(port);
        if (inet_pton(AF_INET, peer, &g_peer.sin_addr) != 1) {
            addrinfo hints = {}, *res = 0;
            hints.ai_family = AF_INET;
            if (getaddrinfo(peer, 0, &hints, &res) == 0 && res) {
                g_peer.sin_addr = ((sockaddr_in *)res->ai_addr)->sin_addr;
                freeaddrinfo(res);
            } else {
                mp_log("net: cannot resolve '%s'", peer);
            }
        }
        if (open_socket(0)) {
            g_have_peer = true;
            mp_log("net: joining %s:%u", peer, port);
        }
    }
    // A joining game only exists as a battle (started by the menu or mp_join.exe); its operational
    // program, reached after the battle ends, goes back to the multiplayer menu.
}

void session_stop(void)
{
    g_block_guest_turn.store(false, std::memory_order_release);
    g_watch_host_worker.store(false, std::memory_order_release);
    if (transport_policy::send_peer(g_tactical, g_sock != INVALID_SOCKET, g_have_peer, g_peer.sin_family == STEAM_FAMILY)) {
        Head bye = make_head(PKT_BYE); send_to(g_peer, &bye, sizeof bye);
    }
    if (g_sock != INVALID_SOCKET) {
        closesocket(g_sock);
        g_sock = INVALID_SOCKET;
    }
    g_have_peer = g_connected = g_bind_pending = false;
    if (g_tactical) {
        g_pending.clear();
        g_held.clear();
        statesync_forget_peer();
    }
}

// ---------------------------------------------------------------- owned units: send transforms
static void send_transforms(ObjMan *om, DWORD now)
{
    static Object *timer;
    if (!timer) timer = engine_find(om, "i_ctrltimer", "ctrltimer");
    unsigned char packet[sizeof(StateHead) + k_units_per_packet * sizeof(UnitRec)];
    StateHead *sh = (StateHead *)packet;
    UnitRec *recs = (UnitRec *)(sh + 1);
    int n = 0;
    auto flush = [&]() {
        sh->h = make_head(PKT_STATE, n);
        sh->snapshot = g_snapshot;
        sh->sim_ms = (u32)g_sim_ms;
        // The engine's timer (base_main, iCtrlTimer): +0x108 speed setting, +0x10c the automatically reduced
        // speed, used only while +0x15d (automatic control) is on and its 16-frame history (+0x158) is full.
        float set = timer ? *(float *)((char *)timer + 0x108) : 1, cur = timer ? *(float *)((char *)timer + 0x10c) : 1;
        bool reduced = timer && ((char *)timer)[0x15d] && *(u32 *)((char *)timer + 0x158) >= 16 && cur < set;
        sh->speed = reduced ? cur : set;
        sh->cur_speed = set;
        sh->paused = timer ? ((char *)timer)[0x15c] : 0;
        sh->phase = (unsigned char)g_phase;
        sh->ready = (unsigned char)g_my_ready;
        sh->pad = 0;
        send_to(g_peer, packet, (int)(sizeof(StateHead) + n * sizeof(UnitRec)));
        g_stat_packets++;
        n = 0;
    };
    u32 count, sent = 0;
    Object **all = all_objects(om, &count);
    g_snapshot++;
    for (u32 i = 0; i < count; i++) {
        u64 key;
        UnitRec r = {};
        if (!unit_key(all[i], &key, &r) || !owns_object(all[i])) continue;
        const char *u = (const char *)all[i];
        const float *pos = (const float *)(u + UNIT_POS), *rot = (const float *)(u + UNIT_ROT);
        transforms::Sent &s = g_sent[key];
        // Compare every rotation row (roll/pitch can change while row 0 stays unchanged). Unchanged
        // units still repeat at the native staggered two-second cadence.
        if (!s.due(pos, rot, now, r.id)) continue;
        s.remember(pos, rot, now);
        memcpy(r.pos, pos, 12);
        for (int row = 0; row < 3; row++) memcpy(r.rot + row * 3, rot + row * 4, 12);
        memcpy(r.lin, u + UNIT_LIN, 12);
        memcpy(r.ang, u + UNIT_ANG, 12);
        recs[n++] = r;
        g_stat_sent++;
        sent++;
        if (n == k_units_per_packet) flush();
    }
    // the clock always goes out: the joining game follows the host's speed, pause and phase, and the host
    // waits for a joining game that has fallen behind
    if (n || !sent) flush();
}

// ---------------------------------------------------------------- followed units: steer towards the owner's state
static void rebuild_units(ObjMan *om)
{
    u32 count;
    Object **all = all_objects(om, &count);
    g_units.clear();
    for (u32 i = 0; i < count; i++) {
        u64 key;
        if (unit_key(all[i], &key, 0)) g_units[key] = all[i];
    }
    // Both caches otherwise retain destroyed units forever, and g_held is walked on every
    // simulation step. Do not touch surviving entries or the transform send cadence.
    g_stat_cache_pruned += (u32)(prune_absent(g_held, g_units) + prune_absent(g_sent, g_units));
    g_units_count_seen = count;
}

Object *session_unit(u64 key)
{
    auto it = g_units.find(key);
    return it == g_units.end() ? 0 : it->second;
}

void session_sim_step(ObjMan *om, u32 sim_ms)
{
    if (!g_connected || !g_tactical) return;
    if (!sim_ms) ++g_stat_zero_steps;
    render_guard_install();                 // before state.cpp can detour the same renderer function
    // Received states wait until this game has been simulating for a while: a battle that has only just
    // appeared is still setting its units up, and a vehicle that gets a state loaded into it at that point
    // crashes the renderer (seen with large operation battles). MP_APPLY_DELAY = seconds, default 12.
    static DWORD first_step;
    static int delay = -1;
    if (delay < 0) {
        int seconds;
        delay = 12000;
        if (env_integer("MP_APPLY_DELAY", &seconds) && seconds >= 0 && seconds <= INT_MAX / 1000)
            delay = seconds * 1000;
        first_step = GetTickCount();
    }
    double t0 = seconds_now();
    // (a state is only loaded once the other game's numbers can be translated - see xlate.cpp)
    if (sync_state_enabled() && xlate_ready() && GetTickCount() - first_step >= (DWORD)delay) statesync_apply(om);
    g_prof_load += seconds_now() - t0;
    u32 count;
    all_objects(om, &count);
    if (count != g_units_count_seen) rebuild_units(om);
    shots_sim_step(om);
    trench_sim_step(om);

    // Followed units are not predicted and not left to this game's own physics: they are shown where their
    // owner had them, a tenth of a second ago, moving smoothly from one received transform to the next
    // ("snapshot interpolation"). Transforms arrive about 15 times a second; applying each one as it came -
    // with this game's physics pushing the unit about in between - is what made the other player's
    // vehicles judder. Prediction is for a unit whose owner is far away; here every unit is simulated by
    // its owner's game without any delay, so nothing has to be guessed ahead.
    //   between two transforms: position and orientation are blended;
    //   past the newest (the next one is late): the unit runs on with its last velocity for at most a
    //     quarter of a second, then stands - which also keeps a unit its owner has parked from driving off
    //     under this game's own simulation and being pulled back.
    double now_s = seconds_now();
    bool new_remote = false;
    for (const UnitRec &r : g_pending.records()) {
        u64 key = ((u64)r.kind << 32) | r.id;
        auto it = g_units.find(key);
        if (it == g_units.end() || owns_object(it->second)) continue;
        Held &h = g_held[key];
        if (h.have && now_s - h.b.at > 0.004) { h.a = h.b; h.have = 2; }       // (same step: just the newer one)
        h.b = { r, now_s };
        h.dirty = true;
        new_remote = true;
        if (!h.have) h.have = 1;
    }
    g_pending.clear();
    // A zero-ms ctrl_tick does not advance physics. Drain incoming shots, control events, state,
    // and freshly received transforms above, but don't re-interpolate every held object on every
    // render-only tick. A fresh deployment/paused update is still applied immediately.
    if (!sim_ms && !new_remote) { ++g_stat_zero_skipped; return; }

    const double k_delay = 0.10, k_run_on = 0.25;
    for (auto &kv : g_held) {
        Held &h = kv.second;
        if (!sim_ms && !h.dirty) continue;
        auto it = g_units.find(kv.first);
        if (it == g_units.end() || owns_object(it->second)) continue;
        h.dirty = false;
        double t = now_s - k_delay;
        float pos[3], rot[9];
        const UnitRec &B = h.b.rec;
        bool moving = true;
        if (!sim_ms) {
            // No wall-clock extrapolation through a pause or render-only tick.
            memcpy(pos, B.pos, sizeof pos);
            memcpy(rot, B.rot, sizeof rot);
        } else if (h.have == 2 && t < h.b.at && h.b.at - h.a.at > 0.001 && h.b.at - h.a.at < 0.6) {
            const UnitRec &A = h.a.rec;
            float k = t <= h.a.at ? 0.0f : (float)((t - h.a.at) / (h.b.at - h.a.at));
            for (int i = 0; i < 3; i++) pos[i] = A.pos[i] + (B.pos[i] - A.pos[i]) * k;
            for (int i = 0; i < 9; i++) rot[i] = A.rot[i] + (B.rot[i] - A.rot[i]) * k;
        } else {
            double run = t - h.b.at;
            run = run < 0 ? 0 : run > k_run_on ? k_run_on : run;
            moving = t - h.b.at < k_run_on;
            for (int i = 0; i < 3; i++) pos[i] = B.pos[i] + B.lin[i] * (float)run;
            memcpy(rot, B.rot, sizeof rot);
        }
        // the blended rows made square again: first row, second row at right angles to it, third their cross
        float n0 = sqrtf(rot[0] * rot[0] + rot[1] * rot[1] + rot[2] * rot[2]);
        if (n0 > 1e-4f) {
            for (int i = 0; i < 3; i++) rot[i] /= n0;
            float d = rot[3] * rot[0] + rot[4] * rot[1] + rot[5] * rot[2];
            for (int i = 0; i < 3; i++) rot[3 + i] -= d * rot[i];
            float n1 = sqrtf(rot[3] * rot[3] + rot[4] * rot[4] + rot[5] * rot[5]);
            if (n1 > 1e-4f) {
                for (int i = 0; i < 3; i++) rot[3 + i] /= n1;
                rot[6] = rot[1] * rot[5] - rot[2] * rot[4];
                rot[7] = rot[2] * rot[3] - rot[0] * rot[5];
                rot[8] = rot[0] * rot[4] - rot[1] * rot[3];
            } else memcpy(rot, B.rot, sizeof rot);
        } else memcpy(rot, B.rot, sizeof rot);

        char *u = (char *)it->second;
        float *upos = (float *)(u + UNIT_POS), *urot = (float *)(u + UNIT_ROT), *uinv = (float *)(u + UNIT_INV);
        float e[3] = { pos[0] - upos[0], pos[1] - upos[1], pos[2] - upos[2] };
        float err = sqrtf(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]);
        g_stat_err_sum += err;
        if (err > g_stat_err_max) g_stat_err_max = err;
        if (err > 3.0f) g_stat_snaps++;
        g_stat_applied++;
        if (!write_remote_transform(upos, urot, uinv, (float *)(u + UNIT_LIN), (float *)(u + UNIT_ANG),
                                    pos, rot, B.lin, B.ang, moving)) g_stat_elided++;
    }
}
#ifdef NETV21_TEST_HOOKS
// Windows no-game fixture drives the real step with a mock manager and remote unit.
void session_test_motion_begin()
{
    g_tactical = g_connected = true; g_role = ROLE_JOIN; g_versus = false;
    g_units.clear(); g_sent.clear(); g_held.clear(); g_pending.clear();
    g_units_count_seen = 0xffffffffu;
    g_stat_applied = g_stat_zero_steps = g_stat_zero_skipped = g_stat_cache_pruned = 0;
}
void session_test_motion_push(const UnitRec &r) { (void)g_pending.put(r); }
u32 session_test_motion_updates() { return g_stat_applied; }
u32 session_test_motion_skipped() { return g_stat_zero_skipped; }
size_t session_test_motion_cached() { return g_held.size(); }
void session_test_motion_end()
{
    g_units.clear(); g_sent.clear(); g_held.clear(); g_pending.clear();
    g_units_count_seen = 0;
    g_stat_applied = g_stat_zero_steps = g_stat_zero_skipped = g_stat_cache_pruned = 0;
    g_tactical = g_connected = false; g_role = ROLE_NONE;
}
#endif

// ---------------------------------------------------------------- detours: Start button, battle clock
// Leaving deployment and leaving the initial orders is the player's decision in the stock game. With another
// player connected it takes both: a press marks this player as ready and the phase steps in both games once
// both are. The interface object (i_intereff, adv_interf) has three ways in, all detoured:
//   +0x3b3b0  the "start game" action: deployment -> initial orders -> battle, then pause on/off
//   +0x3b4f0  the Start button / key: in deployment it opens the "end deployment?" box (or goes straight to
//             its result when confirmations are off), in the initial orders it starts the battle
//   +0x41700  message box results; box 3 answered yes = end deployment
// advance_phase() below calls the original +0x3b3b0 when the time has come.
static bool gate_active(void)
{
    return g_tactical && g_role != ROLE_NONE && g_connected && g_phase < 2;
}

static void mark_ready(void)
{
    if (g_my_ready > g_phase) return;
    g_my_ready = g_phase + 1;
    g_last_hello = 0;       // tell the other game at once
    mp_log("ready: this player has finished %s%s", g_phase == 0 ? "deployment" : "the initial orders",
           g_peer_ready > g_phase ? "" : ", waiting for the other player");
}

void session_test_ready(void) { if (gate_active()) mark_ready(); }

// Tests: end this battle as if the player had confirmed the game's "leave for the operational phase" box
// (message box 6, "it_mb_2oper"). Carried out from the logic pass, not from inside the render pipeline.
static bool g_resign_asked;
void session_test_resign(void) { g_resign_asked = true; }

static void end_send(DWORD now, unsigned char extra);

// Explicit diagnostic resign only. Box 6 abandons a battle and must NEVER be used to implement
// ceasefire or claim an engine result. Normal F9 agreement does not invoke this path.
static bool resign_now(ObjMan *om)
{
    if (!g_tactical || !g_box_result) return false;
    Object *ui = engine_find(om, "i_intereff", "intereff");
    if (!readable(ui, 0xdec)) return false;
    mp_log("end: leaving the battle through the game's own exit (box 6)");
    *(u32 *)((char *)ui + 0xde8) = 6;
    g_box_result(ui, 1, 0, 0);
    return true;
}

static void resign_if_asked(ObjMan *om)
{
    if (g_resign_asked && resign_now(om)) g_resign_asked = false;
}

static u64 start_action_hook(Object *ui, u64 a, u64 b, u64 c)
{
    if (!gate_active()) return g_start_action(ui, a, b, c);
    mark_ready();
    return 0;
}

static void input_action_hook(Object *ui, u64 b, const char *event)
{
    // the engine acts on the event whose value at +8 is negative (the release)
    if (gate_active() && g_phase == 1 && readable(event, 0x10) && *(const float *)(event + 8) < 0) {
        mark_ready();
        return;
    }
    g_input_action(ui, b, event);
}

static void box_result_hook(Object *ui, int result, u64 c, u64 d)
{
    // +0xde8: which box is open. A yes to "end deployment?" becomes a no for the engine (the box closes,
    // nothing else happens) and a ready mark for us.
    if (gate_active() && g_phase == 0 && result && *(u32 *)((char *)ui + 0xde8) == 3) {
        mark_ready();
        result = 0;
    }
    g_box_result(ui, result, c, d);
}

// The engine's battle timer (base_main, iCtrlTimer) turns the frame time and the speed setting into
// simulation steps ("ctrl_tick"), once per rendered frame. Two things are changed while a multiplayer battle
// runs:
//
// 1. A joining game runs on the host's clock. Its own interface keeps setting speed and pause (every frame),
//    and the engine reads them in several places, so the timer's setters themselves are replaced on the
//    timer object (vtable slots 3 and 5 - too short to detour) by versions that apply the host's values
//    whatever the caller asked for. Only the host changes speed or pauses.
//
// 2. Both games simulate at one fixed rate instead of once per rendered frame. Stock, a game at 100 frames
//    per second gives its units three times as many decisions per second as one at 33, and in a versus
//    game each PC simulates its own player's units - the faster PC's units would be quicker to spot, aim
//    and fire. The step function (base_main+0x4200, detoured) is therefore called with a fixed frame time,
//    as often as the real time that has passed asks for.
//
// 3. The two battle clocks are kept together by catching up, never by dropping or redoing anything (the
//    engine cannot replay a stretch of battle: it is not deterministic, and a whole battle cannot be put
//    back from a snapshot). A joining game that is behind the host simulates faster - up to four times, as
//    many steps per frame as its PC manages - until it is level, and slower when it is ahead. A host that
//    gets ahead of the joining game slows down and, half a second of battle time later, stops, until the
//    other game has caught up: both then advance at the pace of the slower PC and neither misses anything.
//    Only a game that is far away in time (it joined a battle already under way) takes over the host's
//    clock instead of replaying minutes of battle.
static bool follows_host_clock(void)
{
    return g_role == ROLE_JOIN && g_tactical && g_connected && g_have_host_clock;
}

static void set_speed_hook(char *ct, float speed)
{
    g_set_speed(ct, follows_host_clock() ? g_host_speed : speed);
}

static void set_pause_hook(char *ct, unsigned char paused)
{
    g_set_pause(ct, follows_host_clock() ? g_host_paused : paused);
}

static void replace_timer_setters(ObjMan *om)
{
    static bool done;
    if (done || g_role != ROLE_JOIN || !g_tactical) return;
    Object *timer = engine_find(om, "i_ctrltimer", "ctrltimer");
    HMODULE base = GetModuleHandleA("base_main.codelib");
    if (!timer || !base) return;
    done = true;
    void **vt = *(void ***)timer;
    if ((char *)vt != (char *)base + 0x23bc8 || (char *)vt[3] != (char *)base + 0x4680 || (char *)vt[5] != (char *)base + 0x3db0) {
        mp_log("net: unknown game build, speed and pause stay local");
        return;
    }
    // a private copy of the class's function table for this one object (with the type information
    // pointer that sits just before it)
    static void *copy[65];
    memcpy(copy, vt - 1, sizeof copy);
    g_set_speed = (SetSpeed)vt[3];
    g_set_pause = (SetPause)vt[5];
    copy[1 + 3] = (void *)set_speed_hook;
    copy[1 + 5] = (void *)set_pause_hook;
    *(void ***)timer = copy + 1;
    mp_log("net: speed and pause are the host's");
}

static void timer_step_hook(char *ct, float dt, u32 arg)
{
    bool multi = g_tactical && g_connected && g_role != ROLE_NONE;
    double scale = 1;
    if (follows_host_clock()) {
        ct[0x15d] = 0;                              // no automatic slow-down of its own
        *(float *)(ct + 0x108) = g_host_speed;
        *(float *)(ct + 0x10c) = g_host_speed;
        ct[0x15c] = g_host_paused;
        if (g_phase == 2 && g_host_phase == 2) {
            double speed = g_host_speed > 1 ? g_host_speed : 1;
            double behind = (double)g_host_sim_ms - (double)g_sim_ms;      // battle time, milliseconds
            if (!g_clock_adopted || fabs(behind) > 20000 * speed) {
                // first sight of the host's clock, or minutes apart: take it over
                if (fabs(behind) > 2000) mp_log("clock: taking over the host's battle time (%.1f s, was %.1f s)", g_host_sim_ms / 1000.0, g_sim_ms / 1000.0);
                g_sim_ms = g_host_sim_ms;
                g_clock_adopted = true;
                behind = 0;
            }
            if (fabs(behind) > 40) scale = 1 + behind / (1500 * speed);
            scale = scale < 0.25 ? 0.25 : scale > 4 ? 4 : scale;
        }
    } else if (g_role == ROLE_HOST && multi && g_phase == 2 && g_peer_phase == 2 && g_peer_clock_at && GetTickCount() - g_peer_clock_at < 3000) {
        double speed = *(float *)(ct + 0x108) > 1 ? *(float *)(ct + 0x108) : 1;
        double lead = (double)g_sim_ms - (double)g_peer_sim_ms;            // battle time the other game is behind
        double margin = 400 * speed;        // what the network and the reporting interval account for
        if (lead > margin && lead < 20000 * speed) {
            scale = 1 - (lead - margin) / (500 * speed);
            if (scale < 0) scale = 0;
        }
    }
    DWORD tick = GetTickCount();
    if (scale >= 0.5 || g_role != ROLE_HOST) g_waiting_since = 0;
    else if (!g_waiting_since) g_waiting_since = tick ? tick : 1;
    g_clock_factor = (float)scale;
    static double due;
    if (!multi || g_phase != 2 || ct[0x15c] || g_sim_step <= 0) {
        due = 0;
        g_waiting_since = 0;
        g_timer_step(ct, dt, arg);
        return;
    }
    // A joining game that has to catch up may take a dozen steps in one frame; a host takes five at most
    // and otherwise lets its clock run slow (the joining game follows it anyway).
    // Tests: MP_TEST_SPEED=n runs the battle n times as fast in real time, in steps of the usual size.
    static double hurry = -1;
    if (hurry < 0) {
        char v[16];
        hurry = env_text("MP_TEST_SPEED", v, sizeof v) && atof(v) > 0 ? atof(v) : 1;
    }
    double most = g_sim_step * (g_role == ROLE_JOIN ? 12 : 5) * hurry;
    due += dt * scale * hurry;
    if (due > most) due = most;
    double t0 = seconds_now();
    for (int steps = 0; due >= g_sim_step; steps++) {
        if (steps >= 3 && seconds_now() - t0 > 0.06 * hurry) break;     // keep the picture moving; the rest is taken next frame
        due -= g_sim_step;
        // a hair over the step: the timer cuts speed x time down to whole 5 ms, and 0.02f is just under 20
        g_timer_step(ct, (float)(g_sim_step * 1.0005), arg);
    }
}

static void install_detours(void)
{
    static const unsigned char start_sig[] = { 0x40, 0x53, 0x48, 0x83, 0xec, 0x40, 0x48, 0x8b, 0xd9, 0x80, 0xb9, 0xda, 0x0e, 0x00, 0x00, 0x00 };
    // the key handler: the game build of 2026-10 keeps the shape and changes the frame and which register
    // holds the event, so both forms are listed and the one that is there is used
    static const unsigned char input_sig[] = { 0x48, 0x89, 0x5c, 0x24, 0x08, 0x57, 0x48, 0x83, 0xec, 0x40, 0x49, 0x8b, 0xf8, 0x48, 0x8b, 0xd9 };
    static const unsigned char input_sig_old[] = { 0x48, 0x89, 0x5c, 0x24, 0x08, 0x57, 0x48, 0x83, 0xec, 0x50, 0x49, 0x8b, 0xd8, 0x48, 0x8b, 0xf9 };
    static const unsigned char box_sig[] = { 0x40, 0x55, 0x48, 0x83, 0xec, 0x30, 0x8b, 0x81, 0xe8, 0x0d, 0x00, 0x00, 0x48, 0x8b, 0xe9 };
    static const unsigned char timer_sig[] = { 0x48, 0x8b, 0xc4, 0x48, 0x89, 0x58, 0x08, 0x48, 0x89, 0x70, 0x10, 0x48, 0x89, 0x78, 0x20 };
    static const HookSig start_where[] = { { 0x3b320, start_sig, sizeof start_sig } };
    static const HookSig input_where[] = { { 0x3b460, input_sig, sizeof input_sig }, { 0x3b4f0, input_sig_old, sizeof input_sig_old } };
    static const HookSig box_where[] = { { 0x41630, box_sig, sizeof box_sig } };
    static const HookSig timer_where[] = { { 0x4200, timer_sig, sizeof timer_sig } };
    static bool done;
    if (done || !g_tactical || g_role == ROLE_NONE) return;
    HMODULE interf = GetModuleHandleA("adv_interf.codelib"), base = GetModuleHandleA("base_main.codelib");
    if (!interf || !base) return;
    done = true;
    char *start = hook_resolve_checked("adv_interf.codelib", start_where, 1, "the Start button");
    char *input = hook_resolve_checked("adv_interf.codelib", input_where, 2, "the Start key");
    char *box = hook_resolve_checked("adv_interf.codelib", box_where, 1, "the end-deployment box");
    char *timer = hook_resolve_checked("base_main.codelib", timer_where, 1, "the battle clock");
    // all three or none: a half-gated Start button would let one player skip the other
    if (start && input && box) {
        g_input_action = (InputAction)install_detour(input, sizeof input_sig, input_action_hook);
        g_box_result = (BoxResult)install_detour(box, sizeof box_sig, box_result_hook);
        g_start_action = (StartAction)install_detour(start, sizeof start_sig, start_action_hook);
    } else if (start || input || box) {
        mp_log("net: only %d of the three Start button pieces is in this game build, the button is left alone",
               (start ? 1 : 0) + (input ? 1 : 0) + (box ? 1 : 0));
    }
    int frequency;
    if (env_integer("MP_SIM_HZ", &frequency)) g_sim_step = frequency > 0 ? 1.0 / frequency : 0;
    if (timer) g_timer_step = (TimerStep)install_detour(timer, sizeof timer_sig, timer_step_hook);
    mp_log("net: Start button %s, battle clock %s, simulation at %d steps per second",
           g_start_action ? "shared between the players" : "not hooked (unknown game build)",
           !g_timer_step ? "not hooked (unknown game build)" : g_role == ROLE_JOIN ? "follows the host" : "is this game's",
           g_timer_step && g_sim_step > 0 ? (int)(1 / g_sim_step + 0.5) : 0);
}

// Steps the battle phase when both players are ready, or when a joining game is behind the host's phase
// (it joined late). Called strictly while this game is before the battle, where the action steps the phase.
static void advance_phase(ObjMan *om, DWORD now)
{
    if (!g_start_action || g_phase >= 2 || now - g_last_phase_step < 1500) return;
    bool both = g_my_ready > g_phase && g_peer_ready > g_phase;
    bool behind = g_role == ROLE_JOIN && g_have_host_clock && g_host_phase <= 2 && g_host_phase > g_phase;
    if (!both && !behind) return;
    Object *ui = engine_find(om, "i_intereff", "intereff");
    if (!ui) return;
    g_last_phase_step = now;
    mp_log("phase: leaving %s (%s)", g_phase == 0 ? "deployment" : "initial orders", both ? "both players are ready" : "the host is already further");
    g_start_action(ui, 0, 0, 0);
}

// Tests: MP_TEST_MAXTIME=<seconds> shortens the battle. The engine ends a battle when its time is up; that
// time is the float "max_game_time" of the battle description, kept by i_gamespec at +0x304 (adv_game reads
// it there: lea rax,[rdi+0x304] next to the key "f:max_game_time"). Nothing else about the end changes.
// The limit is copied into a countdown when the battle is built (i_trigman +0x29c, seconds left, counted
// down while the battle runs; at zero the game shows its red end-of-battle clock and then ends the battle),
// so that is what is shortened once the battle is under way.
static void shorten_battle_for_tests(ObjMan *om)
{
    static int state;          // 0 not looked at, 1 limit set, 2 done or not wanted
    static float wanted;
    if (state == 2 || !g_tactical) return;
    if (!state) {
        char v[16];
        if (!env_text("MP_TEST_MAXTIME", v, sizeof v) || atof(v) <= 0) { state = 2; return; }
        Object *gs = engine_find(om, "i_gamespec", "gamespec");
        if (!gs || !readable((char *)gs + 0x304, 4)) return;
        wanted = (float)atof(v);
        float *limit = (float *)((char *)gs + 0x304), was = *limit;
        if (!(was >= 60 && was <= 86400)) { mp_log("test: the battle's time limit is not where it was expected (%g); left alone", was); state = 2; return; }
        *limit = wanted;
        state = 1;
        mp_log("test: the battle's time limit is %g s instead of %g s", wanted, was);
    }
    if (g_phase != 2) return;
    Object *tm = engine_find(om, "i_trigman", "trigman");
    if (!tm || !readable((char *)tm + 0x29c, 4)) return;
    state = 2;
    float *left = (float *)((char *)tm + 0x29c);
    if (*left > wanted && *left <= 86400) { mp_log("test: %g s of battle left instead of %g s", wanted, *left); *left = wanted; }
}

// When the tactical host disappears, leave the running local battle and its result alone.
// The joining player may wait for reconnection or use the game's own finish/menu controls.

// ---------------------------------------------------------------- the end of a battle (M3)
// F9 exchanges an agreement only. The old box-6 exit abandoned the result, so neither agreement
// nor a peer reaching results forces this game to leave. Each player uses the game's normal controls.
// EndState is repeated and sequenced; a long partition can still prevent agreement.

static bool end_running(void) { return g_tactical && g_role != ROLE_NONE && g_phase == 2; }

void session_toggle_ceasefire(void)
{
    if (!end_running() || !g_end.toggle(g_connected, g_phase)) return;
    g_end_last_tx = 0;                                  // tell the other game at once
    if (!g_end.proposed) {
        mp_log("end: this player takes the ceasefire proposal back");
        netlog_mark("ceasefire taken back");
    } else {
        mp_log("end: this player proposes a ceasefire");
        netlog_mark("ceasefire proposed");
    }
}

bool session_ceasefire_offered(void) { return g_end.proposed; }
#ifdef NETV21_TEST_HOOKS
bool session_test_peer_proposed() { return g_end.peer_proposed; }
bool session_test_peer_over() { return g_end.peer_over; }
bool session_test_end_pending() { return g_end.pending; }
bool session_test_end_left() { return g_end.left; }
bool session_test_udp_open() { return g_sock != INVALID_SOCKET; }
#endif

static void end_send(DWORD now, unsigned char extra)
{
    if (!g_tactical || g_role == ROLE_NONE || !g_connected) return;
    if (!extra && g_end_last_tx && now - g_end_last_tx < 500) return;
    g_end_last_tx = now;
    EndState e = { make_head(PKT_END) };
    if (g_restart_say) extra |= END_RESTART;             // this host is starting the battle again
    e.h.flags = (unsigned char)((g_end.proposed ? END_PROPOSE : 0) |
                                (g_end.left ? END_OVER : 0) | END_SEQUENCED | extra);
    e.h.count = ++g_end.tx_seq;
    e.sim_ms = (unsigned)g_sim_ms;
    e.result = 0;
    send_to(g_peer, &e, sizeof e);
}

static void end_frame(ObjMan *, DWORD now)
{
    if (!g_tactical || g_role == ROLE_NONE) return;
    unsigned events = g_end.advance(now, g_connected, g_phase);
    if (events & ceasefire::natural_end) {
        mp_log("end: the engine reports this battle is over");
        netlog_mark("battle ended in the engine");
        end_send(now, END_OVER);
    } else {
        end_send(now, 0);
    }
    if (events & ceasefire::agreed) {
        mp_log("end: both players want a ceasefire; confirming agreement in 5 s");
        netlog_mark("ceasefire agreed");
    }
    if (events & ceasefire::peer_end_wait)
        mp_log("end: the other game reached its results; finish this battle using the game's own controls");
    if (events & ceasefire::finish_notice)
        mp_log("end: agreement/peer end noted; use the game's own controls to finish; no forced exit");
}

// F9 only exchanges consent. It does not call the game's abandon/exit path or mutate its result.
static void end_keys(void)
{
    if (!end_running()) { g_end_key_was = 0; return; }
    bool down = (GetAsyncKeyState(VK_F9) & 0x8000) != 0;
    if (down && !g_end_key_was) session_toggle_ceasefire();
    g_end_key_was = down;
}

static void end_on_packet(const unsigned char *data, int size, DWORD now)
{
    if (size < (int)sizeof(EndState)) return;
    const EndState *e = (const EndState *)data;
    bool proposed = (e->h.flags & END_PROPOSE) != 0, over = (e->h.flags & END_OVER) != 0;
    if (!g_tactical) {
        // The earlier non-tactical receiver did not interpret the new sequence flag. Preserve its
        // simple state updates and restart timeout instead of changing an M5 packet path.
        if (proposed != g_end.peer_proposed) {
            g_end.peer_proposed = proposed;
            mp_log(proposed ? "end: the other player proposes a ceasefire" : "end: the other player takes the ceasefire proposal back");
        }
        if (over && !g_end.peer_over) { g_end.peer_over = true; g_end.peer_result = e->result; }
        if (e->h.flags & END_RESTART) { g_restart_coming = true; g_restart_until = now + 90000; }
        return;
    }
    unsigned events = g_end.observe(proposed, over, e->result, (e->h.flags & END_SEQUENCED) != 0, e->h.count);
    if (events & ceasefire::stale_packet) return;
    if (events & ceasefire::changed) {
        if (proposed) mp_log("end: the other player proposes a ceasefire");
        else mp_log("end: the other player takes the ceasefire proposal back");
    }
    if (events & ceasefire::cancelled) {
        mp_log("end: ceasefire countdown cancelled by the other player");
        netlog_mark("peer withdrew ceasefire");
    }
    if (e->h.flags & END_RESTART) {
        g_restart_coming = true;
        g_restart_until = now + 90000;
    }
}
#ifdef NETV21_TEST_HOOKS
bool session_test_legacy_end()
{
    bool old_tactical = g_tactical, old_restart = g_restart_coming;
    DWORD old_until = g_restart_until;
    ceasefire::State old = g_end;
    g_tactical = false; g_end = ceasefire::State{};
    EndState e = { make_head(PKT_END) };
    e.h.flags = END_PROPOSE | END_SEQUENCED; e.h.count = 9;
    end_on_packet((const unsigned char *)&e, sizeof e, 100);
    bool ok = g_end.peer_proposed;
    e.h.flags = END_SEQUENCED | END_RESTART; e.h.count = 8; // older sequence is still accepted in old path
    end_on_packet((const unsigned char *)&e, sizeof e, 200);
    ok = ok && !g_end.peer_proposed && g_restart_coming && g_restart_until == 90200;
    g_tactical = old_tactical; g_end = old; g_restart_coming = old_restart; g_restart_until = old_until;
    return ok;
}
#endif

// ---------------------------------------------------------------- the two players' forces (M4)
// Each game's battle description is its own player's Skirmish setup: his own force on his side and, on the
// other side, whatever the setup gave the computer. A two-player battle needs both forces in both games,
// every unit keeping the id it has in its owner's description (force.h does the bytes).
//   - each game offers its own description: the joining game in its Hello, and serves it on request (the
//     image channel with flags 2);
//   - the host fetches it, checks it and writes the joining player's squads and units into its own
//     description, then starts the battle again from the file that now carries both forces;
//   - the joining game sees the host's description change, downloads it, mirrors it (it commands the other
//     side) and starts its battle again from that.
// An operation's battle is left alone: there the forces come from the operation's own save.

static bool force_here(void)
{
    return g_tactical && g_role != ROLE_NONE && g_host_mode != HOST_MODE_OPERATION && !g_image.empty() && !g_force_recovery_blocked;
}

static void force_publish(void)
{
    int value;
    if (!force_here()) return;
    if (env_integer("MP_FORCE_EXCHANGE", &value) && !value) {
        mp_log("lobby: the two players' forces are not exchanged (MP_FORCE_EXCHANGE=0)");
        return;
    }
    if (env_integer("MP_FORCE_APPLIED", &value) && value) {
        g_force_applied = true;
        SetEnvironmentVariableA("MP_FORCE_APPLIED", 0);          // the next battle starts clean
        mp_log("lobby: this battle was started again with both forces - they stay as they are");
        return;
    }
    if (!force::is_description(g_image)) {
        mp_log("lobby: this battle's description has no squads or units - the other player's force is not asked for");
        return;
    }
    force::Counts c = force::user_force(g_image);
    if (!c.squads || !c.units) {
        mp_log("lobby: this player's own force cannot be read out of the battle (%u squads, %u units)", c.squads, c.units);
        return;
    }
    g_force_out = g_image;
    g_force_out_hash = fnv(g_force_out.data(), g_force_out.size());
    g_force_published = true;
    mp_log("ui: this player's own force is offered to the other game: %u squads, %u units of the %s (%u bytes)",
           c.squads, c.units, image::side_name(g_user_side), (unsigned)g_force_out.size());
}

static void force_offer_to_hello(Hello *hello)
{
    if (!g_force_published || g_force_applied) return;
    hello->force_size = (u32)g_force_out.size();
    hello->force_hash = g_force_out_hash;
}

static bool force_offer_changed(u32 size, u32 hash)
{
    return hash != g_force_in_hash || size != g_force_in.size();
}

// Start a new offer even when it happens to have the same length as the previous one.
static void force_begin_offer(u32 size, u32 hash, const char *name)
{
    g_force_in.assign(size, 0);
    g_force_have.assign((size + k_chunk - 1) / k_chunk, 0);
    g_force_asked.assign(g_force_have.size(), 0);
    g_force_got = 0;
    g_force_last_ask = 0;
    g_force_retry.reset();
    mp_log("lobby: %s is sending the force he picked (%u bytes)", name && *name ? name : "the other player", size);
    g_force_in_hash = hash;
}
#ifdef NETV21_TEST_HOOKS
bool session_test_new_force_offer()
{
    force_begin_offer(4, 11, "test");
    bool changed = !force_offer_changed(4, 11) && force_offer_changed(5, 11) && force_offer_changed(4, 12);
    g_force_in[0] = 0x55; g_force_have[0] = 1; g_force_got = 1;
    (void)g_force_retry.due(10, 5000);
    force_begin_offer(4, 12, "test");
    bool ok = changed && g_force_in.size() == 4 && g_force_in[0] == 0 && g_force_have.size() == 1 &&
              g_force_have[0] == 0 && !g_force_got && g_force_in_hash == 12 && g_force_retry.due(10, 5000);
    g_force_in.clear(); g_force_have.clear(); g_force_asked.clear();
    g_force_got = g_force_in_hash = g_force_last_ask = 0;
    g_force_retry.reset();
    return ok;
}
#endif

// The joining game's force, asked for piece by piece (the same channel the description and the map travel on).
static void force_ask(DWORD now)
{
    if (!g_force_in.size() || g_force_applied) return;
    if (g_force_last_ask && now - g_force_last_ask < 60) return;
    g_force_last_ask = now;
    int budget = 24;
    for (size_t i = 0; i < g_force_have.size() && budget > 0; i++) {
        if (g_force_have[i]) continue;
        if (now - g_force_asked[i] < 250) continue;
        g_force_asked[i] = now ? now : 1;
        ImageRequest r = { make_head(PKT_IMAGE_REQUEST), (u32)i };
        r.h.flags = 2;
        send_to(g_peer, &r, sizeof r);
        budget--;
    }
}

static bool force_write_in(const ImageData *d, int size)
{
    if (!g_force_in.size() || d->index >= g_force_have.size() || g_force_have[d->index] || d->size > k_chunk) return false;
    size_t offset = (size_t)d->index * k_chunk;
    size_t expected = min((size_t)k_chunk, g_force_in.size() - offset);
    if (d->size != expected || size < (int)(sizeof(ImageData) - k_chunk + d->size)) return false;
    memcpy(g_force_in.data() + offset, d->data, d->size);
    g_force_have[d->index] = 1;
    g_force_got++;
    return true;
}
#ifdef NETV21_TEST_HOOKS
bool session_test_force_fragments()
{
    force_begin_offer(k_chunk + 3, 123, "test");
    ImageData d = { make_head(PKT_IMAGE_DATA), 0, k_chunk - 1 };
    const int header = sizeof(ImageData) - k_chunk;
    bool ok = !force_write_in(&d, header + d.size) && g_force_got == 0;
    d.size = k_chunk;
    ok = force_write_in(&d, header + d.size) && ok && g_force_got == 1;
    ok = !force_write_in(&d, header + d.size) && ok; // duplicate
    d.index = 1; d.size = 2;
    ok = !force_write_in(&d, header + d.size) && ok; // short final chunk
    d.size = 3;
    ok = !force_write_in(&d, header + 2) && ok; // truncated packet
    ok = force_write_in(&d, header + 3) && ok && g_force_got == 2;
    g_force_in.clear(); g_force_have.clear(); g_force_asked.clear();
    g_force_got = g_force_in_hash = g_force_last_ask = 0; g_force_retry.reset();
    return ok;
}
#endif

// The host: the joining player's force is in. It is his own battle's description; what goes into this
// battle is his squads and units for the side he commands here.
static bool force_apply(ObjMan *)
{
    if (g_force_applied || g_force_in.empty() || g_image.empty() || g_role != ROLE_HOST) return false;
    if (g_phase >= 2) {
        if (!g_force_noted) {
            g_force_noted = true;
            mp_log("lobby: the other player's force arrived after the battle had started - it stays out of this battle");
        }
        return false;
    }
    unsigned my_user = 0, my_enemy = 0, his_user = 0, his_enemy = 0;
    if (!image::sides(g_image, &my_user, &my_enemy)) { mp_log("lobby: this battle's description has no sides - the force is not put in"); return false; }
    if (!image::sides(g_force_in, &his_user, &his_enemy)) { mp_log("lobby: the other player's force has no sides - his side is played as set up"); return false; }
    if (his_user != my_enemy) {
        mp_log("lobby: the other player's force is of the %s and he commands the %s here - his side is played as set up",
               image::side_name(his_user), image::side_name(my_enemy));
        return false;
    }
    std::vector<unsigned char> combined = g_image;
    force::Result r = force::insert(combined, g_force_in);
    if (!r.ok) {
        mp_log("lobby: the other player's own force could not be put into this battle (%s) - his side is played as set up", r.note);
        return false;
    }
    if (!battle_install(g_image_file, g_image, combined)) return false;
    g_image.swap(combined);
    g_image_hash = fnv(g_image.data(), g_image.size());
    g_source_hash = g_image_hash;
    g_force_applied = true;
    const std::string backup = std::string(g_image_file) + ".before_mp";
    mp_log("lobby: the other player's own force is put into the battle: %u squads and %u units of his come in, "
           "%u squads and %u units of this player's stay (%zu bytes; the description as it was is kept in %s)",
           r.added_squads, r.added_units, r.kept_squads, r.kept_units, g_image.size(), backup.c_str());
    return true;
}

// Starting the battle again, from the description that now carries both forces. The other game is told
// first, so it waits instead of reading the silence as the end of the session.
static void force_restart(void)
{
    SetEnvironmentVariableA("MP_FORCE_APPLIED", "1");
    g_restart_say = true;
    for (int i = 0; i < 3; i++) {
        g_end_last_tx = 0;
        end_send(GetTickCount(), END_RESTART);
        Sleep(15);
    }
    std::string cmd = GetCommandLineA();
    STARTUPINFOA si = { sizeof si };
    PROCESS_INFORMATION pi;
    if (CreateProcessA(0, &cmd[0], 0, 0, FALSE, 0, 0, g_root, &si, &pi)) {
        mp_log("lobby: restarting the battle with the other player's own force");
        ExitProcess(0);
    }
    mp_log("lobby: could not restart the battle (%lu), playing it as it is", GetLastError());
    SetEnvironmentVariableA("MP_FORCE_APPLIED", 0);
    g_restart_say = false;
    // The running battle still uses the old description. Restore the file atomically and make
    // the advertised in-memory description match it; do not retry the same failed restart each frame.
    std::vector<unsigned char> restored;
    if (battle_restore_backup(g_image_file, &restored)) {
        g_image.swap(restored);
        g_image_hash = fnv(g_image.data(), g_image.size());
        g_source_hash = g_image_hash;
    } else {
        // Even if the filesystem blocks the rollback, keep the description served to the peer
        // consistent with the still-running battle whenever the retained backup can be read.
        const std::string backup = std::string(g_image_file) + ".before_mp";
        if (read_file(backup.c_str(), restored)) {
            g_image.swap(restored);
            g_image_hash = fnv(g_image.data(), g_image.size());
            g_source_hash = g_image_hash;
        }
        mp_log("lobby: the restart failed and the old description could not be restored on disk; the backup was retained");
    }
}

// The joining game: the host's description has changed (it carries both forces now). Download it, mirror it
// - this player commands the other side - and start the battle again from it.
static void desc_give_up(const char *why);

static void desc_begin(u32 size, u32 hash, DWORD now)
{
    if (++g_desc_tries > 6) { desc_give_up("it was tried six times"); return; }
    g_desc_in.assign(size, 0);
    g_desc_have.assign((size + k_chunk - 1) / k_chunk, 0);
    g_desc_asked.assign(g_desc_have.size(), 0);
    g_desc_got = 0;
    g_desc_hash = hash;
    g_desc_last_ask = 0;
    g_desc_since = now;
    mp_log("lobby: the host's battle has changed (%u bytes, hash %08x) - fetching it, it carries both forces", size, hash);
}

// The battle the host has started with cannot be taken: this game keeps its own. It says so plainly, stops
// asking, and lets the session go on - the battle it plays is its own, which is what it played before M4.
static void desc_give_up(const char *why)
{
    if (g_desc_dead) return;
    g_desc_dead = true;
    g_desc_in.clear();
    g_desc_have.clear();
    g_desc_asked.clear();
    g_desc_got = 0;
    g_desc_last_ask = 0;
    SetEnvironmentVariableA("MP_FORCE_APPLIED", 0);
    mp_log("lobby: the host's battle cannot be taken over (%s) - this game keeps the battle it started with", why);
    netlog_mark("the host's battle was not taken over");
}

static void desc_ask(DWORD now)
{
    if (g_desc_have.empty()) return;
    if (g_desc_last_ask && now - g_desc_last_ask < 60) return;
    g_desc_last_ask = now;
    int budget = 24;
    for (size_t i = 0; i < g_desc_have.size() && budget > 0; i++) {
        if (g_desc_have[i]) continue;
        if (now - g_desc_asked[i] < 250) continue;
        g_desc_asked[i] = now ? now : 1;
        ImageRequest r = { make_head(PKT_IMAGE_REQUEST), (u32)i };   // flags 0: the battle description
        send_to(g_peer, &r, sizeof r);
        budget--;
    }
}

static void desc_write_in(ImageData *d, int size)
{
    if (g_desc_have.empty() || d->index >= g_desc_have.size() || g_desc_have[d->index] || d->size > k_chunk) return;
    size_t offset = (size_t)d->index * k_chunk;
    size_t expected = min((size_t)k_chunk, g_desc_in.size() - offset);
    if (d->size != expected || size < (int)(sizeof(ImageData) - k_chunk + d->size)) return;
    memcpy(g_desc_in.data() + offset, d->data, d->size);
    g_desc_have[d->index] = 1;
    if (++g_desc_got != g_desc_have.size()) return;
    if (fnv(g_desc_in.data(), g_desc_in.size()) != g_desc_hash) {
        if (g_desc_tries < 6) {
            mp_log("lobby: the battle arrived damaged, fetching it again");
            desc_begin((u32)g_desc_in.size(), g_desc_hash, GetTickCount());
        } else {
            desc_give_up("it arrived damaged every time");
        }
        return;
    }
    std::vector<unsigned char> out = g_desc_in;
    if (g_versus && !image::mirror(out)) {
        desc_give_up("it could not be prepared for this side");
        return;
    }
    // Keep this player's original force as well as the host's: the next opponent must not
    // inherit the combined description. A failed write leaves the original file untouched.
    if (!battle_install(g_image_file, g_image, out)) {
        desc_give_up("it cannot be safely written");
        return;
    }
    SetEnvironmentVariableA("MP_FORCE_APPLIED", "1");
    mp_log("lobby: the battle with both forces is in place (%zu bytes, mirrored %d) - starting the battle again", out.size(), (int)g_versus);
    std::string cmd = GetCommandLineA();
    STARTUPINFOA si = { sizeof si };
    PROCESS_INFORMATION pi;
    if (CreateProcessA(0, &cmd[0], 0, 0, FALSE, 0, 0, g_root, &si, &pi)) ExitProcess(0);
    mp_log("lobby: could not restart the battle (%lu), playing it as it is", GetLastError());
    SetEnvironmentVariableA("MP_FORCE_APPLIED", 0);
    if (battle_restore_backup(g_image_file)) desc_give_up("the restart failed; the previous description was restored");
    else desc_give_up("the restart and restoration failed; the backup was retained");
}

// Every frame: fetch what is missing, apply what is in, and keep the other game told.
static void force_frame(ObjMan *om, DWORD now)
{
    if (!g_tactical || g_role == ROLE_NONE) return;
    if (g_role == ROLE_HOST) {
        if (g_force_in.size() && g_force_got == g_force_have.size() && !g_force_applied) {
            if (fnv(g_force_in.data(), g_force_in.size()) != g_force_in_hash) {
                mp_log("lobby: the other player's force arrived damaged, fetching it again");
                g_force_in_hash = 0;
                g_force_in.clear();
                g_force_have.clear();
                g_force_asked.clear();
                g_force_got = 0;
            } else if (g_force_retry.due(now, 5000) && force_apply(om)) {
                force_restart();
            }
        }
        force_ask(now);
    }
    if (g_role == ROLE_JOIN) desc_ask(now);
    // A joining game that is waiting for the host to come back with the battle that carries both forces
    // must not read the silence of that restart as the host leaving.
    if (g_restart_coming && g_restart_until && now > g_restart_until) {
        g_restart_coming = false;
        g_restart_until = 0;
    }
}

// ---------------------------------------------------------------- receive
static void on_packet(const sockaddr_in &from, const unsigned char *data, int size, DWORD now)
{
    const Head *h = (const Head *)data;
    // Every datagram through the counter, before the protocol decides what to do with it: a packet
    // that is dropped for a wrong magic is exactly the kind of thing a battle log has to show.
    netlog_packet("rx", data, size, ntohl(from.sin_addr.s_addr), ntohs(from.sin_port), from.sin_family);
    if (size < (int)sizeof(Head) || h->magic != k_magic) return;
    bool from_peer = g_have_peer &&
        ((from.sin_family == STEAM_FAMILY || g_peer.sin_family == STEAM_FAMILY)
             ? steam_peer_id(&from) != 0 && steam_peer_id(&from) == steam_peer_id(&g_peer)
             : from.sin_family == AF_INET && g_peer.sin_family == AF_INET &&
               from.sin_addr.s_addr == g_peer.sin_addr.s_addr && from.sin_port == g_peer.sin_port);

    if (g_role == ROLE_HOST && h->type == PKT_HELLO) {
        // A third sender must not replace a recently active tactical peer or reset its queues.
        // After three seconds without that peer, a legitimate changed-port reconnect is allowed.
        if (peer_guard::reject_hello(g_tactical || g_map_given,
                                     g_connected, from_peer, now, g_last_rx)) {
#ifdef NETV21_TEST_HOOKS
            ++g_test_hello_rejected;
#endif
            static DWORD last_refused;
            if (!last_refused || now - last_refused >= 5000) {
                last_refused = now ? now : 1;
                mp_log("net: ignored a different sender while the tactical peer is active");
            }
            return;
        }
        if (size < 24) return;
        const Hello *in = (const Hello *)data;
        if (memcmp(in->proto, k_protocol, sizeof k_protocol)) return;
        bool full = size >= (int)sizeof(Hello) && in->version == k_version;
        if (full && !spectator_policy::accept_hello(!g_tactical && g_host_mode == HOST_MODE_OPERATION, in->phase)) {
            mp_log("map: ignoring a tactical peer while the host is on the operation");
            return;
        }
        unsigned prefer = full ? in->prefer_side : 0;
        // The joining player commands the opponent unless it asked for the side the host plays.
        bool versus = !(prefer && prefer == g_user_side);
        Welcome w = { make_head(PKT_WELCOME) };
        memcpy(w.proto, k_protocol, sizeof k_protocol);
        w.version = k_version;
        w.image_size = g_tactical ? (u32)g_image.size() : 0;
        w.image_hash = g_image_hash;
        w.user_side = g_user_side;
        w.enemy_side = g_enemy_side;
        w.in_battle = g_tactical && !g_image.empty();
        w.host_mode = (unsigned char)g_host_mode;
        w.versus = versus;
        w.swap = g_swap;
        strcpy_s(w.name, g_my_name);
        bool map_offered = g_map_given && !g_tactical;
        if (map_offered) {
            w.map_size = (u32)g_map_out.size();
            w.map_hash = g_map_out_hash;
            w.map_turns = g_map_turns;
            strcpy_s(w.map_file, g_map_file);
        }
        w.map_flags = operation_turn::flags(g_map_epoch, operation_turn::Phase::host, false, false);
        w.map_revision = g_map_revision;
        send_to(from, &w, sizeof w);
        if (!full) return;                              // the caller reads the version from the reply and gives up
        char name[24];
        memcpy(name, in->name, sizeof name);
        name[sizeof name - 1] = 0;
        for (char *c = name; *c; c++) if (!isalnum((unsigned char)*c) && *c != '_' && *c != '-') *c = '_';
        if (!from_peer) {
            char ip[64];
            inet_ntop(AF_INET, &from.sin_addr, ip, sizeof ip);
            mp_log("net: player connected from %s:%u (%s)", ip, ntohs(from.sin_port), name[0] ? name : "no name");
            netlog_mark("a player joined: %s:%u (%s), sides us 0x%x them 0x%x, versus %d",
                        ip, ntohs(from.sin_port), name[0] ? name : "no name", w.user_side,
                        w.enemy_side, (int)versus);
            g_sent.clear();            // a new game needs every unit once
            if (g_tactical) g_pending.clear(); // never apply the previous tactical peer's queued transforms
            g_held.clear();
            reset_state_for_mode();
            shots_reset();
            if (g_tactical) { g_end.new_peer(); g_end_last_tx = 0; }
            g_peer_ready = 0;
            g_peer_swap_known = false;
        }
        if (name[0]) strcpy_s(g_peer_name, name);
        g_peer_lobby_ready = in->lobby_ready != 0;
        g_peer_on_map = in->phase == 0xfe && in->map_epoch == g_map_epoch && in->map_revision == g_map_revision;
        // Hello's former guest-order and turn flags are intentionally ignored in protocol 15.
        if (!g_tactical) {
            // lobby: the joining player's SWAP SIDES presses arrive as a counter
            if (g_peer_swap_known && in->swap_req != g_peer_swap_req && g_host_mode != HOST_MODE_OPERATION) session_toggle_swap();
            g_peer_swap_req = in->swap_req;
            g_peer_swap_known = true;
            SetEnvironmentVariableA("MP_PEER_NAME", g_peer_name[0] ? g_peer_name : 0);
        }
        // The joining player's own force: his battle description, from which this game takes his squads and
        // units (force.h). Fetched on the image channel; the host puts it into its own description.
        if (g_tactical && full && in->force_size && in->force_hash && !g_force_applied && !g_force_recovery_blocked &&
            force_offer_changed(in->force_size, in->force_hash) && in->force_size <= (16u << 20)) {
            // A changed hash or length is a different offer, even when the other field is identical.
            // Reusing old fragment bits would mix offers and waste a complete fetch before failing hash.
            force_begin_offer(in->force_size, in->force_hash, name);
        }
        if (in->phase < 0xfe && in->ready != g_peer_ready) {
            g_peer_ready = in->ready;
            mp_log("ready: the other player has finished %s", g_peer_ready >= 2 ? "the initial orders" : g_peer_ready ? "deployment" : "nothing yet");
        }
        g_peer = from;
        g_have_peer = g_connected = true;
        g_last_rx = now;
        if (g_tactical) set_versus(versus);
        return;
    }
    if (!from_peer) return;
    if (g_role == ROLE_JOIN && h->type == PKT_WELCOME && !valid_welcome(data, (size_t)size)) {
        // The lobby checks this too, but a tactical game can reconnect without going through it.
        // Never mark another protocol/version as a connected peer or apply its battle state.
        static DWORD last_bad_welcome;
        if (!last_bad_welcome || now - last_bad_welcome >= 5000) {
            last_bad_welcome = now ? now : 1;
            mp_log("net: rejected a host welcome with a different version, protocol, or size");
        }
        g_connected = false;
        return;
    }
    bool operation_host = !g_tactical && g_role == ROLE_HOST && g_host_mode == HOST_MODE_OPERATION;
    if (!spectator_policy::incoming(operation_host, !g_tactical && g_map_mode, h->type, h->flags)) return;
    g_last_rx = now;

    switch (h->type) {
    case PKT_KEEPALIVE:
        break;
    case PKT_IMAGE_REQUEST:
        if (size >= (int)sizeof(ImageRequest) && (g_role == ROLE_HOST || h->flags == 2)) {
            // flags 0: the battle description (host only); 1: the operational map this game is offering;
            // 2: this game's own force (M4), which either player serves
            if (h->flags > 2) return;
            const ImageRequest *request = (const ImageRequest *)data;
            const std::vector<unsigned char> &blob = h->flags == 1 ? g_map_out : h->flags == 2 ? g_force_out : g_image;
            if (h->flags == 1) {
                u32 epoch = g_role == ROLE_HOST ? g_map_epoch : g_map_base_epoch;
                u32 revision = g_role == ROLE_HOST ? g_map_revision : g_map_base_revision;
                if (!g_map_given || request->map_epoch != epoch || request->map_revision != revision) return;
            }
            if (h->flags == 2 && g_force_out.empty()) return;
            u32 index = request->index;
            if ((u64)index * k_chunk >= blob.size()) return;
            ImageData d = { make_head(PKT_IMAGE_DATA), index, 0 };
            d.h.flags = h->flags;
            if (h->flags == 1) { d.map_epoch = request->map_epoch; d.map_revision = request->map_revision; }
            d.size = (u32)min((size_t)k_chunk, blob.size() - (size_t)index * k_chunk);
            memcpy(d.data, blob.data() + (size_t)index * k_chunk, d.size);
            send_to(from, &d, (int)(sizeof d - k_chunk + d.size));
        }
        break;
    case PKT_IMAGE_DATA:
        if (h->flags == 2 && size >= (int)(sizeof(ImageData) - k_chunk)) {
            force_write_in((const ImageData *)data, size);
            break;
        }
        if (h->flags == 0 && g_role == ROLE_JOIN && !g_desc_have.empty() && size >= (int)(sizeof(ImageData) - k_chunk)) {
            desc_write_in((ImageData *)data, size);
            break;
        }
        if (g_map_mode && h->flags == 1 && !g_map_in.empty() && size >= (int)(sizeof(ImageData) - k_chunk)) {
            const ImageData *d = (const ImageData *)data;
            if (!image_chunks::valid(d, (size_t)size, g_map_in.size(), 1, g_map_in_epoch, g_map_in_revision) ||
                g_map_have[d->index]) return;
            memcpy(g_map_in.data() + (size_t)d->index * k_chunk, d->data, d->size);
            g_map_have[d->index] = 1;
            if (++g_map_got == g_map_have.size()) {
                if (fnv(g_map_in.data(), g_map_in.size()) != g_map_in_hash) {
                    mp_log("map: the map arrived damaged, fetching it again");
                    g_map_in_hash = 0;
                    g_map_in.clear();
                } else {
                    g_map_reload_at = now + 200;
                }
            }
        }
        break;
    case PKT_WELCOME:
        if (g_role == ROLE_JOIN && size >= (int)sizeof(Welcome)) {
            netlog_mark("welcome from the host: version %u, sides us 0x%x them 0x%x, versus %d, "
                        "in battle %d", ((const Welcome *)data)->version,
                        ((const Welcome *)data)->user_side, ((const Welcome *)data)->enemy_side,
                        (int)((const Welcome *)data)->versus, (int)((const Welcome *)data)->in_battle);
            const Welcome *w = (const Welcome *)data;
            if (!g_connected) {
                if (g_tactical) {
                    g_sent.clear(); g_pending.clear(); g_held.clear();
                    statesync_forget_peer();
                    // Keep the outgoing event epoch and saved-state version: the same host may
                    // have stayed alive while only its replies were lost. Incoming state can
                    // request a fresh baseline, but resetting a live shot stream mid-sequence
                    // would strand it until both peers agree on a new epoch.
                    g_end.new_peer(); g_end_last_tx = 0;
                    g_host_lost = 0; // the host returned
                }
                g_connected = true;
                mp_log("net: connected to host (host commands %s)", image::side_name(w->user_side));
            }
            // The two games must be playing the same battle.  The description is each game's own file, so
            // it is worth saying out loud when they differ - that is the one thing that makes every later
            // difference unexplainable.
            // Only when the host has said it is starting the battle again: then its description carries both
            // forces and this game has to take it over. Without the exchange (an operation, MP_FORCE_EXCHANGE=0,
            // a force that could not be put in) nothing restarts, and each game keeps the battle it started with.
            if (g_tactical && g_role == ROLE_JOIN && g_restart_coming && w->image_size && w->image_hash &&
                w->image_hash != g_source_hash && !g_force_applied && !g_force_recovery_blocked && !g_desc_dead && g_desc_have.empty() &&
                g_host_mode != HOST_MODE_OPERATION && w->image_size <= (16u << 20))
                desc_begin(w->image_size, w->image_hash, now);     // it carries both forces: take it
            if (g_tactical && !g_image.empty() && w->image_size &&
                (w->image_size != g_image.size() || w->image_hash != g_source_hash))
                mp_log("net: the battle description differs from the host's: %u bytes (hash %08x) here, %u bytes (hash %08x) on the host",
                       (unsigned)g_image.size(), g_image_hash, w->image_size, w->image_hash);
            memcpy(g_peer_name, w->name, sizeof g_peer_name);
            g_peer_name[sizeof g_peer_name - 1] = 0;
            if (g_map_mode) {
                u32 epoch = 0;
                operation_turn::Phase phase;
                if (!operation_turn::decode(w->map_flags, &epoch, &phase) || !w->map_revision) {
                    mp_log("map: ignored host welcome with invalid phase or snapshot identity");
                    break;
                }
                bool same = epoch == g_map_base_epoch && w->map_revision == g_map_base_revision;
                bool not_old = (epoch == g_map_base_epoch &&
                                (w->map_revision == g_map_base_revision || operation_turn::newer_revision(w->map_revision, g_map_base_revision))) ||
                               operation_turn::newer_epoch(epoch, g_map_base_epoch);
                if (not_old && g_map_client.pending_epoch)
                    not_old = (epoch == g_map_client.pending_epoch &&
                               (w->map_revision == g_map_client.pending_revision || operation_turn::newer_revision(w->map_revision, g_map_client.pending_revision))) ||
                              operation_turn::newer_epoch(epoch, g_map_client.pending_epoch);
                operation_turn::Observation observation = operation_turn::Observation::stale;
                if (same || w->map_size)
                    observation = g_map_client.observe(epoch, w->map_revision, phase);
                if (w->in_battle && not_old) {
                    // Host battle supersedes an incomplete or already assembled map.
                    g_map_reload_at = 0; g_map_in.clear(); g_map_have.clear();
                    g_map_client.pending_epoch = g_map_client.pending_revision = 0;
                    if (!g_map_battle_at) g_map_battle_at = now + 300;
                } else if (observation == operation_turn::Observation::snapshot_needed && w->map_size &&
                           w->map_size <= (64u << 20) && phase != operation_turn::Phase::collect &&
                           phase != operation_turn::Phase::resolve) {
                    if (not_old) g_map_battle_at = 0;
                    char offered_file[96];
                    memcpy(offered_file, w->map_file, sizeof offered_file);
                    bool terminated = offered_file[sizeof offered_file - 1] == 0;
                    offered_file[sizeof offered_file - 1] = 0;
                    if (!terminated || !opsave::safe_file_name(offered_file)) {
                        mp_log("map: host offered an invalid save file name; ignoring the offer");
                        break;
                    }
                    if (g_map_in.empty() || g_map_in_epoch != epoch || g_map_in_revision != w->map_revision ||
                        g_map_in_hash != w->map_hash || g_map_in.size() != w->map_size) {
                        // Superseding an almost-complete download must cancel its scheduled reload.
                        g_map_reload_at = 0;
                        g_map_in.assign(w->map_size, 0);
                        g_map_in_hash = w->map_hash;
                        g_map_have.assign((w->map_size + k_chunk - 1) / k_chunk, 0);
                        g_map_asked.assign(g_map_have.size(), 0);
                        g_map_got = 0;
                        g_map_in_epoch = epoch; g_map_in_revision = w->map_revision;
                        g_map_in_turns = w->map_turns;
                        strcpy_s(g_map_in_file, offered_file);
                        mp_log("map: host offers snapshot %u/%u (%u bytes); local changes are not sent", epoch, w->map_revision, w->map_size);
                    }
                }
            }
            if (w->host_mode && w->host_mode != g_host_mode) {
                g_host_mode = w->host_mode;
                if (g_host_mode == HOST_MODE_OPERATION) {
                    mp_log("net: operation battle - units are followed by position only");
                    char peer[128] = "";
                    if (g_tactical && env_text("MP_PEER", peer, sizeof peer)) SetEnvironmentVariableA("MP_AUTOJOIN", peer);
                }
            }
            set_versus(w->versus != 0);
        }
        break;
    case PKT_STATE:
        if (size >= (int)sizeof(StateHead)) {
            const StateHead *sh = (const StateHead *)data;
            if (size < (int)(sizeof(StateHead) + sh->h.count * sizeof(UnitRec))) return;
            if (g_role == ROLE_JOIN) {
                g_host_speed = sh->speed;
                g_host_cur_speed = sh->cur_speed;
                g_host_paused = sh->paused;
                g_host_phase = sh->phase;
                g_host_sim_ms = sh->sim_ms;
                if (sh->ready != g_peer_ready) {
                    g_peer_ready = sh->ready;
                    mp_log("ready: the other player has finished %s", g_peer_ready >= 2 ? "the initial orders" : g_peer_ready ? "deployment" : "nothing yet");
                }
                g_have_host_clock = true;
            } else {
                g_peer_sim_ms = sh->sim_ms;
                g_peer_phase = sh->phase;
                g_peer_clock_at = now ? now : 1;
            }
            const UnitRec *recs = (const UnitRec *)(sh + 1);
            queue_transforms_for_mode(recs, sh->h.count);
            g_stat_packets++;
        }
        break;
    case PKT_OBJECT:
    case PKT_OBJECT_ACK:
        state_packet_for_mode(data, size, send_peer, g_versus);
        break;
    case PKT_SHOT:
    case PKT_SHOT_ACK:
        shot_packet_for_mode(data, size, send_peer);
        break;
    case PKT_END:
        end_on_packet(data, size, now);
        break;
    case PKT_BYE:
        mp_log("net: the other player's game closed");
        g_connected = false;
        if (g_role == ROLE_JOIN && g_tactical && !g_host_lost) g_host_lost = now ? now : 1;
        if (g_role == ROLE_HOST) { g_have_peer = false; g_peer_on_map = false; }
        break;
    }
}

#ifdef NETV21_TEST_HOOKS
// The no-game fixture substitutes save/load only; packet/transport and identity checks are real.
bool session_test_map_seed(const std::vector<unsigned char> &source, bool refresh)
{
    if (g_role != ROLE_HOST || !spectator_snapshot::publishable(source, refresh, g_map_turns, g_map_out)) return false;
    std::vector<unsigned char> save = source;
    return adopt_host_snapshot(save, "camp_test_rus.opsave", refresh);
}
static unsigned g_test_turn_calls;
static void test_stock_next(void *, u64, const char *) { ++g_test_turn_calls; }
bool session_test_map_control() {
    NextTurn old = g_next_turn.load(); bool block = g_block_guest_turn.load();
    alignas(4) char event[16] = {}; *(float *)(event + 8) = -1.0f;
    g_next_turn = test_stock_next; g_test_turn_calls = 0;
    g_block_guest_turn.store(true); next_turn_hook(nullptr, 0, event);
    bool spectator_blocked = g_test_turn_calls == 0 && !session_map_blocking();
    g_block_guest_turn.store(false); next_turn_hook(nullptr, 0, event);
    bool host_passed = g_test_turn_calls == 1 && !session_map_blocking();
    g_next_turn.store(old); g_block_guest_turn.store(block);
    return spectator_blocked && host_passed;
}
u32 session_test_map_out_hash() { return g_map_out_hash; }
bool session_test_map_no_guest_upload() { return g_map_in.empty(); }
bool session_test_map_peer_loaded() { return g_peer_on_map; }
bool session_test_map_input() { return !session_map_blocking(); }
bool session_test_map_download_ready() { return g_map_reload_at != 0 && !g_map_in.empty() && g_map_got == g_map_have.size(); }
bool session_test_map_adopt_download(std::vector<unsigned char> *out) {
    if (!out || !session_test_map_download_ready() || !opsave_checked::validate(g_map_in) ||
        !g_map_client.loaded(g_map_in_epoch, g_map_in_revision)) return false;
    *out = g_map_in;
    g_map_base_epoch = g_map_in_epoch; g_map_base_revision = g_map_in_revision;
    g_map_in.clear(); g_map_reload_at = 0; g_map_note[0] = 0; g_last_hello = 0;
    return true;
}
void session_test_map_disconnect() {
    g_connected = false;
    if (g_role == ROLE_HOST) g_have_peer = g_peer_on_map = false;
}
void session_test_map_forbidden_packets() {
    StateHead state = {}; state.h = make_head(PKT_STATE);
    on_packet(g_peer, (const unsigned char *)&state, sizeof state, GetTickCount());
    Head obj = make_head(PKT_OBJECT), shot = make_head(PKT_SHOT);
    on_packet(g_peer, (const unsigned char *)&obj, sizeof obj, GetTickCount());
    on_packet(g_peer, (const unsigned char *)&shot, sizeof shot, GetTickCount());
}
void session_test_map_old_hello() {
    if (g_role != ROLE_HOST || !g_have_peer || g_map_revision < 2) return;
    Hello old = { make_head(PKT_HELLO) };
    memcpy(old.proto, k_protocol, sizeof k_protocol); old.version = k_version;
    old.phase = 0xfe; old.lobby_ready = 1; old.turn_done = 1;
    old.map_size = 1000; old.map_hash = 0x1234;
    old.map_epoch = g_map_epoch; old.map_revision = g_map_revision - 1;
    on_packet(g_peer, (const unsigned char *)&old, sizeof old, GetTickCount());
}
#endif

static void spectator_screen_lifecycle(ObjMan *om, DWORD now)
{
    if (g_map_mode) {
        bool on_map = map_ready_for_hello(om);
        if (on_map) { g_guest_seen_map = true; g_guest_left_since = 0; }
        else if (g_guest_seen_map && !g_map_battle_at && !g_map_reload_at) {
            if (!g_guest_left_since) g_guest_left_since = now ? now : 1;
            else if (now - g_guest_left_since >= 2000) {
                mp_log("map: spectator left the operation; stopping updates and returning control to the game");
                session_single_player();
                g_map_mode = false; g_map_in.clear(); g_map_have.clear();
                SetEnvironmentVariableA("MP_MAP", nullptr); SetEnvironmentVariableA("MP_AUTOJOIN", nullptr);
                SetEnvironmentVariableA("MP_MAP_EPOCH", nullptr); SetEnvironmentVariableA("MP_MAP_REV", nullptr);
            }
        }
    }
}

void session_frame(ObjMan *om)
{
    DWORD now = GetTickCount();
    g_prof_frames++;
    // Publish battle state at most once a second rather than taking another log lock every frame.
    static DWORD last_log_state;
    if (g_tactical && (!last_log_state || now - last_log_state >= 1000)) {
        last_log_state = now ? now : 1;
        netlog_state((int)g_phase, (unsigned)g_sim_ms, g_connected ? 1 : 0, g_user_side);
    }
    netlog_frame();
    steam_tick();
    // Retry a failed advertisement and recover if the game's other process briefly cleared the
    // shared Steam presence during a menu/battle transition. Do not stop the IP listener.
    static DWORD last_presence;
    if (g_role == ROLE_HOST && steam_ready() && (!last_presence || now - last_presence >= 5000)) {
        last_presence = now ? now : 1;
        steam_broadcast(true, 0);
    }
    update_ai_commander();
    if (g_tactical && g_role != ROLE_NONE) { render_guard_install(); render_guard_report(now); }
    if (g_tactical && (g_role != ROLE_NONE || GetEnvironmentVariableA("MP_LOG_SHOTS", 0, 0))) shots_install(om);
    if (g_tactical && g_role != ROLE_NONE) trench_install();
    shorten_battle_for_tests(om);
    if (g_role == ROLE_HOST && g_bind_pending && now - g_last_bind_try >= 500) host_listen();
    // Map input safety and leaving its screen must not depend on UDP or Steam being up.
    install_map_detours();
    spectator_screen_lifecycle(om, now);
    // The menu Joiner and this session share one Steam P2P queue. While the Joiner has reserved
    // it, only the Joiner may read its WELCOME/IMAGE_DATA; otherwise this earlier frame would eat
    // and discard those packets before ui_frame calls Joiner::poll(). UDP keeps its own socket.
    if (steam_ready() && !steam_lobby_receiving()) {
        uint64_t from_id = 0;
        for (int guard = 0; guard < 64; guard++) {
            unsigned char steam_buf[2048];
            int n = steam_recv(&from_id, steam_buf, sizeof steam_buf);
            if (n <= 0) break;
            g_stat_rx_bytes += n;
            sockaddr_in from = {};
            steam_peer_make(from_id, &from);
            on_packet(from, steam_buf, n, now);
        }
    }
    if (!transport_policy::drive_session(g_tactical, g_sock != INVALID_SOCKET, steam_ready())) return;

    if (g_sock != INVALID_SOCKET) {
        unsigned char buf[2048];
        for (int i = 0; i < 2048; i++) {
            sockaddr_in from;
            int flen = sizeof from;
            int n = recvfrom(g_sock, (char *)buf, sizeof buf, 0, (sockaddr *)&from, &flen);
            if (n <= 0) break;
            g_stat_rx_bytes += n;
            on_packet(from, buf, n, now);
            if (g_sock == INVALID_SOCKET) return;
        }
    }

    install_detours();
    end_keys();
    end_frame(om, now);
    force_frame(om, now);
    resign_if_asked(om);
    host_map_frame(om, now);
    if (g_map_mode) request_map_pieces(now);
    if (g_map_mode && g_test_save_asked && operation_on_screen(om)) {      // tests: how many orders has this player given?
        g_test_save_asked = false;
        std::vector<unsigned char> save;
        char file[96] = "";
        bool ok = save_operation(om, save, file, sizeof file);
        mp_log("test: operation %s (%s, %zu bytes, turn phase %d, %u orders)", ok ? "saved" : "not saved", file, save.size(),
               ok ? opsave::turn_phase(save) : -1, ok ? opsave::rows_of(save, "user_orders") : 0);
    }
    if (g_map_mode && g_end_turn_asked) {
        g_end_turn_asked = false;
        mp_log("test: spectator cannot advance the host's turn");
    }
    if (g_map_mode && g_map_battle_at && now >= g_map_battle_at) joiner_into_battle();
    if (g_map_mode && g_map_reload_at && now >= g_map_reload_at) {
#ifdef NETV21_TEST_HOOKS
        if (!g_test_map_no_game)
#endif
        joiner_reload_map();
    }
    replace_timer_setters(om);
    if (g_connected && g_tactical) advance_phase(om, now);
    if (g_role == ROLE_JOIN) {
        if (now - g_last_hello > (g_connected ? 1000u : 500u)) {
            g_last_hello = now;
            // the hello doubles as keep-alive and carries the side wish, so a restarted host learns it again
            Hello hello = { make_head(PKT_HELLO) };
            memcpy(hello.proto, k_protocol, sizeof k_protocol);
            hello.version = k_version;
            hello.prefer_side = g_prefer_side;
            hello.ready = (unsigned char)g_my_ready;
            hello.phase = g_map_mode ? (map_ready_for_hello(om) ? 0xfe : 0xff) : (unsigned char)g_phase;
            hello.lobby_ready = 1;
            strcpy_s(hello.name, g_my_name);
            hello.map_epoch = g_map_base_epoch;
            hello.map_revision = g_map_base_revision;
            force_offer_to_hello(&hello);
            send_to(g_peer, &hello, sizeof hello);
        }
    }
    if (g_connected && g_tactical) {
        if (now - g_last_snapshot >= 66) {
            g_last_snapshot = now;
            send_transforms(om, now);
        }
        double t0 = seconds_now();
        if (sync_state_enabled()) statesync_owner_frame(om, now, owns_object, send_peer);
        g_prof_save += seconds_now() - t0;
        xlate_frame(om);
        shots_frame(send_peer);
    }

    // The other game is quiet while it loads a battle (its menu closes and the tactical game takes over),
    // and eight seconds of that is not a lost connection.  Before the battle the wait is 20 s, in the
    // battle itself 8 s.
    DWORD patience = (g_tactical && g_phase >= 2) ? 8000u : 20000u;
    if (g_connected && now - g_last_rx > patience) {
        mp_log("net: connection lost (nothing received for %u s)", (unsigned)(patience / 1000));
        g_connected = false;
        g_peer_ready = 0;
        if (g_role == ROLE_JOIN && g_tactical && !g_host_lost) g_host_lost = now ? now : 1;
        if (g_role == ROLE_HOST) { g_have_peer = false; g_peer_on_map = false; }
    }
    if (now - g_last_report >= 5000) {
        if (g_connected && g_tactical && (g_stat_sent || g_stat_applied || g_stat_zero_skipped || g_stat_cache_pruned)) {
            mp_log("net: 5 s: sent %u and applied %u unit transforms (%u coalesced, %u identical writes elided, %u invalid/full), mean correction %.2f m, largest %.2f m, %u jumps, rx %.1f / tx %.1f KB/s; zero-ms %u (%u idle skipped), cache pruned %u",
                   g_stat_sent, g_stat_applied, g_stat_coalesced, g_stat_elided, g_stat_bad_transforms,
                   g_stat_applied ? g_stat_err_sum / g_stat_applied : 0.0, g_stat_err_max, g_stat_snaps,
                   g_stat_rx_bytes / 5120.0, g_stat_tx_bytes / 5120.0, g_stat_zero_steps, g_stat_zero_skipped, g_stat_cache_pruned);
            statesync_report();
            shots_report();
            trench_report();
            if (g_role == ROLE_JOIN && g_have_host_clock)
                mp_log("clock: battle time here %.1f s, on the host %.1f s (time scaled by %.2f to stay on it)",
                       g_sim_ms / 1000.0, g_host_sim_ms / 1000.0, g_clock_factor);
            else if (g_role == ROLE_HOST)
                mp_log("clock: battle time %.1f s, in the other game %.1f s (time scaled by %.2f to wait for it)",
                       g_sim_ms / 1000.0, g_peer_sim_ms / 1000.0, g_clock_factor);
        }
        if (g_tactical && g_role != ROLE_NONE)
            mp_log("load: 5 s: %u network ticks (not FPS), %u simulation steps covering %u ms, %.0f ms saving state, %.0f ms loading state, peer %s",
                   g_prof_frames, g_prof_steps, g_prof_step_ms, g_prof_save * 1000, g_prof_load * 1000,
                   g_connected ? "connected" : "absent");
        g_prof_frames = g_prof_steps = g_prof_step_ms = 0;
        g_prof_save = g_prof_load = 0;
        g_last_report = now;
        g_stat_sent = g_stat_applied = g_stat_elided = g_stat_coalesced = g_stat_bad_transforms = 0;
        g_stat_zero_steps = g_stat_zero_skipped = g_stat_cache_pruned = 0;
        g_stat_packets = g_stat_rx_bytes = g_stat_tx_bytes = g_stat_snaps = 0;
        g_stat_err_sum = 0;
        g_stat_err_max = 0;
    }
}

// Trenches. A force that is set to defend starts the battle dug in, and while its player places his
// squads during deployment each one is dug in again where he puts it; after the battle has started nobody
// digs. All of it goes through one function of the trench manager (i_trencher, adv_game): virtual slot 3,
// adv_game+0xdbc20: dig(trencher, squad id, position or 0). What comes out depends on where the squad
// stands, which way it faces, the kind of trench and its dig-in settings - and each game dug for every
// squad from what it knew itself. A squad the other player moved during deployment kept its trench at the
// old place in this game: trenches in other places, facing other ways, and since a trench changes the
// ground, vehicles crossing one were shown displaced on the other PC.
//
// Digging is therefore the owner's, like firing (shots.cpp):
//   - a dig for a squad this game's player commands happens as usual, and what went into it is sent to
//     the other game: the squad's dig-in settings, the kind of trench, the place if one was given, where
//     the squad's leading unit stands and how it is turned, and the trench manager's random state;
//   - a dig this game would make for one of the other player's squads is dropped;
//   - a dig received from the other game is carried out here from the owner's values, in the order the
//     owner made them (the last one for a squad is where it ends up).
// The records travel on the same reliable, ordered channel as the shots (ShotRec.kind 2).
// MP_SYNC_TRENCH=0 switches this off; MP_LOG_TRENCH=1 logs every dig.
#include "common.h"
#include "env_value.h"
#include "netlog.h"
#include "protocol.h"
#include "hooks.h"
#include "detour.h"
#include <atomic>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

typedef u64 (*Dig)(char *trencher, int squad, float *pos);
// The trampoline is published before the detour can expose the hook to another thread.
static std::atomic<Dig> g_dig{nullptr};
static char *g_trencher;
static bool g_injecting, g_enabled = true, g_trace;
static DWORD g_main_thread;
static u64 g_stat_own, g_stat_dropped, g_stat_applied, g_stat_missing, g_stat_queue_full, g_stat_unavailable;
static std::atomic<u64> g_stat_off_thread{0};
static std::vector<ShotRec> g_wait;         // received, carried out before the next simulation step
static std::vector<ShotRec> g_mine;         // squads this game's player has placed, for a player who connects later
static std::vector<ShotRec> g_all;          // tests: every dig of this player's squads

#pragma pack(push, 1)
struct TrenchRec {                          // travels in ShotRec.launch
    u32 squad, has_pos;
    float pos[4];
    u32 flags, type, seed;                  // squad record +0x340, +0x328; trench manager +0x2d0
    unsigned char b3e3, b3e4, b3ef, has_leader;
    u64 leader;                             // the squad's leading unit (squad record +0x40) ...
    float lpos[3], lrot[9];                 // ... and where it stands and how it is turned
};
#pragma pack(pop)
static_assert(sizeof(TrenchRec) <= sizeof(((ShotRec *)0)->launch), "a dig has to fit into a shot record");

// The squad's record in i_divisions (trench manager +0x1b8, virtual slot 11: record by squad or unit id).
static char *squad_record(char *trencher, u32 squad)
{
    void **div = *(void ***)(trencher + 0x1b8);
    if (!readable(div, 8) || !readable(*div, 0x60)) return 0;
    char *rec = ((char *(*)(void *, u32))(*(void ***)div)[11])(div, squad);
    return readable(rec, 0x400) ? rec : 0;
}

static u64 dig_hook(char *trencher, int squad, float *pos)
{
    // Acquire also publishes the owner thread set before the detour was installed.
    Dig original = g_dig.load(std::memory_order_acquire);
    if (!original) return 0; // the hook is not exposed before the trampoline is published
    // A foreign engine thread must neither publish a manager pointer nor read session-owned
    // state. Its engine call remains untouched; this does not prove the engine is single-threaded.
    if (GetCurrentThreadId() != g_main_thread) {
        g_stat_off_thread.fetch_add(1, std::memory_order_relaxed);
        return original(trencher, squad, pos);
    }
    g_trencher = trencher;
    bool connected = session_connected() && g_enabled;      // (hooked for the log only: each game digs for itself)
    if (g_injecting || session_role() == ROLE_NONE) return original(trencher, squad, pos);
    char *sq = squad_record(trencher, (u32)squad);
    if (connected && !session_owns_squad((u32)squad)) {
        g_stat_dropped++;
        if (g_trace) mp_log("trench: dig for squad %08x of the other player dropped", (u32)squad);
        return 0;
    }
    ShotRec s = {};
    TrenchRec r = {};
    r.squad = (u32)squad;
    r.has_pos = pos != 0;
    if (pos) memcpy(r.pos, pos, 16);
    r.seed = *(u32 *)(trencher + 0x2d0);
    if (sq) {
        r.flags = *(u32 *)(sq + 0x340);
        r.type = *(u32 *)(sq + 0x328);
        r.b3e3 = sq[0x3e3]; r.b3e4 = sq[0x3e4]; r.b3ef = sq[0x3ef];
        Object *leader = *(Object **)(sq + 0x40);
        if (readable(leader, 0x2c0) && session_unit_key(leader, &r.leader)) {
            r.has_leader = 1;
            const float *p = (const float *)((char *)leader + UNIT_POS), *rot = (const float *)((char *)leader + UNIT_ROT);
            memcpy(r.lpos, p, 12);
            for (int row = 0; row < 3; row++) memcpy(r.lrot + row * 3, rot + row * 4, 12);
        }
    }
    u64 result = original(trencher, squad, pos);
    g_stat_own++;
    if (g_trace) mp_log("trench: squad %08x dug in (kind %u, settings %x, %s), %s", r.squad, r.type, r.flags,
                        pos ? "at a given place" : "where it stands", connected ? "queued for peer" : "local only");
    netlog_count("trench_dug");
    s.kind = 2;
    memcpy(s.launch, &r, sizeof r);
    // What is kept for a player who connects later: the squads this player has placed himself. The digging
    // done when the battle is built comes out the same in both games by itself (same battle, nobody has
    // moved anything yet) - sent again it would be dug twice there.
    if (r.has_pos && g_mine.size() < 4096) g_mine.push_back(s);
    if (g_all.size() < 4096) g_all.push_back(s);
    if (connected) { shots_queue(s); netlog_count("trench_queued"); }
    return result;
}

#ifdef NETV21_TEST_HOOKS
// Exercise the production hook on two real threads without patching the game's code.
void trench_test_engine(Dig original)
{
    g_main_thread = GetCurrentThreadId();
    g_trencher = nullptr;
    g_injecting = false;
    g_dig.store(original, std::memory_order_release);
}
u64 trench_test_dig(char *trencher, int squad, float *pos) { return dig_hook(trencher, squad, pos); }
char *trench_test_current() { return g_trencher; }
size_t trench_test_pending() { return g_wait.size(); }
u64 trench_test_queue_full() { return g_stat_queue_full; }
u64 trench_test_unavailable() { return g_stat_unavailable; }
bool trench_test_install(void *target)
{
    g_main_thread = GetCurrentThreadId();
    g_trencher = nullptr;
    g_injecting = false;
    g_dig.store(nullptr, std::memory_order_release);
    return install_published_detour(target, 14, dig_hook, g_dig);
}
#endif

// A player has connected (again): he gets this game's digs of its own player's squads.
void trench_resend(void)
{
    if (!g_enabled || !g_dig.load(std::memory_order_acquire)) return;
    for (ShotRec s : g_mine) {
        TrenchRec r;
        memcpy(&r, s.launch, sizeof r);
        if (session_owns_squad(r.squad)) { shots_queue(s); netlog_count("trench_queued"); }
    }
}

static void apply(const TrenchRec &r)
{
    char *sq = squad_record(g_trencher, r.squad);
    if (!sq) { g_stat_missing++; return; }
    *(u32 *)(sq + 0x340) = r.flags;
    *(u32 *)(sq + 0x328) = r.type;
    sq[0x3e3] = r.b3e3; sq[0x3e4] = r.b3e4; sq[0x3ef] = r.b3ef;
    if (r.has_leader) {
        if (Object *u = session_unit(r.leader)) {
            float *p = (float *)((char *)u + UNIT_POS), *rot = (float *)((char *)u + UNIT_ROT), *inv = (float *)((char *)u + UNIT_INV);
            memcpy(p, r.lpos, 12);
            for (int row = 0; row < 3; row++)
                for (int col = 0; col < 3; col++) {
                    rot[row * 4 + col] = r.lrot[row * 3 + col];
                    inv[col * 4 + row] = r.lrot[row * 3 + col];
                }
        }
    }
    *(u32 *)(g_trencher + 0x2d0) = r.seed;
    float pos[4];
    memcpy(pos, r.pos, 16);
    g_injecting = true;
    g_dig.load(std::memory_order_acquire)(g_trencher, (int)r.squad, r.has_pos ? pos : 0);
    g_injecting = false;
    g_stat_applied++;
    if (g_trace) mp_log("trench: squad %08x of the other player dug in here from his values (kind %u, settings %x)", r.squad, r.type, r.flags);
    netlog_count("trench_applied");
}

// A dig of the other game has arrived. The shot-channel ACK covers this handoff, not
// application by the engine; a full local queue is an explicit loss, not successful replay.
void trench_received(const ShotRec &s)
{
    if (g_wait.size() < 4096) g_wait.push_back(s);
    else {
        ++g_stat_queue_full;
        if (g_stat_queue_full == 1) mp_log("trench: incoming digs refused: local queue is full (ACK does not mean application)");
    }
}

// Tests: "dig <n> <metres>" - this game's player puts n of his squads somewhere else during deployment.
// The game's deployment screen then digs the squad in at the new place through the same function; that
// call is made here (the units themselves stay where they are - it is the trench that is compared).
static int g_test_squads;
static float g_test_shift;
void trench_test_move(int squads, float metres) { g_test_squads = squads; g_test_shift = metres; }

static void test_move(void)
{
    int n = g_test_squads, moved = 0;
    g_test_squads = 0;
    if (!g_dig.load(std::memory_order_acquire) || !g_trencher) return;
    std::vector<ShotRec> mine = g_all;          // the hook appends to it
    std::vector<u32> seen;
    for (const ShotRec &s : mine) {
        TrenchRec r;
        memcpy(&r, s.launch, sizeof r);
        bool again = false;
        for (u32 id : seen) again |= id == r.squad;
        if (again || !r.has_leader || !session_owns_squad(r.squad)) continue;
        seen.push_back(r.squad);
        if (moved >= n) break;
        float pos[4] = { r.lpos[0] + (moved % 2 ? -g_test_shift : g_test_shift), r.lpos[1], r.lpos[2] + g_test_shift * 0.6f, 0 };
        dig_hook(g_trencher, (int)r.squad, pos);
        moved++;
    }
    mp_log("test: %d squads of this player dug in again %.0f m away", moved, g_test_shift);
}

// Before a simulation step: the digs received from the other game are carried out.
void trench_sim_step(ObjMan *om)
{
    if (g_test_squads) test_move();
    if (g_wait.empty()) return;
    if (!g_dig.load(std::memory_order_acquire) || !g_enabled) {
        g_stat_unavailable += (u64)g_wait.size();
        mp_log("trench: %zu incoming digs refused: local hook unavailable (they were not applied)", g_wait.size());
        g_wait.clear();
        return;
    }
    if (!g_trencher) g_trencher = (char *)engine_find(om, "i_trencher", "trencher");
    if (!g_trencher) return;
    for (const ShotRec &s : g_wait) {
        TrenchRec r;
        memcpy(&r, s.launch, sizeof r);
        apply(r);
    }
    g_wait.clear();
}

void trench_install(void)
{
    static const unsigned char sig[] = { 0x48, 0x8b, 0xc4, 0x48, 0x89, 0x58, 0x10, 0x55, 0x56, 0x57, 0x41, 0x54, 0x41, 0x55, 0x41, 0x56, 0x41, 0x57 };
    static bool done;
    if (done) return;
    HMODULE m = GetModuleHandleA("adv_game.codelib");
    if (!m) return;
    done = true;
    g_main_thread = GetCurrentThreadId();
    int value;
    if (env_integer("MP_SYNC_TRENCH", &value)) g_enabled = value != 0;
    g_trace = env_integer("MP_LOG_TRENCH", &value) && value;
    if (!g_enabled && !g_trace) { mp_log("trench: each game digs for itself (MP_SYNC_TRENCH=0)"); return; }
    const HookSig where[] = { { 0xdbc20, sig, sizeof sig } };
    char *dig = hook_resolve_checked("adv_game.codelib", where, 1, "trench digging");
    if (!dig) { mp_log("trench: digging hook refused, each game digs for itself"); g_enabled = false; return; }
    // Publishing the trampoline first avoids entering the hook with a null original. The code
    // bytes themselves are still patched without suspending engine threads (see detour.h).
    bool installed = install_published_detour(dig, 14, dig_hook, g_dig);
    mp_log("trench: %s", !installed ? "could not be hooked" : g_enabled ? "a squad's trenches are its owner's: dug there, sent, and dug the same here"
                                                                       : "each game digs for itself (MP_SYNC_TRENCH=0); digs are logged");
    if (!installed) g_enabled = false;
}

void trench_report(void)
{
    u64 off_thread = g_stat_off_thread.exchange(0, std::memory_order_relaxed);
    if (!g_stat_own && !g_stat_dropped && !g_stat_applied && !g_stat_missing && !g_stat_queue_full && !g_stat_unavailable && !off_thread) return;
    mp_log("trench: %llu squads of this player dug in and sent, %llu of the other player dug in here from his values, %llu local digs for his squads dropped, %llu for squads this game does not have, %llu refused for a full queue, %llu refused for an unavailable hook, %llu other-thread calls passed through",
           g_stat_own, g_stat_applied, g_stat_dropped, g_stat_missing, g_stat_queue_full, g_stat_unavailable, off_thread);
    g_stat_own = g_stat_dropped = g_stat_applied = g_stat_missing = g_stat_queue_full = g_stat_unavailable = 0;
}

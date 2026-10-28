// Shots: every shell, bullet, grenade and flare in a battle is created by one engine function, the
// "launch" of the shell manager (i_shellphys, virtual slot 3, basic_physic+0x18430). It makes the
// projectile, its trail and tracer, the muzzle flash, the sound of the shot and the report the computer
// opponents hear. A unit's own update calls it when the unit fires.
//
// Each game simulates every unit, including the ones the other player commands, so left alone each game
// fires those units' weapons by itself: at slightly different moments, in slightly different directions,
// and - when a newly loaded state says the weapon has already fired - not at all. Those were the shots
// that looked different on the two PCs or were missing on one of them.
//
// With another player connected the launch is therefore the owner's alone:
//   - a launch by a unit this game owns happens as usual and is sent to the other game;
//   - a launch this game's own simulation makes for a unit the other game owns is dropped;
//   - a launch received from the other game is carried out here with exactly the owner's values.
// The launch record holds pointers to the ammunition and weapon descriptions and to the firing unit. They
// travel as the ids the engine's own savegame code uses for them (object manager, virtual slots 56/57) and
// as unit numbers, and are checked against the local data before use.
//
// MP_SYNC_SHOTS=0 switches all of this off; MP_LOG_SHOTS=1 writes every launch to out\mp_shots_<pid>.txt.
#include "common.h"
#include "env_value.h"
#include "netlog.h"
#include "protocol.h"
#include "vitals.h"
#include "reliable_inbox.h"
#include "hooks.h"
#include "detour.h"
#include <intrin.h>
#include <share.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <deque>
#include <vector>

typedef void (*Launch)(char *sp, char *rec, u64 flags, float *pos, void *obj5, u32 arg6);
typedef void (*Explode)(char *sp, char *rec);
static Launch g_launch;
static Explode g_explode;
static ObjMan *g_om;
static char *g_shellphys;
static DWORD g_main_thread;
static bool g_injecting, g_install_done, g_enabled = true;
static FILE *g_file;
static u32 g_seq, g_epoch, g_dumped;
static u64 g_stat_own, g_stat_dropped, g_stat_injected, g_stat_refused, g_stat_wait_refused, g_stat_free, g_stat_sent_bytes, g_stat_repeats;
struct Pending { ShotRec rec; DWORD sent; unsigned char tries; };
static std::deque<Pending> g_out;               // launched here, not yet acknowledged by the other game
static reliable::Inbox<ShotRec> g_in;           // receipt is separate from completion / acknowledgement
static reliable::HeadWait g_head_wait;          // only the first unfinished gameplay event is timed
// Non-tactical processes never run shots_sim_step, but preserve their old receive/ACK semantics.
static std::vector<ShotRec> g_in_legacy;
static u32 g_epoch_in_legacy, g_last_seq_in_legacy;
static void (*g_ack_send)(const void *, int);

// launch record: pointers at +0x38 (ammunition), +0x40 (weapon), +0x48 (firing object), +0x50 (object),
// +0x60 (description)
enum { L_AMMO = 0x38, L_WEAPON = 0x40, L_SHOOTER = 0x48, L_OBJ2 = 0x50, L_DESC3 = 0x60, L_SIZE = 0x90 };

static u32 desc_id(void *p) { return p ? ((u32 (*)(ObjMan *, void *))g_om->vtbl[56])(g_om, p) : 0xffffffffu; }
static void *desc_ptr(u32 id) { return id == 0xffffffffu ? 0 : ((void *(*)(ObjMan *, u32))g_om->vtbl[57])(g_om, id); }

static bool is_object(void *p)
{
    if (!readable(p, sizeof(Object))) return false;
    Object *o = (Object *)p;
    return o->objman == g_om && readable(o->iface, 4);
}

static u64 object_key(void *p)
{
    u64 key = 0;
    if (p && is_object(p)) session_unit_key((Object *)p, &key);
    return key;
}

// what identifies an ammunition description besides its id: its first two numbers and its type code
static void ammo_check(void *ammo, float *two, u32 *code)
{
    two[0] = two[1] = 0;
    *code = 0;
    if (!readable(ammo, 0xc8)) return;
    memcpy(two, ammo, 8);
    memcpy(code, (char *)ammo + 0xc4, 4);
}

static const char *name_of(void *p)
{
    return !p ? "-" : is_object(p) ? ((Object *)p)->full : "?";
}

static void log_launch(const char *who, char kind, const char *rec, u64 flags, void *obj5, void *caller)
{
    if (!g_file) return;
    const float *f = (const float *)rec;
    void *ammo = *(void **)(rec + L_AMMO), *weapon = *(void **)(rec + L_WEAPON), *shooter = *(void **)(rec + L_SHOOTER);
    HMODULE m = 0;
    char mod[MAX_PATH] = "?";
    if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCSTR)caller, &m) && m)
        GetModuleFileNameA(m, mod, MAX_PATH);
    const char *base = strrchr(mod, '\\');
    u32 aflags = readable(ammo, 0xc8) ? *(u32 *)((char *)ammo + 0xc0) : 0, acode = readable(ammo, 0xc8) ? *(u32 *)((char *)ammo + 0xc4) : 0;
    fprintf(g_file, "%c %llu %s %s ammo=%u weapon=%u aflags=%08x acode=%08x flags=%llx pos=%.2f,%.2f,%.2f dir=%.4f,%.4f,%.4f speed=%.1f n=%u obj2=%s obj5=%s from=%s+0x%llx\n",
            kind, session_battle_ms(), who, name_of(shooter), desc_id(ammo), desc_id(weapon), aflags, acode, flags,
            f[0], f[1], f[2], f[4], f[5], f[6], kind == 'L' ? f[0xc] : 0.0f, kind == 'L' ? *(u32 *)(rec + 0x70) : 0,
            kind == 'L' ? name_of(*(void **)(rec + L_OBJ2)) : "-", name_of(obj5), base ? base + 1 : mod, m ? (u64)((char *)caller - (char *)m) : 0);
    fflush(g_file);
    if (g_dumped < 6 && kind == 'L') {
        g_dumped++;
        char line[3 * L_SIZE + 8];
        size_t w = 0;
        for (int i = 0; i < L_SIZE; i++) w += sprintf_s(line + w, sizeof line - w, "%02x%s", (unsigned char)rec[i], (i & 7) == 7 ? " " : "");
        mp_log("shots: launch record of %s: %s", name_of(shooter), line);
    }
}

static bool replicating(void)
{
    return g_enabled && session_connected() && session_role() != ROLE_NONE;
}

// Whose launch is it? A unit's launch belongs to the game that owns the unit; anything else (a launch with
// no firing unit behind it) to the host.
static bool mine(void *shooter)
{
    if (shooter && is_object(shooter)) return session_owns((Object *)shooter);
    return session_role() == ROLE_HOST;
}

static void queue_shot(char kind, const char *rec, u64 flags, const float *pos, void *obj5, u32 arg6)
{
    ShotRec s = {};
    if (!g_epoch) g_epoch = (GetTickCount() ^ (GetCurrentProcessId() << 16)) | 1;
    s.seq = ++g_seq;
    s.kind = kind == 'L' ? 0 : 1;
    s.flags = (unsigned char)flags;
    s.has_pos = pos != 0;
    void *ammo = *(void **)(rec + L_AMMO);
    s.ammo = desc_id(ammo);
    s.weapon = desc_id(*(void **)(rec + L_WEAPON));
    ammo_check(ammo, s.ammo_check, &s.ammo_code);
    if (kind == 'L') {
        s.desc3 = desc_id(*(void **)(rec + L_DESC3));
        s.shooter = object_key(*(void **)(rec + L_SHOOTER));
        s.obj2 = object_key(*(void **)(rec + L_OBJ2));
        s.obj5 = object_key(obj5);
        s.arg6 = arg6;
        if (pos) memcpy(s.pos, pos, 16);
        memcpy(s.launch, rec, L_SIZE);
        memset(s.launch + L_AMMO, 0, 0x20);         // the four pointers
        memset(s.launch + L_DESC3, 0, 8);
    } else {
        s.desc3 = 0xffffffffu;
        memcpy(s.launch, rec, 0x40);                // an explosion is a place and the two descriptions
    }
    g_out.push_back({ s, 0, 0 });
}

// Another kind of record for the same channel (trench.cpp).
void shots_queue(ShotRec &s)
{
    if (!g_epoch) g_epoch = (GetTickCount() ^ (GetCurrentProcessId() << 16)) | 1;
    s.seq = ++g_seq;
    g_out.push_back({ s, 0, 0 });
}

// Do not let a new producer add unbounded events to the legacy queue. A refused admission
// must not consume a sequence number or create a gap in the other producer's stream.
bool shots_try_queue(ShotRec &s, size_t limit)
{
    if (!limit || g_out.size() >= limit || g_seq == 0xffffffffu) return false;
    shots_queue(s);
    return true;
}

bool shots_queue_latest(ShotRec &s, size_t limit)
{
    if (s.kind != 8 || !s.shooter || !limit) return false;
    // Never mutate a record already sent, and never move a post-hit snapshot across a hit or a
    // launch. That preserves causal order while coalescing an unsent suffix of snapshot records.
    for (auto it = g_out.rbegin(); it != g_out.rend(); ++it) {
        if (it->sent || it->rec.kind != s.kind) break;
        if (it->rec.shooter == s.shooter) {
            s.seq = it->rec.seq;
            it->rec = s;
            return true;
        }
    }
    return shots_try_queue(s, limit);
}

static void launch_hook(char *sp, char *rec, u64 flags, float *pos, void *obj5, u32 arg6)
{
    // bit 1: asked for from another thread; the shell manager queues it and launches it later through
    // this same function, on the simulation's thread
    if ((flags & 2) || g_injecting || GetCurrentThreadId() != g_main_thread || !readable(rec, L_SIZE)) {
        g_launch(sp, rec, flags, pos, obj5, arg6);
        return;
    }
    g_shellphys = sp;
    void *shooter = *(void **)(rec + L_SHOOTER);
    bool repl = replicating(), own = !repl || mine(shooter);
    log_launch(!repl ? "free" : own ? "own" : "dropped", 'L', rec, flags, obj5, _ReturnAddress());
    if (!repl) { g_stat_free++; g_launch(sp, rec, flags, pos, obj5, arg6); return; }
    if (!own) { g_stat_dropped++; return; }
    g_stat_own++;
    g_launch(sp, rec, flags, pos, obj5, arg6);
    if (*(void **)(rec + L_AMMO) && *(void **)(rec + L_WEAPON)) queue_shot('L', rec, flags, pos, obj5, arg6);
}

// Virtual slot 4 (basic_physic+0x19260): an ammunition going off at a place, without a flight - only
// watched for now (MP_LOG_SHOTS), to learn what uses it.
static void explode_hook(char *sp, char *rec)
{
    if (g_file && GetCurrentThreadId() == g_main_thread && readable(rec, 0x50)) {
        char fake[L_SIZE] = {};
        memcpy(fake, rec, 0x30);
        memcpy(fake + L_AMMO, rec + 0x48, 8);
        memcpy(fake + L_WEAPON, rec + 0x40, 8);
        log_launch("explosion", 'X', fake, 0, 0, _ReturnAddress());
    }
    g_explode(sp, rec);
}

void shots_install(ObjMan *om)
{
    static const unsigned char launch_sig[] = { 0x48, 0x89, 0x54, 0x24, 0x10, 0x55, 0x53, 0x56, 0x57, 0x41, 0x54, 0x41, 0x56,
                                                0x48, 0x8d, 0xac, 0x24, 0xe8, 0xfd, 0xff, 0xff };
    static const unsigned char explode_sig[] = { 0x48, 0x89, 0x5c, 0x24, 0x08, 0x57, 0x48, 0x81, 0xec, 0x70, 0x02, 0x00, 0x00, 0x48, 0x8b, 0xf9 };
    if (g_install_done) return;
    HMODULE m = GetModuleHandleA("basic_physic.codelib");
    if (!m) return;
    g_install_done = true;
    g_om = om;
    g_main_thread = GetCurrentThreadId();
    int value;
    if (env_integer("MP_SYNC_SHOTS", &value)) g_enabled = value != 0;
    if (env_integer("MP_LOG_SHOTS", &value) && value) {
        char path[MAX_PATH];
        sprintf_s(path, "%s\\mp_shots_%lu.txt", g_out_dir, GetCurrentProcessId());
        g_file = _fsopen(path, "w", _SH_DENYNO);
    }
    if (!g_enabled && !g_file) { mp_log("shots: each game fires for itself (MP_SYNC_SHOTS=0)"); return; }
    const HookSig launch_where[] = { { 0x18430, launch_sig, sizeof launch_sig } };
    const HookSig explode_where[] = { { 0x19260, explode_sig, sizeof explode_sig } };
    char *launch = hook_resolve_checked("basic_physic.codelib", launch_where, 1, "shell launch");
    if (!launch) { mp_log("shots: launch hook refused, each game fires for itself"); g_enabled = false; return; }
    g_launch = (Launch)install_detour(launch, sizeof launch_sig, launch_hook);
    if (g_file) {
        char *explode = hook_resolve_checked("basic_physic.codelib", explode_where, 1, "shell explosion log");
        if (explode) g_explode = (Explode)install_detour(explode, sizeof explode_sig, explode_hook);
    }
    mp_log("shots: %s%s", !g_launch ? "could not be hooked" : g_enabled ? "a unit's shots are its owner's: sent to the other game, not made up locally" : "watched only",
           g_file ? ", every launch is written to mp_shots" : "");
    if (!g_launch) g_enabled = false;
}

// Every frame: what the other game has not acknowledged goes out - new launches at once, older ones again
// after a pause. Nothing is given up: a launch stays in the queue until the other game has it.
void shots_frame(void (*send)(const void *, int))
{
    if (g_out.empty()) return;
    if (!(session_connected() && session_role() != ROLE_NONE)) { g_out.clear(); return; }
    const u32 owner = vitals_epoch(), target = vitals_peer_epoch();
    if (!owner || !target) return; // wait for both tactical process identities
    const size_t per = 5, window = 60;
    DWORD now = GetTickCount();
    size_t n = g_out.size() < window ? g_out.size() : window, at = 0;
    while (at < n) {
        // a datagram starts at the first launch that is due and carries the ones after it; a launch the
        // other game keeps missing is repeated with a growing pause instead of six times a second
        if (g_out[at].sent) {
            const Pending &p = g_out[at];
            DWORD wait = 150u << (p.tries < 4 ? p.tries : 4);        // 150, 300, 600, 1200, 1200 ms
            if (now - p.sent < wait) { at++; continue; }
        }
        unsigned char packet[sizeof(ShotHead) + per * sizeof(ShotRec)];
        size_t k = n - at < per ? n - at : per;
        ShotHead h = { make_head(PKT_SHOT, (int)k), g_epoch, owner, target };
        memcpy(packet, &h, sizeof h);
        for (size_t i = 0; i < k; i++) {
            memcpy(packet + sizeof h + i * sizeof(ShotRec), &g_out[at + i].rec, sizeof(ShotRec));
            if (g_out[at + i].sent) g_stat_repeats++;
            if (g_out[at + i].tries < 4) g_out[at + i].tries++;
            g_out[at + i].sent = now ? now : 1;
        }
        send(packet, (int)(sizeof h + k * sizeof(ShotRec)));
        g_stat_sent_bytes += sizeof h + k * sizeof(ShotRec);
        netlog_count("shots_sent", (long long)k);
        at += k;
    }
}

static void shots_ack(void (*send)(const void *, int))
{
    if (!send || !g_in.current_epoch() || !vitals_epoch() || !vitals_peer_epoch()) return;
    ShotAck ack = { make_head(PKT_SHOT_ACK), g_in.current_epoch(), g_in.committed(), vitals_peer_epoch(), vitals_epoch() };
    send(&ack, sizeof ack);
}

#ifdef NETV21_TEST_HOOKS
static bool g_test_wait_time_set;
static DWORD g_test_wait_time;
void shots_test_wait_time(DWORD now) { g_test_wait_time_set = true; g_test_wait_time = now; }
void shots_test_wait_real_time() { g_test_wait_time_set = false; }
#endif

static DWORD shots_wait_now()
{
#ifdef NETV21_TEST_HOOKS
    if (g_test_wait_time_set) return g_test_wait_time;
#endif
    return GetTickCount();
}

// A timeout is a terminal refusal, not successful replay. ACK can then pass the refused
// event; a retransmission must never apply it later. Only runs on the simulation thread.
static bool shots_refuse_expired(const ShotRec &s, const char *reason)
{
    const DWORD limit_ms = 10000;
    if (!g_head_wait.expired(g_in.current_epoch(), s.seq, shots_wait_now(), limit_ms)) return false;
    ++g_stat_refused;
    ++g_stat_wait_refused;
    netlog_count("shot_wait_refused");
    mp_log("shots: event %u (kind %u, epoch %u) refused without replay after waiting %lu ms: %s",
           s.seq, (unsigned)s.kind, g_in.current_epoch(), (unsigned long)limit_ms, reason);
    g_head_wait.reset();
    return true;
}

void shots_on_packet(const unsigned char *data, int size, void (*send)(const void *, int))
{
    if (!data || size < (int)sizeof(Head)) return;
    const Head *head = (const Head *)data;
    if (head->type == PKT_SHOT_ACK) {
        if (size != (int)sizeof(ShotAck) || head->magic != k_magic || head->flags || head->count) return;
        const ShotAck *a = (const ShotAck *)data;
        if (!vitals_epoch() || !vitals_peer_epoch() || a->owner != vitals_epoch() || a->target != vitals_peer_epoch() ||
            a->epoch != g_epoch || a->seq > g_seq) return;
        // An event that has never been sent cannot have been acknowledged by the peer.
        while (!g_out.empty() && g_out.front().sent && g_out.front().rec.seq <= a->seq) g_out.pop_front();
        return;
    }
    if (head->type != PKT_SHOT || head->magic != k_magic || head->flags || !head->count || head->count > 5 ||
        size < (int)sizeof(ShotHead) || size != (int)(sizeof(ShotHead) + head->count * sizeof(ShotRec))) return;
    const ShotHead *h = (const ShotHead *)data;
    if (!vitals_epoch() || !vitals_peer_epoch() || h->owner != vitals_peer_epoch() || h->target != vitals_epoch()) return;
    const ShotRec *recs = (const ShotRec *)(h + 1);
    u32 old_epoch = g_in.current_epoch();
    size_t old_pending = g_in.pending();
    if (!g_in.epoch(h->epoch, recs[0].seq)) return;
    if (old_epoch != h->epoch) {
        g_head_wait.reset();
        if (old_epoch) {
            // Sequence numbers can restart while the enclosing tactical process epochs stay the
            // same. Never use the old stream's number tables or handler deadlines for new events.
            xlate_resend();
            hits_reset_peer();
            anim_reset_wait();
            trench_forget_peer();
            mp_log("shots: peer restarted its event stream; number tables invalidated, %zu unfinished events discarded", old_pending);
        }
    }
    g_ack_send = send;
    unsigned long long fresh = 0;
    for (unsigned i = 0; i < head->count; i++)
        if (g_in.receive(recs[i]) == reliable::Inbox<ShotRec>::accepted) fresh++;
    netlog_count("shots_in", (long long)fresh);
    // ACK only what the simulation has completed, not the records just copied into the queue.
    shots_ack(send);
}

// Receive and acknowledge exactly as before the tactical reliability changes when in a non-tactical
// process. That mode does not run the shot producer or consumer; keep its historical packet behavior.
void shots_on_packet_legacy(const unsigned char *data, int size, void (*send)(const void *, int))
{
    const Head *head = (const Head *)data;
    if (head->type == PKT_SHOT_ACK) {
        if (size < (int)sizeof(ShotAck)) return;
        const ShotAck *a = (const ShotAck *)data;
        if (a->epoch != g_epoch) return;
        while (!g_out.empty() && g_out.front().rec.seq <= a->seq) g_out.pop_front();
        return;
    }
    if (size < (int)sizeof(ShotHead) || size < (int)(sizeof(ShotHead) + head->count * sizeof(ShotRec))) return;
    const ShotHead *h = (const ShotHead *)data;
    const ShotRec *recs = (const ShotRec *)(h + 1);
    if (h->epoch != g_epoch_in_legacy) { g_epoch_in_legacy = h->epoch; g_last_seq_in_legacy = 0; }
    unsigned long long fresh = 0;
    for (unsigned i = 0; i < head->count; i++) {
        if (recs[i].seq != g_last_seq_in_legacy + 1) continue;
        g_last_seq_in_legacy = recs[i].seq;
        g_in_legacy.push_back(recs[i]);
        fresh++;
    }
    netlog_count("shots_in", (long long)fresh);
    ShotAck ack = { make_head(PKT_SHOT_ACK), g_epoch_in_legacy, g_last_seq_in_legacy };
    send(&ack, sizeof ack);
}

// A new connection: both directions start again.
void shots_reset(void)
{
    g_in.reset();
    g_head_wait.reset();
    g_in_legacy.clear();
    g_ack_send = 0;
    g_out.clear();
    g_seq = 0;
    g_epoch = (GetTickCount() ^ (GetCurrentProcessId() << 16) ^ (g_epoch * 2654435761u)) | 1;
    hits_reset_peer();
    anim_reset_peer();
    trench_forget_peer();
    trench_resend();
    xlate_resend();
}

// Before a simulation step: the other game's launches are carried out here.
void shots_sim_step(ObjMan *om)
{
    if (g_in.empty()) return;
    // Tables and trenches do not depend on the shell manager. Handle them once even if an earlier
    // shot must wait; the cumulative ACK still cannot move past that earlier shot.
    for (auto &item : g_in.items()) {
        if (item.complete) continue;
        const ShotRec &s = item.record;
        if (s.kind == 2) { trench_received(s); item.complete = true; }
        else if (s.kind == 3 || s.kind == 4) { xlate_received(s, om); item.complete = true; }
        else if (s.kind != 0 && s.kind != 7 && s.kind != 8) {
            // kind 1 was never emitted by this module; reject unknown records explicitly.
            g_stat_refused++;
            item.complete = true;
        }
    }
    if (g_launch && g_enabled && !g_shellphys && readable(om, sizeof *om) &&
        readable(om->vtbl, 10 * sizeof(void *)) && om->vtbl[9])
        g_shellphys = (char *)engine_find(om, "i_shellphys", "shellphys");
    g_om = om;
    for (auto &item : g_in.items()) {
        if (item.complete) continue;
        const ShotRec &s = item.record;
        if (s.kind == 7 || s.kind == 8) {
            // A false return must have no engine side effects: it means "try again".
            bool done = s.kind == 7 ? hits_received(s, om) : anim_received(s, om);
            if (!done && !shots_refuse_expired(s, s.kind == 7 ? "hit not ready" : "animation not ready")) break;
            g_head_wait.reset();
            item.complete = true;
            continue;
        }
        if (s.kind != 0) continue;
        if (!g_install_done || (g_launch && g_enabled && !g_shellphys)) {
            if (shots_refuse_expired(s, "shell manager not ready")) { item.complete = true; continue; }
            static DWORD last_wait;
            DWORD now = GetTickCount();
            if (!last_wait || now - last_wait >= 5000) {
                last_wait = now ? now : 1;
                mp_log("shots: %zu incoming events are waiting for the shell manager", g_in.pending());
            }
            break;
        }
        if (!g_launch || !g_enabled) {
            // This build explicitly refused/disabled its hook; do not pretend the shot was played.
            g_stat_refused++;
            mp_log("shots: event %u refused without replay: launch hook unavailable", s.seq);
            g_head_wait.reset();
            item.complete = true;
            continue;
        }
        u32 ammo_id, weapon_id, desc3_id;
        if (!xlate_desc_checked(s.ammo, &ammo_id) || !xlate_desc_checked(s.weapon, &weapon_id) ||
            !xlate_desc_checked(s.desc3, &desc3_id)) {
            if (shots_refuse_expired(s, "description table incomplete or ID not matched")) { item.complete = true; continue; }
            break;
        }
        if (!readable(om, sizeof *om) || !readable(om->vtbl, 58 * sizeof(void *)) || !om->vtbl[57]) {
            if (shots_refuse_expired(s, "description lookup unavailable")) { item.complete = true; continue; }
            break;
        }
        void *ammo = desc_ptr(ammo_id), *weapon = desc_ptr(weapon_id), *desc3 = desc_ptr(desc3_id);
        float two[2];
        u32 code;
        ammo_check(ammo, two, &code);
        Object *shooter = s.shooter ? session_unit(s.shooter) : 0;
        Object *obj2 = s.obj2 ? session_unit(s.obj2) : 0;
        Object *obj5 = s.obj5 ? session_unit(s.obj5) : 0;
        // Never replace an identified but absent local object with a null pointer, or use an
        // unreadable mapped description. A fingerprint match is still only a heuristic.
        if (!readable(ammo, 0xc8) || !readable(weapon, 0x60) ||
            (s.desc3 != 0xffffffffu && (desc3_id == 0xffffffffu || !readable(desc3, 0x60))) ||
            memcmp(two, s.ammo_check, 8) || code != s.ammo_code ||
            (s.shooter && !readable(shooter, sizeof(Object))) || (s.obj2 && !readable(obj2, sizeof(Object))) ||
            (s.obj5 && !readable(obj5, sizeof(Object)))) {
            g_stat_refused++;
            mp_log("shots: event %u refused without replay: description or unit unavailable/mismatched", s.seq);
            g_head_wait.reset();
            item.complete = true;
            continue;
        }
        char rec[L_SIZE];
        memcpy(rec, s.launch, L_SIZE);
        *(void **)(rec + L_AMMO) = ammo;
        *(void **)(rec + L_WEAPON) = weapon;
        *(void **)(rec + L_SHOOTER) = shooter;
        *(void **)(rec + L_OBJ2) = obj2;
        *(void **)(rec + L_DESC3) = desc3;
        float pos[4];
        memcpy(pos, s.pos, 16);
        log_launch("received", 'L', rec, s.flags, 0, 0);
        g_injecting = true;
        g_launch(g_shellphys, rec, s.flags & ~2u, s.has_pos ? pos : 0, obj5, s.arg6);
        g_injecting = false;
        g_stat_injected++;
        g_head_wait.reset();
        item.complete = true;
    }
    u32 before = g_in.committed();
    if (g_in.commit() != before) shots_ack(g_ack_send);
}

#ifdef NETV21_TEST_HOOKS
// The Windows no-game integration test supplies a fake shell manager; none of these seams is
// compiled into the game module.
void shots_test_engine(ObjMan *om, char *shell, Launch launch)
{
    g_om = om;
    g_shellphys = shell;
    g_launch = launch;
    g_enabled = true;
    g_install_done = true;
}
size_t shots_test_outstanding() { return g_out.size(); }
size_t shots_test_legacy_pending() { return g_in_legacy.size(); }
u32 shots_test_epoch() { return g_epoch; }
#endif

void shots_report(void)
{
    if (!g_stat_own && !g_stat_dropped && !g_stat_injected && !g_stat_refused) return;
    mp_log("shots: 5 s: %llu fired by this game's units and sent (%.1f KB/s, %llu repeats, %zu waiting for an answer), %llu received and fired here, %llu local ones for the other player's units dropped, %llu refused without replay (%llu timed out)",
           g_stat_own, g_stat_sent_bytes / 5120.0, g_stat_repeats, g_out.size(), g_stat_injected, g_stat_dropped, g_stat_refused, g_stat_wait_refused);
    g_stat_own = g_stat_dropped = g_stat_injected = g_stat_refused = g_stat_wait_refused = g_stat_sent_bytes = g_stat_repeats = 0;
}

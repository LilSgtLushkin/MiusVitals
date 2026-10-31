// Experimental owner-authoritative hits. Only the checked human/vehicle vtable entries are
// patched; the game still performs the hit normally on its target's owner. The remote game
// replays a pointer-free record on its simulation thread. No game binary or save is modified.
// The calling convention and object layouts are recovered, not verified in the real game.
#include "common.h"
#include "env_value.h"
#include "hit_wire.h"
#include "hooks.h"
#include <atomic>
#include <stdint.h>
#include <string.h>

typedef uintptr_t (*HitFn)(Object *, void *, u32, Object *, u32, bool);
static std::atomic<HitFn> g_human, g_vehicle;
static std::atomic<bool> g_installed;
static std::atomic<unsigned long long> g_wrong_thread;
static void **g_human_slot, **g_vehicle_slot;
static ObjMan *g_om;
static DWORD g_thread;                   // thread id, not a clock
static u64   g_wait_at;
static u32 g_wait_seq;
static bool g_attempted, g_operational[2] = {true, true}, g_log, g_injecting;
static unsigned long long g_sent, g_applied, g_suppressed, g_refused, g_overflow;

static bool writable(void *p, size_t n)
{
    MEMORY_BASIC_INFORMATION m;
    if (!p || !VirtualQuery(p, &m, sizeof m) || m.State != MEM_COMMIT || (m.Protect & PAGE_GUARD)) return false;
    DWORD prot = m.Protect & 0xff;
    if (prot != PAGE_READWRITE && prot != PAGE_WRITECOPY && prot != PAGE_EXECUTE_READWRITE && prot != PAGE_EXECUTE_WRITECOPY) return false;
    uintptr_t a = (uintptr_t)p, b = (uintptr_t)m.BaseAddress;
    return a >= b && a - b <= m.RegionSize && n <= m.RegionSize - (a - b);
}

static bool unit(Object *o, u64 *key)
{
    if (!readable(o, sizeof(Object)) || o->objman != g_om || !memchr(o->full, 0, sizeof o->full) || !readable(o->iface, 8)) return false;
    if (memcmp(o->iface, "i_human", 8) && memcmp(o->iface, "i_techn", 8)) return false;
    return session_unit_key(o, key) && hit_wire::unit_key(*key, false) && session_unit(*key) == o;
}

// A hit may contain a source object that is not an indexed combat unit. v20 also represents
// such pointers as null; keeping them as raw addresses across processes would be unsafe.
static u64 optional_unit(Object *o)
{
    u64 key = 0;
    return o && unit(o, &key) ? key : 0;
}

static bool description_id(void *desc, u32 *out)
{
    if (!desc) { *out = 0xffffffffu; return true; }
    if (!readable(desc, 0x60) || !readable(g_om, sizeof *g_om) || !readable(g_om->vtbl, 58 * sizeof(void *)) || !g_om->vtbl[56]) return false;
    *out = ((u32 (*)(ObjMan *, void *))g_om->vtbl[56])(g_om, desc);
    return *out != 0xffffffffu;
}

static bool component_rows(Object *target, const unsigned char **rows, u32 *count)
{
    const char *t = (const char *)target;
    if (!readable(t, 0x860)) return false;
    memcpy(rows, t + 0x850, sizeof *rows);
    memcpy(count, t + 0x85c, sizeof *count);
    return *count && *count <= hit_wire::max_parts && readable(*rows, (size_t)*count * 0x60);
}

static void disable_sync(bool vehicle, const char *reason)
{
    unsigned category = vehicle ? 1u : 0u;
    if (g_operational[category]) mp_log("hit: %s synchronization stopped for this session: %s; local game remains in control",
                                        vehicle ? "vehicle" : "human", reason);
    g_operational[category] = false;
}
static void disable_all(const char *reason) { disable_sync(false, reason); disable_sync(true, reason); }

static uintptr_t capture(bool vehicle, Object *target, void *record, u32 a3, Object *source, u32 a5, bool a6)
{
    HitFn original = (vehicle ? g_vehicle : g_human).load(std::memory_order_acquire);
    if (!original) return 0; // never deliberately publish a slot before its original
    if (GetCurrentThreadId() != g_thread) {
        g_wrong_thread.fetch_add(1, std::memory_order_relaxed);
        return original(target, record, a3, source, a5, a6);
    }
    unsigned feature = vehicle ? PEER_VEHICLE_HIT_SYNC : PEER_HIT_SYNC;
    if (g_injecting || !g_operational[vehicle ? 1 : 0] || !g_installed.load(std::memory_order_acquire) || !session_connected() ||
        !(session_peer_features() & feature) || !xlate_ready())
        return original(target, record, a3, source, a5, a6);
    u64 key = 0;
    if (!unit(target, &key) || (key >> 32) != (vehicle ? 1u : 2u) || !readable(record, hit_wire::engine_size))
        return original(target, record, a3, source, a5, a6);
    if (!session_owns(target)) {
        ++g_suppressed;
        return 0;
    }
    const size_t seed_offset = vehicle ? 0xb64 : 0x2768;
    if (!readable(target, seed_offset + sizeof(u32))) {
        disable_sync(vehicle, "target layout is unreadable");
        return original(target, record, a3, source, a5, a6);
    }
    void *d1 = 0, *d2 = 0; Object *object = 0;
    memcpy(&d1, (char *)record + 0x68, sizeof d1);
    memcpy(&d2, (char *)record + 0x70, sizeof d2);
    memcpy(&object, (char *)record + 0x78, sizeof object);
    u32 id1 = 0, id2 = 0, seed = 0;
    if (!description_id(d1, &id1) || !description_id(d2, &id2)) {
        disable_sync(vehicle, "hit descriptions could not be identified");
        return original(target, record, a3, source, a5, a6);
    }
    memcpy(&seed, (char *)target + seed_offset, sizeof seed);
    ShotRec s;
    hit_wire::encode(s, vehicle, key, optional_unit(source), optional_unit(object), id1, id2, a3, a5, a6, seed, record);
    u32 part = 0; memcpy(&part, (char *)record + 0x60, 4);
    if (vehicle && part != 0xffffffffu) {
        const unsigned char *rows = nullptr; u32 count = 0; hit_wire::PartHint hint = {};
        if (!component_rows(target, &rows, &count)) {
            mp_log("hit: component failure target=%llx type=%u part=%08x count=%u: list header/rows unavailable, empty or oversized",
                   key, vehicle ? 1u : 0u, part, count);
            disable_sync(vehicle, "component list cannot be matched safely");
            return original(target, record, a3, source, a5, a6);
        }
        if (!hit_wire::make_hint(rows, count, part, &hint)) {
            // make_hint writes a nonzero shape before searching for the part. This separates a
            // rejected list shape (>6 groups, for example) from a missing part group, without
            // sending an untranslated renderer handle or changing the existing fail-safe policy.
            mp_log("hit: component failure target=%llx type=%u part=%08x count=%u: %s",
                   key, vehicle ? 1u : 0u, part, count,
                   hint.shape ? "part group absent from list" : "list shape rejected");
            disable_sync(vehicle, "component list cannot be matched safely");
            return original(target, record, a3, source, a5, a6);
        }
        hit_wire::put_hint(s, hint);
    }
    if (!shots_try_queue(s, hit_wire::queue_limit)) {
        ++g_overflow;
        disable_sync(vehicle, "bounded event queue is full or its sequence was exhausted");
    } else {
        ++g_sent;
        if (g_log) mp_log("hit: queued target=%llx type=%u kind=%u seq=%u", key, vehicle ? 1u : 0u, a3, s.seq);
    }
    return original(target, record, a3, source, a5, a6);
}

static uintptr_t human_hook(Object *o, void *r, u32 a3, Object *a4, u32 a5, bool a6)
{ return capture(false, o, r, a3, a4, a5, a6); }
static uintptr_t vehicle_hook(Object *o, void *r, u32 a3, Object *a4, u32 a5, bool a6)
{ return capture(true, o, r, a3, a4, a5, a6); }

void hits_install(ObjMan *om)
{
    if (g_attempted || !GetModuleHandleA("adv_physic.codelib")) return;
    g_attempted = true;
    g_om = om; g_thread = GetCurrentThreadId(); g_operational[0] = g_operational[1] = true;
    int value = 0;
    if (env_integer("MP_SYNC_HITS", &value) && !value) { disable_all("MP_SYNC_HITS=0"); return; }
    g_log = env_integer("MP_LOG_HITS", &value) && value;
    if (!hook_image_identity("adv_physic.codelib", 0x66f2a91e, 0xf2000)) {
        disable_all("unrecognised physics image/layout"); return;
    }
    static const unsigned char human_sig[] = { 0x48,0x8b,0xc4,0x48,0x89,0x58,0x20,0x55,0x56,0x41,0x57,0x48,0x83,0xec,0x60,0x80,0xbc,0x24,0xa8,0x00,0x00,0x00,0x00,0x4d,0x8b,0xf9,0x41,0x8b,0xe8,0x48,0x8b,0xf2 };
    static const unsigned char vehicle_sig[] = { 0x48,0x8b,0xc4,0x48,0x89,0x58,0x20,0x44,0x89,0x40,0x18,0x55,0x56,0x57,0x41,0x54,0x41,0x55,0x41,0x56,0x41,0x57,0x48,0x8d,0xa8,0xa8,0xfe,0xff,0xff,0x48,0x81,0xec };
    const HookSig hs[] = {{0x66c90, human_sig, sizeof human_sig}};
    const HookSig vs[] = {{0x43230, vehicle_sig, sizeof vehicle_sig}};
    char *h = hook_resolve_checked("adv_physic.codelib", hs, 1, "human hit");
    char *v = hook_resolve_checked("adv_physic.codelib", vs, 1, "vehicle hit");
    if (!h || !v) { disable_all("unrecognised hit functions"); return; }
    HookVtableMember hm[] = {{0, h}}, vm[] = {{0, v}};
    void **ht = hook_vtable_checked("adv_physic.codelib", 0xc6760, hm, 1, "human hit slot");
    void **vt = hook_vtable_checked("adv_physic.codelib", 0xc79d8, vm, 1, "vehicle hit slot");
    if (!ht || !vt || ht == vt) { disable_all("unrecognised or ambiguous hit slots"); return; }
    g_human.store((HitFn)h, std::memory_order_release);
    g_vehicle.store((HitFn)v, std::memory_order_release);
    if (!hook_swap_vtable_slot(ht, h, (void *)human_hook)) { disable_all("human slot changed or could not be patched"); return; }
    if (!hook_swap_vtable_slot(vt, v, (void *)vehicle_hook)) {
        if (!hook_swap_vtable_slot(ht, (void *)human_hook, h)) {
            g_human_slot = ht; // retain the address for another conditional rollback on stop
            mp_log("hit: WARNING: human slot rollback was not possible");
        }
        disable_all("vehicle slot changed or could not be patched"); return;
    }
    g_human_slot = ht; g_vehicle_slot = vt;
    g_installed.store(true, std::memory_order_release);
    mp_log("hit: experimental checked hooks installed; enabled only when both peers and number tables are ready");
}

unsigned hits_features(void)
{
    if (!g_installed.load(std::memory_order_acquire) || !xlate_ready()) return 0;
    return (g_operational[0] ? PEER_HIT_SYNC : 0) | (g_operational[1] ? PEER_VEHICLE_HIT_SYNC : 0);
}
bool hits_available(void) { return hits_features() != 0; }

// Wait briefly for a unit/table to appear without acknowledging a later event. Refusal is
// explicit after the deadline; otherwise a deleted unit could stall every shot indefinitely.
static bool wait_or_refuse(u32 seq, const char *why)
{
    u64 now = GetTickCount64();
    if (g_wait_seq != seq) { g_wait_seq = seq; g_wait_at = now; return false; }
    if (now - g_wait_at < 5000) return false;
    ++g_refused;
    mp_log("hit: event %u refused after waiting: %s", seq, why);
    g_wait_seq = 0;
    return true;
}

// Isolate the structured exception guard from C++ objects. Catch only access violations, report
// the failure and disable new replication. This cannot undo partial engine side effects.
static bool invoke(HitFn fn, Object *target, void *record, u32 a3, Object *source, u32 a5, bool a6)
{
    __try { fn(target, record, a3, source, a5, a6); return true; }
    __except (GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) { return false; }
}

bool hits_received(const ShotRec &s, ObjMan *om)
{
    if (!hit_wire::valid(s)) { ++g_refused; mp_log("hit: malformed event %u refused", s.seq); return true; }
    if (!g_installed.load(std::memory_order_acquire) || !g_operational[s.flags ? 1 : 0] || om != g_om) {
        ++g_refused; mp_log("hit: event %u refused: category hook unavailable", s.seq); return true;
    }
    if (!xlate_ready()) return wait_or_refuse(s.seq, "number tables are not ready");
    Object *target = session_unit(s.shooter);
    Object *source = s.obj2 ? session_unit(s.obj2) : 0;
    Object *object = s.obj5 ? session_unit(s.obj5) : 0;
    if (!target || (s.obj2 && !source) || (s.obj5 && !object)) return wait_or_refuse(s.seq, "unit not found");
    u64 key = 0;
    if (!unit(target, &key) || key != s.shooter || session_owns(target)) { ++g_refused; mp_log("hit: event %u refused: target/ownership mismatch", s.seq); return true; }
    u32 n1 = 0, n2 = 0;
    if (!xlate_desc_checked(s.ammo, &n1) || !xlate_desc_checked(s.weapon, &n2)) { ++g_refused; mp_log("hit: event %u refused: description number not mapped", s.seq); return true; }
    if (!readable(om, sizeof *om) || !readable(om->vtbl, 58 * sizeof(void *)) || !om->vtbl[57]) { ++g_refused; return true; }
    void *d1 = n1 == 0xffffffffu ? 0 : ((void *(*)(ObjMan *, u32))om->vtbl[57])(om, n1);
    void *d2 = n2 == 0xffffffffu ? 0 : ((void *(*)(ObjMan *, u32))om->vtbl[57])(om, n2);
    if ((n1 != 0xffffffffu && !readable(d1, 0x60)) || (n2 != 0xffffffffu && !readable(d2, 0x60))) { ++g_refused; mp_log("hit: event %u refused: local description unreadable", s.seq); return true; }
    char record[hit_wire::engine_size] = {};
    memcpy(record, s.launch, hit_wire::copied);
    u32 part = 0; memcpy(&part, record + 0x60, sizeof part);
    if (part != 0xffffffffu) {
        const unsigned char *rows = nullptr; u32 count = 0, local_part = 0; hit_wire::PartHint hint = {};
        memcpy(&hint, s.pos, sizeof hint);
        if (!component_rows(target, &rows, &count) || !hit_wire::translate(rows, count, part, hint, &local_part)) {
            ++g_refused; mp_log("hit: event %u refused: component layout/part %08x not matched", s.seq, part); return true;
        }
        memcpy(record + 0x60, &local_part, sizeof local_part);
    }
    memcpy(record + 0x68, &d1, sizeof d1);
    memcpy(record + 0x70, &d2, sizeof d2);
    memcpy(record + 0x78, &object, sizeof object);
    const size_t seed_offset = s.flags ? 0xb64 : 0x2768;
    if (!readable(target, seed_offset + sizeof(u32)) || !writable((char *)target + seed_offset, sizeof(u32))) { ++g_refused; mp_log("hit: event %u refused: target seed not writable", s.seq); return true; }
    HitFn original = (s.flags ? g_vehicle : g_human).load(std::memory_order_acquire);
    if (!original) { ++g_refused; return true; }
    memcpy((char *)target + seed_offset, &s.desc3, sizeof s.desc3);
    g_injecting = true;
    bool ok = invoke(original, target, record, s.arg6, source, s.ammo_code, s.has_pos != 0);
    g_injecting = false;
    g_wait_seq = 0;
    if (!ok) { ++g_refused; disable_sync(s.flags != 0, "the replayed engine call raised an access violation; effects may be partial"); }
    else { ++g_applied; if (g_log) mp_log("hit: replayed target=%llx kind=%u seq=%u", s.shooter, s.arg6, s.seq); }
    return true;
}

void hits_report(void)
{
    unsigned long long other = g_wrong_thread.exchange(0, std::memory_order_relaxed);
    if (g_sent || g_applied || g_suppressed || g_refused || g_overflow || other)
        mp_log("hit: 5 s: %llu queued, %llu replayed, %llu non-owner local calls held, %llu refused, %llu queue failures, %llu calls on other threads passed through", g_sent, g_applied, g_suppressed, g_refused, g_overflow, other);
    g_sent = g_applied = g_suppressed = g_refused = g_overflow = 0;
}

void hits_reset_peer(void) { g_wait_seq = 0; g_wait_at = 0; }

#ifdef NETV21_TEST_HOOKS
// No PE/vtable patching in the fixture. The production capture and replay paths are unchanged.
void hits_test_engine(ObjMan *om, HitFn human_fn, HitFn vehicle_fn)
{
    g_om = om; g_thread = GetCurrentThreadId(); g_operational[0] = g_operational[1] = true; g_attempted = true;
    g_human.store(human_fn); g_vehicle.store(vehicle_fn); g_installed.store(true);
    g_wait_seq = 0;
}
uintptr_t hits_test_capture(bool vehicle, Object *target, void *record, u32 a3, Object *source, u32 a5, bool a6)
{ return capture(vehicle, target, record, a3, source, a5, a6); }
void hits_test_expire_wait() { g_wait_at = GetTickCount64() - 5001; }
#endif

void hits_stop(void)
{
    g_installed.store(false, std::memory_order_release);
    if (g_human_slot && !hook_swap_vtable_slot(g_human_slot, (void *)human_hook, (void *)g_human.load())) mp_log("hit: human slot was changed by another module; not overwritten");
    if (g_vehicle_slot && !hook_swap_vtable_slot(g_vehicle_slot, (void *)vehicle_hook, (void *)g_vehicle.load())) mp_log("hit: vehicle slot was changed by another module; not overwritten");
    g_human_slot = g_vehicle_slot = 0;
    g_attempted = false; g_wait_seq = 0; g_om = 0; g_operational[0] = g_operational[1] = true;
}

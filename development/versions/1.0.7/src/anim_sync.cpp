// Experimental i_animch synchronization. A checked, instance-local vtable copy holds back
// local setters for a remote-owned soldier only after a recent owner snapshot has arrived.
// The snapshot contains selected scalars and a group name, never pointers. Game ABI and thread
// assumptions remain unverified without the game; unknown layouts are refused.
#include "common.h"
#include "anim_wire.h"
#include "animation_channel.h"
#include "env_value.h"
#include "hooks.h"
#include <atomic>
#include <stdint.h>
#include <string.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {
typedef uintptr_t (*U32Fn)(Object *, u32, u32);
typedef uintptr_t (*GroupFn)(Object *, u32, const char *);
typedef void (*FloatFn)(Object *, u32, float);
typedef void (*VoidU32Fn)(Object *, u32, u32);
typedef void (*VoidFn)(Object *, u32);
typedef void (*BoolFn)(Object *, u32, bool);
typedef void (*VectorFn)(Object *, u32, bool, float, float, float);
static const unsigned indices[] = {4,8,10,11,12,13,14,16,20,21,26};
static void *originals[41], *table_copy[42]; // copy[-1] preserves MSVC's complete-object locator
static Object *anim;
static ObjMan *manager;
static void **original_table;
static DWORD thread_id, last_scan, wait_at;
static u32 wait_seq;
static std::atomic<bool> installed;
static std::atomic<unsigned long long> other_thread;
static bool attempted, enabled = true;
static unsigned long long sent, applied, held, refused, full;
struct Index { u64 key; bool remote, ambiguous; };
struct Local { unsigned char last[anim_wire::snapshot_size]; DWORD at; bool has; };
static std::unordered_map<u32, Index> by_index;
static std::unordered_map<u64, Local> local;
static std::unordered_map<u64, DWORD> received_at;
static animation_channel::Channel transport;
static void observe_critical(u32 index);

static bool writable(void *p, size_t n)
{
    MEMORY_BASIC_INFORMATION m;
    if (!p || !VirtualQuery(p, &m, sizeof m) || m.State != MEM_COMMIT || (m.Protect & PAGE_GUARD)) return false;
    DWORD prot = m.Protect & 0xff;
    if (prot != PAGE_READWRITE && prot != PAGE_WRITECOPY && prot != PAGE_EXECUTE_READWRITE && prot != PAGE_EXECUTE_WRITECOPY) return false;
    uintptr_t a = (uintptr_t)p, b = (uintptr_t)m.BaseAddress;
    return a >= b && a - b <= m.RegionSize && n <= m.RegionSize - (a - b);
}
static bool human(Object *o, u64 *key, u32 *index)
{
    if (!readable(o, 0x1efc) || o->objman != manager || !readable(o->iface, 8) ||
        memcmp(o->iface, "i_human", 8) || !memchr(o->full, 0, sizeof o->full) ||
        !session_unit_key(o, key) || (*key >> 32) != 2 || session_unit(*key) != o) return false;
    memcpy(index, (char *)o + 0x1d60, 4);
    return *index < anim_wire::max_index;
}
static unsigned char *instance(u32 index)
{
    if (!anim || index >= anim_wire::max_index || !readable(anim, 0x148)) return nullptr;
    unsigned char *base = nullptr; memcpy(&base, (char *)anim + 0x140, sizeof base);
    uintptr_t at = (uintptr_t)base + (size_t)index * anim_wire::instance_size;
    if (!base || at < (uintptr_t)base || !readable((void *)at, anim_wire::instance_size)) return nullptr;
    return (unsigned char *)at;
}
// A group pointer points 0x3e bytes after its inline name in the checked engine layout.
static bool group_name(const unsigned char *inst, char out[28])
{
    memset(out, 0, 28);
    void *group = nullptr; memcpy(&group, inst + 0x70, sizeof group);
    if (!group) return true;
    uintptr_t p = (uintptr_t)group;
    if (p < 0x3e || !readable(group, 16) || !readable((const void *)(p - 0x3e), 28)) return false;
    const char *name = (const char *)(p - 0x3e);
    size_t n = 0;
    while (n < 28 && name[n]) {
        if ((unsigned char)name[n] < 0x20 || (unsigned char)name[n] > 0x7e) return false;
        ++n;
    }
    if (!n || n == 28) return false;
    memcpy(out, name, n);
    return true;
}
static bool block(u32 index)
{
    if (GetCurrentThreadId() != thread_id) {
        other_thread.fetch_add(1, std::memory_order_relaxed);
        return false;
    }
    if (!installed.load(std::memory_order_acquire) || !enabled || !session_connected() ||
        !(session_peer_features() & PEER_ANIM_SYNC) || !xlate_ready()) return false;
    auto it = by_index.find(index);
    if (it == by_index.end() || !it->second.remote || it->second.ambiguous) return false;
    auto ready = received_at.find(it->second.key);
    if (ready == received_at.end() || (DWORD)(GetTickCount() - ready->second) >= 3000) return false;
    ++held;
    return true;
}
static uintptr_t slot4(Object *o, u32 i, u32 v) {
    if (block(i)) return 0; uintptr_t result = ((U32Fn)originals[4])(o,i,v); observe_critical(i); return result;
}
static void slot8(Object *o, u32 i, float v) { if (!block(i)) ((FloatFn)originals[8])(o,i,v); }
static uintptr_t slot10(Object *o, u32 i, u32 v) {
    if (block(i)) return 0; uintptr_t result = ((U32Fn)originals[10])(o,i,v); observe_critical(i); return result;
}
static uintptr_t slot11(Object *o, u32 i, const char *v) {
    if (block(i)) return 0; uintptr_t result = ((GroupFn)originals[11])(o,i,v); observe_critical(i); return result;
}
static void slot12(Object *o, u32 i, u32 v) { if (!block(i)) { ((VoidU32Fn)originals[12])(o,i,v); observe_critical(i); } }
static void slot13(Object *o, u32 i) { if (!block(i)) { ((VoidFn)originals[13])(o,i); observe_critical(i); } }
static void slot14(Object *o, u32 i, bool v, float x, float y, float z) { if (!block(i)) ((VectorFn)originals[14])(o,i,v,x,y,z); }
static void slot16(Object *o, u32 i, float v) { if (!block(i)) ((FloatFn)originals[16])(o,i,v); }
static void slot20(Object *o, u32 i, float v) { if (!block(i)) ((FloatFn)originals[20])(o,i,v); }
static void slot21(Object *o, u32 i, u32 v) { if (!block(i)) ((VoidU32Fn)originals[21])(o,i,v); }
static void slot26(Object *o, u32 i, bool v) { if (!block(i)) ((BoolFn)originals[26])(o,i,v); }
static void *const hooks[] = {(void *)slot4,(void *)slot8,(void *)slot10,(void *)slot11,(void *)slot12,(void *)slot13,(void *)slot14,(void *)slot16,(void *)slot20,(void *)slot21,(void *)slot26};

// Code signatures are checked before the matching table is used. These cover every hook slot.
struct Sig { unsigned rva, size; unsigned char bytes[32]; };
static const Sig sigs[] = {
 {0xa4040,24,{0x8b,0xc2,0x48,0x8d,0x14,0x80,0x48,0x8b,0x81,0x40,0x01,0,0,0x48,0xc1,0xe2,0x06,0x44,0x89,0x84,0x02,0xa8,0,0}},
 {0xa1350,24,{0x8b,0xc2,0x48,0x8d,0x14,0x80,0x48,0x8b,0x81,0x40,0x01,0,0,0x48,0xc1,0xe2,0x06,0xf3,0x0f,0x11,0x94,0x02,0xd4,0}},
 {0xa13b0,24,{0x8b,0xc2,0x48,0x8d,0x14,0x80,0x48,0x8b,0x81,0x40,0x01,0,0,0x48,0xc1,0xe2,0x06,0x44,0x89,0x84,0x02,0x00,0x01,0}},
 {0xa4ec0,24,{0x48,0x89,0x5c,0x24,0x08,0x57,0x48,0x83,0xec,0x60,0x8b,0xc2,0x4d,0x8b,0xc8,0x48,0x8d,0x3c,0x80,0x48,0xc1,0xe7,0x06,0x48}},
 {0xa4ff0,24,{0x8b,0xc2,0x48,0x8d,0x14,0x80,0x48,0xc1,0xe2,0x06,0x48,0x03,0x91,0x40,0x01,0,0,0x48,0x8b,0x42,0x70,0x48,0x85,0xc0}},
 {0xa13d0,24,{0x8b,0xc2,0x48,0x8d,0x14,0x80,0x48,0xc1,0xe2,0x06,0x48,0x03,0x91,0x40,0x01,0,0,0xc7,0x82,0x04,0x01,0,0,0}},
 {0xa5030,24,{0xf3,0x0f,0x10,0x44,0x24,0x28,0xf3,0x0f,0x10,0x4c,0x24,0x30,0x8b,0xc2,0x48,0x8d,0x14,0x80,0x48,0xc1,0xe2,0x06,0x48,0x03}},
 {0xa1420,24,{0x8b,0xc2,0x48,0x8d,0x14,0x80,0x48,0x8b,0x81,0x40,0x01,0,0,0x48,0xc1,0xe2,0x06,0xf3,0x0f,0x11,0x94,0x02,0x28,0x01}},
 {0xa1440,24,{0x8b,0xc2,0x48,0x8d,0x14,0x80,0x48,0x8b,0x81,0x40,0x01,0,0,0x48,0xc1,0xe2,0x06,0xf3,0x0f,0x11,0x94,0x02,0x2c,0x01}},
 {0xa1460,24,{0x8b,0xc2,0x48,0x8d,0x14,0x80,0x48,0x8b,0x81,0x40,0x01,0,0,0x48,0xc1,0xe2,0x06,0x44,0x89,0x84,0x02,0x14,0x01}},
 {0xa14c0,24,{0x8b,0xc2,0x48,0x8d,0x14,0x80,0x48,0x8b,0x81,0x40,0x01,0,0,0x48,0xc1,0xe2,0x06,0x44,0x88,0x84,0x02,0x39,0x01}}
};
static_assert(sizeof indices / sizeof *indices == sizeof sigs / sizeof *sigs, "animation hook count");

static bool invoke_group(GroupFn fn, Object *a, u32 index, const char *name)
{
    __try { fn(a, index, name); return true; }
    __except (GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) { return false; }
}
static bool wait_or_refuse(u32 seq)
{
    DWORD now = GetTickCount();
    if (seq != wait_seq) { wait_seq = seq; wait_at = now; return false; }
    if ((DWORD)(now - wait_at) < 5000) return false;
    ++refused; wait_seq = 0; mp_log("anim: snapshot %u refused after waiting for its soldier", seq); return true;
}
static bool critical_change(const Local &previous, const ShotRec &rec)
{
    const unsigned char *a = previous.last, *b = rec.launch;
    // Frame/time and vectors are replaceable; group identity, activation, action identifiers and
    // human state transitions are retained in order until replayed and acknowledged.
    return !previous.has || memcmp(a + 4, b + 4, 8) || memcmp(a + 16, b + 16, 8) ||
           memcmp(a + 50, b + 50, 34);
}
static void publish(Object *object, u64 key, u32 index, DWORD now, bool only_critical)
{
    unsigned char *inst = instance(index);
    char group[28];
    if (!inst || !group_name(inst, group)) return;
    ShotRec rec; anim_wire::encode(rec, key, inst, group[0] ? group : nullptr, (const unsigned char *)object);
    if (!anim_wire::valid(rec)) return;
    Local &l = local[key];
    bool critical = critical_change(l, rec);
    bool changed = !l.has || memcmp(l.last, rec.launch, sizeof l.last) != 0;
    if (only_critical && !critical) return;
    // Under channel pressure only the cosmetic sampling rate drops, and it returns to the
    // normal cadence by itself once the outstanding queue has drained.
    if (!critical && l.has && (DWORD)(now - l.at) < (changed ? transport.changed_ms() : transport.unchanged_ms())) return;
    if (!transport.queue(rec, critical)) { ++full; return; }
    memcpy(l.last, rec.launch, sizeof l.last); l.has = true; l.at = now; ++sent;
}
static void observe_critical(u32 index)
{
    // The setter hooks also run on other threads; never touch session maps from those calls.
    if (GetCurrentThreadId() != thread_id || !anim_available() || !session_connected() ||
        !(session_peer_features() & PEER_ANIM_SYNC)) return;
    auto it = by_index.find(index);
    if (it == by_index.end() || it->second.remote || it->second.ambiguous) return;
    Object *object = session_unit(it->second.key); u64 key = 0; u32 actual = 0;
    if (!object || !human(object, &key, &actual) || key != it->second.key || actual != index || !session_owns(object)) return;
    publish(object, key, index, GetTickCount(), true);
}
} // namespace

void anim_install(ObjMan *om)
{
    if (attempted || !GetModuleHandleA("adv_game.codelib") || !readable(om, sizeof *om) ||
        !readable(om->vtbl, 10 * sizeof(void *)) || !om->vtbl[9]) return;
    Object *object = engine_find(om, "i_animch", "animch");
    if (!object) return; // the interface may appear later during tactical loading
    attempted = true; manager = om; thread_id = GetCurrentThreadId(); enabled = true;
    int value = 0;
    if (env_integer("MP_SYNC_ANIM", &value) && !value) { enabled = false; mp_log("anim: local animation only (MP_SYNC_ANIM=0)"); return; }
    if (!hook_image_identity("adv_game.codelib", 0x66f2d3a9, 0x178000)) {
        enabled = false; mp_log("anim: unrecognised game image/layout; hook refused"); return;
    }
    if (!readable(object, 0x148) || object->objman != om) { enabled = false; mp_log("anim: invalid i_animch object; hook refused"); return; }
    HookVtableMember members[sizeof indices / sizeof *indices];
    for (size_t i = 0; i < sizeof indices / sizeof *indices; ++i) {
        HookSig sig = {sigs[i].rva, sigs[i].bytes, sigs[i].size};
        char *code = hook_resolve_checked("adv_game.codelib", &sig, 1, "animation setter");
        if (!code) { enabled = false; mp_log("anim: a setter is unrecognised; all animation hooks refused"); return; }
        members[i] = {indices[i], code};
    }
    void **table = hook_vtable_checked("adv_game.codelib", 0x12f128, members, (unsigned)(sizeof members / sizeof *members), "animation");
    if (!table || object->vtbl != table || !readable(table - 1, sizeof table_copy)) {
        enabled = false; mp_log("anim: interface table does not match the checked table; hook refused"); return;
    }
    memcpy(table_copy, table - 1, sizeof table_copy);
    memcpy(originals, table, sizeof originals);
    for (size_t i = 0; i < sizeof indices / sizeof *indices; ++i) table_copy[1 + indices[i]] = hooks[i];
    anim = object; original_table = table;
    if (!hook_swap_vtable_slot((void **)&object->vtbl, table, table_copy + 1)) {
        anim = nullptr; original_table = nullptr; enabled = false;
        mp_log("anim: interface table changed or could not be replaced; hook refused"); return;
    }
    installed.store(true, std::memory_order_release);
    mp_log("anim: experimental instance-local hooks installed; enabled only when both peers and number tables are ready");
}

bool anim_available(void) { return enabled && installed.load(std::memory_order_acquire) && xlate_ready(); }

void anim_frame(ObjMan *om, DWORD now)
{
    if (!anim_available() || !session_connected() || !(session_peer_features() & PEER_ANIM_SYNC) || om != manager ||
        GetCurrentThreadId() != thread_id) return;
    if (transport.update_pressure())
        mp_log("anim: %llu critical events outstanding; animation sampling %s", (unsigned long long)transport.pending_events(),
               transport.slow() ? "temporarily reduced until the queue drains" : "back to normal");
    if (last_scan && (DWORD)(now - last_scan) < transport.scan_ms()) return;
    last_scan = now ? now : 1;
    if (!readable(om, 0x100) || !readable(anim, 0x148) || anim->vtbl != table_copy + 1) return;
    u32 count = 0; Object **objects = all_objects(om, &count);
    if (count > 1000000 || (count && !readable(objects, (size_t)count * sizeof *objects))) return;
    by_index.clear();
    std::unordered_set<u64> alive;
    struct Candidate { Object *object; u64 key; u32 index; };
    std::vector<Candidate> owned;
    for (u32 i = 0; i < count; ++i) {
        u64 key = 0; u32 index = 0;
        if (!human(objects[i], &key, &index)) continue;
        if (alive.size() >= 20000) { ++full; break; }
        alive.insert(key);
        bool ours = session_owns(objects[i]);
        auto inserted = by_index.emplace(index, Index{key, !ours, false});
        if (!inserted.second) { inserted.first->second.ambiguous = true; continue; }
        if (ours) owned.push_back({objects[i], key, index});
    }
    // Discover duplicate indices before emitting either soldier's state.
    for (const Candidate &c : owned) {
        if (by_index[c.index].ambiguous) continue;
        publish(c.object, c.key, c.index, now, false);
    }
    for (auto it = local.begin(); it != local.end();) {
        if (!alive.count(it->first)) it = local.erase(it); else ++it;
    }
    for (auto it = received_at.begin(); it != received_at.end();) {
        if (!alive.count(it->first)) it = received_at.erase(it); else ++it;
    }
}

static animation_channel::Apply anim_apply(const ShotRec &s, ObjMan *om, bool timed_wait)
{
    if (!anim_wire::valid(s)) { ++refused; mp_log("anim: malformed snapshot %u refused", s.seq); return animation_channel::Apply::refused; }
    if (!installed.load(std::memory_order_acquire) || !enabled || om != manager || GetCurrentThreadId() != thread_id) {
        ++refused; mp_log("anim: snapshot %u refused: hook/thread unavailable", s.seq); return animation_channel::Apply::refused;
    }
    Object *o = session_unit(s.shooter);
    if (!o) return timed_wait && wait_or_refuse(s.seq) ? animation_channel::Apply::refused : animation_channel::Apply::retry;
    u64 key = 0; u32 index = 0;
    if (!human(o, &key, &index) || key != s.shooter || session_owns(o)) { ++refused; mp_log("anim: snapshot %u refused: soldier/ownership mismatch", s.seq); return animation_channel::Apply::refused; }
    auto slot = by_index.find(index);
    if (slot == by_index.end()) return timed_wait && wait_or_refuse(s.seq) ? animation_channel::Apply::refused : animation_channel::Apply::retry;
    if (slot->second.ambiguous || slot->second.key != key || !slot->second.remote ||
        !readable(anim, 0x148) || anim->vtbl != table_copy + 1) {
        ++refused; mp_log("anim: snapshot %u refused: ambiguous index or changed interface", s.seq); return animation_channel::Apply::refused;
    }
    unsigned char *inst = instance(index);
    if (!inst || !writable(inst, anim_wire::instance_size) || !writable((char *)o + 0x1210, 12) ||
        !writable((char *)o + 0x1e70, 4) || !writable((char *)o + 0x1eac, 4) || !writable((char *)o + 0x1ef8, 4)) {
        ++refused; mp_log("anim: snapshot %u refused: animation memory not writable", s.seq); return animation_channel::Apply::refused;
    }
    char current[28];
    if (!group_name(inst, current)) { ++refused; return animation_channel::Apply::refused; }
    const unsigned char *p = s.launch;
    if (p[50] && strcmp(current, (const char *)p + 52)) {
        if (!invoke_group((GroupFn)originals[11], anim, index, (const char *)p + 52)) {
            enabled = false; ++refused; mp_log("anim: engine group setter faulted; synchronization disabled (effects may be partial)"); return animation_channel::Apply::refused;
        }
        inst = instance(index);
        if (!inst || !writable(inst, anim_wire::instance_size) || !group_name(inst, current) || strcmp(current, (const char *)p + 52)) {
            ++refused; mp_log("anim: snapshot %u refused: local group not found", s.seq); return animation_channel::Apply::refused;
        }
    }
    // Check the owner's sequence number against the local group's size before applying it.
    u32 active = 0, frame = 0, count = 0;
    memcpy(&active, p + 8, 4); memcpy(&frame, p + 12, 4);
    void *group = nullptr; memcpy(&group, inst + 0x70, sizeof group);
    if (!active) frame = 0xffffffffu;
    else {
        if (!p[50] || !group || !readable(group, 16)) { ++refused; return animation_channel::Apply::refused; }
        memcpy(&count, (char *)group + 12, 4);
        if (frame >= count) { ++refused; mp_log("anim: snapshot %u refused: frame outside local group", s.seq); return animation_channel::Apply::refused; }
    }
    memcpy(inst + 0xa8, p + 4, 4); memcpy(inst + 0x100, p + 16, 4); memcpy(inst + 0x114, p + 20, 4);
    memcpy(inst + 0xd4, p + 24, 4); memcpy(inst + 0x11c, p + 28, 12); memcpy(inst + 0x128, p + 40, 8);
    inst[0x136] = p[48]; inst[0x139] = p[49];
    memcpy(inst + 0x104, &active, 4); memcpy(inst + 0x10c, &frame, 4);
    u32 state = 0; memcpy(&state, p + 80, 4);
    if (state < 8) memcpy((char *)o + 0x1e70, &state, 4);
    memcpy((char *)o + 0x1210, p + 84, 12); memcpy((char *)o + 0x1eac, p + 96, 4); memcpy((char *)o + 0x1ef8, p + 100, 4);
    received_at[key] = GetTickCount(); wait_seq = 0; ++applied;
    return animation_channel::Apply::applied;
}


bool anim_received(const ShotRec &s, ObjMan *om)
{
    // Test/legacy seam; protocol 18 uses the independent channel below.
    return anim_apply(s, om, true) != animation_channel::Apply::retry;
}

static animation_channel::Apply transport_replay(const ShotRec &s, void *context)
{ return anim_apply(s, (ObjMan *)context, false); }

void anim_transport_epoch(u32 local_epoch, u32 peer_epoch) { transport.start(local_epoch, peer_epoch); }
void anim_transport_reset(void) { transport.reset(); }
void anim_on_packet(const unsigned char *data, int size, void (*send)(const void *, int))
{ if (size >= 0) transport.packet(data, (size_t)size, send); }
void anim_transport_tick(DWORD now, void (*send)(const void *, int)) { transport.tick(now, send); }
void anim_sim_step(ObjMan *om, DWORD now, void (*send)(const void *, int))
{ transport.apply(now, transport_replay, om, send); }

void anim_report(void)
{
    unsigned long long other = other_thread.exchange(0, std::memory_order_relaxed);
    if (sent || applied || held || refused || full || other || transport.pending_events() || transport.rebases())
        mp_log("anim: 5 s: %llu updates retained, %llu applied, %llu local setters held, %llu refused, %llu deferred, %llu calls on other threads passed through; %zu critical events awaiting replay/ACK, %zu superseded under pressure, sampling %s",
               sent, applied, held, refused, full, other, transport.pending_events(), transport.rebases(),
               transport.slow() ? "reduced" : "normal");
    sent = applied = held = refused = full = 0;
}

void anim_reset_wait(void) { wait_seq = 0; wait_at = 0; }

void anim_reset_peer(void)
{
    anim_reset_wait(); last_scan = 0;
    by_index.clear(); local.clear(); received_at.clear();
}

#ifdef NETV21_TEST_HOOKS
// Install fake functions and an in-memory fixture only; never compiled into the game module.
void anim_test_engine(ObjMan *om, Object *object)
{
    manager = om; anim = object; thread_id = GetCurrentThreadId(); attempted = enabled = true;
    original_table = object->vtbl;
    memcpy(originals, original_table, sizeof originals);
    memcpy(table_copy, original_table - 1, sizeof table_copy);
    object->vtbl = table_copy + 1;
    last_scan = wait_seq = 0; by_index.clear(); local.clear(); received_at.clear();
    installed.store(true);
}
uintptr_t anim_test_slot4(Object *o, u32 index, u32 value) { return slot4(o, index, value); }
void anim_test_stale(u64 key) { received_at[key] = GetTickCount() - 3001; }
void anim_test_expire_wait() { wait_at = GetTickCount() - 5001; }
#endif

void anim_stop(void)
{
    installed.store(false, std::memory_order_release);
    if (anim && original_table && readable(anim, sizeof *anim) &&
        !hook_swap_vtable_slot((void **)&anim->vtbl, table_copy + 1, original_table))
        mp_log("anim: interface table changed by another module; not overwritten");
    anim = nullptr; manager = nullptr; original_table = nullptr;
    attempted = false; enabled = true; last_scan = wait_seq = 0;
    by_index.clear(); local.clear(); received_at.clear(); transport.reset();
}

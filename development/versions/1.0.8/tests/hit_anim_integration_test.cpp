// Windows fake-engine integration test of the production hit/animation adapters. The fixture
// never patches a game module and cannot establish that real-game layouts or threads are correct.
#include "common.h"
#include "protocol.h"
#include "hit_wire.h"
#include "anim_wire.h"
#include "animation_channel.h"
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <vector>
#include <atomic>

using TestHit = uintptr_t (*)(Object *, void *, u32, Object *, u32, bool);
void hits_test_engine(ObjMan *, TestHit, TestHit);
uintptr_t hits_test_capture(bool, Object *, void *, u32, Object *, u32, bool);
void hits_test_expire_wait();
void anim_test_engine(ObjMan *, Object *);
uintptr_t anim_test_slot4(Object *, u32, u32);
void anim_test_stale(u64);
void anim_test_age(u64);
void anim_test_expire_wait();

char g_out_dir[MAX_PATH] = ".";
char g_program[32] = "tacgame";
static char component_failure[512];
void mp_log(const char *format, ...) {
    if (!strstr(format, "hit: component failure")) return;
    va_list args; va_start(args, format);
    vsnprintf(component_failure, sizeof component_failure, format, args);
    va_end(args);
}
static int checks, failures;
static void check(bool ok, const char *label) { ++checks; if (!ok) { ++failures; printf("FAIL: %s\n", label); } }
bool readable(const void *p, size_t n) {
    MEMORY_BASIC_INFORMATION m = {};
    if (!p || !VirtualQuery(p, &m, sizeof m) || m.State != MEM_COMMIT || (m.Protect & (PAGE_NOACCESS | PAGE_GUARD))) return false;
    uintptr_t a = (uintptr_t)p, b = (uintptr_t)m.BaseAddress;
    return a >= b && a - b <= m.RegionSize && n <= m.RegionSize - (a - b);
}
static unsigned features = PEER_FEATURES;
static Object *soldier, *other, *vehicle, *missing;
static bool soldier_owned = true, other_owned = false, vehicle_owned = true, bad_mapping;
static unsigned char desc1[0x80], desc2[0x80];
static std::vector<ShotRec> queued;
static std::atomic<unsigned> hit_calls{0};
static u32 observed_seed, observed_a3, observed_a5, observed_part;
static bool observed_a6;
static void *observed_d1, *observed_d2, *observed_obj;
static Object *observed_source;

bool session_connected() { return true; }
int session_role() { return ROLE_HOST; }
unsigned session_peer_features() { return features; }
bool session_owns(Object *o) { return o == soldier ? soldier_owned : o == other ? other_owned : o == vehicle ? vehicle_owned : false; }
bool session_unit_key(Object *o, u64 *key) {
    if (o == soldier) *key = (2ull << 32) | 17;
    else if (o == other) *key = (2ull << 32) | 18;
    else if (o == vehicle) *key = (1ull << 32) | 19;
    else return false;
    return true;
}
Object *session_unit(u64 key) {
    Object *o = key == ((2ull << 32) | 17) ? soldier : key == ((2ull << 32) | 18) ? other : key == ((1ull << 32) | 19) ? vehicle : nullptr;
    return o == missing ? nullptr : o;
}
bool xlate_ready() { return true; }
bool xlate_desc_checked(u32 value, u32 *out) {
    if (bad_mapping && value != 0xffffffffu) return false;
    *out = value; return true;
}
bool shots_try_queue(ShotRec &s, size_t limit) {
    if (queued.size() >= limit) return false;
    s.seq = (u32)queued.size() + 1; queued.push_back(s); return true;
}
static std::vector<std::vector<unsigned char>> anim_packets;
static void capture_anim(const void *data, int n) {
    anim_packets.emplace_back((const unsigned char *)data, (const unsigned char *)data + n);
}
static u32 description_id(ObjMan *, void *p) { return p == desc1 ? 101 : p == desc2 ? 202 : 0xffffffffu; }
static void *description_ptr(ObjMan *, u32 id) { return id == 101 ? desc1 : id == 202 ? desc2 : nullptr; }
static Object *find_nothing(ObjMan *, Dep *) { return nullptr; }
static uintptr_t hit_original(Object *o, void *r, u32 a3, Object *source, u32 a5, bool a6) {
    ++hit_calls; observed_a3 = a3; observed_a5 = a5; observed_a6 = a6; observed_source = source;
    if (o) memcpy(&observed_seed, (char *)o + (o == vehicle ? 0xb64 : 0x2768), 4);
    if (r) { memcpy(&observed_part, (char *)r + 0x60, 4); memcpy(&observed_d1, (char *)r + 0x68, 8); memcpy(&observed_d2, (char *)r + 0x70, 8); memcpy(&observed_obj, (char *)r + 0x78, 8); }
    return 123;
}
static DWORD WINAPI off_thread_hit(void *p) {
    hits_test_capture(false, soldier, p, 1, nullptr, 2, false); return 0;
}
static void put32(void *p, u32 n) { memcpy(p, &n, 4); }
static void putf(void *p, float f) { memcpy(p, &f, 4); }
static Object *new_unit(ObjMan *om, const char *iface, const char *full, u32 index) {
    Object *o = (Object *)VirtualAlloc(nullptr, 0x3000, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (!o) return nullptr;
    o->iface = iface; o->objman = om; strcpy_s(o->full, sizeof o->full, full);
    put32((char *)o + 0x1d60, index);
    return o;
}

static unsigned group_calls, setter_calls;
static bool group_success = true;
static unsigned char group_walk[128], group_idle[128];
static unsigned char *instances;
static uintptr_t set_group(Object *, u32 index, const char *name) {
    ++group_calls;
    if (group_success && !strcmp(name, "walk")) {
        void *p = group_walk + 0x3e; memcpy(instances + (size_t)index * 0x140 + 0x70, &p, 8);
    }
    return 1;
}
static uintptr_t set_scalar(Object *, u32 index, u32 value) {
    ++setter_calls; put32(instances + (size_t)index * 0x140 + 0xa8, value); return 0;
}

int main()
{
    alignas(8) unsigned char manager_mem[0x1800] = {};
    ObjMan *om = (ObjMan *)manager_mem;
    void *vt[58] = {}; vt[9] = (void *)find_nothing; vt[56] = (void *)description_id; vt[57] = (void *)description_ptr; om->vtbl = vt;
    soldier = new_unit(om, "i_human", "i_human:soldier_11", 0);
    other = new_unit(om, "i_human", "i_human:soldier_12", 1);
    vehicle = new_unit(om, "i_techn", "i_techn:vehicle_13", 0);
    if (!soldier || !other || !vehicle) return 2;
    Object *objects[] = {soldier, other, vehicle};
    Object **list = objects; memcpy(manager_mem + 0xf0, &list, 8); put32(manager_mem + 0xfc, 3);
    alignas(8) unsigned char parts[0x60] = {};
    void *part_ptr = parts; u32 part = (12u << 10) | 23;
    memcpy((char *)soldier + 0x850, &part_ptr, 8); put32((char *)soldier + 0x85c, 1); put32(parts + 0x48, part);
    put32((char *)soldier + 0x2768, 9876);
    unsigned char rec[0x80] = {};
    put32(rec + 0x60, part);
    void *d1 = desc1, *d2 = desc2, *obj = vehicle;
    memcpy(rec + 0x68, &d1, 8); memcpy(rec + 0x70, &d2, 8); memcpy(rec + 0x78, &obj, 8);
    hits_test_engine(om, hit_original, hit_original);

    features = 0;
    check(hits_test_capture(false, soldier, rec, 5, other, 6, true) == 123 && queued.empty() && hit_calls == 1,
          "without peer capability hit passes through and is not queued");
    features = PEER_FEATURES;
    check(hits_test_capture(false, soldier, rec, 5, other, 6, true) == 123 && queued.size() == 1 && hit_calls == 2,
          "owner hit queued and original called once");
    ShotRec h = queued.back();
    check(hit_wire::valid(h) && h.desc3 == 9876 && h.ammo == 101 && h.weapon == 202 && h.obj2 == ((2ull << 32) | 18) && h.obj5 == ((1ull << 32) | 19),
          "captured metadata contains IDs, not engine pointers");
    soldier_owned = false;
    check(hits_test_capture(false, soldier, rec, 5, other, 6, true) == 0 && queued.size() == 1 && hit_calls == 2,
          "local hit on the other owner's unit is held");
    put32((char *)soldier + 0x2768, 1);
    check(hits_received(h, om) && hit_calls == 3 && observed_seed == 9876 && observed_a3 == 5 && observed_a5 == 6 && observed_a6 &&
          observed_d1 == desc1 && observed_d2 == desc2 && observed_obj == vehicle && observed_source == other && observed_part == 0xffffffffu,
          "remote human hit reconstructs references and seed without a process-local component");
    put32(parts + 0x48, (77u << 10) | 24);
    check(hits_received(h, om) && hit_calls == 4 && observed_part == 0xffffffffu,
          "human replay is independent of component list layout");
    put32(parts + 0x48, part);
    bad_mapping = true;
    check(hits_received(h, om) && hit_calls == 4, "unmapped description refused"); bad_mapping = false;
    soldier_owned = true;
    check(hits_received(h, om) && hit_calls == 4, "hit targeting our own unit refused"); soldier_owned = false;
    missing = soldier;
    check(!hits_received(h, om) && hit_calls == 4, "missing target is retained, not acknowledged");
    hits_test_expire_wait(); hits_reset_peer();
    check(!hits_received(h, om) && hit_calls == 4, "stream restart resets a hit's old wait deadline");
    hits_test_expire_wait();
    check(hits_received(h, om) && hit_calls == 4, "missing target is explicitly refused after timeout"); missing = nullptr;
    DWORD old = 0, ignored = 0;
    check(VirtualProtect((char *)soldier + 0x2000, 0x1000, PAGE_READONLY, &old) != 0, "fixture seed page made read-only");
    check(hits_received(h, om) && hit_calls == 4, "read-only seed refused");
    check(VirtualProtect((char *)soldier + 0x2000, 0x1000, old, &ignored) != 0, "fixture page restored");
    HANDLE worker = CreateThread(nullptr, 0, off_thread_hit, rec, 0, nullptr);
    if (!worker) return 3;
    WaitForSingleObject(worker, 5000); CloseHandle(worker);
    check(hit_calls == 5 && queued.size() == 1, "off-thread engine call passes through without touching session maps");

    // The human path must survive the exact three list cases that previously disabled all hits.
    soldier_owned = true; queued.clear();
    put32(rec + 0x60, (99u << 10) | 23);
    check(hits_test_capture(false, soldier, rec, 5, other, 6, true) == 123 && hit_calls == 6 && queued.size() == 1,
          "human hit with an unmatched component is still queued");
    put32((char *)soldier + 0x85c, 0);
    check(hits_test_capture(false, soldier, rec, 5, other, 6, true) == 123 && hit_calls == 7 && queued.size() == 2,
          "human hit with no component list is still queued");
    alignas(8) unsigned char many_parts[7 * 0x60] = {};
    void *many_ptr = many_parts; memcpy((char *)soldier + 0x850, &many_ptr, 8); put32((char *)soldier + 0x85c, 7);
    for (u32 i = 0; i < 7; ++i) put32(many_parts + (size_t)i * 0x60 + 0x48, (i + 1) << 10);
    check(hits_test_capture(false, soldier, rec, 5, other, 6, true) == 123 && hit_calls == 8 && queued.size() == 3 &&
          hits_features() == (PEER_HIT_SYNC | PEER_VEHICLE_HIT_SYNC), "human hit ignores a large component list");

    // Vehicles still require an independently validated component translation. A vehicle-only
    // failure withdraws that capability and leaves the human path working.
    memcpy((char *)vehicle + 0x850, &part_ptr, 8); put32((char *)vehicle + 0x85c, 1);
    put32((char *)vehicle + 0xb64, 4321); component_failure[0] = 0;
    check(hits_test_capture(true, vehicle, rec, 5, other, 6, true) == 123 && hit_calls == 9 && queued.size() == 3 &&
          hits_features() == PEER_HIT_SYNC && strstr(component_failure, "part group absent from list"),
          "unmatched vehicle part disables only vehicle synchronization");
    check(hits_test_capture(false, soldier, rec, 5, other, 6, true) == 123 && hit_calls == 10 && queued.size() == 4,
          "vehicle failure does not disable human hits");
    hits_test_engine(om, hit_original, hit_original);
    put32(rec + 0x60, part);
    check(hits_test_capture(true, vehicle, rec, 5, other, 6, true) == 123 && hit_calls == 11 && queued.size() == 5,
          "mapped vehicle hit is queued separately");
    ShotRec v = queued.back(); vehicle_owned = false;
    put32(parts + 0x48, (77u << 10) | 23);
    check(hits_received(v, om) && hit_calls == 12 && observed_seed == 4321 && observed_part == ((77u << 10) | 23),
          "vehicle component group is translated before replay");
    vehicle_owned = true;
    hits_test_engine(om, hit_original, hit_original);
    put32((char *)vehicle + 0x85c, 0); component_failure[0] = 0;
    check(hits_test_capture(true, vehicle, rec, 5, other, 6, true) == 123 && hit_calls == 13 && hits_features() == PEER_HIT_SYNC &&
          strstr(component_failure, "list header/rows unavailable, empty or oversized"), "empty vehicle list is identified");
    hits_test_engine(om, hit_original, hit_original);
    memcpy((char *)vehicle + 0x850, &many_ptr, 8); put32((char *)vehicle + 0x85c, 7); component_failure[0] = 0;
    check(hits_test_capture(true, vehicle, rec, 5, other, 6, true) == 123 && hit_calls == 14 && hits_features() == PEER_HIT_SYNC &&
          strstr(component_failure, "list shape rejected"), "oversized vehicle group shape is identified");
    hits_stop();

    // Fake i_animch and its two instance rows. The normal group setter is called only after
    // the receiver has validated the target, instance and group name.
    instances = (unsigned char *)VirtualAlloc(nullptr, 0x1000, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    Object *anim = (Object *)VirtualAlloc(nullptr, 0x1000, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (!instances || !anim) return 4;
    void *anim_table_storage[42] = {}; void **avt = anim_table_storage + 1;
    avt[4] = (void *)set_scalar; avt[11] = (void *)set_group;
    anim->vtbl = avt; anim->objman = om;
    memcpy((char *)anim + 0x140, &instances, 8);
    strcpy_s((char *)group_idle, sizeof group_idle, "idle"); strcpy_s((char *)group_walk, sizeof group_walk, "walk");
    put32(group_idle + 0x3e + 12, 8); put32(group_walk + 0x3e + 12, 8);
    void *idle = group_idle + 0x3e;
    memcpy(instances + 0x70, &idle, 8); memcpy(instances + 0x140 + 0x70, &idle, 8);
    put32(instances + 0x104, 1); put32(instances + 0x10c, 1);
    put32(instances + 0x140 + 0x104, 1); put32(instances + 0x140 + 0x10c, 1);
    soldier_owned = true; other_owned = false;
    queued.clear();
    anim_test_engine(om, anim); anim_transport_epoch(11, 22);
    anim_frame(om, GetTickCount64()); anim_transport_tick(GetTickCount64(), capture_anim);
    animation_channel::Data first = {};
    if (anim_packets.size() == 1 && anim_packets[0].size() == sizeof first) memcpy(&first, anim_packets[0].data(), sizeof first);
    check(anim_packets.size() == 1 && anim_wire::valid(first.record) && first.record.shooter == ((2ull << 32) | 17) &&
          first.h.type == PKT_ANIM && first.h.flags == animation_channel::critical_flag && queued.empty(),
          "owned soldier enters the independent animation channel, not the shot queue");
    animation_channel::Ack first_ack = {make_head(PKT_ANIM_ACK), 11, 22, first.record.seq, first.record.shooter};
    anim_on_packet((const unsigned char *)&first_ack, sizeof first_ack, capture_anim);
    anim_test_slot4(anim, 0, 33); anim_transport_tick(GetTickCount64(), capture_anim);
    animation_channel::Data changed = {};
    if (anim_packets.size() == 2 && anim_packets[1].size() == sizeof changed) memcpy(&changed, anim_packets[1].data(), sizeof changed);
    check(anim_packets.size() == 2 && changed.record.seq > first.record.seq && changed.h.flags == animation_channel::critical_flag && queued.empty(),
          "checked owner setter publishes a reliable critical change immediately");
    anim_test_slot4(anim, 1, 10);
    check(setter_calls == 2, "remote setter passes before the first owner snapshot");
    unsigned char remote[0x140] = {}, human_values[0x1f00] = {};
    put32(remote + 0xa8, 99); put32(remote + 0x104, 1); put32(remote + 0x10c, 2);
    putf(remote + 0xd4, 0.75f); putf(remote + 0x11c, 1.25f); put32(human_values + 0x1e70, 3); putf(human_values + 0x1210, 4.f);
    ShotRec a; anim_wire::encode(a, (2ull << 32) | 18, remote, "walk", human_values); a.seq = 1;
    missing = other;
    check(!anim_received(a, om), "missing animation target is retained");
    anim_test_expire_wait(); anim_reset_wait();
    check(!anim_received(a, om), "stream restart resets only the animation wait deadline");
    anim_test_expire_wait();
    check(anim_received(a, om) && group_calls == 0, "missing animation target is eventually refused");
    missing = nullptr; a.seq = 2; // a new snapshot, not a retry of the refused event
    check(anim_received(a, om) && group_calls == 1, "remote group found through original checked setter without clearing cached indices");
    u32 applied_scalar = 0, applied_frame = 0, applied_state = 0;
    memcpy(&applied_scalar, instances + 0x140 + 0xa8, 4); memcpy(&applied_frame, instances + 0x140 + 0x10c, 4);
    memcpy(&applied_state, (char *)other + 0x1e70, 4);
    check(applied_scalar == 99 && applied_frame == 2 && applied_state == 3, "animation scalars and soldier fields applied");
    anim_test_slot4(anim, 1, 11);
    check(setter_calls == 2, "recent owner snapshot holds back local remote setter");
    anim_test_age((2ull << 32) | 18);          // an hour without updates from the owner
    anim_test_slot4(anim, 1, 12);
    check(setter_calls == 2, "a soldier the owner stopped updating stays on its last snapshot");
    anim_test_stale((2ull << 32) | 18);
    anim_test_slot4(anim, 1, 13);
    check(setter_calls == 3, "a unit with no owner snapshot at all is released to the local game");
    ShotRec invalid = a; put32(invalid.launch + 8, 2);
    check(anim_received(invalid, om) && group_calls == 1, "malformed animation is refused before engine call");
    memcpy(instances + 0x140 + 0x70, &idle, 8); group_success = false;
    put32(instances + 0x140 + 0xa8, 42);
    check(anim_received(a, om) && group_calls == 2, "missing group is explicitly refused");
    memcpy(&applied_scalar, instances + 0x140 + 0xa8, 4);
    check(applied_scalar == 42, "failed group change does not write animation scalars");
    put32((char *)soldier + 0x1d60, 1); group_success = true;
    anim_frame(om, GetTickCount64() + 500);
    check(anim_received(a, om) && group_calls == 2, "duplicate instance index is refused before group setter");
    anim_stop();
    check(anim->vtbl == avt, "fixture animation vtable restored on stop");

    VirtualFree(instances, 0, MEM_RELEASE); VirtualFree(anim, 0, MEM_RELEASE);
    VirtualFree(soldier, 0, MEM_RELEASE); VirtualFree(other, 0, MEM_RELEASE); VirtualFree(vehicle, 0, MEM_RELEASE);
    printf("hit/animation adapter selftest: %d checks, %d failed\n", checks, failures);
    return failures ? 1 : 0;
}

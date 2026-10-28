// Vitals port for net_v21. The frame/packet calls are direct session APIs, not detours into v20.
// Monotonic health/death and vehicle status rules originate in kit-source/src/mp_vitals.c. This
// version has its own bounded channel, explicit handshake epochs, checked memory and peer reset.
// A matching image and signatures prove layout identity, not real-game correctness: the feature
// and its statistics seed still require a real tactical test on both machines.
#include "vitals.h"
#include "vitals_core.h"
#include "common.h"
#include "env_value.h"
#include "hooks.h"
#include "detour.h"
#include <windows.h>
#include <atomic>
#include <stdint.h>
#include <string.h>
#include <unordered_map>
#include <vector>

namespace {
using vitals::Entry;
enum { H_SELF = 0x7c8, H_SQUAD_COPY = 0x7d0, H_INDEX = 0x1da4, H_SQUAD = 0x2388,
       S_MEMBERS = 0x1900, S_COUNT = 0x190c, M_SIZE = 0x140, M_STATE = 0x2c,
       M_HEALTH = 0x60, M_OWNER = 0xc8, STATE_KILLED = 5, STAT_SEED_OFF = 0x130 };
const uint16_t vehicle_offsets[vitals::vehicle_bytes] = { 0xd07, 0xd08, 0xd09, 0xd0a, 0xd0b, 0xd0c, 0xd0e, 0xd0f, 0xd10 };
const unsigned char member_sig[] = { 0x39,0x91,0x0c,0x19,0,0, 0x76,0x12, 0x8b,0xc2, 0x48,0x8d,0x04,0x80, 0x48,0xc1,0xe0,0x06, 0x48,0x03,0x81,0x00,0x19,0,0, 0xc3, 0x33,0xc0, 0xc3 };
const unsigned char human_sig[] = { 0x48,0x8b,0xc4,0x48,0x89,0x58,0x20,0x55,0x56,0x41,0x57,0x48,0x83,0xec,0x60,0x80,0xbc,0x24,0xa8,0,0,0,0 };
const unsigned char techn_sig[] = { 0x48,0x8b,0xc4,0x48,0x89,0x58,0x20,0x44,0x89,0x40,0x18,0x55,0x56,0x57,0x41,0x54,0x41,0x55,0x41,0x56,0x41,0x57 };
const unsigned char stat_sig[] = { 0x48,0x8b,0xc4,0x44,0x88,0x40,0x18,0x48,0x89,0x50,0x10,0x48,0x89,0x48,0x08,0x55,0x53,0x56,0x57,0x41,0x54,0x41,0x55,0x41,0x56 };
const unsigned char stat_rng[] = { 0x30,0x01,0,0,0x0d,0x66,0x19,0 };

struct LocalSoldier { float sent = 1.f; uint32_t last_sent = 0; bool told_dead = false; };
struct LocalVehicle { uint8_t sent[vitals::vehicle_bytes] = {}; uint32_t last_sent = 0; };
struct Candidate { Object *obj; uint32_t id; bool machine; };
static vitals::Channel channel;
static std::unordered_map<uint32_t, LocalSoldier> soldiers;
static std::unordered_map<uint32_t, LocalVehicle> vehicles;
static uint32_t next_nonce, since, last_scan, last_report, scan_no, sanity_seen, sanity_good;
static uint16_t baseline_mask;
static bool on, waiting, baseline_checked, start_checked, broken;
static int layout_state; // 0 not tried, 1 verified, -1 unknown; retry only while module absent
static void *member_fn;
static DWORD frame_thread;
static ObjMan *last_om, *active_om;
static vitals::Send last_send;
static bool in_frame;
static std::atomic<bool> stats_enabled{false};
static std::atomic<DWORD> stats_thread{0};
static std::atomic<DWORD> stats_last_active{0};
typedef void (*StatFn)(void *, long long, char);
static std::atomic<StatFn> original_stat{nullptr};
static bool stat_tried;
#ifdef NETV21_TEST_HOOKS
static bool mock_layout;
#endif

static bool span_write(const void *ptr, size_t n) {
    if (!ptr || !n) return false;
    MEMORY_BASIC_INFORMATION m = {};
    if (!VirtualQuery(ptr, &m, sizeof m) || m.State != MEM_COMMIT || (m.Protect & PAGE_GUARD)) return false;
    DWORD p = m.Protect & 0xff;
    if (p != PAGE_READWRITE && p != PAGE_WRITECOPY && p != PAGE_EXECUTE_READWRITE && p != PAGE_EXECUTE_WRITECOPY) return false;
    uintptr_t a = (uintptr_t)ptr, b = (uintptr_t)m.BaseAddress;
    return a >= b && a - b <= m.RegionSize && n <= m.RegionSize - (a - b);
}
static bool image_stamp(HMODULE module, uint32_t stamp, uint32_t size) {
    const unsigned char *p = (const unsigned char *)module;
    if (!readable(p, sizeof(IMAGE_DOS_HEADER))) return false;
    const IMAGE_DOS_HEADER *dos = (const IMAGE_DOS_HEADER *)p;
    if (dos->e_magic != IMAGE_DOS_SIGNATURE || dos->e_lfanew < (long)sizeof(*dos) || dos->e_lfanew > 0x100000) return false;
    const IMAGE_NT_HEADERS64 *nt = (const IMAGE_NT_HEADERS64 *)(p + dos->e_lfanew);
    return readable(nt, sizeof *nt) && nt->Signature == IMAGE_NT_SIGNATURE &&
           nt->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC && nt->FileHeader.TimeDateStamp == stamp &&
           nt->OptionalHeader.SizeOfImage == size;
}
static bool ensure_layout() {
#ifdef NETV21_TEST_HOOKS
    if (mock_layout) return true;
#endif
    if (layout_state) return layout_state > 0;
    HMODULE phys = GetModuleHandleA("adv_physic.codelib");
    if (!phys) return false;
    layout_state = -1;
    if (!image_stamp(phys, 0x66f2a91e, 0xf2000)) {
        mp_log("vitals: unrecognised adv_physic image; no health or vehicle memory is accessed");
        return false;
    }
    const HookSig member[] = { { 0x9c410, member_sig, sizeof member_sig } };
    const HookSig human[] = { { 0x66c90, human_sig, sizeof human_sig } };
    const HookSig techn[] = { { 0x43230, techn_sig, sizeof techn_sig } };
    member_fn = hook_resolve_checked("adv_physic.codelib", member, 1, "vitals squad member lookup");
    void *human_fn = hook_resolve_checked("adv_physic.codelib", human, 1, "vitals human hit layout");
    void *techn_fn = hook_resolve_checked("adv_physic.codelib", techn, 1, "vitals vehicle hit layout");
    if (!member_fn || !human_fn || !techn_fn) {
        mp_log("vitals: engine signatures do not agree; health and vehicle sync stay off");
        return false;
    }
    layout_state = 1;
    mp_log("vitals: checked 3598/1 physics image and member/hit signatures; runtime behaviour remains unverified");
    return true;
}
static int current_mode() {
    int value;
    if (env_integer("MP_VITALS", &value) && value >= 0) return value > 3 ? 3 : value;
    return session_host_mode() == HOST_MODE_OPERATION ? 3 : 1;
}
static int vehicle_mode(int mode) {
    if (!mode) return 0;
    int value;
    if (env_integer("MP_VEHICLES", &value) && value >= 0) return value > 2 ? 2 : value;
    return mode >= 2 ? 2 : 1;
}
static bool stats_mode() { int v; return !env_integer("MP_STATS", &v) || v != 0; }
static bool event_log() { int v; return env_integer("MP_LOG_VITALS", &v) && v != 0; }
static void clear_local() {
    soldiers.clear(); vehicles.clear(); since = last_scan = last_report = scan_no = sanity_seen = sanity_good = 0;
    baseline_mask = 0; on = waiting = baseline_checked = start_checked = broken = false;
    stats_enabled.store(false, std::memory_order_release);
}
static bool name_and_kind(Object *o, ObjMan *om, bool *machine, uint32_t *id) {
    if (!readable(o, sizeof *o) || o->objman != om || !memchr(o->full, 0, sizeof o->full)) return false;
    if (!readable(o->iface, 8)) return false;
    if (!memcmp(o->iface, "i_human", 8)) *machine = false;
    else if (!memcmp(o->iface, "i_techn", 8)) *machine = true;
    else return false;
    const char *end = (const char *)memchr(o->full, 0, sizeof o->full);
    if (end == o->full) return false;
    if (*machine) { *id = fnv((const unsigned char *)o->full, (size_t)(end - o->full)); return true; }
    const char *last = nullptr;
    for (const char *p = o->full; p < end; ++p) if (*p == '_') last = p;
    if (!last || last + 1 == end || end - last - 1 > 8) return false;
    uint32_t n = 0;
    for (const char *p = last + 1; p < end; ++p) {
        int d = *p >= '0' && *p <= '9' ? *p - '0' : *p >= 'a' && *p <= 'f' ? *p - 'a' + 10 :
                *p >= 'A' && *p <= 'F' ? *p - 'A' + 10 : -1;
        if (d < 0) return false;
        n = (n << 4) | (unsigned)d;
    }
    *id = n; return true;
}
// Reconstruct the checked get_member's array access without calling into the game. A record must
// belong to this exact soldier; all source pointers/counts and each write are independently checked.
static char *squad_record(char *human) {
    if (!readable(human + H_SQUAD, sizeof(void *)) || !readable(human + H_INDEX, 4)) return nullptr;
    char *squad; int index; memcpy(&squad, human + H_SQUAD, sizeof squad); memcpy(&index, human + H_INDEX, sizeof index);
    if (!squad || index < 0 || !readable(squad, S_COUNT + 4)) return nullptr;
    void **vt; memcpy(&vt, squad, sizeof vt);
#ifdef NETV21_TEST_HOOKS
    if (!mock_layout)
#endif
    if (!readable(vt, 0x140) || vt[0x138 / 8] != member_fn) return nullptr;
    char *rows; uint32_t count; memcpy(&rows, squad + S_MEMBERS, sizeof rows); memcpy(&count, squad + S_COUNT, sizeof count);
    if (!rows || count > 100000 || (uint32_t)index >= count || (size_t)index > (SIZE_MAX / M_SIZE)) return nullptr;
    uintptr_t at = (uintptr_t)rows + (size_t)index * M_SIZE;
    if (at < (uintptr_t)rows || !readable((void *)at, M_SIZE)) return nullptr;
    char *rec = (char *)at; void *owner; memcpy(&owner, rec + M_OWNER, sizeof owner);
    return owner == human ? rec : nullptr;
}
static bool set_health(char *human, float new_health) {
    if (!span_write(human + H_SELF, 4)) return false;
    memcpy(human + H_SELF, &new_health, 4); return true;
}
static bool mark_dead(char *human, float health, float rec_health) {
    if (!(health >= 0.f && health < 0.5f)) return false;
    char *rec = squad_record(human);
    // Do not detach an alive soldier if the squad record cannot be proven and updated as well.
    if (!rec || !span_write(rec + M_STATE, 4) || !span_write(rec + M_HEALTH, 4) ||
        !span_write(human + H_SELF, 4) || !span_write(human + H_SQUAD, 8) || !span_write(human + H_SQUAD_COPY, 8)) return false;
    float current; memcpy(&current, rec + M_HEALTH, 4);
    float target = rec_health >= 0.f && rec_health < 1000000.f ? rec_health : 0.f;
    if (current >= 0.f && current < target) target = current;
    uint32_t dead = STATE_KILLED; void *none = nullptr;
    memcpy(rec + M_STATE, &dead, 4); memcpy(rec + M_HEALTH, &target, 4);
    memcpy(human + H_SELF, &health, 4); memcpy(human + H_SQUAD, &none, 8); memcpy(human + H_SQUAD_COPY, &none, 8);
    return true;
}
static void frame(ObjMan *om, uint32_t now, vitals::Send send, bool force);
static void stat_hook(void *object, long long arg2, char arg3) {
    StatFn original = original_stat.load(std::memory_order_acquire);
    DWORD now = GetTickCount();
    if (stats_enabled.load(std::memory_order_acquire) && stats_mode() &&
        (uint32_t)(now - stats_last_active.load(std::memory_order_relaxed)) < 2000) {
        if (GetCurrentThreadId() == stats_thread.load(std::memory_order_acquire) && last_om && last_send && !in_frame) {
            frame(last_om, now, last_send, true);
            mp_log("vitals: final local scan before the game's statistics (delivery is not guaranteed before the calculation)");
        } else {
            mp_log("vitals: statistics called on another thread or during a scan; no unsafe final scan");
        }
        if (object && span_write((char *)object + STAT_SEED_OFF, 4)) {
            const uint32_t seed = 0x4d495553;
            memcpy((char *)object + STAT_SEED_OFF, &seed, 4);
            mp_log("vitals: checked statistics RNG seed set; equal final results are not guaranteed");
        }
    }
    if (original) original(object, arg2, arg3);
}
static void try_stat_hook() {
#ifdef NETV21_TEST_HOOKS
    if (mock_layout) return;
#endif
    if (stat_tried) return;
    HMODULE game = GetModuleHandleA("adv_game.codelib");
    if (!game) return;
    stat_tried = true;
    if (!image_stamp(game, 0x66f2d3a9, 0x178000)) {
        mp_log("vitals: unknown statistics image; RNG left unchanged"); return;
    }
    const HookSig where[] = { { 0xf80c0, stat_sig, sizeof stat_sig } };
    char *fn = hook_resolve_checked("adv_game.codelib", where, 1, "vitals statistics");
    if (!fn || !readable(fn, 11000)) { mp_log("vitals: statistics body unreadable; RNG left unchanged"); return; }
    unsigned occurrences = 0;
    for (unsigned i = 0; i + sizeof stat_rng <= 11000; ++i)
        if (!memcmp(fn + i, stat_rng, sizeof stat_rng)) ++occurrences;
    if (occurrences != 3 || !install_published_detour(fn, 15, stat_hook, original_stat)) {
        mp_log("vitals: statistics body/patch refused (%u RNG signatures); RNG left unchanged", occurrences); return;
    }
    mp_log("vitals: checked statistics hook installed; only a connected, validated tactical session can set its RNG");
}

static void frame(ObjMan *om, uint32_t now, vitals::Send send, bool force) {
    if (!om || !send || in_frame) return;
    in_frame = true;
    struct Done { ~Done() { in_frame = false; } } done;
    if (!session_connected() || session_role() == ROLE_NONE || !current_mode() || !channel.peer_epoch()) {
        stats_enabled.store(false, std::memory_order_release); return;
    }
    if (!ensure_layout()) { stats_enabled.store(false, std::memory_order_release); return; }
    try_stat_hook();
    if (!xlate_ready()) {
        stats_enabled.store(false, std::memory_order_release);
        if (!waiting) { waiting = true; mp_log("vitals: waiting for the checked object-number exchange"); }
        return;
    }
    if (waiting) { waiting = false; mp_log("vitals: number exchange ready; tactical state channel enabled"); }
    if (!on) {
        on = true; since = now; last_scan = now - 100; last_report = now; frame_thread = GetCurrentThreadId();
        mp_log("vitals: on (soldiers %d, vehicles %d); initial validation runs for 3 seconds", current_mode(), vehicle_mode(current_mode()));
    }
    stats_thread.store(frame_thread, std::memory_order_release);
    stats_last_active.store(now, std::memory_order_relaxed);
    stats_enabled.store(!broken && start_checked && current_mode() >= 2, std::memory_order_release);
    channel.tick(now, send);
    if (broken || (!force && (uint32_t)(now - last_scan) < 100)) return;
    last_scan = now; ++scan_no;
    if (!readable(om, 0x100)) return;
    uint32_t count; Object **all;
    memcpy(&count, (char *)om + 0xfc, 4); memcpy(&all, (char *)om + 0xf0, sizeof all);
    if (count > 1000000 || (count && !readable(all, (size_t)count * sizeof(*all)))) {
        mp_log("vitals: invalid object list; scan skipped"); return;
    }
    std::vector<Candidate> candidates;
    std::unordered_map<uint64_t, unsigned> ids;
    for (uint32_t i = 0; i < count; ++i) {
        Object *obj = all[i]; bool machine; uint32_t id;
        if (!name_and_kind(obj, om, &machine, &id)) continue;
        uint64_t key = ((uint64_t)(machine ? 1 : 2) << 32) | id;
        ++ids[key]; candidates.push_back({ obj, id, machine });
    }
    uint32_t seen = 0, dead = 0, veh_seen = 0, veh_nonzero[vitals::vehicle_bytes] = {}, duplicates = 0, applied = 0, refused = 0;
    std::vector<Entry> pending;
    pending.reserve(candidates.size());
    // Rotate traversal so a persistently busy connection does not always privilege the first units.
    size_t start = candidates.empty() ? 0 : ((size_t)scan_no * 137) % candidates.size();
    for (size_t k = 0; k < candidates.size(); ++k) {
        const Candidate &c = candidates[(start + k) % candidates.size()];
        uint64_t key = ((uint64_t)(c.machine ? 1 : 2) << 32) | c.id;
        if (ids[key] != 1) { ++duplicates; continue; }
        char *p = (char *)c.obj;
        if (c.machine) {
            int vm = vehicle_mode(current_mode());
            if (!vm || !readable(p + 0xd00, 0x20)) continue;
            uint8_t values[vitals::vehicle_bytes];
            for (int b = 0; b < vitals::vehicle_bytes; ++b) { values[b] = (uint8_t)p[vehicle_offsets[b]]; if (values[b]) ++veh_nonzero[b]; }
            ++veh_seen;
            if (!baseline_checked) continue;
            for (int b = 0; b < vitals::vehicle_bytes; ++b) if (baseline_mask & (1u << b)) values[b] = 0;
            vitals::Vehicle *remote = channel.find_vehicle(c.id);
            if (remote && start_checked) for (int b = 0; b < vitals::vehicle_bytes; ++b) {
                uint16_t bit = (uint16_t)(1u << b);
                if (!(remote->pending & bit)) continue;
                if (baseline_mask & bit) { remote->pending &= (uint16_t)~bit; continue; }
                if (values[b]) { remote->pending &= (uint16_t)~bit; continue; }
                if (vm >= 2 && span_write(p + vehicle_offsets[b], 1)) {
                    p[vehicle_offsets[b]] = (char)remote->pending_value[b]; values[b] = remote->pending_value[b];
                    remote->pending &= (uint16_t)~bit; ++applied;
                } else if (vm == 1) { remote->pending &= (uint16_t)~bit; ++refused; }
            }
            if (!vehicles.count(c.id) && vehicles.size() >= vitals::max_vehicles) { ++refused; continue; }
            LocalVehicle &local = vehicles[c.id];
            bool changed = memcmp(local.sent, values, sizeof values) != 0;
            bool any = false; for (uint8_t b : values) if (b) any = true;
            if (changed || (any && (uint32_t)(now - local.last_sent) >= 2000)) pending.push_back(vitals::machine(c.id, values));
            continue;
        }
        if (!readable(p + H_SELF, 4)) continue;
        float health; memcpy(&health, p + H_SELF, 4); ++seen;
        bool valid = health >= 0.f && health <= 2.f;
        if (sanity_seen < 40) { ++sanity_seen; if (valid) ++sanity_good; }
        if (!valid) { ++refused; continue; }
        if (health < 0.5f) ++dead;
        if (!soldiers.count(c.id) && soldiers.size() >= vitals::max_soldiers) { ++refused; continue; }
        LocalSoldier &local = soldiers[c.id];
        const vitals::Soldier *remote = channel.find_soldier(c.id);
        if (remote && start_checked && current_mode() >= 2 && remote->health < health - 0.005f) {
            if (health >= 0.5f && remote->is_dead) {
                if (mark_dead(p, remote->health, remote->record_health)) { health = remote->health; ++applied; }
                else ++refused;
            } else if (health < 0.5f || remote->health >= 0.5f) {
                if (set_health(p, remote->health)) { health = remote->health; ++applied; }
                else ++refused;
            }
        }
        bool is_dead = health < 0.5f;
        if (health < 0.999f && (health < local.sent - 0.005f || (is_dead && !local.told_dead) ||
                                (uint32_t)(now - local.last_sent) >= 2000))
            pending.push_back(vitals::soldier(c.id, health));
    }
    size_t retained = pending.empty() ? 0 : channel.enqueue(pending.data(), pending.size(), now, send);
    for (size_t i = 0; i < retained; ++i) {
        const Entry &e = pending[i];
        if (e.flags & vitals::vehicle) {
            auto it = vehicles.find(e.id); if (it != vehicles.end()) { vitals::machine_bytes(e, it->second.sent); it->second.last_sent = now; }
        } else {
            auto it = soldiers.find(e.id); if (it != soldiers.end()) {
                it->second.sent = vitals::value(e.a); it->second.told_dead = (e.flags & vitals::dead) != 0; it->second.last_sent = now;
            }
        }
    }
    if (!start_checked && (uint32_t)(now - since) >= 3000 && sanity_seen >= 40) {
        if (sanity_good * 10 < sanity_seen * 9 || (seen >= 20 && dead * 100 > seen * 30)) {
            broken = true; stats_enabled.store(false, std::memory_order_release);
            mp_log("vitals: unexpected initial health/death distribution (%u/%u sane, %u/%u dead); sync stopped", sanity_good, sanity_seen, dead, seen);
        } else { start_checked = true; mp_log("vitals: initial health validation passed (%u/%u samples)", sanity_good, sanity_seen); }
    }
    if (!baseline_checked && (uint32_t)(now - since) >= 3000) {
        baseline_checked = true;
        if (veh_seen >= 10) for (int b = 0; b < vitals::vehicle_bytes; ++b)
            if (veh_nonzero[b] * 100 > veh_seen * 20) baseline_mask |= (uint16_t)(1u << b);
        mp_log("vitals: %u initial vehicles; baseline status mask %03x is excluded", veh_seen, baseline_mask);
    }
    if ((uint32_t)(now - last_report) >= 5000 || (force && event_log())) {
        const vitals::Counters &n = channel.counters();
        mp_log("vitals: soldiers %u dead %u, vehicles %u, applied %u refused %u ambiguous %u; channel sent %llu acked %llu rx %llu retry %llu rejected %llu pending %zu",
               seen, dead, veh_seen, applied, refused, duplicates, n.sent, n.acked, n.received, n.retries, n.rejected, channel.outstanding());
        last_report = now;
    }
}
} // namespace

void vitals_start() {
    next_nonce += 0x9e3779b9u;
    uint32_t epoch = GetTickCount() ^ (GetCurrentProcessId() * 2654435761u) ^ next_nonce;
    if (!epoch) epoch = 1;
    channel.start(epoch); clear_local(); last_om = active_om = nullptr; last_send = nullptr;
}
void vitals_pause() { stats_enabled.store(false, std::memory_order_release); }
uint32_t vitals_epoch() { return channel.epoch(); }
void vitals_peer(uint32_t epoch) {
    if (channel.set_peer(epoch)) { clear_local(); mp_log("vitals: peer process identity changed; local state will be republished"); }
}
void vitals_on_packet(const unsigned char *data, int size, void (*send)(const void *, int)) {
    if (!data || size < 0) return;
    bool ready = session_connected() && session_role() != ROLE_NONE && current_mode() > 0 && ensure_layout() && xlate_ready();
    channel.packet(data, (size_t)size, ready, send);
}
void vitals_frame(ObjMan *om, uint32_t now, void (*send)(const void *, int)) {
    if (active_om && om && active_om != om) {
        mp_log("vitals: object manager changed; a fresh tactical handshake is required");
        vitals_start();
    }
    active_om = om; last_om = om; last_send = send; frame(om, now, send, false);
}
#ifdef NETV21_TEST_HOOKS
void vitals_test_mock_layout(bool enabled) { mock_layout = enabled; }
uint32_t vitals_test_peer_epoch() { return channel.peer_epoch(); }
unsigned vitals_test_outstanding() { return (unsigned)channel.outstanding(); }
uint64_t vitals_test_received() { return channel.counters().received; }
void vitals_test_stat_invoke(void *object, void (*original)(void *, long long, char)) {
    if (!mock_layout || original_stat.load(std::memory_order_acquire)) return;
    original_stat.store(original, std::memory_order_release);
    stat_hook(object, 17, 1);
    original_stat.store(nullptr, std::memory_order_release);
}
#endif

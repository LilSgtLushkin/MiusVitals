// Windows no-game integration of the real engine adapter, using synthetic object layouts.
// No game code is loaded. The test-only layout switch does not exist in the release DLL.
#include "vitals.h"
#include "vitals_core.h"
#include "common.h"
#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <vector>

static int checks, failures, host_mode = HOST_MODE_OPERATION;
static bool connected = true, ready = true;
static std::vector<std::vector<unsigned char>> output;
static void capture(const void *p, int n) { output.emplace_back((const unsigned char *)p, (const unsigned char *)p + n); }
static int original_stat_calls;
static void fake_stat(void *, long long arg, char flag) { if (arg == 17 && flag == 1) ++original_stat_calls; }
static void expect(bool ok, const char *s) { ++checks; if (!ok) { ++failures; printf("FAIL: %s\n", s); } }
void mp_log(const char *, ...) {}
bool session_connected() { return connected; }
int session_role() { return ROLE_HOST; }
int session_host_mode() { return host_mode; }
bool xlate_ready() { return ready; }
bool readable(const void *p, size_t n) {
    MEMORY_BASIC_INFORMATION m = {};
    if (!p || !n || !VirtualQuery(p, &m, sizeof m) || m.State != MEM_COMMIT || (m.Protect & (PAGE_NOACCESS | PAGE_GUARD))) return false;
    uintptr_t a = (uintptr_t)p, b = (uintptr_t)m.BaseAddress;
    return a >= b && a - b <= m.RegionSize && n <= m.RegionSize - (a - b);
}
static void put(void *p, size_t offset, const void *value, size_t n) { memcpy((char *)p + offset, value, n); }
template<class T> static void put(void *p, size_t offset, T value) { put(p, offset, &value, sizeof value); }
template<class T> static T get(const void *p, size_t offset) { T out; memcpy(&out, (const char *)p + offset, sizeof out); return out; }
static void *memory(size_t n) { return VirtualAlloc(nullptr, n, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE); }
static void release(void *p) { VirtualFree(p, 0, MEM_RELEASE); }
struct Person { void *human, *squad, *record; };
static Person person(ObjMan *om, unsigned id, bool correct_record = true) {
    Person p = { memory(0x3000), memory(0x2000), memory(0x1000) };
    Object *o = (Object *)p.human; o->objman = om; o->iface = "i_human";
    sprintf_s(o->full, "i_human:human_%08x", id);
    put<float>(p.human, 0x7c8, 1.f); put<void *>(p.human, 0x2388, p.squad); put<void *>(p.human, 0x7d0, p.squad);
    put<int>(p.human, 0x1da4, 0); put<void *>(p.squad, 0x1900, p.record); put<unsigned>(p.squad, 0x190c, 1);
    put<void *>(p.record, 0xc8, correct_record ? p.human : nullptr); put<float>(p.record, 0x60, 1.f);
    return p;
}
static Object *vehicle(ObjMan *om, unsigned id) {
    Object *o = (Object *)memory(0x2000); o->objman = om; o->iface = "i_techn";
    sprintf_s(o->full, "i_techn:vehicle_%08x", id); return o;
}
static void deliver(const vitals::Entry *entries, unsigned n, uint32_t &seq) {
    std::vector<unsigned char> bytes(sizeof(vitals::PacketHead) + n * sizeof(vitals::Entry));
    vitals::PacketHead h = { make_head(PKT_VITALS, n), 777, ++seq };
    memcpy(bytes.data(), &h, sizeof h); memcpy(bytes.data() + sizeof h, entries, n * sizeof(*entries));
    vitals_on_packet(bytes.data(), (int)bytes.size(), capture);
}
int main() {
    vitals_test_mock_layout(true);
    SetEnvironmentVariableA("MP_VITALS", nullptr); SetEnvironmentVariableA("MP_VEHICLES", nullptr);
    unsigned char manager[0x200] = {}; ObjMan *om = (ObjMan *)manager;
    Person one = person(om, 1), two = person(om, 2), invalid = person(om, 3, false);
    Person duplicate_a = person(om, 4), duplicate_b = person(om, 4);
    Object *machine = vehicle(om, 10);
    Object *objects[] = { (Object *)one.human, (Object *)two.human, (Object *)invalid.human,
                          (Object *)duplicate_a.human, (Object *)duplicate_b.human, machine };
    put<Object **>(manager, 0xf0, objects); put<unsigned>(manager, 0xfc, (unsigned)(sizeof objects / sizeof objects[0]));
    uint32_t vid = fnv((const unsigned char *)machine->full, strlen(machine->full));
    vitals_start(); vitals_peer(777);
    expect(vitals_epoch() != 0 && vitals_test_peer_epoch() == 777, "per-process identities initialized");
    uint32_t now = GetTickCount(), seq = 0;
    vitals_frame(om, now, capture);
    vitals::Entry incoming[] = { vitals::soldier(1, 0.3f), vitals::soldier(2, 0.6f),
                                 vitals::soldier(3, 0.2f), vitals::soldier(4, 0.1f) };
    ready = false; size_t before = output.size(); deliver(incoming, 4, seq);
    expect(output.size() == before && get<float>(one.human, 0x7c8) == 1.f, "unready number exchange is not acknowledged or applied");
    ready = true; deliver(incoming, 4, seq);
    uint8_t status[vitals::vehicle_bytes] = {}; status[2] = 7;
    vitals::Entry veh = vitals::machine(vid, status); deliver(&veh, 1, seq);
    expect(output.size() >= before + 2, "accepted records acknowledged");
    for (unsigned step = 1; step <= 30; ++step) vitals_frame(om, now + step * 100, capture);
    expect(get<float>(one.human, 0x7c8) == 1.f && get<float>(two.human, 0x7c8) == 1.f,
           "state not applied before startup validation finishes");
    vitals_frame(om, now + 3200, capture);
    expect(get<float>(one.human, 0x7c8) == 0.3f && get<unsigned>(one.record, 0x2c) == 5,
           "validated death updates health and owned squad record");
    expect(get<void *>(one.human, 0x2388) == nullptr && get<void *>(one.human, 0x7d0) == nullptr,
           "validated death detaches both squad references");
    expect(get<float>(two.human, 0x7c8) == 0.6f, "alive health merged downward");
    expect(get<float>(invalid.human, 0x7c8) == 1.f && get<void *>(invalid.human, 0x2388) == invalid.squad,
           "missing or mismatched squad record refuses death without detaching");
    expect(get<float>(duplicate_a.human, 0x7c8) == 1.f && get<float>(duplicate_b.human, 0x7c8) == 1.f,
           "ambiguous local IDs are never applied");
    expect(*((unsigned char *)machine + 0xd09) == 7, "vehicle status transition applied after baseline");
    vitals::Entry heal = vitals::soldier(2, 0.9f); deliver(&heal, 1, seq);
    vitals_frame(om, now + 3400, capture);
    expect(get<float>(two.human, 0x7c8) == 0.6f, "later higher peer health cannot heal");
    // A readable but read-only human must be skipped, not written.
    DWORD previous; expect(VirtualProtect(two.human, 0x3000, PAGE_READONLY, &previous) != 0, "protect fake human");
    vitals::Entry worse = vitals::soldier(2, 0.55f); deliver(&worse, 1, seq);
    vitals_frame(om, now + 3600, capture);
    expect(get<float>(two.human, 0x7c8) == 0.6f, "read-only health is not written");
    DWORD ignored; VirtualProtect(two.human, 0x3000, previous, &ignored);
    vitals_frame(om, now + 3800, capture);
    expect(get<float>(two.human, 0x7c8) == 0.55f, "pending health applies when writable again");
    alignas(8) unsigned char statistics[0x140] = {};
    put<unsigned>(statistics, 0x130, 0x12345678);
    vitals_frame(om, GetTickCount(), capture);
    vitals_test_stat_invoke(statistics, fake_stat);
    expect(get<unsigned>(statistics, 0x130) == 0x4d495553 && original_stat_calls == 1,
           "validated active session seeds the synthetic statistics object and calls original once");
    vitals_pause(); put<unsigned>(statistics, 0x130, 0x12345678);
    vitals_test_stat_invoke(statistics, fake_stat);
    expect(get<unsigned>(statistics, 0x130) == 0x12345678 && original_stat_calls == 2,
           "paused session leaves statistics untouched and still calls original");
    // A changed peer cannot use old packets or old state.
    vitals_peer(888); expect(vitals_test_peer_epoch() == 888 && vitals_test_outstanding() == 0, "new peer clears state and transmissions");
    before = output.size(); deliver(&worse, 1, seq); expect(output.size() == before, "old peer epoch is not acknowledged");
    vitals_peer(777); expect(vitals_test_peer_epoch() == 888, "retired handshake cannot roll peer identity back");
    vitals_pause();
    for (Person p : { one, two, invalid, duplicate_a, duplicate_b }) { release(p.human); release(p.squad); release(p.record); }
    release(machine);
    printf("vitals engine adapter selftest: %d checks, %d failed\n", checks, failures);
    return failures ? 1 : 0;
}

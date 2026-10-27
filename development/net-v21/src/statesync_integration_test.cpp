// Exercise the real state-fragment receiver and apply path with a mock object manager, no game.
#include "common.h"
#include "protocol.h"
#include "netlog.h"
#include <vector>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>

size_t statesync_test_incoming_count();
bool statesync_test_fresh(u64);
double statesync_test_budget();
static int checks, failed, ack_count, loads;
static Object *last_loaded;
static ObjectAck last_ack = {};
static std::vector<unsigned char> loaded;
void mp_log(const char *, ...) {}
void netlog_count(const char *, long long) {}
bool session_state_wanted(Object *) { return true; }
bool state_is_unit(Object *o) { return !strcmp(o->iface, "i_techn") || !strcmp(o->iface, "i_human"); }
bool state_save(Object *, std::vector<unsigned char> &) { return false; }
bool state_handles_match(Object *, const unsigned char *, size_t) { return true; }
int state_load(Object *o, const unsigned char *p, size_t n) { last_loaded = o; loaded.assign(p, p + n); ++loads; return 0; }
int state_load_keep_motion(Object *o, const unsigned char *p, size_t n, bool) { return state_load(o, p, n); }
u64 state_bad_lookups() { return 0; }
static bool owns(Object *) { return true; }
static void send_packet(const void *p, int n) {
    if (n == sizeof(ObjectAck)) { memcpy(&last_ack, p, sizeof last_ack); ++ack_count; }
}
static void check(bool b, const char *name) { ++checks; if (!b) { ++failed; printf("FAIL: %s\n", name); } }
static void receive(ObjectFrag f, const std::vector<unsigned char> &payload) {
    std::vector<unsigned char> packet(sizeof f + payload.size());
    memcpy(packet.data(), &f, sizeof f);
    if (!payload.empty()) memcpy(packet.data() + sizeof f, payload.data(), payload.size());
    statesync_on_packet(packet.data(), (int)packet.size(), send_packet, false);
}
static ObjectFrag fragment(u64 key, unsigned version, unsigned raw, unsigned delta, unsigned index, unsigned count, unsigned length) {
    ObjectFrag f = {}; f.h = make_head(PKT_OBJECT, length); f.key = key; f.version = version;
    f.raw_size = raw; f.delta_size = delta; f.frag = (unsigned short)index; f.nfrags = (unsigned short)count; return f;
}
int main() {
    alignas(8) unsigned char storage[0x108] = {};
    ObjMan *manager = (ObjMan *)storage;
    SetEnvironmentVariableA("MP_STATE_KBPS", "2147483647");
    statesync_owner_frame(manager, 1000, owns, send_packet);
    statesync_owner_frame(manager, 2000, owns, send_packet);
    check(statesync_test_budget() == (double)INT_MAX * 1024.0 * 0.25, "maximum rate avoids signed multiplication overflow");

    const u64 key = (1ull << 32) | 123;
    std::vector<unsigned char> a = {0,4,1,2,3,4};
    ObjectFrag f = fragment(key, 1, 4, (unsigned)a.size(), 0, 1, (unsigned)a.size());
    receive(f, a);
    check(ack_count == 1 && last_ack.version == 1 && statesync_test_fresh(key), "valid first state acknowledged and pending");
    statesync_apply(manager);
    check(loads == 0 && statesync_test_fresh(key), "missing object keeps its state pending");
    Object unit = {}; unit.iface = "i_techn"; strcpy_s(unit.full, "i_techn:unit_7b");
    Object *all[] = {&unit};
    *(Object ***)(storage + 0xf0) = all; *(u32 *)(storage + 0xfc) = 1;
    statesync_apply(manager);
    const unsigned char expect_a[] = {1,2,3,4};
    check(loads == 1 && loaded.size() == 4 && !memcmp(loaded.data(), expect_a, 4), "late-created object receives state");

    std::vector<unsigned char> b = {0,4,5,6,7,8};
    f.version = 2; receive(f, b); statesync_apply(manager);
    check(loads == 2 && loaded[0] == 5 && last_ack.version == 2, "newer state applied");
    f.version = 1; receive(f, a); statesync_apply(manager);
    check(loads == 2 && last_ack.version == 2, "delayed older full baseline cannot roll state back");

    std::vector<unsigned char> bad = {0x80};
    f.version = 3; f.delta_size = f.h.count = 1;
    int before = ack_count; receive(f, bad);
    check(ack_count == before, "invalid delta is not acknowledged");
    f.delta_size = f.h.count = (unsigned short)a.size(); receive(f, a); statesync_apply(manager);
    check(ack_count == before + 1 && last_ack.version == 3 && loads == 3, "same-version valid retry succeeds after decode failure");

    std::vector<unsigned char> large(1103, 9); large[0] = 0; large[1] = 0xcc; large[2] = 0x08; // 1100 bytes
    std::vector<unsigned char> first(large.begin(), large.begin() + 1100), last(large.begin() + 1100, large.end());
    ObjectFrag part = fragment(key, 4, 1100, 1103, 0, 2, 1100);
    before = ack_count; receive(part, first);
    ObjectFrag inconsistent = fragment(key, 4, 1100, 1104, 1, 2, 4);
    receive(inconsistent, std::vector<unsigned char>(4, 9));
    part.frag = 1; part.h.count = 3; receive(part, last);
    check(ack_count == before, "different fragment metadata cannot mix into one assembly");
    part.frag = 0; part.h.count = 1100; receive(part, first); statesync_apply(manager);
    check(ack_count == before + 1 && last_ack.version == 4 && loads == 4 && loaded.size() == 1100,
          "reordered matching fragments assemble after retry");
    std::vector<unsigned char> overflow_varint = {0xff, 0xff, 0xff, 0xff, 0x10, 0};
    f = fragment(key, 5, 4, (unsigned)overflow_varint.size(), 0, 1, (unsigned)overflow_varint.size());
    before = ack_count; receive(f, overflow_varint);
    check(ack_count == before, "overflowing u32 varint is rejected without an ACK");
    f.delta_size = f.h.count = (unsigned short)a.size(); receive(f, a); statesync_apply(manager);
    check(ack_count == before + 1 && loads == 5, "valid retransmission after overflowing varint");

    f = fragment(key + 1, 1, 1, 1, 0, 3, 1); // impossible number of fragments
    size_t entries = statesync_test_incoming_count(); before = ack_count; receive(f, std::vector<unsigned char>(1, 0));
    check(statesync_test_incoming_count() == entries && ack_count == before, "malformed shape rejected before allocating");
    statesync_forget_peer();
    check(statesync_test_incoming_count() == 0, "peer change clears old bases and partials");
    Object replacement = {}; replacement.iface = "i_techn"; strcpy_s(replacement.full, "i_techn:unit_7b");
    all[0] = &replacement; // same count, different actual object after a new peer/battle
    f = fragment(key, 1, 4, (unsigned)a.size(), 0, 1, (unsigned)a.size()); receive(f, a); statesync_apply(manager);
    check(last_ack.version == 1 && loads == 6 && loaded[0] == 1 && last_loaded == &replacement,
          "new peer version one uses the current object even if the count is unchanged");
    printf("state sync integration selftest: %d checks, %d failed\n", checks, failed);
    return failed ? 1 : 0;
}

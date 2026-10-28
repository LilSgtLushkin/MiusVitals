// Production xlate reader and table gating, without a game or patched engine.
#include "common.h"
#include "protocol.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unordered_map>
#include <utility>
#include <vector>

u32 xlate_test_fingerprint(const void *p);
bool xlate_test_tables_complete();
void xlate_test_send_changed(ObjMan *);
void xlate_test_match(const std::vector<std::pair<u32, u32>> &theirs,
                      const std::vector<std::pair<u32, u32>> &mine,
                      std::unordered_map<u32, u32> &out);
void mp_log(const char *, ...) {}
bool session_connected() { return true; }
int session_role() { return ROLE_JOIN; }
bool state_remote_loading() { return false; }
static std::vector<unsigned char> queued_kinds;
void shots_queue(ShotRec &r) { queued_kinds.push_back(r.kind); }
bool readable(const void *p, size_t n)
{
    MEMORY_BASIC_INFORMATION m = {};
    if (!p || !n || !VirtualQuery(p, &m, sizeof m) || m.State != MEM_COMMIT ||
        (m.Protect & (PAGE_NOACCESS | PAGE_GUARD))) return false;
    uintptr_t at = (uintptr_t)p, base = (uintptr_t)m.BaseAddress;
    return at >= base && at - base <= m.RegionSize && n <= m.RegionSize - (at - base);
}

static int checks, failed;
static void check(bool ok, const char *label) {
    ++checks;
    if (!ok) { ++failed; printf("FAIL: %s\n", label); }
}
int main()
{
    using Table = std::vector<std::pair<u32, u32>>;
    std::unordered_map<u32, u32> mapping;
    const Table forward_theirs = { {10, 101}, {11, 0}, {12, 0}, {13, 202} };
    const Table forward_mine = { {20, 202}, {21, 77}, {22, 77}, {23, 101}, {24, 77}, {25, 77} };
    xlate_test_match(forward_theirs, forward_mine, mapping);
    check(mapping.size() == 4 && mapping[10] == 23 && mapping[11] == 24 && mapping[12] == 25 && mapping[13] == 20,
          "forward pass preserves the existing anchor and positional inference");
    // This asserts the algorithm's output, not the truth of an ambiguous positional match.
    const Table leading_theirs = { {30, 0}, {31, 303}, {32, 0}, {33, 404} };
    const Table leading_mine = { {40, 0}, {41, 303}, {42, 0}, {43, 404} };
    xlate_test_match(leading_theirs, leading_mine, mapping);
    check(mapping.size() == 4 && mapping[30] == 40 && mapping[31] == 41 && mapping[32] == 42 && mapping[33] == 43,
          "backward pass covers leading positions before the first anchor");
    xlate_test_match(Table{{50, 505}}, Table{{60, 505}, {61, 505}}, mapping);
    check(mapping.empty(), "a fingerprint unique on only one side is not an anchor");
    xlate_test_match(Table{{70, 0}, {71, 0}}, Table{{80, 0}, {81, 0}}, mapping);
    check(mapping.empty(), "zero fingerprints alone do not create anchors");
    // Fixture ground truth swaps two indistinguishable entries between the anchors. The matcher
    // cannot observe that fact: no unmatched entries does not establish that the pairs are right.
    xlate_test_match(Table{{1, 101}, {2, 0}, {3, 0}, {4, 202}},
                     Table{{11, 101}, {12, 0}, {13, 0}, {14, 202}}, mapping);
    check(mapping.size() == 4 && mapping[2] == 12 && mapping[3] == 13 &&
          mapping[2] != 13 && mapping[3] != 12,
          "KNOWN LIMITATION: fully placed ambiguous entries can disagree with fixture identity");

    SYSTEM_INFO info = {};
    GetSystemInfo(&info);
    size_t page = info.dwPageSize;
    unsigned char *region = (unsigned char *)VirtualAlloc(nullptr, page * 2, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!region) return 2;
    DWORD old = 0;
    if (!VirtualProtect(region + page, page, PAGE_NOACCESS, &old)) return 2;

    unsigned char desc[0x60] = {};
    const u32 zero = xlate_test_fingerprint(desc);
    char *s = (char *)region + page - 16;
    memset(s, 'A', 16);                   // first 16 readable, next byte inaccessible
    memcpy(desc, &s, sizeof s);
    check(xlate_test_fingerprint(desc) == zero, "unterminated boundary string ignored safely");
    memcpy(s, "abc", 4);
    check(xlate_test_fingerprint(desc) != zero, "short terminated boundary string hashed");

    s = (char *)region + page - 20;
    memset(s, 'B', 16);
    s[16] = 0;                           // less than 64 readable bytes, but terminator is readable
    memcpy(desc, &s, sizeof s);
    u32 near_end = xlate_test_fingerprint(desc);
    char *interior = (char *)region + 128;
    memset(interior, 'B', 16); interior[16] = 0;
    memcpy(desc, &interior, sizeof interior);
    check(near_end == xlate_test_fingerprint(desc), "same hash across readable page boundary cases");
    VirtualFree(region, 0, MEM_RELEASE);

    // Complete both received number tables in the actual code while no verified game hooks exist.
    // This must not enable loading of a remote full object state into arbitrary local numbers.
    unsigned char *manager = (unsigned char *)VirtualAlloc(nullptr, 0x2000, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!manager) return 2;
    Object object = {};
    strcpy_s(object.full, "i_human:test");
    Object *objects[] = { &object, nullptr };
    *(Object ***)(manager + 0x150) = objects;
    *(u32 *)(manager + 0x15c) = 1;
    ShotRec empty_desc = {};
    empty_desc.kind = 3;
    ShotRec one_object = {};
    one_object.kind = 4;
    u32 total = 1, count = 1, pair[2] = {0, 123};
    memcpy(one_object.launch, &total, 4);
    memcpy(one_object.launch + 4, &count, 4);
    memcpy(one_object.launch + 8, pair, sizeof pair);
    xlate_received(one_object, (ObjMan *)manager);
    check(!xlate_test_tables_complete(), "object table alone cannot stand in for a missing description table");
    xlate_received(empty_desc, (ObjMan *)manager);
    check(xlate_test_tables_complete(), "explicit empty description and complete object tables are ready");
    check(!xlate_ready(), "tables alone never enable full-state loading without verified hooks");

    // Produce a real, unique description mapping without installing any engine hooks. A peer
    // reset or a malformed new table must not leave the previous map accessible to legacy callers.
    alignas(8) unsigned char mapped_desc[0x60] = {}, mapped_row[0x10] = {};
    const u32 remote_id = 41, local_id = 7;
    memcpy(mapped_row, &local_id, 4);
    void *mapped_pointer = mapped_desc;
    memcpy(mapped_row + 8, &mapped_pointer, sizeof mapped_pointer);
    *(void **)(manager + 0x1748) = mapped_row;
    *(u32 *)(manager + 0x1754) = 1;
    ShotRec one_desc = {};
    one_desc.kind = 3;
    total = count = 1;
    pair[0] = remote_id; pair[1] = xlate_test_fingerprint(mapped_desc);
    memcpy(one_desc.launch, &total, 4);
    memcpy(one_desc.launch + 4, &count, 4);
    memcpy(one_desc.launch + 8, pair, sizeof pair);
    xlate_received(one_desc, (ObjMan *)manager);
    u32 translated = 0;
    check(xlate_desc_checked(remote_id, &translated) && translated == local_id &&
          xlate_desc(remote_id) == local_id, "fixture formed a real description mapping");
    xlate_resend();
    check(!xlate_ready() && !xlate_test_tables_complete(), "new table disables stale mappings");
    check(!xlate_desc_checked(remote_id, &translated) && xlate_desc(remote_id) == remote_id,
          "peer reset clears the old map even for legacy unchecked callers");

    // Restore both tables and then feed an invalid table header; it must invalidate the map too.
    xlate_received(one_object, (ObjMan *)manager);
    check(!xlate_test_tables_complete(), "peer reset forgets that the prior peer sent a description table");
    xlate_received(one_desc, (ObjMan *)manager);
    check(xlate_desc(remote_id) == local_id, "mapping is rebuilt after a fresh exchange");
    total = 1000001;
    memcpy(one_object.launch, &total, 4);
    xlate_received(one_object, (ObjMan *)manager);
    check(!xlate_test_tables_complete() && !xlate_ready() && xlate_desc(remote_id) == remote_id,
          "oversized remote table invalidates and clears stale mappings");
    xlate_resend();
    *(void **)(manager + 0x1748) = nullptr;
    *(u32 *)(manager + 0x1754) = 0;

    // Drive the real sender without waiting for the three-second clock or installing game hooks.
    xlate_test_send_changed((ObjMan *)manager);
    check(queued_kinds.size() == 2 && queued_kinds[0] == 3 && queued_kinds[1] == 4,
          "a fresh peer receives both tables");
    queued_kinds.clear();
    xlate_test_send_changed((ObjMan *)manager);
    check(queued_kinds.empty(), "unchanged counters send nothing");
    *(u32 *)(manager + 0x15c) = 2; // an unreadable slot changes the count, not the transferable table
    xlate_test_send_changed((ObjMan *)manager);
    check(queued_kinds.empty(), "changed counter alone does not resend an unchanged table");
    objects[1] = &object;
    *(u32 *)(manager + 0x15c) = 1; // force another scan without changing the table yet
    xlate_test_send_changed((ObjMan *)manager);
    *(u32 *)(manager + 0x15c) = 2;
    xlate_test_send_changed((ObjMan *)manager);
    check(queued_kinds.size() == 1 && queued_kinds[0] == 4, "only changed object table is queued");
    queued_kinds.clear();
    alignas(8) unsigned char raw_desc[0x60] = {}, row[0x10] = {};
    *(u32 *)row = 7; *(void **)(row + 8) = raw_desc;
    *(void **)(manager + 0x1748) = row; *(u32 *)(manager + 0x1754) = 1;
    xlate_test_send_changed((ObjMan *)manager);
    check(queued_kinds.size() == 1 && queued_kinds[0] == 3, "only changed description table is queued");
    queued_kinds.clear();
    xlate_resend();
    xlate_test_send_changed((ObjMan *)manager);
    check(queued_kinds.size() == 2 && queued_kinds[0] == 3 && queued_kinds[1] == 4,
          "peer reset sends both tables even if contents match");
    VirtualFree(manager, 0, MEM_RELEASE);

    printf("translation selftest: %d checks, %d failed\n", checks, failed);
    return failed ? 1 : 0;
}

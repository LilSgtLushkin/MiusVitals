// Production xlate reader and table gating, without a game or patched engine.
#include "common.h"
#include "protocol.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

u32 xlate_test_fingerprint(const void *p);
bool xlate_test_tables_complete();
void mp_log(const char *, ...) {}
bool session_connected() { return true; }
int session_role() { return ROLE_JOIN; }
bool state_remote_loading() { return false; }
void shots_queue(ShotRec &) {}
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
    Object *objects[] = { &object };
    *(Object ***)(manager + 0x150) = objects;
    *(u32 *)(manager + 0x15c) = 1;
    ShotRec empty_desc = {};
    empty_desc.kind = 3;
    xlate_received(empty_desc, (ObjMan *)manager);
    ShotRec one_object = {};
    one_object.kind = 4;
    u32 total = 1, count = 1, pair[2] = {0, 123};
    memcpy(one_object.launch, &total, 4);
    memcpy(one_object.launch + 4, &count, 4);
    memcpy(one_object.launch + 8, pair, sizeof pair);
    xlate_received(one_object, (ObjMan *)manager);
    check(xlate_test_tables_complete(), "fixture completed number tables");
    check(!xlate_ready(), "tables alone never enable full-state loading without verified hooks");
    total = 1000001;
    memcpy(one_object.launch, &total, 4);
    xlate_received(one_object, (ObjMan *)manager);
    check(!xlate_test_tables_complete() && !xlate_ready(), "oversized remote table invalidates stale mappings");
    xlate_resend();
    check(!xlate_ready() && !xlate_test_tables_complete(), "new table disables stale mappings");
    VirtualFree(manager, 0, MEM_RELEASE);

    printf("translation selftest: %d checks, %d failed\n", checks, failed);
    return failed ? 1 : 0;
}

#include "squad_ownership.h"
#include <stdio.h>
#include <unordered_set>

static int checks, failed;
static void check(bool ok, const char *what) {
    ++checks;
    if (!ok) { ++failed; printf("FAIL: %s\n", what); }
}
int main() {
    SquadOwnership owned;
    std::unordered_set<unsigned long long> units = {(2ull << 32) | 0x101, (1ull << 32) | 0x120};
    owned.rebuild(units);
    check(owned.owns(0x100) && owned.owns(0x11f), "members share their squad ID");
    check(owned.owns(0x120) && !owned.owns(0x200), "only indexed squads belong to player");
    units = {(2ull << 32) | 0x201, (1ull << 32) | 0x220};
    owned.rebuild(units);
    check(!owned.owns(0x100) && !owned.owns(0x120), "same-size replacement removes stale squads");
    check(owned.owns(0x200) && owned.owns(0x23f), "same-size replacement adds new squads");
    units.clear(); owned.rebuild(units);
    check(!owned.owns(0x200), "empty replacement removes all ownership");
    units.insert((2ull << 32) | 0x80000305u); owned.rebuild(units);
    check(owned.owns(0x300) && owned.owns(0x8000031f), "existing high-bit and low-five-bit normalization is preserved");
    owned.clear(); check(!owned.owns(0x300), "explicit reset clears ownership");
    printf("squad ownership selftest: %d checks, %d failed\n", checks, failed);
    return failed ? 1 : 0;
}

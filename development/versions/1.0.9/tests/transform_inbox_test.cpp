#include "transform_inbox.h"
#include <stdio.h>
#include <limits>

static int checks, failed;
static void check(bool ok, const char *name) {
    ++checks;
    if (!ok) { ++failed; printf("FAIL: %s\n", name); }
}
int main()
{
    transforms::Inbox in;
    UnitRec a = {}; a.kind = 1; a.id = 11; a.pos[0] = 1;
    UnitRec b = {}; b.kind = 2; b.id = 11; b.pos[0] = 2;
    check(in.put(a, 10) == transforms::Put::inserted && in.put(b, 10) == transforms::Put::inserted, "kind and id distinguish units");
    a.pos[0] = 3;
    check(in.put(a, 11) == transforms::Put::replaced && in.records().size() == 2, "newer same-step arrivals coalesce");
    check(in.records()[0].pos[0] == 3 && in.records()[1].pos[0] == 2, "newest snapshot wins without changing unit order");
    a.pos[0] = 5;
    check(in.put(a, 10) == transforms::Put::stale && in.records()[0].pos[0] == 3, "older arrival cannot replace a newer snapshot");
    check(in.put(a, 11) == transforms::Put::stale, "duplicate unit in the same snapshot ignored");
    UnitRec bad = a;
    bad.kind = 99;
    check(in.put(bad, 12) == transforms::Put::invalid, "unknown unit kind rejected");
    bad = a; bad.pos[1] = std::numeric_limits<float>::quiet_NaN();
    check(in.put(bad, 12) == transforms::Put::invalid && in.records()[0].pos[0] == 3, "NaN cannot replace a valid update");
    bad = a; bad.rot[8] = std::numeric_limits<float>::infinity();
    check(in.put(bad, 12) == transforms::Put::invalid, "infinite rotation rejected");
    in.drain();
    check(in.records().empty() && in.put(a, 10) == transforms::Put::stale, "late packet cannot roll a unit back after a simulation step");
    check(in.put(a, 12) == transforms::Put::inserted, "newer snapshot is accepted after a simulation step");
    b.pos[0] = 4;
    check(in.put(b, 11) == transforms::Put::inserted, "a globally older chunk remains useful for a different unit");
    in.clear();
    check(in.records().empty() && in.put(a, 1) == transforms::Put::inserted, "new peer generation clears order memory");
    for (uint32_t i = 2; i < 10002; i++) { a.pos[0] = (float)i; in.put(a, i); }
    check(in.records().size() == 1 && in.records()[0].pos[0] == 10001, "repeated paused arrivals remain bounded");
    in.clear();
    check(in.put(a, 0xfffffff0u) == transforms::Put::inserted &&
          in.put(a, 0x10u) == transforms::Put::replaced &&
          in.put(a, 0xfffffff1u) == transforms::Put::stale, "per-unit snapshot ordering handles 32-bit wrap");
    transforms::HeaderOrder header;
    check(header.accept(20) && !header.accept(19) && !header.accept(20) && header.accept(21), "clock/control header accepts only new snapshots");
    header.reset();
    check(header.accept(0xfffffff0u) && header.accept(0x10u) && !header.accept(0xfffffff1u), "clock/control order handles 32-bit wrap");
    header.reset();
    check(header.accept(0), "new generation can start at snapshot zero");
    printf("transform inbox selftest: %d checks, %d failed\n", checks, failed);
    return failed ? 1 : 0;
}

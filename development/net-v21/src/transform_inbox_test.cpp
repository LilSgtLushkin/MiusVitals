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
    check(in.put(a) == transforms::Put::inserted && in.put(b) == transforms::Put::inserted, "kind and id distinguish units");
    a.pos[0] = 3;
    check(in.put(a) == transforms::Put::replaced && in.records().size() == 2, "same-step arrivals coalesced");
    check(in.records()[0].pos[0] == 3 && in.records()[1].pos[0] == 2, "latest arrival wins without changing unit order");
    UnitRec bad = a;
    bad.kind = 99;
    check(in.put(bad) == transforms::Put::invalid, "unknown unit kind rejected");
    bad = a; bad.pos[1] = std::numeric_limits<float>::quiet_NaN();
    check(in.put(bad) == transforms::Put::invalid && in.records()[0].pos[0] == 3, "NaN cannot replace a valid update");
    bad = a; bad.rot[8] = std::numeric_limits<float>::infinity();
    check(in.put(bad) == transforms::Put::invalid, "infinite rotation rejected");
    in.clear();
    check(in.records().empty() && in.put(a) == transforms::Put::inserted, "reset removes indices on peer change");
    for (int i = 0; i < 10000; i++) { a.pos[0] = (float)i; in.put(a); }
    check(in.records().size() == 1 && in.records()[0].pos[0] == 9999, "repeated paused arrivals remain bounded");
    printf("transform inbox selftest: %d checks, %d failed\n", checks, failed);
    return failed ? 1 : 0;
}

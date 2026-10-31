#include "retry_gate.h"
#include <stdio.h>

static int checks, failed;
static void check(bool b, const char *s) { ++checks; if (!b) { ++failed; printf("FAIL: %s\n", s); } }
int main()
{
    RetryGate g;
    check(g.due(0, 5000), "first try at tick zero");
    check(!g.due(0, 5000) && !g.due(4999, 5000), "no per-frame retry after tick zero");
    check(g.due(5000, 5000), "retry after full interval");
    g.reset();
    check(g.due(5001, 5000), "changed input can be tried immediately");
    g.reset();
    check(g.due(0xfffffff0u, 40), "near tick wrap");
    check(!g.due(10, 40) && g.due(24, 40), "elapsed time across wrap");
    printf("retry gate selftest: %d checks, %d failed\n", checks, failed);
    return failed ? 1 : 0;
}

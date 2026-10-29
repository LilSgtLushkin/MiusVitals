#include "peer_guard.h"
#include <stdio.h>
static int checks, failed;
static void check(bool value, const char *name) { ++checks; if (!value) { ++failed; printf("FAIL: %s\n", name); } }
int main()
{
    using peer_guard::reject_hello;
    check(reject_hello(true, true, false, 1999, 1000), "foreign active peer rejected");
    check(!reject_hello(true, true, true, 1999, 1000), "same peer accepted");
    check(!reject_hello(true, false, false, 1999, 1000), "no peer may connect");
    check(!reject_hello(false, true, false, 1999, 1000), "non-tactical unchanged");
    check(reject_hello(true, true, false, 3999, 1000), "guard lasts just under three seconds");
    check(!reject_hello(true, true, false, 4000, 1000), "different-port reconnect accepted after three seconds");
    check(reject_hello(true, true, false, 0x10u, 0xfffffff0u), "clock wrap with recent peer");
    check(!reject_hello(true, true, false, 0x1000u, 0xfffffff0u), "clock wrap with stale peer");
    printf("peer guard selftest: %d checks, %d failed\n", checks, failed);
    return failed ? 1 : 0;
}

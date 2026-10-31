#include "state_apply_delay.h"
#include <stdio.h>

static int checks, failed;
static void check(bool ok, const char *label) {
    ++checks;
    if (!ok) { ++failed; printf("FAIL: %s\n", label); }
}
int main()
{
    int a, b;
    StateApplyDelay gate;
    check(!gate.ready(&a, 100, 12000), "new manager waits");
    check(!gate.ready(&a, 12099, 12000), "default gate does not open early");
    check(gate.ready(&a, 12100, 12000), "same manager opens at its deadline");
    check(gate.ready(&a, 50000, 12000), "same manager remains open");
    check(!gate.ready(&b, 50001, 12000), "changed manager starts a fresh wait");
    check(!gate.ready(&b, 62000, 12000) && gate.ready(&b, 62001, 12000), "new manager gets its own full delay");
    gate.reset();
    check(!gate.ready(&b, 70000, 12000), "session restart resets even a reused address");
    check(gate.ready(&b, 70000, 0), "explicit zero delay remains immediate");
    gate.reset();
    check(!gate.ready(&a, 0xfffffff0u, 32), "gate starts before clock wrap");
    check(!gate.ready(&a, 0x0000000fu, 32) && gate.ready(&a, 0x00000010u, 32), "deadline handles 32-bit wrap");
    printf("state apply delay selftest: %d checks, %d failed\n", checks, failed);
    return failed ? 1 : 0;
}

#include "transform_send.h"
#include <stdio.h>
static int checks, failures;
static void check(bool ok, const char *name) { ++checks; if (!ok) { ++failures; printf("FAIL: %s\n", name); } }
int main()
{
    transforms::Sent s;
    float p[3] = {0,0,0};
    float r[12] = {1,0,0,44, 0,1,0,55, 0,0,1,66};
    check(s.due(p, r, 1, 0), "first observation is always sent");
    s.remember(p, r, 1);
    check(!s.due(p, r, 2, 0), "unchanged early state waits");
    r[3] = r[7] = r[11] = 0;
    check(!s.due(p, r, 3, 0), "matrix padding is ignored");
    r[0] += .001f;
    check(s.due(p, r, 4, 0), "row zero movement");
    r[0] -= .001f; r[4] += .001f;
    check(s.due(p, r, 5, 0), "roll in row one without row zero movement");
    r[4] -= .001f; r[8] += .001f;
    check(s.due(p, r, 6, 0), "pitch in row two without row zero movement");
    r[8] -= .001f; r[5] += .0001f; r[9] += .0001f;
    check(!s.due(p, r, 7, 0), "per-row thresholds do not sum separate small changes");
    r[5] -= .0001f; r[9] -= .0001f;
    p[2] += .003f;
    check(s.due(p, r, 8, 0), "position movement");
    p[2] -= .003f;
    check(!s.due(p, r, 2000, 0) && s.due(p, r, 2001, 0), "two-second refresh");
    check(!s.due(p, r, 2450, 15) && s.due(p, r, 2451, 15), "staggered refresh");
    check(s.due(p, r, 2, 15, 0), "explicit zero repeats every tick");
    check(!s.due(p, r, 500, 15, 500) && s.due(p, r, 501, 15, 500), "explicit interval removes stagger");
    check(!s.due(p, r, 2450, 15, 2000) && s.due(p, r, 2451, 15, 2000), "explicit 2000 retains stagger");
    check(!s.due(p, r, 2, 15, 6001), "invalid interval preserves default");
    s.remember(p, r, 0xfffffff0u);
    check(!s.due(p, r, 0x00000010u, 0) && s.due(p, r, 0x000007c0u, 0), "32-bit clock wrap");
    printf("transform send selftest: %d checks, %d failed\n", checks, failures);
    return failures ? 1 : 0;
}

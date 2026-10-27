#include "transform_write.h"
#include <stdio.h>
#include <stdint.h>

static int checks, failed;
static void check(bool ok, const char *name) {
    ++checks;
    if (!ok) { ++failed; printf("FAIL: %s\n", name); }
}
int main()
{
    float pos[3] = {}, rot[12] = {}, inv[12] = {}, lin[3] = {}, ang[3] = {};
    float p[3] = { 2, -3, 4 }, r[9] = { 1,2,3,4,5,6,7,8,9 }, v[3] = { 10,11,12 }, w[3] = { 13,14,15 };
    rot[3] = rot[7] = rot[11] = 99;
    inv[3] = inv[7] = inv[11] = 77;
    check(write_remote_transform(pos, rot, inv, lin, ang, p, r, v, w, true), "changed transform written");
    check(!memcmp(pos, p, sizeof p) && !memcmp(lin, v, sizeof v) && !memcmp(ang, w, sizeof w), "position and velocities written");
    bool matrix = true;
    for (int i = 0; i < 3; i++) for (int j = 0; j < 3; j++)
        matrix &= rot[i * 4 + j] == r[i * 3 + j] && inv[i * 4 + j] == r[j * 3 + i];
    check(matrix, "rotation and inverse transpose");
    check(rot[3] == 99 && rot[7] == 99 && rot[11] == 99 && inv[3] == 77 && inv[7] == 77 && inv[11] == 77,
          "matrix padding untouched");
    check(!write_remote_transform(pos, rot, inv, lin, ang, p, r, v, w, true), "identical bits elided");
    inv[4] += 1;
    check(write_remote_transform(pos, rot, inv, lin, ang, p, r, v, w, true) && inv[4] == r[1], "stale inverse repaired");
    lin[2] += 1;
    check(write_remote_transform(pos, rot, inv, lin, ang, p, r, v, w, true) && lin[2] == v[2], "velocity difference is written");
    lin[0] = 123; ang[0] = 456;
    check(!write_remote_transform(pos, rot, inv, lin, ang, p, r, v, w, false) && lin[0] == 123 && ang[0] == 456,
          "stationary update leaves velocities untouched");
    p[1] += 1;
    check(write_remote_transform(pos, rot, inv, lin, ang, p, r, v, w, false) && lin[0] == 123 && ang[0] == 456,
          "changed stationary transform still leaves velocities untouched");
    // +0 and -0 compare equal as floats, but not as the exact bits the engine would have stored.
    uint32_t negative_zero = 0x80000000u;
    memcpy(&p[0], &negative_zero, 4); pos[0] = 0.0f;
    check(write_remote_transform(pos, rot, inv, lin, ang, p, r, v, w, true) && !memcmp(pos, p, sizeof p), "signed zero keeps exact write semantics");
    printf("transform write selftest: %d checks, %d failed\n", checks, failed);
    return failed ? 1 : 0;
}

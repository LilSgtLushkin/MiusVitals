#include "transform_write.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

static int checks, failed;
static void check(bool ok, const char *name) {
    ++checks;
    if (!ok) { ++failed; printf("FAIL: %s\n", name); }
}

// Reference is the pre-elision production path: unconditional writes followed by a velocity
// copy or zero. Compare entire buffers (including padding) across arbitrary bit patterns.
struct Buffers { float pos[3], rot[12], inv[12], lin[3], ang[3]; };
static void reference(Buffers &b, const float *p, const float *r, const float *v, const float *w, bool moving) {
    memcpy(b.pos, p, sizeof b.pos);
    for (int row = 0; row < 3; ++row) for (int col = 0; col < 3; ++col) {
        memcpy(&b.rot[row * 4 + col], &r[row * 3 + col], sizeof(float));
        memcpy(&b.inv[col * 4 + row], &r[row * 3 + col], sizeof(float));
    }
    if (moving) { memcpy(b.lin, v, sizeof b.lin); memcpy(b.ang, w, sizeof b.ang); }
    else { memset(b.lin, 0, sizeof b.lin); memset(b.ang, 0, sizeof b.ang); }
}
static uint32_t random_word(uint32_t &x) { x ^= x << 13; x ^= x >> 17; x ^= x << 5; return x; }
static void random_bytes(void *data, size_t size, uint32_t &seed) {
    unsigned char *out = (unsigned char *)data;
    for (size_t i = 0; i < size; ++i) out[i] = (unsigned char)random_word(seed);
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
    const float zero[3] = {};
    check(write_remote_transform(pos, rot, inv, lin, ang, p, r, v, w, false) &&
          !memcmp(lin, zero, sizeof zero) && !memcmp(ang, zero, sizeof zero),
          "stationary update clears velocities even when the transform did not move");
    check(!write_remote_transform(pos, rot, inv, lin, ang, p, r, v, w, false),
          "identical stationary state is elided");
    p[1] += 1;
    check(write_remote_transform(pos, rot, inv, lin, ang, p, r, v, w, false) &&
          !memcmp(lin, zero, sizeof zero) && !memcmp(ang, zero, sizeof zero),
          "changed stationary transform keeps velocities zero");
    check(write_remote_transform(pos, rot, inv, lin, ang, p, r, v, w, true) &&
          !memcmp(lin, v, sizeof v) && !memcmp(ang, w, sizeof w),
          "moving update restores the peer velocities");
    // +0 and -0 compare equal as floats, but not as the exact bits the engine would have stored.
    uint32_t negative_zero = 0x80000000u;
    memcpy(&p[0], &negative_zero, 4); pos[0] = 0.0f;
    check(write_remote_transform(pos, rot, inv, lin, ang, p, r, v, w, true) && !memcmp(pos, p, sizeof p), "signed zero keeps exact write semantics");
    uint32_t seed = 0x98137531u;
    bool equivalent = true;
    for (int i = 0; i < 1000; ++i) {
        Buffers original, changed;
        float np[3], nr[9], nv[3], nw[3];
        random_bytes(&original, sizeof original, seed);
        random_bytes(np, sizeof np, seed); random_bytes(nr, sizeof nr, seed);
        random_bytes(nv, sizeof nv, seed); random_bytes(nw, sizeof nw, seed);
        changed = original;
        bool moving = (i & 1) != 0;
        reference(original, np, nr, nv, nw, moving);
        write_remote_transform(changed.pos, changed.rot, changed.inv, changed.lin, changed.ang, np, nr, nv, nw, moving);
        if (memcmp(&original, &changed, sizeof original) ||
            write_remote_transform(changed.pos, changed.rot, changed.inv, changed.lin, changed.ang, np, nr, nv, nw, moving)) {
            equivalent = false; break;
        }
    }
    check(equivalent, "1000 arbitrary bit-pattern updates match the pre-elision path and repeat without writes");
    // A late transform runs a unit on with the motion its owner reported. The orientation has to
    // follow the angular velocity the same way the position follows the linear one, and it has to
    // stay an orientation: rows of r are the body axes in world space.
    {
        float r[9] = { 1, 0, 0, 0, 1, 0, 0, 0, 1 };
        float none[3] = {};
        spin_orientation(r, none, 0.25f);
        const float still[9] = { 1, 0, 0, 0, 1, 0, 0, 0, 1 };   // the heading it arrived with
        check(!memcmp(r, still, sizeof r), "a unit that is not turning keeps the heading it arrived with");

        float z[3] = { 0, 0, (float)(3.14159265358979 / 2.0) };   // a quarter turn about the vertical, per second
        spin_orientation(r, z, 1.0f);
        const float quarter[9] = { 0, 1, 0, -1, 0, 0, 0, 0, 1 };  // body x goes to world y, body y to world -x
        float worst = 0;
        for (int i = 0; i < 9; ++i) worst = fmaxf(worst, fabsf(r[i] - quarter[i]));
        check(worst < 1e-5f, "a quarter turn about the vertical takes the body axes where they belong");

        // Two halves of a turn are the whole turn, and an orientation stays an orientation.
        float one[9] = { 1, 0, 0, 0, 1, 0, 0, 0, 1 }, two[9] = { 1, 0, 0, 0, 1, 0, 0, 0, 1 };
        float w[3] = { 0.7f, -1.1f, 2.3f };
        spin_orientation(one, w, 0.25f);
        spin_orientation(two, w, 0.125f); spin_orientation(two, w, 0.125f);
        worst = 0;
        for (int i = 0; i < 9; ++i) worst = fmaxf(worst, fabsf(one[i] - two[i]));
        check(worst < 1e-5f, "spinning in two halves is the same turn as spinning once");
        float skew = 0;
        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < 3; ++col) {
                float dot = 0;
                for (int i = 0; i < 3; ++i) dot += one[row * 3 + i] * one[col * 3 + i];
                skew = fmaxf(skew, fabsf(dot - (row == col ? 1.0f : 0.0f)));
            }
        }
        check(skew < 1e-5f, "a spun orientation is still orthonormal");

        // Many small steps agree with one step: the same rotation, however the frame divided it.
        float many[9] = { 1, 0, 0, 0, 1, 0, 0, 0, 1 };
        for (int i = 0; i < 400; ++i) spin_orientation(many, w, 0.25f / 400.0f);
        worst = 0;
        for (int i = 0; i < 9; ++i) worst = fmaxf(worst, fabsf(many[i] - one[i]));
        check(worst < 1e-4f, "integrating the turn over 400 steps lands on the same heading");
    }

    printf("transform write selftest: %d checks, %d failed\n", checks, failed);
    return failed ? 1 : 0;
}

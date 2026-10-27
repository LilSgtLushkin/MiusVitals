// Exact idempotent-write elision for a remote unit's already-interpolated motion. No epsilon, no
// physics pause, and no change to the shot/trench pipeline: if any written bit differs, perform the
// original writes, including the inverse rotation. Arrays with a 4-float stride retain their padding.
#pragma once
#include <string.h>

inline bool write_remote_transform(float *pos, float *rot, float *inv, float *lin, float *ang,
                                   const float *p, const float *r, const float *v, const float *w, bool moving)
{
    const float zero[3] = {};
    const float *want_lin = moving ? v : zero, *want_ang = moving ? w : zero;
    bool same = !memcmp(pos, p, 3 * sizeof(float)) &&
                !memcmp(lin, want_lin, 3 * sizeof(float)) && !memcmp(ang, want_ang, 3 * sizeof(float));
    for (int i = 0; i < 3 && same; i++) {
        if (memcmp(rot + i * 4, r + i * 3, 3 * sizeof(float))) same = false;
        for (int j = 0; j < 3 && same; j++)
            if (memcmp(inv + i * 4 + j, r + j * 3 + i, sizeof(float))) same = false;
    }
    if (same) return false;
    memcpy(pos, p, 3 * sizeof(float));
    for (int i = 0; i < 3; i++) for (int j = 0; j < 3; j++) {
        rot[i * 4 + j] = r[i * 3 + j];
        inv[j * 4 + i] = r[i * 3 + j];
    }
    memcpy(lin, want_lin, 3 * sizeof(float));
    memcpy(ang, want_ang, 3 * sizeof(float));
    return true;
}

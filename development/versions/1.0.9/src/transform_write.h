// Exact idempotent-write elision for a remote unit's already-interpolated motion. No epsilon, no
// physics pause, and no change to the shot/trench pipeline: if any written bit differs, perform the
// original writes, including the inverse rotation. Arrays with a 4-float stride retain their padding.
#pragma once
#include <string.h>
#include <cmath>

// A late transform must not freeze a turning hull. While the next one is missing the position runs
// on with the linear velocity its owner reported; the orientation used to be copied unchanged, so a
// tank in a turn slid straight ahead and then snapped to its new heading. Rows of r are the body
// axes in world space - the same rows the blend keeps square - so a world-frame turn rotates each
// of them. Rodrigues, not a small-angle approximation: a quarter second of a hard turn covers an
// arc where the difference shows.
inline void spin_orientation(float *r, const float *ang, float dt)
{
    float w[3] = { ang[0] * dt, ang[1] * dt, ang[2] * dt };
    float angle = sqrtf(w[0] * w[0] + w[1] * w[1] + w[2] * w[2]);
    if (!(angle > 1e-6f)) return;                    // not turning: heading stands as it was received
    float axis[3] = { w[0] / angle, w[1] / angle, w[2] / angle };
    float c = cosf(angle), s = sinf(angle), k = 1.0f - c, out[9];
    for (int row = 0; row < 3; ++row) {
        const float *v = r + row * 3;
        float cross[3] = { axis[1] * v[2] - axis[2] * v[1],
                           axis[2] * v[0] - axis[0] * v[2],
                           axis[0] * v[1] - axis[1] * v[0] };
        float dot = axis[0] * v[0] + axis[1] * v[1] + axis[2] * v[2];
        for (int i = 0; i < 3; ++i) out[row * 3 + i] = v[i] * c + cross[i] * s + axis[i] * dot * k;
    }
    memcpy(r, out, sizeof out);
}

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

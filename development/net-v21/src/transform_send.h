// Tactical sender: compare all three rows of the 3x4 engine rotation (padding is ignored).
// Preserve the native per-row movement threshold and staggered two-second idle refresh.
#pragma once
#include <stdint.h>
#include <math.h>
#include <string.h>

namespace transforms {
struct Sent {
    float pos[3] = {}, rot[9] = {};
    uint32_t when = 0;
    bool valid = false;

    bool due(const float *p, const float *r, uint32_t now, uint32_t id) const {
        if (!valid) return true;
        if (fabsf(p[0] - pos[0]) + fabsf(p[1] - pos[1]) + fabsf(p[2] - pos[2]) > 0.002f) return true;
        for (int row = 0; row < 3; ++row) {
            const float *cur = r + row * 4, *old = rot + row * 3;
            if (fabsf(cur[0] - old[0]) + fabsf(cur[1] - old[1]) + fabsf(cur[2] - old[2]) > 0.0005f) return true;
        }
        return (uint32_t)(now - when) >= 2000u + (id % 16u) * 30u;
    }
    void remember(const float *p, const float *r, uint32_t now) {
        memcpy(pos, p, sizeof pos);
        for (int row = 0; row < 3; ++row) memcpy(rot + row * 3, r + row * 4, 3 * sizeof(float));
        when = now;
        valid = true;
    }
};
}

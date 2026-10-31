// Wrap-safe throttling for expensive retry work. Does not delay the first attempt and can be reset
// immediately when the input changes; all times are unsigned Windows tick counts in milliseconds.
#pragma once
#include <stdint.h>

struct RetryGate {
    uint32_t last = 0;
    bool attempted = false;
    bool due(uint32_t now, uint32_t interval) {
        if (attempted && (uint32_t)(now - last) < interval) return false;
        last = now; attempted = true; return true;
    }
    void reset() { last = 0; attempted = false; }
};

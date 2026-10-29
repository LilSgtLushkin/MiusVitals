// Delay full-state application until a tactical object manager has had time to initialize.
// A manager change or session restart is a new initialization, even in the same process.
#pragma once
#include <stdint.h>

class StateApplyDelay {
    const void *manager_ = nullptr;
    uint32_t since_ = 0;
    bool started_ = false;
public:
    bool ready(const void *manager, uint32_t now, uint32_t delay_ms) {
        if (!started_ || manager_ != manager) {
            manager_ = manager;
            since_ = now;
            started_ = true;
        }
        return (uint32_t)(now - since_) >= delay_ms;
    }
    void reset() { manager_ = nullptr; since_ = 0; started_ = false; }
};

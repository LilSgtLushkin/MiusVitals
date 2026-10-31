// Host-authoritative spectator snapshot eligibility. These checks validate the serialized
// fixture format and turn markers, not the engine's real save/completion lifecycle.
#pragma once
#include "operation_turn.h"
#include "opsave_checked.h"
#include <vector>

namespace spectator_snapshot {
inline bool publishable(const std::vector<unsigned char> &candidate, bool after_worker,
                        unsigned previous_frames, const std::vector<unsigned char> &previous) {
    if (!opsave_checked::validate(candidate)) return false;
    int phase = opsave::turn_phase(candidate);
    if (phase != 0 && phase != 2) return false;
    if (after_worker && !previous.empty()) {
        if (!operation_turn::resolution_saved(phase, opsave::turns(candidate), previous_frames)) return false;
        if (candidate == previous) return false;
    }
    return true;
}
}

// Derived squad ownership. Rebuild on every unit-set replacement, even when its size is unchanged.
#pragma once
#include <stdint.h>
#include <unordered_set>

class SquadOwnership {
    std::unordered_set<uint32_t> squads_;
public:
    template <class Units> void rebuild(const Units &units) {
        squads_.clear();
        for (auto key : units) squads_.insert((uint32_t)key & 0x7fffffe0u);
    }
    bool owns(uint32_t id) const { return squads_.count(id & 0x7fffffe0u) != 0; }
    void clear() { squads_.clear(); }
};

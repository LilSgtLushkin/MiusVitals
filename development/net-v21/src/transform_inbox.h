// Between physics steps the engine only keeps the last arrival per unit. Coalesce exactly those
// arrivals at receipt so a paused or slow game cannot accumulate repeated copies of the same unit.
// No cross-step snapshot ordering is assumed: the wire format has no connection epoch.
#pragma once
#include "protocol.h"
#include <cmath>
#include <stdint.h>
#include <unordered_map>
#include <vector>

namespace transforms {
enum class Put { inserted, replaced, invalid, full };
inline bool valid(const UnitRec &r)
{
    if (r.kind != 1 && r.kind != 2) return false;
    for (int i = 0; i < 3; i++)
        if (!std::isfinite(r.pos[i]) || !std::isfinite(r.lin[i]) || !std::isfinite(r.ang[i])) return false;
    for (int i = 0; i < 9; i++) if (!std::isfinite(r.rot[i])) return false;
    return true;
}
class Inbox {
    static const size_t limit = 1000000;  // local object tables are bounded at the same size
    std::vector<UnitRec> records_;
    std::unordered_map<uint64_t, size_t> index_;
public:
    Put put(const UnitRec &r)
    {
        if (!valid(r)) return Put::invalid;
        uint64_t key = (uint64_t(r.kind) << 32) | r.id;
        auto it = index_.find(key);
        if (it != index_.end()) { records_[it->second] = r; return Put::replaced; }
        if (records_.size() >= limit) return Put::full;
        index_.emplace(key, records_.size());
        records_.push_back(r);
        return Put::inserted;
    }
    const std::vector<UnitRec> &records() const { return records_; }
    void clear() { records_.clear(); index_.clear(); }
};
}

// Tactical transforms are ordered per unit, not per datagram: a snapshot can contain several
// datagrams and subsequent snapshots may omit stationary units. The caller checks both process
// epochs before inserting records, and resets this inbox when either process generation changes.
#pragma once
#include "protocol.h"
#include <cmath>
#include <stdint.h>
#include <unordered_map>
#include <vector>

namespace transforms {
enum class Put { inserted, replaced, stale, invalid, full };
// RFC-1982-style comparison, valid when consecutive accepted snapshots are less than 2^31 apart.
inline bool newer(uint32_t a, uint32_t b) {
    uint32_t distance = a - b;
    return distance != 0 && distance < UINT32_C(0x80000000);
}
class HeaderOrder {
    uint32_t last_ = 0;
    bool have_ = false;
public:
    bool accept(uint32_t snapshot) {
        if (have_ && !newer(snapshot, last_)) return false;
        have_ = true; last_ = snapshot; return true;
    }
    void reset() { have_ = false; last_ = 0; }
    bool have() const { return have_; }
};
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
    std::unordered_map<uint64_t, size_t> pending_;
    std::unordered_map<uint64_t, uint32_t> latest_;
public:
    Put put(const UnitRec &r, uint32_t snapshot)
    {
        if (!valid(r)) return Put::invalid;
        uint64_t key = (uint64_t(r.kind) << 32) | r.id;
        auto last = latest_.find(key);
        if (last != latest_.end() && !newer(snapshot, last->second)) return Put::stale;
        auto item = pending_.find(key);
        if ((last == latest_.end() && latest_.size() >= limit) ||
            (item == pending_.end() && records_.size() >= limit)) return Put::full;
        if (last == latest_.end()) latest_.emplace(key, snapshot);
        else last->second = snapshot;
        if (item != pending_.end()) { records_[item->second] = r; return Put::replaced; }
        pending_.emplace(key, records_.size());
        records_.push_back(r);
        return Put::inserted;
    }
    const std::vector<UnitRec> &records() const { return records_; }
    // Simulation consumed this step; preserve order memory for late packets in later steps.
    void drain() { records_.clear(); pending_.clear(); }
    // New process generation/session: even an identical object ID belongs to the new stream.
    void clear() { drain(); latest_.clear(); }
};
}

// Independent tactical animation transport. Normally observed group/start/stop/state changes
// use per-unit stop-and-wait with ACK only after replay. On saturation, that unit's queued
// transitions may be superseded by an explicitly flagged current-state rebase; this degrades
// animation rather than disconnecting or stopping the battle. ACK still follows successful replay.
// No sequence is shared with shots, hits, number tables or trenches. All methods run on the
// session/simulation thread and rekey when either process epoch changes.
#pragma once
#include "anim_wire.h"
#include <stdint.h>
#include <string.h>
#include <deque>
#include <new>
#include <unordered_map>
#include <vector>

namespace animation_channel {
enum { critical_flag = 1, rebase_flag = 2, max_units = 20000, max_events = 32768, max_unit_events = 256,
       retry_ms = 400, attempt_ms = 200, work_per_tick = 128 };

// Scalar snapshots are cosmetic and replaceable. Pressure affects only their sampling rate;
// critical events continue to be observed immediately. Hysteresis restores the exact normal
// cadence when the outstanding queue has drained sufficiently.
class Cadence {
public:
    enum { high = 8192, low = 2048 };
    bool update(size_t pending) {
        bool old = slow_;
        if (!slow_ && pending >= high) slow_ = true;
        else if (slow_ && pending <= low) slow_ = false;
        return old != slow_;
    }
    void reset() { slow_ = false; }
    bool slow() const { return slow_; }
    uint32_t scan_ms() const { return slow_ ? 500u : 200u; }
    uint32_t changed_ms() const { return slow_ ? 500u : 200u; }
    uint32_t unchanged_ms() const { return slow_ ? 2500u : 1000u; }
private:
    bool slow_ = false;
};
enum class Apply { applied, retry, refused };
typedef void (*Send)(const void *, int);
typedef Apply (*Replay)(const ShotRec &, void *);
#pragma pack(push, 1)
struct Data {
    Head h;
    uint32_t owner, target, revision, barrier;
    ShotRec record;
};
struct Ack {
    Head h;
    uint32_t owner, target, seq;
    uint64_t unit;
};
#pragma pack(pop)
static_assert(sizeof(Data) <= 1200, "animation packet must fit in a single datagram");

class Channel {
    struct Event { Data packet; uint32_t sent_at = 0; bool sent = false; };
    struct TxUnit {
        std::deque<Event> events;
        ShotRec latest = {};
        uint32_t latest_revision = 0, last_ack = 0, tail = 0;
        bool dirty = false;
    };
    struct RxUnit {
        Data event = {}, latest = {};
        uint32_t last_event = 0, last_revision = 0, attempted_at = 0;
        bool has_event = false, has_latest = false, attempted = false, halted = false;
    };
public:
    void start(uint32_t local, uint32_t peer) {
        if (!local || !peer || (local == local_ && peer == peer_)) return;
        reset(); local_ = local; peer_ = peer;
    }
    void reset() {
        local_ = peer_ = seq_ = revision_ = 0; event_count_ = 0; rebase_count_ = 0;
        tx_.clear(); rx_.clear(); tx_order_.clear(); rx_order_.clear();
        tx_cursor_ = rx_cursor_ = room_cursor_ = 0; cadence_.reset();
    }
    size_t pending_events() const { return event_count_; }
    size_t send_units() const { return tx_.size(); }
    size_t receive_units() const { return rx_.size(); }
    size_t rebases() const { return rebase_count_; }

    // Cosmetic sampling only: the caller asks before it scans and before it keeps a snapshot.
    // Slowing reduces the rate at which new transitions are observed; it never drops one that
    // has already been queued, and the normal cadence returns once the queue has drained.
    bool update_pressure() { return cadence_.update(event_count_); }
    bool slow() const { return cadence_.slow(); }
    uint32_t scan_ms() const { return cadence_.scan_ms(); }
    uint32_t changed_ms() const { return cadence_.changed_ms(); }
    uint32_t unchanged_ms() const { return cadence_.unchanged_ms(); }

    // A critical update is never coalesced or assigned a sequence unless it is retained. When
    // retention is impossible the unit's queued transitions are superseded by one flagged
    // current state, so the newest observed animation is what the other game ends up showing;
    // the intermediate transitions are dropped. Only a completely saturated channel (no queued
    // transition of any unit to reclaim) returns false, and then the caller retries next scan.
    bool queue(const ShotRec &record, bool critical) {
        if (!local_ || !peer_ || !anim_wire::valid(record) || revision_ == UINT32_MAX) return false;
        uint64_t key; memcpy(&key, &record.shooter, sizeof key); // packed wire fields cannot bind as aligned references
        auto it = tx_.find(key);
        if (it == tx_.end()) {
            if (tx_.size() >= max_units) return false;
            it = tx_.emplace(key, TxUnit{}).first;
            tx_order_.push_back(key);
        }
        TxUnit &unit = it->second;
        if (!critical) {
            unit.latest = record; unit.latest.seq = 0; unit.latest_revision = ++revision_; unit.dirty = true;
            return true;
        }
        if (seq_ == UINT32_MAX) return false;
        if (unit.events.size() >= max_unit_events || event_count_ >= max_events) {
            if (event_count_ >= max_events) make_room();
            if (unit.events.size() >= max_unit_events) { collapse(unit, record, ++revision_); return true; }
            if (event_count_ >= max_events) {
                if (!unit.events.empty()) { collapse(unit, record, ++revision_); return true; }
                return false; // every unit already holds one unreplayable event; retried next scan
            }
        }
        uint32_t revision = ++revision_;
        if (critical) {
            Data data = {}; data.h = make_head(PKT_ANIM, 1); data.h.flags = critical_flag;
            data.owner = local_; data.target = peer_; data.revision = revision; data.barrier = unit.tail;
            data.record = record; data.record.seq = ++seq_;
            unit.events.push_back(Event{data, 0, false});
            unit.tail = data.record.seq; unit.dirty = false; ++event_count_;
        } else {
            unit.latest = record; unit.latest.seq = 0; unit.latest_revision = revision; unit.dirty = true;
        }
        return true;
    }

    // Iterate fairly over units. A permanently blocked soldier cannot stop another soldier's
    // animation or the independent gameplay event stream.
    void tick(uint32_t now, Send send) {
        if (!send || tx_order_.empty()) return;
        size_t visited = 0, work = 0;
        while (visited++ < tx_order_.size() && work < work_per_tick) {
            if (tx_cursor_ >= tx_order_.size()) tx_cursor_ = 0;
            TxUnit &u = tx_.find(tx_order_[tx_cursor_++])->second;
            if (!u.events.empty()) {
                Event &event = u.events.front();
                if (!event.sent || uint32_t(now - event.sent_at) >= retry_ms) {
                    event.sent = true; event.sent_at = now; send(&event.packet, sizeof event.packet); ++work;
                }
            } else if (u.dirty) {
                Data data = {}; data.h = make_head(PKT_ANIM, 1); data.owner = local_; data.target = peer_;
                data.revision = u.latest_revision; data.barrier = u.last_ack; data.record = u.latest;
                u.dirty = false; send(&data, sizeof data); ++work;
            }
        }
    }

    // Data ACKs are sent on the simulation thread, only after Replay::applied. An ACK lost on
    // the wire is answered again for a duplicate; a refusal is never reported as successful.
    bool packet(const void *bytes, size_t size, Send send) {
        if (!bytes || size < sizeof(Head) || !local_ || !peer_ || !send) return false;
        Head h; memcpy(&h, bytes, sizeof h);
        if (h.magic != k_magic) return false;
        if (h.type == PKT_ANIM_ACK) {
            if (size != sizeof(Ack) || h.flags || h.count) return false;
            Ack ack; memcpy(&ack, bytes, sizeof ack);
            if (ack.owner != local_ || ack.target != peer_ || !ack.seq) return false;
            uint64_t key; memcpy(&key, &ack.unit, sizeof key);
            auto it = tx_.find(key);
            if (it == tx_.end() || it->second.events.empty() || it->second.events.front().packet.record.seq != ack.seq ||
                !it->second.events.front().sent) return false;
            TxUnit &u = it->second;
            u.events.pop_front(); --event_count_; u.last_ack = ack.seq;
            return true;
        }
        if (h.type != PKT_ANIM || size != sizeof(Data) || h.count != 1 ||
            (h.flags & ~(critical_flag | rebase_flag))) return false;
        Data data; memcpy(&data, bytes, sizeof data);
        if (data.owner != peer_ || data.target != local_ || !data.revision || !anim_wire::valid(data.record)) return false;
        bool critical = (h.flags & critical_flag) != 0;
        bool rebase = (h.flags & rebase_flag) != 0;
        if (critical != (data.record.seq != 0) || (rebase && !critical)) return false;
        uint64_t key; memcpy(&key, &data.record.shooter, sizeof key);
        auto it = rx_.find(key);
        if (it == rx_.end()) {
            // A snapshot is never a substitute for the first acknowledged critical state.
            if (!critical || (!rebase && data.barrier) || rx_.size() >= max_units) return false;
            it = rx_.emplace(key, RxUnit{}).first;
            rx_order_.push_back(key);
        }
        RxUnit &u = it->second;
        if (critical) {
            if (data.record.seq <= u.last_event) { acknowledge(key, data.record.seq, send); return true; }
            if (!rebase) {
                if (data.barrier != u.last_event || data.revision <= u.last_revision) return false;
                if (u.has_event) return u.event.record.seq == data.record.seq; // repeated, still awaiting replay
            } else if (data.revision <= u.last_revision) {
                return false; // an old rebase cannot roll a newer applied state back
            }
            u.event = data; u.has_event = true; u.attempted = u.halted = false;
            return true;
        }
        if (!u.last_event || u.has_event || data.barrier != u.last_event || data.revision <= u.last_revision ||
            (u.has_latest && data.revision <= u.latest.revision)) return false;
        u.latest = data; u.has_latest = true;
        return true;
    }

    void apply(uint32_t now, Replay replay, void *context, Send send) {
        if (!replay || !send || rx_order_.empty()) return;
        size_t visited = 0, work = 0;
        while (visited++ < rx_order_.size() && work < work_per_tick) {
            if (rx_cursor_ >= rx_order_.size()) rx_cursor_ = 0;
            uint64_t key = rx_order_[rx_cursor_++];
            RxUnit &u = rx_.find(key)->second;
            if (u.has_event) {
                if (u.halted || (u.attempted && uint32_t(now - u.attempted_at) < attempt_ms)) continue;
                u.attempted = true; u.attempted_at = now; ++work;
                Apply result = replay(u.event.record, context);
                if (result == Apply::applied) {
                    u.last_event = u.event.record.seq; u.last_revision = u.event.revision;
                    u.has_event = u.attempted = false;
                    acknowledge(key, u.last_event, send);
                } else if (result == Apply::refused) {
                    // A refused engine call may already have had side effects. Keep the event
                    // unacknowledged and stop replaying it until a fresh process epoch.
                    u.halted = true;
                }
            } else if (u.has_latest) {
                if (u.attempted && uint32_t(now - u.attempted_at) < attempt_ms) continue;
                u.attempted = true; u.attempted_at = now; ++work;
                if (u.latest.barrier != u.last_event || u.latest.revision <= u.last_revision) { u.has_latest = u.attempted = false; continue; }
                Apply result = replay(u.latest.record, context);
                if (result == Apply::applied) { u.last_revision = u.latest.revision; u.has_latest = u.attempted = false; }
                else if (result == Apply::refused) u.has_latest = u.attempted = false;
            }
        }
    }
private:
    void acknowledge(uint64_t unit, uint32_t seq, Send send) const {
        Ack ack = { make_head(PKT_ANIM_ACK), peer_, local_, seq, unit };
        send(&ack, sizeof ack);
    }
    // Replace a unit's retained transitions with one explicitly flagged current state. The
    // dropped transitions are animation only: the alternative is unbounded memory, stopping
    // the battle, or keeping an old state that no longer describes the soldier.
    void collapse(TxUnit &unit, const ShotRec &record, uint32_t revision) {
        event_count_ -= unit.events.size();
        unit.events.clear();
        Data data = {};
        data.h = make_head(PKT_ANIM, 1);
        data.h.flags = (unsigned char)(critical_flag | rebase_flag);
        data.owner = local_; data.target = peer_; data.revision = revision; data.barrier = unit.tail;
        data.record = record; data.record.seq = ++seq_;
        unit.events.push_back(Event{data, 0, false});
        unit.tail = data.record.seq; unit.dirty = false;
        ++event_count_; ++rebase_count_;
    }
    void make_room(void) {
        if (event_count_ < max_events) return;
        size_t visited = 0;
        while (visited++ < tx_order_.size() && event_count_ >= max_events) {
            if (room_cursor_ >= tx_order_.size()) room_cursor_ = 0;
            TxUnit &u = tx_.find(tx_order_[room_cursor_++])->second;
            if (u.events.size() < 2 || seq_ == UINT32_MAX || revision_ == UINT32_MAX) continue;
            collapse(u, u.events.back().packet.record, ++revision_);
        }
    }
    uint32_t local_ = 0, peer_ = 0, seq_ = 0, revision_ = 0;
    size_t event_count_ = 0, rebase_count_ = 0, tx_cursor_ = 0, rx_cursor_ = 0, room_cursor_ = 0;
    Cadence cadence_;
    std::unordered_map<uint64_t, TxUnit> tx_;
    std::unordered_map<uint64_t, RxUnit> rx_;
    std::vector<uint64_t> tx_order_, rx_order_;
};
} // namespace animation_channel

// Portable, bounded Vitals state channel. Adapted from the monotonic state exchange in
// development/kit-source/src/mp_vitals.c, not from the old DLL's v20 hooks.
//
// Only the session thread calls this class. The session authenticates the peer address and
// supplies an epoch learned from the versioned Hello/Welcome before accepting any data. Receipt
// and validation precede the ACK; a full table or an unavailable number exchange is never ACKed.
// Packet loss, reordering and duplicates are normal. Soldiers merge by minimum health/death;
// vehicle flags use per-vehicle sequence numbers and accumulate pending nonzero transitions.
#pragma once
#include "protocol.h"
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace vitals {
enum { dead = 1, hurt = 2, record = 4, vehicle = 8, vehicle_bytes = 9 };
enum { per_packet = 70, max_packets = 32, max_soldiers = 6144, max_vehicles = 3072, retry_ms = 400 };

#pragma pack(push, 1)
struct PacketHead { Head h; uint32_t epoch, seq; };
struct Entry { uint32_t id, flags, a, b; };
#pragma pack(pop)
static_assert(sizeof(PacketHead) == 16 && sizeof(Entry) == 16, "vitals wire layout");
static_assert(sizeof(PacketHead) + per_packet * sizeof(Entry) <= 1200, "vitals datagram MTU");

inline uint32_t bits(float value) { uint32_t b; memcpy(&b, &value, sizeof b); return b; }
inline float value(uint32_t b) { float v; memcpy(&v, &b, sizeof v); return v; }
inline Entry soldier(uint32_t id, float health) {
    Entry e = { id, (uint32_t)(hurt | (health < 0.5f ? dead : 0)), bits(health), 0 };
    return e;
}
inline Entry machine(uint32_t id, const uint8_t b[vehicle_bytes]) {
    Entry e = { id, (uint32_t)(vehicle | ((uint32_t)b[8] << 8)), 0, 0 };
    memcpy(&e.a, b, 4); memcpy(&e.b, b + 4, 4); return e;
}
inline void machine_bytes(const Entry &e, uint8_t b[vehicle_bytes]) {
    memcpy(b, &e.a, 4); memcpy(b + 4, &e.b, 4); b[8] = (uint8_t)(e.flags >> 8);
}
inline bool valid(const Entry &e) {
    if (e.flags & vehicle) return (e.flags & ~(uint32_t)(vehicle | 0xff00u)) == 0;
    if (e.flags & ~(uint32_t)(dead | hurt | record)) return false;
    if (!(e.flags & hurt)) return false;
    float h = value(e.a);
    if (!(h >= 0.f && h <= 2.f)) return false; // rejects NaN, infinity, negative and implausible values
    if (((e.flags & dead) != 0) != (h < 0.5f)) return false;
    if (e.flags & record) { float r = value(e.b); if (!(r >= 0.f && r < 1000000.f)) return false; }
    return true;
}

struct Soldier {
    float health = 9.f, record_health = 1000000.f;
    bool is_dead = false;
};
struct Vehicle {
    uint32_t last_seq = 0;
    uint16_t peer_nonzero = 0, pending = 0;
    uint8_t pending_value[vehicle_bytes] = {};
};
struct Counters {
    uint64_t sent = 0, received = 0, acked = 0, retries = 0, rejected = 0, full = 0;
};
typedef void (*Send)(const void *, int);

class Channel {
public:
    void start(uint32_t epoch) {
        epoch_ = epoch; peer_ = seq_ = 0; retired_count_ = 0;
        out_.clear(); soldiers_.clear(); vehicles_.clear(); counts_ = Counters{};
    }
    uint32_t epoch() const { return epoch_; }
    uint32_t peer_epoch() const { return peer_; }
    size_t outstanding() const { return out_.size(); }
    const Counters &counters() const { return counts_; }

    // Called only with a nonce from a verified current peer's Hello/Welcome. A peer process change
    // invalidates its pending data and requires the engine adapter to republish local state. Keep
    // our sequence/epoch: the other endpoint may have restarted while we stayed alive.
    bool set_peer(uint32_t epoch) {
        if (!epoch || epoch == peer_) return false;
        for (size_t i = 0; i < retired_count_; ++i) if (retired_[i] == epoch) return false;
        if (peer_) {
            if (retired_count_ < 8) retired_[retired_count_++] = peer_;
            else { for (size_t i = 1; i < 8; ++i) retired_[i - 1] = retired_[i]; retired_[7] = peer_; }
        }
        peer_ = epoch; out_.clear(); soldiers_.clear(); vehicles_.clear(); return true;
    }
    const Soldier *find_soldier(uint32_t id) const {
        auto it = soldiers_.find(id); return it == soldiers_.end() ? nullptr : &it->second;
    }
    Vehicle *find_vehicle(uint32_t id) {
        auto it = vehicles_.find(id); return it == vehicles_.end() ? nullptr : &it->second;
    }

    // On saturation returns the number actually retained. The caller must not mark the rest sent.
    // Retained data never expires or evicts older entries while connected; retries are bounded in
    // memory and the adapter can retry unsent entries on the next scan.
    size_t enqueue(const Entry *items, size_t count, uint32_t now, Send send) {
        if (!epoch_ || !peer_ || !items || !send) return 0;
        size_t at = 0;
        while (at < count && out_.size() < max_packets && seq_ != UINT32_MAX) {
            size_t n = count - at < (size_t)per_packet ? count - at : (size_t)per_packet;
            bool okay = true; for (size_t i = 0; i < n; ++i) if (!valid(items[at + i])) okay = false;
            if (!okay) { ++counts_.rejected; break; }
            PacketHead h = { make_head(PKT_VITALS, (int)n), epoch_, ++seq_ };
            Tx tx; tx.seq = h.seq; tx.sent_at = now; tx.bytes.resize(sizeof h + n * sizeof(Entry));
            memcpy(tx.bytes.data(), &h, sizeof h);
            memcpy(tx.bytes.data() + sizeof h, items + at, n * sizeof(Entry));
            out_.push_back(tx);
            ++counts_.sent;
            send(out_.back().bytes.data(), (int)out_.back().bytes.size());
            at += n;
        }
        if (at < count) ++counts_.full;
        return at;
    }
    void tick(uint32_t now, Send send) {
        if (!send) return;
        for (Tx &tx : out_) if ((uint32_t)(now - tx.sent_at) >= retry_ms) {
            tx.sent_at = now; ++counts_.retries; send(tx.bytes.data(), (int)tx.bytes.size());
        }
    }

    // A false ready gate leaves data unacknowledged for retry. ACKs are independent of the gate.
    bool packet(const void *data, size_t size, bool ready, Send send) {
        if (!data || size < sizeof(PacketHead)) { ++counts_.rejected; return false; }
        PacketHead h; memcpy(&h, data, sizeof h);
        if (h.h.magic != k_magic || h.h.flags != 0 || !h.epoch || !h.seq) { ++counts_.rejected; return false; }
        if (h.h.type == PKT_VITALS_ACK) {
            if (size != sizeof h || h.h.count || h.epoch != epoch_) { ++counts_.rejected; return false; }
            for (auto it = out_.begin(); it != out_.end(); ++it) if (it->seq == h.seq) {
                out_.erase(it); ++counts_.acked; return true;
            }
            return false; // stale/duplicate ACK cannot retire another packet
        }
        if (h.h.type != PKT_VITALS || !ready || !send || h.epoch != peer_ ||
            !h.h.count || h.h.count > per_packet || size != sizeof h + h.h.count * sizeof(Entry)) {
            ++counts_.rejected; return false;
        }
        Entry entries[per_packet];
        memcpy(entries, (const uint8_t *)data + sizeof h, h.h.count * sizeof(Entry));
        std::unordered_set<uint32_t> new_soldiers, new_vehicles;
        for (unsigned i = 0; i < h.h.count; ++i) {
            const Entry &e = entries[i];
            if (!valid(e)) { ++counts_.rejected; return false; }
            if (e.flags & vehicle) { if (!vehicles_.count(e.id)) new_vehicles.insert(e.id); }
            else if (!soldiers_.count(e.id)) new_soldiers.insert(e.id);
        }
        if (new_soldiers.size() > max_soldiers - soldiers_.size() ||
            new_vehicles.size() > max_vehicles - vehicles_.size()) { ++counts_.full; return false; }
        for (unsigned i = 0; i < h.h.count; ++i) {
            const Entry &e = entries[i];
            if (e.flags & vehicle) {
                Vehicle &v = vehicles_[e.id];
                if (h.seq <= v.last_seq) continue; // ignore an old zero or stale nonzero transition
                v.last_seq = h.seq;
                uint8_t b[vehicle_bytes]; machine_bytes(e, b);
                for (int k = 0; k < vehicle_bytes; ++k) {
                    uint16_t bit = (uint16_t)(1u << k);
                    if (b[k] && !(v.peer_nonzero & bit)) { v.pending |= bit; v.pending_value[k] = b[k]; }
                    if (b[k]) v.peer_nonzero |= bit; else v.peer_nonzero &= (uint16_t)~bit;
                }
            } else {
                Soldier &s = soldiers_[e.id]; float health = value(e.a);
                if (health < s.health) s.health = health;
                if (e.flags & dead) s.is_dead = true;
                if (e.flags & record) { float r = value(e.b); if (r < s.record_health) s.record_health = r; }
            }
        }
        ++counts_.received;
        PacketHead ack = { make_head(PKT_VITALS_ACK), h.epoch, h.seq };
        send(&ack, sizeof ack);
        return true;
    }
private:
    struct Tx { uint32_t seq = 0, sent_at = 0; std::vector<uint8_t> bytes; };
    uint32_t epoch_ = 0, peer_ = 0, seq_ = 0, retired_[8] = {};
    size_t retired_count_ = 0;
    std::deque<Tx> out_;
    std::unordered_map<uint32_t, Soldier> soldiers_;
    std::unordered_map<uint32_t, Vehicle> vehicles_;
    Counters counts_;
};
} // namespace vitals

// Ordered M5 operation-map ownership. The host owns input first; after a checked handoff, the
// client owns it; the host may resolve only after collecting the client's matching snapshot.
// This is transport-independent and deliberately fails closed on stale/invalid identities.
#pragma once
#include <stdint.h>

namespace operation_turn {
enum class Phase : uint8_t { host = 0, handoff = 1, client = 2, collect = 3, resolve = 4 };
enum class Observation { invalid, stale, current, snapshot_needed };
static const uint32_t epoch_mask = 0x00ffffffu;
static const unsigned phase_shift = 2;
static const uint32_t phase_mask = 7u << phase_shift;

inline bool valid_epoch(uint32_t epoch) { return epoch && !(epoch & ~epoch_mask); }
inline bool valid_phase(Phase phase) { return (unsigned)phase <= (unsigned)Phase::resolve; }
inline uint32_t next_revision(uint32_t revision) { ++revision; return revision ? revision : 1; }
inline bool newer_revision(uint32_t value, uint32_t previous) {
    uint32_t delta = value - previous;
    return value && delta && delta < 0x80000000u;
}
inline bool newer_epoch(uint32_t value, uint32_t previous) {
    uint32_t delta = (value - previous) & epoch_mask;
    return valid_epoch(value) && valid_epoch(previous) && delta && delta < 0x00800000u;
}
inline uint32_t flags(uint32_t epoch, Phase phase, bool collecting, bool host_done) {
    return ((epoch & epoch_mask) << 8) | ((uint32_t)phase << phase_shift) |
           (collecting ? 1u : 0u) | (host_done ? 2u : 0u);
}
inline bool decode(uint32_t packed, uint32_t *epoch, Phase *phase) {
    uint32_t e = packed >> 8;
    Phase p = (Phase)((packed & phase_mask) >> phase_shift);
    if (!valid_epoch(e) || !valid_phase(p) || (packed & 0xe0u)) return false;
    if (epoch) *epoch = e;
    if (phase) *phase = p;
    return true;
}

struct Host {
    uint32_t epoch = 0, revision = 0;
    Phase phase = Phase::host;
    bool shared = false, published = false;

    bool begin(uint32_t new_epoch, uint32_t initial_revision) {
        if (!valid_epoch(new_epoch) || !initial_revision) return false;
        epoch = new_epoch; revision = initial_revision; phase = Phase::host;
        shared = true; published = false;
        return true;
    }
    bool finish_host() {
        if (!shared || phase != Phase::host) return false;
        phase = Phase::handoff; published = false;
        return true;
    }
    bool publish(uint32_t new_revision) {
        if (!shared || phase != Phase::handoff || published || !newer_revision(new_revision, revision)) return false;
        revision = new_revision; published = true;
        return true;
    }
    bool peer_loaded(uint32_t peer_epoch, uint32_t peer_revision) {
        if (!shared || phase != Phase::handoff || !published || peer_epoch != epoch || peer_revision != revision) return false;
        phase = Phase::client;
        return true;
    }
    bool peer_finished(uint32_t peer_epoch, uint32_t peer_revision) {
        if (!shared || phase != Phase::client || peer_epoch != epoch || peer_revision != revision) return false;
        phase = Phase::collect;
        return true;
    }
    bool committed(uint32_t peer_epoch, uint32_t peer_revision) {
        if (!shared || phase != Phase::collect || peer_epoch != epoch || peer_revision != revision) return false;
        phase = Phase::resolve;
        return true;
    }
    bool host_input() const { return shared && phase == Phase::host; }
    bool may_resolve() const { return shared && phase == Phase::resolve; }
    void clear() { epoch = revision = 0; phase = Phase::host; shared = published = false; }
};

struct Client {
    uint32_t epoch = 0, revision = 0, pending_epoch = 0, pending_revision = 0;
    Phase phase = Phase::host;
    bool done = false;

    bool begin(uint32_t loaded_epoch, uint32_t loaded_revision) {
        if (!valid_epoch(loaded_epoch) || !loaded_revision) return false;
        epoch = loaded_epoch; revision = loaded_revision; pending_epoch = pending_revision = 0;
        phase = Phase::host; done = false;
        return true;
    }
    Observation observe(uint32_t offered_epoch, uint32_t offered_revision, Phase offered_phase) {
        if (!valid_epoch(epoch) || !revision || !valid_epoch(offered_epoch) || !offered_revision || !valid_phase(offered_phase))
            return Observation::invalid;
        if (offered_epoch != epoch && !newer_epoch(offered_epoch, epoch)) return Observation::stale;
        if (offered_epoch == epoch && offered_revision != revision && !newer_revision(offered_revision, revision))
            return Observation::stale;
        if (offered_epoch != epoch || offered_revision != revision) {
            // A later offer may replace an earlier incomplete one, never the reverse.
            if (pending_epoch) {
                if (offered_epoch != pending_epoch && !newer_epoch(offered_epoch, pending_epoch)) return Observation::stale;
                if (offered_epoch == pending_epoch && offered_revision != pending_revision &&
                    !newer_revision(offered_revision, pending_revision)) return Observation::stale;
            }
            pending_epoch = offered_epoch; pending_revision = offered_revision;
            return Observation::snapshot_needed;
        }
        if (pending_epoch) return Observation::stale; // never unlock an old snapshot while a new one is pending
        if ((unsigned)offered_phase < (unsigned)phase) return Observation::stale;
        phase = offered_phase;
        return Observation::current;
    }
    bool loaded(uint32_t loaded_epoch, uint32_t loaded_revision) {
        if (!pending_epoch || pending_epoch != loaded_epoch || pending_revision != loaded_revision) return false;
        epoch = loaded_epoch; revision = loaded_revision; pending_epoch = pending_revision = 0;
        phase = Phase::host; done = false; // wait for a matching host phase announcement
        return true;
    }
    bool input() const { return epoch && revision && !pending_epoch && phase == Phase::client && !done; }
    bool finish() {
        if (!input()) return false;
        done = true;
        return true;
    }
};
} // namespace operation_turn

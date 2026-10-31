// Two-player pause agreement, independent of game UI and transport. Host prepares a change only
// when both current votes agree; the guest acknowledges that exact pair of vote revisions before
// the host commits. Repeated, ordered state snapshots carry the decision until the guest reports
// it applied. A lost connection enters a local recovery pause, propagated to the other process;
// resuming after recovery requires both new votes. A network partition cannot guarantee perfectly
// simultaneous stops: loss is only detected after the transport timeout.
#pragma once
#include "protocol.h"
#include <stdint.h>

namespace pause_sync {
class State {
public:
    void reset(bool host) {
        *this = State{}; host_ = host; vote_revision_ = 1;
    }
    bool active() const { return peer_known_; }
    bool paused() const { return committed_; }
    bool my_vote() const { return vote_; }
    bool peer_vote() const { return peer_vote_; }
    bool waiting() const { return peer_known_ && (vote_ != committed_ || peer_vote_ != committed_ || stage_ != 0); }
    bool recovering() const { return recovering_; }
    bool exhausted() const { return exhausted_; }
    uint32_t proposal() const { return proposal_; }
    unsigned stage() const { return stage_; }

    bool press() {
        if (exhausted_ || vote_revision_ == UINT32_MAX || (host_ && proposal_ == UINT32_MAX)) return false;
        vote_ = !vote_; ++vote_revision_;
        if (host_ && stage_ == 1) stage_ = 0; // an uncommitted proposal no longer reflects this vote
        maybe_prepare();
        return true;
    }
    // Only call on a transition from an established connection to a lost one, or when replacing
    // a battle peer. Both sides hold locally even before the other can receive the recovery notice.
    void lost() {
        if (recovery_revision_ != UINT32_MAX) ++recovery_revision_;
        else exhausted_ = true; // cannot communicate another recovery under these epochs
        force_hold(); peer_known_ = false;
    }
    // Both wire epochs delimit this stream. When either changes, old packets are rejected by the
    // session; reset the counters and require a fresh two-player recovery agreement.
    void new_peer() { bool host = host_; reset(host); lost(); }
#ifdef NETV21_PAUSE_TEST
    void test_counters(uint32_t vote, uint32_t recovery, uint32_t proposal) {
        vote_revision_ = vote; recovery_revision_ = recovery; proposal_ = proposal;
    }
#endif

    PauseControl message() const {
        PauseControl m = {};
        m.vote_revision = vote_revision_; m.recovery_revision = recovery_revision_;
        m.vote = vote_ ? 1 : 0;
        if (host_) {
            m.proposal = proposal_; m.host_vote_revision = host_revision_;
            m.guest_vote_revision = guest_revision_; m.target = target_ ? 1 : 0; m.stage = (unsigned char)stage_;
        } else { m.ack = accepted_; m.applied = applied_; }
        return m;
    }
    static bool valid(const PauseControl &m, bool sender_is_host) {
        if (!m.vote_revision || m.vote > 1 || m.target > 1 || m.reserved || m.stage > 2) return false;
        if (sender_is_host) {
            if (m.ack || m.applied) return false;
            if (m.stage && (!m.proposal || !m.host_vote_revision || !m.guest_vote_revision)) return false;
        } else if (m.stage || m.proposal || m.host_vote_revision || m.guest_vote_revision || m.target) return false;
        return true;
    }
    // snapshot has already passed the enclosing process-epoch checks; retain ordering here too,
    // including when this class is exercised without the rest of the session.
    bool receive(const PauseControl &m, uint32_t snapshot) {
        if (!valid(m, !host_)) return false;
        if (have_snapshot_) {
            uint32_t d = snapshot - last_snapshot_;
            if (!d || d >= UINT32_C(0x80000000)) return false;
        }
        have_snapshot_ = true; last_snapshot_ = snapshot;
        if (m.recovery_revision != seen_peer_recovery_) {
            seen_peer_recovery_ = m.recovery_revision;
            force_hold();
        }
        peer_known_ = true; peer_vote_ = m.vote != 0; peer_revision_ = m.vote_revision;
        if (host_) {
            if (stage_ == 1 && (host_revision_ != vote_revision_ || guest_revision_ != peer_revision_ ||
                                target_ != vote_ || target_ != peer_vote_)) stage_ = 0;
            if (stage_ == 1 && m.ack == proposal_) {
                stage_ = 2; committed_ = target_;
                if (!committed_) recovering_ = false;
            }
            if (stage_ == 2 && m.applied == proposal_) stage_ = 0;
            maybe_prepare();
        } else {
            if (m.stage == 1 && m.host_vote_revision == m.vote_revision &&
                m.guest_vote_revision == vote_revision_ && m.target == m.vote && m.target == (vote_ ? 1 : 0)) {
                accepted_ = m.proposal;
            } else if (m.stage == 2 && m.proposal == accepted_) {
                committed_ = m.target != 0; applied_ = m.proposal;
                if (!committed_) recovering_ = false;
            }
        }
        return true;
    }
private:
    void force_hold() {
        committed_ = vote_ = true; recovering_ = true;
        if (vote_revision_ != UINT32_MAX) ++vote_revision_;
        else exhausted_ = true;
        stage_ = 0; accepted_ = applied_ = 0;
    }
    void maybe_prepare() {
        if (!host_ || exhausted_ || !peer_known_ || stage_ || vote_ != peer_vote_ || vote_ == committed_ || proposal_ == UINT32_MAX) return;
        ++proposal_; host_revision_ = vote_revision_; guest_revision_ = peer_revision_;
        target_ = vote_; stage_ = 1;
    }
    bool host_ = false, vote_ = false, peer_vote_ = false, committed_ = false, recovering_ = false, peer_known_ = false, exhausted_ = false;
    bool target_ = false, have_snapshot_ = false;
    unsigned stage_ = 0;
    uint32_t vote_revision_ = 1, peer_revision_ = 0, recovery_revision_ = 0, seen_peer_recovery_ = 0;
    uint32_t proposal_ = 0, host_revision_ = 0, guest_revision_ = 0, accepted_ = 0, applied_ = 0;
    uint32_t last_snapshot_ = 0;
};
} // namespace pause_sync

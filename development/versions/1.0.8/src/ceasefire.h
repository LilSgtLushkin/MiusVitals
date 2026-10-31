// Tactical ceasefire agreement, independent of the engine and transport. Agreement is NOT an
// engine battle result: only an actual phase-3 announcement sets the end flag.
#pragma once
#include <stdint.h>

namespace ceasefire {
enum Event {
    changed = 1, cancelled = 2, agreed = 4, finish_notice = 8,
    peer_end_wait = 16, natural_end = 32, stale_packet = 64
};

struct State {
    bool proposed = false, peer_proposed = false, peer_over = false;
    bool left = false, pending = false, have_peer_seq = false;
    uint16_t peer_seq = 0, tx_seq = 0;
    uint32_t peer_result = 0, deadline = 0, peer_deadline = 0;

    static uint32_t after(uint32_t now, uint32_t wait) {
        uint32_t result = now + wait;
        return result ? result : 1; // zero is the 'no deadline' sentinel
    }
    static bool due(uint32_t now, uint32_t when) {
        return when && (int32_t)(now - when) >= 0; // deadlines remain valid across GetTickCount wrap
    }

    bool toggle(bool connected, uint32_t phase) {
        if (!connected || phase != 2 || left || peer_over) return false;
        proposed = !proposed;
        if (!proposed) { deadline = 0; pending = false; }
        return true;
    }

    // Head.count is a 16-bit sequence only when the new flag is set. Legacy packets have no such
    // flag. Once sequenced packets are seen, ignore delayed legacy packets and old/duplicate values.
    unsigned observe(bool peer_wants, bool over, uint32_t result, bool sequenced, uint16_t sequence) {
        if (sequenced) {
            uint16_t delta = (uint16_t)(sequence - peer_seq);
            if (have_peer_seq && (!delta || delta >= 0x8000u)) return stale_packet;
            have_peer_seq = true;
            peer_seq = sequence;
        } else if (have_peer_seq) {
            return stale_packet;
        }
        return word(peer_wants, over, result);
    }

    // The same word from the shots' reliable channel: delivered exactly once, in the order it was
    // spoken, so there is no duplicate or overtaking to guard against - and no numbering of its own
    // to confuse with the datagrams', which a delayed packet could still interleave with.
    unsigned observe_word(bool peer_wants, bool over, uint32_t result) { return word(peer_wants, over, result); }

    // One player's word, whichever channel carried it. Applying the same word twice changes nothing.
    unsigned word(bool peer_wants, bool over, uint32_t result) {
        unsigned events = peer_wants != peer_proposed ? changed : 0;
        peer_proposed = peer_wants;
        if (over && !peer_over) { peer_over = true; peer_result = result; }
        // A withdrawal cancels the notification while both players are still fighting.
        if (!peer_wants && !peer_over && !left && (deadline || pending)) {
            deadline = 0;
            pending = false;
            events |= cancelled;
        }
        return events;
    }

    // Called on the session thread. Only the engine announcing phase 3 sets the end flag.
    // pending is a notice to use the game's own finish controls, NEVER a request to abandon the battle.
    unsigned advance(uint32_t now, bool connected, uint32_t phase) {
        unsigned events = 0;
        if (phase == 3 && !left) {
            left = true; pending = false; deadline = 0;
            events |= natural_end;
        }
        if (!connected && !peer_over && !left) {
            proposed = peer_proposed = false;
            deadline = 0; pending = false; peer_deadline = 0; have_peer_seq = false;
            return events;
        }
        if (phase == 2 && connected && proposed && peer_proposed && !left && !pending && !deadline) {
            deadline = after(now, 5000);
            events |= agreed;
        }
        if (!left && deadline && due(now, deadline)) {
            deadline = 0; pending = true;
            events |= finish_notice;
        }
        if (peer_over && !left) {
            if (!peer_deadline) {
                peer_deadline = after(now, 3000);
                events |= peer_end_wait;
            } else if (!pending && due(now, peer_deadline)) {
                pending = true;
                events |= finish_notice;
            }
        }
        return events;
    }

    void new_peer() {
        bool already_left = left;
        uint16_t last_tx = tx_seq;
        *this = State{};
        left = already_left;
        tx_seq = last_tx; // do not make packets look older to a peer reconnecting at the same address
    }
};
} // namespace ceasefire

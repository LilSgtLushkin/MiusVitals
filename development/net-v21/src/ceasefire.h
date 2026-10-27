// Tactical battle end state, independent of the engine and transport. A request to exit is not
// reported as an actual end until the engine's exit path is available and invoked.
#pragma once
#include <stdint.h>

namespace ceasefire {
enum Event {
    changed = 1, cancelled = 2, agreed = 4, exit_requested = 8,
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
        unsigned events = peer_wants != peer_proposed ? changed : 0;
        peer_proposed = peer_wants;
        if (over && !peer_over) { peer_over = true; peer_result = result; }
        // The other player's withdrawal cancels even an exit which is waiting for the engine UI.
        if (!peer_wants && !peer_over && !left && (deadline || pending)) {
            deadline = 0;
            pending = false;
            events |= cancelled;
        }
        return events;
    }

    // Called on the session thread. The end flag is only set when the engine itself announces phase
    // 3, or confirm_exit() is called immediately before invoking the game's verified exit path.
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
            events |= exit_requested;
        }
        if (peer_over && !left) {
            if (!peer_deadline) {
                peer_deadline = after(now, 3000);
                events |= peer_end_wait;
            } else if (!pending && due(now, peer_deadline)) {
                pending = true;
                events |= exit_requested;
            }
        }
        return events;
    }

    bool confirm_exit() {
        if (!pending || left) return false;
        pending = false; left = true; deadline = 0;
        return true;
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

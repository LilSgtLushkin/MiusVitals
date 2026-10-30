#include "ceasefire.h"
#include <stdio.h>
using namespace ceasefire;
static int checks, failures;
static void check(bool ok, const char *name) { ++checks; if (!ok) { ++failures; printf("FAIL: %s\n", name); } }

int main()
{
    State s;
    check(!s.toggle(false, 2) && !s.proposed, "cannot propose to a disconnected peer");
    check(!s.toggle(true, 1) && !s.proposed, "cannot propose during deployment");
    check(s.toggle(true, 2) && s.proposed, "local F9 proposes");
    check(s.observe(true, false, 0, true, 10) & changed, "peer proposal observed");
    check(s.advance(100, true, 2) & agreed, "both proposals start five-second wait");
    check(s.deadline == 5100 && !s.left && !s.pending, "no premature end announcement");
    check(!(s.advance(5099, true, 2) & finish_notice), "not before five seconds");
    check(s.advance(5100, true, 2) & finish_notice, "notify that agreement is due");
    check(s.pending && !s.left, "agreement does not claim battle ended");
    check(!(s.advance(6000, true, 2) & natural_end) && !s.left, "agreement cannot claim an engine result or exit");
    check((s.advance(6001, true, 3) & natural_end) && s.left && !s.pending, "only engine phase three ends the battle");
    check(!s.toggle(true, 2), "completed end cannot repeat or be revoked");

    s = State{};
    check(s.toggle(true, 2), "new local proposal");
    s.observe(true, false, 0, true, 1);
    s.advance(100, true, 2);
    check(s.toggle(true, 2) && !s.proposed && !s.deadline, "local withdrawal cancels countdown");
    check(!(s.advance(6000, true, 2) & finish_notice), "local withdrawal does not end battle");
    check(s.toggle(true, 2), "local re-proposal");
    s.advance(7000, true, 2);
    check(s.observe(false, false, 0, true, 2) & cancelled, "peer withdrawal cancels countdown");
    check(!s.deadline && !(s.advance(13000, true, 2) & finish_notice), "peer withdrawal prevents exit");
    s.observe(true, false, 0, true, 3);
    s.advance(14000, true, 2);
    s.advance(19000, true, 2);
    check(s.pending && (s.observe(false, false, 0, true, 4) & cancelled) && !s.pending,
          "peer withdrawal cancels the finish notice");

    s = State{};
    s.observe(false, true, 7, true, 4);
    check(s.peer_over && s.peer_result == 7, "peer's actual end recorded");
    check(s.advance(100, true, 2) & peer_end_wait, "peer end starts three-second wait");
    check(!(s.advance(3099, false, 2) & finish_notice), "peer end survives disconnection without early exit");
    check(s.advance(3100, false, 2) & finish_notice, "finish notice after peer end wait");
    check(s.pending && !s.toggle(true, 2), "peer's completed battle cannot be withdrawn");

    s = State{};
    check((s.advance(1, true, 3) & natural_end) && s.left, "actual engine phase three announces natural end");
    check(!(s.advance(2, true, 3) & natural_end), "natural end announced once");
    s.tx_seq = 22;
    s.new_peer();
    check(s.left && !s.peer_over && !s.peer_proposed && s.tx_seq == 22,
          "new peer keeps local end and outgoing sequence");

    s = State{};
    s.toggle(true, 2); s.observe(true, false, 0, true, 7); s.advance(100, true, 2);
    s.advance(101, false, 2);
    check(!s.proposed && !s.peer_proposed && !s.deadline && !s.have_peer_seq,
          "disconnect clears stale proposals and ordering");
    check(!(s.advance(6000, true, 2) & finish_notice), "reconnect cannot reuse stale agreement");

    s = State{};
    s.toggle(true, 2); s.observe(true, false, 0, true, 65535);
    s.advance(0xffffff00u, true, 2);
    check(!(s.advance(0xfffffffeu, true, 2) & finish_notice), "wraparound before deadline");
    check(s.advance(0x00001288u, true, 2) & finish_notice, "wraparound deadline due");
    check(s.observe(false, false, 0, true, 0) & cancelled, "16-bit sequence wraps forward");
    check(s.observe(true, false, 0, true, 65535) & stale_packet, "reordered old packet ignored");
    check(!s.peer_proposed, "old packet cannot resurrect proposal");
    check(s.observe(true, false, 0, true, 0) & stale_packet, "duplicate packet ignored");
    check(s.observe(true, false, 0, false, 0) & stale_packet, "late legacy packet ignored after sequence");
    s.new_peer();
    check((s.observe(true, false, 0, false, 0) & changed) && s.peer_proposed,
          "legacy packet accepted before sequenced support is seen");

    printf("ceasefire selftest: %d checks, %d failed\n", checks, failures);
    return failures ? 1 : 0;
}

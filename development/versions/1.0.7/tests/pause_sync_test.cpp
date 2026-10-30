// Portable model tests for the actual two-player pause agreement state machine.
#define NETV21_PAUSE_TEST 1
#include "pause_sync.h"
#include <stdio.h>
using pause_sync::State;
static int checks, failures;
static void expect(bool ok, const char *what) { ++checks; if (!ok) { ++failures; printf("FAIL: %s\n", what); } }
struct Pair {
    State host, guest; uint32_t hs = 0, gs = 0;
    Pair() { host.reset(true); guest.reset(false); }
    void h_to_g() { guest.receive(host.message(), ++hs); }
    void g_to_h() { host.receive(guest.message(), ++gs); }
    void round() { h_to_g(); g_to_h(); }
    void settle() { for (int i = 0; i < 5; ++i) round(); }
};
int main() {
    Pair p; p.round();
    expect(p.host.active() && p.guest.active() && !p.host.paused() && !p.guest.paused(), "initial connection runs");
    expect(p.host.press(), "host can propose pause"); p.settle();
    expect(!p.host.paused() && !p.guest.paused() && p.guest.peer_vote(), "one vote alone cannot pause");
    expect(p.guest.press(), "guest can confirm pause");
    p.g_to_h(); expect(p.host.stage() == 1 && !p.host.paused(), "host prepares exact matching votes");
    p.h_to_g(); expect(!p.guest.paused() && p.guest.message().ack == p.host.proposal(), "guest acknowledges prepare without pausing");
    p.g_to_h(); expect(p.host.stage() == 2 && p.host.paused(), "host commits only after guest acknowledgement");
    p.h_to_g(); expect(p.guest.paused() && p.guest.message().applied == p.host.proposal(), "guest applies committed pause");
    p.g_to_h(); expect(p.host.stage() == 0 && p.host.paused() && p.guest.paused(), "both remain paused after applied acknowledgement");
    p.guest.press(); p.settle();
    expect(p.host.paused() && p.guest.paused(), "one resume vote alone cannot resume");
    p.host.press(); p.settle();
    expect(!p.host.paused() && !p.guest.paused(), "both resume through the same agreement");

    Pair withdraw; withdraw.round(); withdraw.host.press(); withdraw.guest.press(); withdraw.g_to_h();
    expect(withdraw.host.stage() == 1, "pause prepared before withdrawal");
    withdraw.guest.press(); withdraw.g_to_h(); withdraw.settle();
    expect(!withdraw.host.paused() && !withdraw.guest.paused() && withdraw.host.stage() == 0,
           "withdrawal before acknowledgement cancels the pending vote pair");

    Pair race; race.round(); race.host.press(); race.guest.press(); race.g_to_h(); race.h_to_g();
    PauseControl accepted = race.guest.message();
    race.guest.press(); // the already issued acknowledgement can be in flight
    expect(race.host.receive(accepted, ++race.gs) && race.host.paused(), "an in-flight accepted proposal may commit");
    race.h_to_g(); expect(race.guest.paused(), "guest accepts the decision it previously acknowledged even after a new vote");
    race.settle(); expect(race.host.paused() && race.guest.paused(), "a later lone resume vote cannot undo the commitment");

    Pair stale; stale.round();
    PauseControl old = stale.guest.message(); uint32_t old_seq = stale.gs;
    stale.guest.press(); stale.g_to_h();
    expect(!stale.host.receive(old, old_seq) && stale.host.peer_vote(), "old snapshot cannot roll a pause vote back");
    PauseControl malformed = stale.guest.message(); malformed.stage = 1;
    expect(!stale.host.receive(malformed, ++stale.gs), "a guest cannot forge a host proposal");

    Pair loss; loss.round(); loss.host.lost();
    expect(loss.host.paused() && !loss.host.active() && loss.host.recovering(), "disconnect locally enters recovery pause");
    loss.h_to_g(); expect(loss.guest.paused() && loss.guest.recovering(), "recovery notice pauses the other game");
    loss.g_to_h(); loss.settle();
    expect(loss.host.paused() && loss.guest.paused(), "reconnection alone does not resume");
    loss.host.press(); loss.settle();
    expect(loss.host.paused() && loss.guest.paused(), "recovery resume still needs both votes");
    loss.guest.press(); loss.settle();
    expect(!loss.host.paused() && !loss.guest.paused() && !loss.host.recovering() && !loss.guest.recovering(),
           "both can resume after reconnect");

    Pair replace; replace.round(); replace.host.new_peer();
    expect(replace.host.paused() && replace.host.recovering(), "new process identity enters recovery hold");
    State new_guest; new_guest.reset(false);
    expect(replace.host.receive(new_guest.message(), 1), "new process may start snapshot ordering from one");
    expect(new_guest.receive(replace.host.message(), 1) && new_guest.paused() && new_guest.recovering(),
           "replacement learns the recovery hold from the fresh epoch");

    Pair dropped; dropped.round(); dropped.host.press(); dropped.guest.press(); dropped.g_to_h();
    PauseControl first_prepare = dropped.host.message();
    // A lost prepare or ACK is not a commit: later snapshots repeat the same phase.
    dropped.h_to_g(); PauseControl first_ack = dropped.guest.message();
    expect(!dropped.host.paused() && first_prepare.stage == 1 && first_ack.ack == first_prepare.proposal,
           "lost prepare/ACK leaves the host uncommitted");
    dropped.h_to_g(); dropped.g_to_h();
    expect(dropped.host.paused() && !dropped.guest.paused(), "repeated prepare/ACK eventually commits");
    PauseControl commit = dropped.host.message(); dropped.h_to_g();
    expect(dropped.guest.paused(), "repeated committed decision eventually reaches guest");
    dropped.h_to_g(); dropped.g_to_h();
    expect(commit.stage == 2 && dropped.host.stage() == 0 && dropped.guest.paused(),
           "lost applied acknowledgement is recovered without a second change");

    State exhaustion; exhaustion.reset(true);
    exhaustion.test_counters(UINT32_MAX, 0, 0);
    expect(!exhaustion.press(), "vote revision never wraps");
    exhaustion.lost();
    expect(exhaustion.paused() && exhaustion.exhausted() && !exhaustion.press(),
           "exhausted vote revision remains safely held after loss");
    exhaustion.new_peer();
    expect(exhaustion.paused() && exhaustion.recovering() && !exhaustion.exhausted() &&
           exhaustion.message().vote_revision == 2 && exhaustion.message().recovery_revision == 1,
           "a new process epoch resets exhausted counters but not the hold");
    exhaustion.reset(true); exhaustion.test_counters(1, UINT32_MAX, 0); exhaustion.lost();
    expect(exhaustion.paused() && exhaustion.exhausted() && !exhaustion.press(),
           "exhausted recovery revision cannot silently resume");
    exhaustion.reset(true); exhaustion.test_counters(1, 0, UINT32_MAX);
    expect(!exhaustion.press(), "exhausted host proposal counter refuses another vote");
    printf("pause agreement selftest: %d checks, %d failed\n", checks, failures);
    return failures ? 1 : 0;
}

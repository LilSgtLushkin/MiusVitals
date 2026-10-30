// Pure C++ tests for the same inbox the battle's shot channel uses. No game or Windows required.
#include "reliable_inbox.h"
#include "protocol.h"
#include <stdio.h>
#include <vector>

static int checks, failures;
static void check(bool value, const char *name)
{
    ++checks;
    if (!value) { ++failures; printf("FAIL %s\n", name); }
}

static ShotRec record(unsigned seq)
{
    ShotRec r = {};
    r.seq = seq;
    r.ammo = seq ^ 0x4a59;
    return r;
}

int main()
{
    reliable::HeadWait wait;
    const uint32_t start = 0xffffff00u;
    check(!wait.expired(7, 1, start, 10000), "a new head starts its own deadline");
    check(!wait.expired(7, 1, start + 9999u, 10000), "head still waits just before the deadline, across clock wrap");
    check(wait.expired(7, 1, start + 10000u, 10000), "head expires at the deadline across clock wrap");
    check(!wait.expired(7, 2, start + 10000u, 10000), "the next sequence gets a fresh deadline");
    check(!wait.expired(9, 2, start + 20000u, 10000), "a different epoch gets a fresh deadline");
    wait.reset();
    check(!wait.expired(9, 2, start + 30000u, 10000), "reset clears a pending head deadline");
    check(wait.expired(9, 2, start + 40000u, 10000), "same head eventually expires after reset");

    typedef reliable::Inbox<ShotRec, 2> Small;
    Small q;
    check(!q.epoch(0, 1), "zero epoch is invalid");
    check(!q.epoch(7, 2), "a new epoch must start at 1");
    check(q.epoch(7, 1), "first epoch");
    check(q.receive(record(2)) == Small::gap, "out-of-order datagram waits for retransmission");
    check(q.receive(record(1)) == Small::accepted, "first record is accepted");
    check(q.receive(record(1)) == Small::duplicate, "duplicate is not queued twice");
    check(q.receive(record(2)) == Small::accepted, "second record is accepted");
    check(q.receive(record(3)) == Small::full, "bounded queue does not overflow");
    check(q.committed() == 0 && q.received() == 2, "receipt is not an application ACK");
    q.items()[1].complete = true;
    check(q.commit() == 0 && q.pending() == 2, "later completion cannot ACK past an unfinished event");
    q.items()[0].complete = true;
    check(q.commit() == 2 && q.pending() == 0, "completion advances the contiguous ACK");
    check(q.receive(record(3)) == Small::accepted, "a previously full record can be retried");
    check(q.epoch(9, 1) && q.pending() == 0 && q.committed() == 0, "new epoch resets pending events");
    check(!q.epoch(7, 1) && q.current_epoch() == 9, "late retired epoch cannot reset the stream");
    check(q.receive(record(1)) == Small::accepted, "new epoch can start at 1");
    q.reset();
    check(q.epoch(7, 1), "an enclosing new connection may use an old epoch");

    // Repeated, duplicated, lost and reordered records; completion is delayed independently of
    // receipt. The sender keeps retrying everything past the cumulative application ACK.
    const unsigned total = 120;
    reliable::Inbox<ShotRec, 256> inbox;
    check(inbox.epoch(45, 1), "simulation epoch");
    std::vector<unsigned> applied(total + 1, 0);
    unsigned ack = 0, rng = 0x1234567u;
    auto random = [&]() { rng ^= rng << 13; rng ^= rng >> 17; rng ^= rng << 5; return rng; };
    for (unsigned frame = 0; frame < 6000 && ack < total; ++frame) {
        for (int n = 0; n < 12; ++n) {
            unsigned seq = ack + 1 + random() % 16;
            if (seq > total || (random() & 3) == 0) continue; // deterministic packet loss
            ShotRec r = record(seq);
            inbox.receive(r);
            if (random() & 1) inbox.receive(r);               // duplicate
        }
        for (auto &item : inbox.items()) {
            if (item.complete || (random() & 3) != 0) continue;
            check(item.record.ammo == (item.record.seq ^ 0x4a59), "payload stayed intact");
            ++applied[item.record.seq];
            item.complete = true;
        }
        ack = inbox.commit();
    }
    check(ack == total, "loss/reordering simulation makes progress");
    for (unsigned i = 1; i <= total; ++i) check(applied[i] == 1, "every simulated event applied exactly once");
    printf("reliable inbox selftest: %d checks, %d failed\n", checks, failures);
    return failures ? 1 : 0;
}

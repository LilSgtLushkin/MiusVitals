#include "operation_turn.h"
#include <stdio.h>

using namespace operation_turn;
static int checks, failures;
static void test(bool value, const char *what) {
    ++checks;
    if (!value) { ++failures; printf("FAIL %s\n", what); }
}

int main() {
    uint32_t epoch = 0; Phase phase = Phase::host;
    uint32_t packed = flags(17, Phase::client, false, true);
    test(decode(packed, &epoch, &phase) && epoch == 17 && phase == Phase::client && (packed & 2), "phase/epoch round trip");
    test(!decode(flags(0, Phase::host, false, false), nullptr, nullptr), "zero epoch refused");
    test(!decode(flags(1, (Phase)7, false, false), nullptr, nullptr), "unknown phase refused");
    test(!decode(flags(1, Phase::host, false, false) | 0x20, nullptr, nullptr), "reserved bits refused");
    test(newer_epoch(1, 0xffffff) && !newer_epoch(0xffffff, 1), "24-bit epoch wraps safely");
    test(newer_revision(1, 0xffffffff) && !newer_revision(0xffffffff, 1), "revision wraps safely");
    test(next_revision(0xffffffff) == 1 && next_revision(1) == 2, "revision never uses zero");
    test(!newer_revision(0, 4) && !newer_revision(4, 4), "zero and duplicate revisions are not newer");

    Host host;
    test(!host.begin(0, 1) && !host.begin(1, 0), "invalid host identity rejected");
    test(host.begin(123, 10) && host.host_input() && !host.may_resolve(), "host starts with sole input");
    test(!host.peer_finished(123, 10) && !host.committed(123, 10), "peer and resolver cannot jump ahead");
    test(host.finish_host() && !host.host_input(), "host finish locks local input");
    test(!host.finish_host() && !host.peer_loaded(123, 10), "finish is not repeated and old snapshot cannot ack");
    test(!host.publish(10) && !host.publish(0), "handoff must advance the snapshot identity");
    test(host.publish(11) && !host.publish(12), "one authoritative handoff per phase");
    test(!host.peer_loaded(122, 11) && !host.peer_loaded(123, 10), "wrong epoch or old revision cannot unlock guest");
    test(host.peer_loaded(123, 11) && host.phase == Phase::client, "guest gains turn only after matching load ack");
    test(!host.peer_loaded(123, 11) && !host.peer_finished(122, 11), "duplicate load and stale finish ignored");
    test(!host.committed(123, 11), "host cannot resolve while guest is moving");
    test(host.peer_finished(123, 11) && host.phase == Phase::collect, "matching guest completion starts collection");
    test(!host.peer_finished(123, 11) && !host.committed(123, 10), "duplicate finish and stale save cannot commit");
    test(host.committed(123, 11) && host.may_resolve(), "verified commit enables host resolution");
    test(!host.committed(123, 11), "duplicate commit cannot resolve twice");
    host.clear();
    test(!host.may_resolve() && !host.finish_host(), "clear removes ownership");

    Client client;
    test(!client.begin(0, 1) && !client.begin(1, 0), "invalid client identity rejected");
    test(client.begin(123, 10) && !client.input(), "client starts locked on the initial map");
    test(client.observe(123, 10, Phase::host) == Observation::current && !client.input(), "host phase is passive for guest");
    test(client.observe(123, 10, Phase::handoff) == Observation::current && !client.input(), "handoff stays passive");
    test(client.observe(123, 11, Phase::handoff) == Observation::snapshot_needed && !client.input(), "new handoff demands download");
    test(client.observe(123, 10, Phase::client) == Observation::stale && !client.input(), "old welcome cannot unlock while downloading");
    test(!client.loaded(123, 10) && !client.loaded(122, 11), "wrong loaded identity rejected");
    test(client.loaded(123, 11) && !client.input(), "loaded snapshot waits for host permission");
    test(client.observe(123, 11, Phase::client) == Observation::current && client.input(), "matching host permission unlocks client");
    test(client.observe(123, 10, Phase::client) == Observation::stale && client.input(), "old revision ignored");
    test(client.observe(123, 11, Phase::handoff) == Observation::stale && client.input(), "reordered phase cannot regress");
    test(client.finish() && !client.input() && !client.finish(), "client finishes only once");
    test(client.observe(123, 11, Phase::client) == Observation::current && !client.input(), "repeated client phase cannot reopen input");
    test(client.observe(123, 11, Phase::collect) == Observation::current && !client.input(), "collection remains locked");
    test(client.observe(124, 1, Phase::host) == Observation::snapshot_needed && !client.input(), "next epoch requires a fresh snapshot");
    test(client.observe(123, 11, Phase::client) == Observation::stale && !client.input(), "previous epoch cannot reopen input");
    test(client.observe(125, 1, Phase::host) == Observation::snapshot_needed && !client.loaded(124, 1), "newer pending offer replaces older one");
    test(client.loaded(125, 1) && client.observe(125, 1, Phase::host) == Observation::current && !client.input(), "next turn begins host-first");
    test(client.observe(125, 0, Phase::client) == Observation::invalid, "zero revision refused");
    test(client.observe(125, 1, (Phase)7) == Observation::invalid, "invalid phase refused");

    printf("operation turn selftest: %d checks, %d failed\n", checks, failures);
    return failures ? 1 : 0;
}

// Portable fault test of the production animation transport: no engine code or real network.
#include "animation_channel.h"
#include <stdio.h>
#include <string.h>
#include <vector>
#include <utility>
using namespace animation_channel;
static int checks, failures;
static void expect(bool ok, const char *what) { ++checks; if (!ok) { ++failures; printf("FAIL: %s\n", what); } }
static std::vector<std::vector<unsigned char>> ab, ba, c2d, d2c, g2h, h2g;
static void send_a(const void *p, int n) { ab.emplace_back((const unsigned char *)p, (const unsigned char *)p + n); }
static void send_b(const void *p, int n) { ba.emplace_back((const unsigned char *)p, (const unsigned char *)p + n); }
static void send_c(const void *p, int n) { c2d.emplace_back((const unsigned char *)p, (const unsigned char *)p + n); }
static void send_d(const void *p, int n) { d2c.emplace_back((const unsigned char *)p, (const unsigned char *)p + n); }
static void send_g(const void *p, int n) { g2h.emplace_back((const unsigned char *)p, (const unsigned char *)p + n); }
static void send_h(const void *p, int n) { h2g.emplace_back((const unsigned char *)p, (const unsigned char *)p + n); }
static uint64_t blocked;
static bool refuse;
static std::vector<std::pair<uint64_t, uint32_t>> applied;
static Apply replay(const ShotRec &rec, void *) {
    if (rec.shooter == blocked) return refuse ? Apply::refused : Apply::retry;
    uint32_t marker = 0; memcpy(&marker, rec.launch + 4, 4);
    uint64_t key; memcpy(&key, &rec.shooter, sizeof key);
    applied.push_back({key, marker}); return Apply::applied;
}
static ShotRec snapshot(uint64_t key, uint32_t marker) {
    unsigned char instance[anim_wire::instance_size] = {}, human[0x1f00] = {};
    memcpy(instance + 0xa8, &marker, 4);
    ShotRec out; anim_wire::encode(out, key, instance, nullptr, human); return out;
}
static bool deliver(Channel &to, const std::vector<unsigned char> &v, Send response) {
    return to.packet(v.data(), v.size(), response);
}
int main() {
    const uint64_t u = (2ull << 32) | 1, v = (2ull << 32) | 2;
    Channel a, b; a.start(11, 22); b.start(22, 11);
    ShotRec u1 = snapshot(u, 1), u2 = snapshot(u, 2), u3 = snapshot(u, 3), v1 = snapshot(v, 10);
    expect(a.queue(u1, true) && a.queue(v1, true) && a.pending_events() == 2, "two initial critical states retained");
    a.tick(1, send_a); expect(ab.size() == 2, "independent unit heads emitted");
    expect(deliver(b, ab[1], send_b) && deliver(b, ab[0], send_b), "out-of-order units accepted");
    blocked = u; b.apply(1, replay, nullptr, send_b);
    expect(applied.size() == 1 && applied[0] == std::make_pair(v, 10u) && ba.size() == 1,
           "blocked unit does not block other critical replay or ACK");
    expect(deliver(a, ba[0], send_a) && a.pending_events() == 1, "ACK retires only its unit");
    ba.clear(); ab.clear();
    expect(a.queue(u2, true) && a.queue(u3, false) && a.pending_events() == 2,
           "second transition retained while latest scalar is coalescible");
    a.tick(100, send_a); expect(ab.empty(), "no second event or scalar passes an unacknowledged transition");
    a.tick(401, send_a); expect(ab.size() == 1 && deliver(b, ab[0], send_b), "lost critical state retried");
    blocked = 0; b.apply(401, replay, nullptr, send_b);
    expect(applied.size() == 2 && applied.back() == std::make_pair(u, 1u) && ba.size() == 1, "retry applied and ACKed");
    ba.clear(); ab.clear(); // deliberately lose the ACK
    a.tick(801, send_a); expect(ab.size() == 1 && deliver(b, ab[0], send_b) && ba.size() == 1,
                              "duplicate after lost ACK is re-ACKed");
    b.apply(801, replay, nullptr, send_b);
    expect(applied.size() == 2, "duplicate critical event never replays twice");
    expect(deliver(a, ba[0], send_a) && a.pending_events() == 1, "repeated ACK unblocks next event");
    ba.clear(); ab.clear(); a.tick(802, send_a);
    expect(ab.size() == 1 && deliver(b, ab[0], send_b), "next same-unit critical event follows ACK");
    b.apply(802, replay, nullptr, send_b);
    expect(applied.size() == 3 && applied.back() == std::make_pair(u, 2u) && ba.size() == 1,
           "critical transitions replay in observed order");
    expect(deliver(a, ba[0], send_a) && a.pending_events() == 0, "second event acknowledged after replay");
    ba.clear(); ab.clear(); a.tick(803, send_a);
    expect(ab.size() == 1 && deliver(b, ab[0], send_b), "latest scalar follows its critical barrier");
    std::vector<unsigned char> latest = ab[0];
    b.apply(803, replay, nullptr, send_b);
    expect(applied.size() == 4 && applied.back() == std::make_pair(u, 3u) && ba.empty(), "replaceable scalar applied without ACK");
    expect(!deliver(b, latest, send_b), "duplicate/stale scalar cannot roll animation back");

    // An explicit failure is not a successful ACK, and is isolated to this unit.
    ab.clear(); expect(a.queue(snapshot(u, 4), true) && a.queue(snapshot(v, 11), true), "later critical changes queued");
    a.tick(900, send_a); for (const auto &wire : ab) deliver(b, wire, send_b);
    blocked = u; refuse = true; b.apply(900, replay, nullptr, send_b);
    expect(ba.size() == 1 && applied.back() == std::make_pair(v, 11u), "refused unit remains unacknowledged; other unit proceeds");
    deliver(a, ba[0], send_a); ba.clear(); expect(a.pending_events() == 1, "refused event remains retained");
    blocked = 0; refuse = false; b.apply(1100, replay, nullptr, send_b);
    expect(ba.empty() && applied.back() == std::make_pair(v, 11u) && a.pending_events() == 1,
           "a possibly partial refusal is not retried or falsely acknowledged");

    expect(!latest.empty(), "captured snapshot available for identity tests");
    Data wrong; memcpy(&wrong, latest.data(), sizeof wrong); wrong.owner = 99; wrong.revision++;
    expect(!b.packet(&wrong, sizeof wrong, send_b) && ba.empty(), "old or foreign owner is refused without ACK");
    wrong.owner = 11; wrong.target = 99;
    expect(!b.packet(&wrong, sizeof wrong, send_b), "packet for a different local process refused");
    expect(a.queue(snapshot(u, 5), true), "event before peer restart retained");
    a.start(11, 33); expect(a.pending_events() == 0, "new peer epoch invalidates previous stream");
    Ack stale = {make_head(PKT_ANIM_ACK), 11, 22, 1, u};
    expect(!a.packet(&stale, sizeof stale, send_a), "old peer ACK cannot retire new-stream events");
    expect(a.queue(snapshot(u, 6), true), "fresh epoch publishes a fresh initial state");
    size_t pending = a.pending_events(); a.start(11, 33);
    expect(a.pending_events() == pending, "same-epoch reconnect preserves unacknowledged events");

    // A soldier that transitions faster than the channel can retain must not be dropped, stop
    // the battle, or grow without bound: the newest observed state supersedes the queue.
    Channel c, d; c.start(41, 42); d.start(42, 41);
    for (uint32_t i = 1; i <= 300; ++i)
        expect(c.queue(snapshot(u, i), true), "every observed transition is accepted by the channel");
    expect(c.pending_events() <= (size_t)max_unit_events && c.rebases() == 1,
           "one soldier's pressure is bounded and supersedes its own queue");
    c.tick(1, send_c);
    expect(c2d.size() == 1 && deliver(d, c2d[0], send_d), "the superseding state is what goes on the wire");
    applied.clear(); d2c.clear();
    d.apply(1, replay, nullptr, send_d);
    expect(applied.size() == 1 && applied[0] == std::make_pair(u, 257u),
           "the newest observed state is applied; superseded transitions never replay");
    expect(d2c.size() == 1 && deliver(c, d2c[0], send_c) && c.pending_events() == 43,
           "the superseding state is acknowledged and releases the rest of the queue");
    Data old_rebase; memcpy(&old_rebase, c2d[0].data(), sizeof old_rebase);
    d2c.clear();
    expect(d.packet(&old_rebase, sizeof old_rebase, send_d) && d2c.size() == 1 && applied.size() == 1,
           "a repeated superseding state is re-acknowledged, never replayed twice");
    Data forged; memcpy(&forged, c2d[0].data(), sizeof forged);
    forged.record.seq = 100000; forged.revision = 1; d2c.clear();
    expect(!d.packet(&forged, sizeof forged, send_d) && d2c.empty(),
           "an older superseding state cannot roll animation back");

    // Global saturation keeps the bound and still retains a newest state per soldier.
    Channel g, h; g.start(51, 52); h.start(52, 51);
    for (uint32_t unit = 1; unit <= 200; ++unit)
        for (uint32_t i = 0; i < 300; ++i) g.queue(snapshot((2ull << 32) | unit, i + 1), true);
    expect(g.pending_events() <= (size_t)max_events && g.send_units() == 200,
           "a globally saturated channel stays inside its memory bound");
    expect(g.rebases() > 0, "global pressure supersedes queued transitions instead of dropping the newest");
    expect(g.update_pressure() && g.slow() && g.scan_ms() == 500u && g.unchanged_ms() == 2500u,
           "cosmetic sampling slows once the queue is under pressure");
    for (uint32_t now = 1, guard = 0; g.pending_events() && guard < 200000; ++guard) {
        g.tick(now, send_g);
        for (const auto &wire : g2h) deliver(h, wire, send_h);
        g2h.clear();
        h.apply(now, replay, nullptr, send_h);
        for (const auto &wire : h2g) deliver(g, wire, send_g);
        h2g.clear();
        now += 401;
    }
    expect(g.pending_events() == 0, "a drained channel releases every retained state");
    expect(g.update_pressure() && !g.slow() && g.scan_ms() == 200u && g.unchanged_ms() == 1000u,
           "the normal cadence returns by itself once the queue has drained");
    printf("animation channel selftest: %d checks, %d failed\n", checks, failures);
    return failures ? 1 : 0;
}

// Portable no-game tests of the production Vitals channel.
#include "vitals_core.h"
#include <stdio.h>
#include <limits>
#include <vector>
using namespace vitals;
static int checks, failures;
static void expect(bool ok, const char *message) { ++checks; if (!ok) { ++failures; printf("FAIL: %s\n", message); } }
static std::vector<std::vector<uint8_t>> a_to_b, b_to_a;
static void a_send(const void *p, int n) { a_to_b.emplace_back((const uint8_t *)p, (const uint8_t *)p + n); }
static void b_send(const void *p, int n) { b_to_a.emplace_back((const uint8_t *)p, (const uint8_t *)p + n); }
static PacketHead head(const std::vector<uint8_t> &v) { PacketHead h; memcpy(&h, v.data(), sizeof h); return h; }

int main() {
    Channel a, b; a.start(101); b.start(202); a.set_peer(202); b.set_peer(101);
    expect(sizeof(PacketHead) == 20 && sizeof(Entry) == 16, "packed wire sizes");
    expect(!a.set_peer(0) && !a.set_peer(202), "zero or unchanged peer identity does not reset");
    Entry one = soldier(7, 0.7f);
    expect(a.enqueue(&one, 1, 100, a_send) == 1 && a.outstanding() == 1, "state retained and sent");
    expect(!b.packet(a_to_b[0].data(), a_to_b[0].size(), false, b_send) && b_to_a.empty(), "no ACK before number exchange");
    a.tick(499, a_send); expect(a_to_b.size() == 1, "retry waits for deadline");
    a.tick(500, a_send); expect(a_to_b.size() == 2, "lost packet retried");
    expect(b.packet(a_to_b.back().data(), a_to_b.back().size(), true, b_send), "retry accepted");
    expect(b.find_soldier(7) && b.find_soldier(7)->health == 0.7f && !b.find_soldier(7)->is_dead, "health copied");
    expect(b.packet(a_to_b.back().data(), a_to_b.back().size(), true, b_send), "duplicate acknowledged again");
    expect(b_to_a.size() == 2, "duplicate does not suppress ACK");
    expect(a.packet(b_to_a.back().data(), b_to_a.back().size(), true, a_send) && !a.outstanding(), "matching ACK retires exactly one packet");
    expect(!a.packet(b_to_a.back().data(), b_to_a.back().size(), true, a_send), "duplicate ACK harmless");

    a_to_b.clear(); b_to_a.clear();
    Entry hurt = soldier(7, 0.45f), worse = soldier(7, 0.2f);
    expect(a.enqueue(&hurt, 1, 600, a_send) == 1 && a.enqueue(&worse, 1, 601, a_send) == 1, "two death states queued");
    expect(b.packet(a_to_b[1].data(), a_to_b[1].size(), true, b_send), "later death accepted first");
    expect(b.packet(a_to_b[0].data(), a_to_b[0].size(), true, b_send), "older state accepted out of order");
    expect(b.find_soldier(7)->is_dead && b.find_soldier(7)->health == 0.2f, "older packet cannot overwrite newer death");
    Entry owner_healed = soldier(7, 0.8f);
    expect(a.enqueue(&owner_healed, 1, 602, a_send) == 1 &&
           b.packet(a_to_b.back().data(), a_to_b.back().size(), true, b_send), "new owner state accepted");
    expect(b.packet(a_to_b[0].data(), a_to_b[0].size(), true, b_send) &&
           b.find_soldier(7)->health == 0.8f && !b.find_soldier(7)->is_dead,
           "delayed older death cannot replace newer owner state");
    for (auto &x : b_to_a) a.packet(x.data(), x.size(), true, a_send);
    expect(a.outstanding() == 0, "selective out-of-order ACKs retire their own records");

    a_to_b.clear(); b_to_a.clear();
    uint8_t v1[vehicle_bytes] = {}; v1[1] = 9;
    uint8_t v0[vehicle_bytes] = {};
    Entry ev1 = machine(44, v1), ev0 = machine(44, v0);
    a.enqueue(&ev1, 1, 700, a_send); a.enqueue(&ev0, 1, 701, a_send);
    b.packet(a_to_b[1].data(), a_to_b[1].size(), true, b_send);
    b.packet(a_to_b[0].data(), a_to_b[0].size(), true, b_send);
    Vehicle *v = b.find_vehicle(44);
    expect(v && v->pending == 0 && v->peer_nonzero == 0, "older vehicle transition cannot overtake a newer zero");
    a.enqueue(&ev1, 1, 702, a_send);
    b.packet(a_to_b.back().data(), a_to_b.back().size(), true, b_send);
    expect(v->pending == 2 && v->pending_value[1] == 9, "new vehicle transition retained");
    b.packet(a_to_b.back().data(), a_to_b.back().size(), true, b_send);
    expect(v->pending == 2 && v->pending_value[1] == 9, "duplicate vehicle transition remains idempotent");
    v->pending = 0; // the engine applied the first event; the other game then observes 1->0->1
    Entry new_transition = machine(44, v1, 2);
    a.enqueue(&ev0, 1, 703, a_send); a.enqueue(&new_transition, 1, 704, a_send);
    b.packet(a_to_b.back().data(), a_to_b.back().size(), true, b_send);
    expect(v->pending == 2, "explicit second 0-to-1 transition survives a delayed zero");
    b.packet(a_to_b[a_to_b.size() - 2].data(), a_to_b[a_to_b.size() - 2].size(), true, b_send);
    expect(v->pending == 2 && v->peer_nonzero == 2, "delayed zero cannot erase the newer event");
    v->pending = 0;
    b.packet(a_to_b.back().data(), a_to_b.back().size(), true, b_send);
    expect(v->pending == 0, "duplicate new transition is acknowledged but not applied twice");

    std::vector<uint8_t> good = a_to_b.back();
    size_t old_acks = b_to_a.size();
    expect(!b.packet(good.data(), good.size() - 1, true, b_send), "truncated packet rejected");
    good.push_back(0); expect(!b.packet(good.data(), good.size(), true, b_send), "oversized packet rejected"); good.pop_back();
    PacketHead h = head(good); h.epoch = 999; memcpy(good.data(), &h, sizeof h);
    expect(!b.packet(good.data(), good.size(), true, b_send), "unadvertised peer epoch rejected");
    h = head(a_to_b.back()); h.target = 999; memcpy(good.data(), &h, sizeof h);
    expect(!b.packet(good.data(), good.size(), true, b_send), "packet aimed at a different process incarnation rejected");
    h = head(a_to_b.back()); h.h.flags = 1; memcpy(good.data(), &h, sizeof h);
    expect(!b.packet(good.data(), good.size(), true, b_send), "unknown packet flags rejected");
    h.h.flags = 0; h.h.count = per_packet + 1; memcpy(good.data(), &h, sizeof h);
    expect(!b.packet(good.data(), good.size(), true, b_send), "too many entries rejected");
    expect(b_to_a.size() == old_acks, "invalid data not acknowledged");
    expect(!valid(soldier(1, std::numeric_limits<float>::quiet_NaN())), "NaN rejected");
    expect(!valid(soldier(1, std::numeric_limits<float>::infinity())), "infinity rejected");
    expect(!valid(soldier(1, -0.1f)) && !valid(soldier(1, 2.1f)), "implausible health rejected");
    Entry contradictory = soldier(1, 0.8f); contradictory.flags |= dead;
    expect(!valid(contradictory), "contradictory alive/dead flags rejected");
    Entry badv = ev1; badv.flags |= 0x02000000; expect(!valid(badv), "unknown vehicle bits rejected");
    badv = machine(44, v0, 2); expect(!valid(badv), "transition asserted for zero vehicle byte rejected");
    Entry packet_bad[2] = { soldier(111, 0.8f), contradictory };
    Channel manual; manual.start(333); manual.set_peer(202); a_to_b.clear();
    expect(manual.enqueue(packet_bad, 2, 1, a_send) == 0 && a_to_b.empty(), "invalid local batch is not emitted");
    // Bypass the sender to assert that one invalid entry cannot partly update the receiving tables.
    std::vector<uint8_t> wire(sizeof(PacketHead) + sizeof packet_bad);
    PacketHead crafted = { make_head(PKT_VITALS, 2), 101, 202, 1000 };
    memcpy(wire.data(), &crafted, sizeof crafted); memcpy(wire.data() + sizeof crafted, packet_bad, sizeof packet_bad);
    expect(!b.packet(wire.data(), wire.size(), true, b_send) && !b.find_soldier(111), "mixed valid/invalid packet is atomic");
    packet_bad[1] = soldier(111, 0.2f);
    memcpy(wire.data() + sizeof crafted, packet_bad, sizeof packet_bad);
    expect(!b.packet(wire.data(), wire.size(), true, b_send) && !b.find_soldier(111),
           "ambiguous repeated unit in one packet is rejected atomically");

    Channel bounded; bounded.start(400); bounded.set_peer(401); a_to_b.clear();
    std::vector<Entry> rows((max_packets + 2) * per_packet);
    for (size_t i = 0; i < rows.size(); ++i) rows[i] = soldier((uint32_t)i, 0.9f);
    size_t retained = bounded.enqueue(rows.data(), rows.size(), 100, a_send);
    expect(retained == max_packets * per_packet && bounded.outstanding() == max_packets, "bounded window backpressures unsent entries");
    bounded.tick(500, a_send); bounded.tick(900, a_send);
    expect(bounded.outstanding() == max_packets && bounded.counters().retries == max_packets * 2, "no unacknowledged packet is discarded");
    PacketHead wrong = { make_head(PKT_VITALS_ACK), 400, 401, 999999 };
    expect(!bounded.packet(&wrong, sizeof wrong, true, a_send) && bounded.outstanding() == max_packets, "future ACK cannot retire packets");
    PacketHead ack = { make_head(PKT_VITALS_ACK), 400, 401, 1 };
    expect(bounded.packet(&ack, sizeof ack, false, a_send) && bounded.outstanding() == max_packets - 1, "ACK accepted while number gate is closed");
    expect(bounded.enqueue(rows.data() + retained, rows.size() - retained, 1000, a_send) == per_packet, "backpressured entries resume when capacity frees");
    expect(bounded.set_peer(402) && bounded.outstanding() == 0 && bounded.peer_epoch() == 402, "new advertised peer invalidates old state");
    expect(!bounded.packet(&ack, sizeof ack, true, a_send), "old ACK cannot retire fresh/absent data");
    expect(b.set_peer(303) && !b.find_soldier(7) && !b.find_vehicle(44), "new peer clears previous remote states");
    expect(!b.set_peer(101) && b.peer_epoch() == 303, "a delayed old handshake cannot restore a retired peer epoch");
    expect(!b.packet(a_to_b[0].data(), a_to_b[0].size(), true, b_send), "old epoch rejected after peer replacement");

    // Even if a delayed old handshake arrives first after *our* process restarts, its queued
    // packet was addressed to the previous incarnation and cannot affect the new one.
    Channel restarted; restarted.start(999); restarted.set_peer(400);
    expect(!restarted.packet(a_to_b[0].data(), a_to_b[0].size(), true, b_send) && !restarted.find_soldier(0),
           "a packet addressed to a previous local process is never applied or ACKed");
    a_to_b.clear();
    expect(bounded.enqueue(&one, 1, 2000, a_send) == 1, "new peer permits a fresh state");
    PacketHead fresh = head(a_to_b.back());
    PacketHead stale_ack = { make_head(PKT_VITALS_ACK), 400, 401, fresh.seq };
    size_t pending = bounded.outstanding();
    expect(!bounded.packet(&stale_ack, sizeof stale_ack, true, a_send) && bounded.outstanding() == pending,
           "old peer cannot ACK a new peer's packet");

    Channel many; many.start(777); many.set_peer(1000);
    for (uint32_t i = 1; i <= max_retired; ++i) expect(many.set_peer(1000 + i), "new peer within retired-epoch budget");
    expect(!many.set_peer(1000 + max_retired + 1) && many.peer_epoch() == 1000 + max_retired,
           "retired-epoch budget fails closed instead of accepting old processes");
    expect(!many.set_peer(1000), "oldest peer stays retired at capacity");

    printf("vitals core selftest: %d checks, %d failed\n", checks, failures);
    return failures ? 1 : 0;
}

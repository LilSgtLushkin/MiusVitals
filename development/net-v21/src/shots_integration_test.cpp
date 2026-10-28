// Exercise the actual shots.cpp receive/send paths with a fake engine, no game and no Steam.
// Built only in selftest with NETV21_TEST_HOOKS; these seams are absent from the game DLL.
#include "common.h"
#include "netlog.h"
#include "protocol.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <vector>

using TestLaunch = void (*)(char *, char *, u64, float *, void *, u32);
void shots_test_engine(ObjMan *, char *, TestLaunch);
size_t shots_test_outstanding();
size_t shots_test_legacy_pending();
u32 shots_test_epoch();

char g_out_dir[MAX_PATH] = ".";
char g_program[32] = "tacgame";
static unsigned char ammo[0xc8], weapon[0xc8];
static unsigned launched, tables, trenches, hit_calls, hit_completed;
static bool hit_ready;
static std::vector<ShotAck> acks;
static std::vector<std::vector<unsigned char>> sent_packets;
static unsigned datagrams;
static int checks, failures;

void mp_log(const char *, ...) {}
bool readable(const void *p, size_t) { return p != nullptr; }
void netlog_count(const char *, long long) {}
bool session_connected() { return true; }
int session_role() { return ROLE_HOST; }
bool session_owns(Object *) { return true; }
bool session_unit_key(Object *, u64 *) { return false; }
Object *session_unit(u64) { return nullptr; }
u64 session_battle_ms() { return 0; }
void trench_received(const ShotRec &) { ++trenches; }
void trench_resend() {}
void xlate_received(const ShotRec &, ObjMan *) { ++tables; }
void xlate_resend() {}
u32 xlate_desc(u32 n) { return n; }
u32 vitals_epoch() { return 101; }
u32 vitals_peer_epoch() { return 202; }
bool hits_received(const ShotRec &, ObjMan *) { ++hit_calls; if (!hit_ready) return false; ++hit_completed; return true; }
bool anim_received(const ShotRec &, ObjMan *) { return true; }
void hits_reset_peer() {}
void anim_reset_peer() {}

static void *lookup(ObjMan *, u32 id) { return id == 1 ? ammo : id == 2 ? weapon : nullptr; }
static Object *find(ObjMan *, Dep *) { return nullptr; }
static void launch(char *, char *, u64, float *, void *, u32) { ++launched; }
static void capture(const void *data, int size)
{
    if (size < (int)sizeof(Head)) return;
    const Head *h = (const Head *)data;
    if (h->type == PKT_SHOT_ACK && size == (int)sizeof(ShotAck)) acks.push_back(*(const ShotAck *)data);
    if (h->type == PKT_SHOT) {
        ++datagrams;
        const unsigned char *bytes = (const unsigned char *)data;
        sent_packets.emplace_back(bytes, bytes + size);
    }
}
static void check(bool ok, const char *name)
{
    ++checks;
    if (!ok) { ++failures; printf("FAIL %s\n", name); }
}
static ShotRec make_record(u32 seq, unsigned char kind = 0)
{
    ShotRec r = {};
    r.seq = seq;
    r.kind = kind;
    r.ammo = 1;
    r.weapon = 2;
    r.desc3 = 0xffffffffu;
    memcpy(r.ammo_check, ammo, 8);
    memcpy(&r.ammo_code, ammo + 0xc4, 4);
    return r;
}
static void incoming(u32 epoch, const ShotRec *records, unsigned count, bool legacy = false)
{
    unsigned char packet[sizeof(ShotHead) + 5 * sizeof(ShotRec)] = {};
    ShotHead h = { make_head(PKT_SHOT, count), epoch, 202, 101 };
    memcpy(packet, &h, sizeof h);
    memcpy(packet + sizeof h, records, count * sizeof(ShotRec));
    if (legacy) shots_on_packet_legacy(packet, (int)(sizeof h + count * sizeof(ShotRec)), capture);
    else shots_on_packet(packet, (int)(sizeof h + count * sizeof(ShotRec)), capture);
}
static u32 latest_ack() { return acks.empty() ? 0xffffffffu : acks.back().seq; }

int main()
{
    void *vtbl[58] = {};
    vtbl[9] = (void *)find;
    vtbl[57] = (void *)lookup;
    ObjMan om = { vtbl };
    float numbers[] = { 1.25f, 2.5f };
    memcpy(ammo, numbers, sizeof numbers);
    const u32 code = 0x12345678;
    memcpy(ammo + 0xc4, &code, 4);

    shots_reset();
    shots_test_engine(&om, nullptr, launch);
    ShotRec one = make_record(1);
    struct { ShotHead head; ShotRec rec; } stale = {};
    stale.head = { make_head(PKT_SHOT, 1), 1001, 999, 101 }; stale.rec = one;
    shots_on_packet((const unsigned char *)&stale, sizeof stale, capture);
    check(acks.empty(), "packet from a retired sender epoch is rejected without ACK");
    stale.head.owner = 202; stale.head.target = 999;
    shots_on_packet((const unsigned char *)&stale, sizeof stale, capture);
    check(acks.empty(), "packet addressed to an old local process is rejected without ACK");
    incoming(1001, &one, 1);
    check(latest_ack() == 0, "receipt is not acknowledged as application");
    shots_sim_step(&om);
    check(launched == 0 && latest_ack() == 0, "missing shell manager retains the shot");
    incoming(1001, &one, 1);
    check(latest_ack() == 0, "duplicate while waiting does not advance the ACK");
    shots_test_engine(&om, (char *)ammo, launch);
    shots_sim_step(&om);
    check(launched == 1 && latest_ack() == 1, "actual launch advances ACK after application");
    incoming(1001, &one, 1);
    shots_sim_step(&om);
    check(launched == 1 && latest_ack() == 1, "a retransmission is not applied twice");

    ShotRec three = make_record(3);
    incoming(1001, &three, 1);
    check(latest_ack() == 1, "a gap is not acknowledged");
    ShotRec two = make_record(2);
    incoming(1001, &two, 1);
    shots_sim_step(&om);
    check(launched == 2 && latest_ack() == 2, "missing record can be retransmitted");
    incoming(1001, &three, 1);
    shots_sim_step(&om);
    check(launched == 3 && latest_ack() == 3, "reordered record later applies exactly once");

    shots_test_engine(&om, nullptr, launch);
    ShotRec mixed[2] = { make_record(4), make_record(5, 3) };
    incoming(1001, mixed, 2);
    shots_sim_step(&om);
    shots_sim_step(&om);
    check(tables == 1 && latest_ack() == 3, "control table processed once without ACKing blocked shot");
    shots_test_engine(&om, (char *)ammo, launch);
    shots_sim_step(&om);
    check(launched == 4 && latest_ack() == 5, "unblocking the shot commits the completed table");

    ShotRec bad = make_record(6);
    bad.ammo_code ^= 1;
    incoming(1001, &bad, 1);
    shots_sim_step(&om);
    check(launched == 4 && latest_ack() == 6, "mismatched ammunition is explicitly refused, not launched");

    shots_reset(); acks.clear();
    ShotRec hit_then_shot[2] = { make_record(1, 7), make_record(2) };
    unsigned before_launch = launched;
    incoming(2002, hit_then_shot, 2);
    hit_ready = false;
    shots_sim_step(&om);
    check(hit_calls == 1 && hit_completed == 0 && launched == before_launch && latest_ack() == 0,
          "waiting hit blocks a later shot and does not advance ACK");
    hit_ready = true;
    shots_sim_step(&om);
    check(hit_completed == 1 && launched == before_launch + 1 && latest_ack() == 2,
          "hit then shot complete in order and are ACKed after replay");
    incoming(2002, hit_then_shot, 2);
    shots_sim_step(&om);
    check(hit_completed == 1 && launched == before_launch + 1, "retransmission does not replay the hit twice");

    shots_reset();
    ShotRec bounded = make_record(0, 7);
    check(!shots_try_queue(bounded, 0) && shots_test_outstanding() == 0 && bounded.seq == 0,
          "zero limit refuses without consuming a sequence");
    check(shots_try_queue(bounded, 1) && bounded.seq == 1, "bounded queue accepts within its limit");
    ShotRec blocked = make_record(0, 7);
    check(!shots_try_queue(blocked, 1) && blocked.seq == 0 && shots_test_outstanding() == 1,
          "full bounded queue refuses without creating a sequence gap");

    shots_reset();
    ShotRec snapshot = make_record(0, 8); snapshot.shooter = (2ull << 32) | 7;
    check(shots_queue_latest(snapshot, 8) && snapshot.seq == 1, "new animation snapshot queued");
    snapshot.launch[0] = 42;
    check(shots_queue_latest(snapshot, 8) && snapshot.seq == 1 && shots_test_outstanding() == 1,
          "unsent snapshot for the same unit is coalesced");
    ShotRec causal_hit = make_record(0, 7); shots_queue(causal_hit);
    snapshot.launch[0] = 43;
    check(shots_queue_latest(snapshot, 8) && snapshot.seq == 3 && shots_test_outstanding() == 3,
          "a newer snapshot is not moved across an intervening hit");
    shots_frame(capture);
    snapshot.launch[0] = 44;
    check(shots_queue_latest(snapshot, 8) && snapshot.seq == 4 && shots_test_outstanding() == 4,
          "already-sent snapshot is never rewritten under the same sequence");

    shots_reset();
    ShotRec outbound = make_record(0);
    shots_queue(outbound);
    ShotAck ack = { make_head(PKT_SHOT_ACK), shots_test_epoch(), 1, 101, 202 };
    shots_on_packet((const unsigned char *)&ack, sizeof ack, capture);
    check(shots_test_outstanding() == 1, "an ACK cannot remove an event never sent");
    shots_frame(capture);
    check(datagrams > 0, "outbound event is sent");
    ack.owner = 999;
    shots_on_packet((const unsigned char *)&ack, sizeof ack, capture);
    check(shots_test_outstanding() == 1, "ACK for an old local process cannot retire an event");
    ack.owner = 101; ack.target = 999;
    shots_on_packet((const unsigned char *)&ack, sizeof ack, capture);
    check(shots_test_outstanding() == 1, "ACK from an old peer cannot retire an event");
    ack.target = 202;
    ack.seq = 2;
    shots_on_packet((const unsigned char *)&ack, sizeof ack, capture);
    check(shots_test_outstanding() == 1, "an ACK beyond the latest sequence is rejected");
    ack.seq = 1;
    shots_on_packet((const unsigned char *)&ack, sizeof ack, capture);
    check(shots_test_outstanding() == 0, "a valid ACK retires the sent event");

    // Characterize the existing 60-event window, not a fix: while a head event stays blocked,
    // the cumulative ACK cannot move and a table at sequence 61 is not transmitted. This fake
    // hit remains blocked until the test releases it; production hits currently time out.
    shots_reset(); acks.clear(); sent_packets.clear(); hit_ready = false;
    ShotRec blocked_hit = make_record(0, 7); shots_queue(blocked_hit);
    for (unsigned i = 0; i < 59; ++i) { ShotRec later = make_record(0); shots_queue(later); }
    ShotRec beyond_window = make_record(0, 3); shots_queue(beyond_window);
    check(beyond_window.seq == 61 && shots_test_outstanding() == 61, "fixture queues a control table beyond the sender window");
    shots_frame(capture);
    u32 highest_sent = 0; bool table_sent = false;
    // Feed the actual outbound datagrams to the production receiver, reversing the synthetic
    // endpoint identities. capture() collects ACKs but does not mutate this local packet copy.
    auto outbound_packets = sent_packets;
    for (auto packet : outbound_packets) {
        ShotHead *head = (ShotHead *)packet.data();
        ShotRec *records = (ShotRec *)(head + 1);
        for (unsigned i = 0; i < head->h.count; ++i) {
            if (records[i].seq > highest_sent) highest_sent = records[i].seq;
            if (records[i].kind == 3) table_sent = true;
        }
        head->owner = 202; head->target = 101;
        shots_on_packet(packet.data(), (int)packet.size(), capture);
    }
    check(highest_sent == 60 && !table_sent, "KNOWN LIMITATION: the real sender stops before the table at sequence 61");
    unsigned table_count = tables;
    shots_sim_step(&om);
    check(latest_ack() == 0 && tables == table_count, "blocked head leaves cumulative ACK at zero without delivering the later table");
    ShotAck blocked_ack = { make_head(PKT_SHOT_ACK), shots_test_epoch(), 0, 101, 202 };
    shots_on_packet((const unsigned char *)&blocked_ack, sizeof blocked_ack, capture);
    check(shots_test_outstanding() == 61, "zero ACK cannot free the sender window for the table");
    printf("shots window characterization: while the head is blocked, a table outside the 60-event window cannot be sent; limitation remains OPEN\n");

    shots_reset(); acks.clear();
    incoming(7001, &one, 1, true);
    check(latest_ack() == 1 && shots_test_legacy_pending() == 1, "non-tactical receipt keeps the old immediate ACK");
    incoming(7001, &one, 1, true);
    incoming(7001, &three, 1, true);
    check(latest_ack() == 1 && shots_test_legacy_pending() == 1, "non-tactical duplicate and gap behavior stays unchanged");
    incoming(7001, &two, 1, true);
    check(latest_ack() == 2 && shots_test_legacy_pending() == 2, "non-tactical stream accepts the next sequence");
    shots_reset();
    check(shots_test_legacy_pending() == 0, "non-tactical reset clears its old queue");

    printf("shots integration selftest: %d checks, %d failed\n", checks, failures);
    return failures ? 1 : 0;
}

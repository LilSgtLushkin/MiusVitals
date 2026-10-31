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
void shots_test_wait_time(DWORD);
void shots_test_wait_real_time();
size_t shots_test_outstanding();
size_t shots_test_legacy_pending();
u32 shots_test_epoch();

char g_out_dir[MAX_PATH] = ".";
char g_program[32] = "tacgame";
static unsigned char ammo[0xc8], weapon[0xc8], third_desc[0xc8];
static unsigned launched, tables, trenches, hit_calls, hit_completed, anim_calls, ammo_queries;
static unsigned hit_resets, anim_wait_resets, table_resets, trench_forgets;
static unsigned end_words;      // the other player's word, as the session would receive it
static EndRec last_end;
static bool hit_ready, anim_ready = true, mapping_ready = true, third_maps_to_null;
static u32 missing_description = 0xffffffffu;
static void *unreadable_pointer, *last_obj2, *last_obj5, *last_desc3;
static Object known_object = {};
static const u64 known_key = (2ull << 32) | 17;
static std::vector<ShotAck> acks;
static std::vector<std::vector<unsigned char>> sent_packets;
static unsigned datagrams;
static int checks, failures;

void mp_log(const char *, ...) {}
bool readable(const void *p, size_t size) {
    if (p == ammo && size == 0xc8) ++ammo_queries;
    return p != nullptr && p != unreadable_pointer;
}
void netlog_count(const char *, long long) {}
bool session_connected() { return true; }
int session_role() { return ROLE_HOST; }
bool session_owns(Object *) { return true; }
bool session_unit_key(Object *, u64 *) { return false; }
Object *session_unit(u64 key) { return key == known_key ? &known_object : nullptr; }
u64 session_battle_ms() { return 0; }
void trench_received(const ShotRec &) { ++trenches; }
void trench_forget_peer() { ++trench_forgets; }
void trench_resend() {}
void xlate_received(const ShotRec &, ObjMan *) { ++tables; }
void xlate_resend() { ++table_resets; }
u32 xlate_desc(u32 n) { return n; }
bool xlate_desc_checked(u32 n, u32 *out) {
    if (n == 0xffffffffu) { *out = n; return true; }
    if (!mapping_ready || n == missing_description) return false;
    *out = n == 3 && third_maps_to_null ? 0xffffffffu : n;
    return true;
}
u32 vitals_epoch() { return 101; }
u32 vitals_peer_epoch() { return 202; }
bool hits_received(const ShotRec &, ObjMan *) { ++hit_calls; if (!hit_ready) return false; ++hit_completed; return true; }
bool anim_received(const ShotRec &, ObjMan *) { ++anim_calls; return anim_ready; }
void hits_reset_peer() { ++hit_resets; }
void anim_reset_wait() { ++anim_wait_resets; }
void anim_reset_peer() {}
void session_end_record(const ShotRec &s) { ++end_words; memcpy(&last_end, s.launch, sizeof last_end); }

static void *lookup(ObjMan *, u32 id) { return id == 1 ? ammo : id == 2 ? weapon : id == 3 ? third_desc : nullptr; }
static Object *find(ObjMan *, Dep *) { return nullptr; }
static void launch(char *, char *rec, u64, float *, void *obj5, u32) {
    memcpy(&last_obj2, rec + 0x50, sizeof last_obj2);
    memcpy(&last_desc3, rec + 0x60, sizeof last_desc3);
    last_obj5 = obj5;
    ++launched;
}
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
    unsigned queries_before = ammo_queries;
    shots_sim_step(&om);
    check(launched == 1 && latest_ack() == 1, "actual launch advances ACK after application");
    check(ammo_queries == queries_before + 1, "ammunition readability is checked once per replay");
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

    // The end of a battle rides this same channel: the other player's word, answered and repeated
    // like any other record here, and spoken to the session exactly once.
    ShotRec word = make_record(7, SHOT_END);
    EndRec spoken = {};
    spoken.propose = 1; spoken.over = 1; spoken.result = 3;
    memcpy(word.launch, &spoken, sizeof spoken);
    incoming(1001, &word, 1);
    shots_sim_step(&om);
    check(end_words == 1 && last_end.propose == 1 && last_end.over == 1 && last_end.result == 3,
          "the other player's word arrives on the shots channel with the result it carried");
    check(latest_ack() == 7, "a word is acknowledged like any other record of the channel");
    incoming(1001, &word, 1);
    shots_sim_step(&om);
    check(end_words == 1, "a retransmitted word is not spoken to the session twice");
    check(launched == 4, "a word launches no shell");

    // Missing descriptions must never fall back to their numeric value. A later table may
    // unblock the head before its deadline; a completed shot must not replay on retransmission.
    shots_reset(); acks.clear(); shots_test_wait_time(100);
    unsigned prior = launched;
    missing_description = 1;
    incoming(1010, &one, 1); shots_sim_step(&om);
    check(launched == prior && latest_ack() == 0, "unknown ammunition holds the head without replay or ACK");
    shots_test_wait_time(10099); shots_sim_step(&om);
    check(launched == prior && latest_ack() == 0, "unknown ammunition still waits just before ten seconds");
    missing_description = 0xffffffffu;
    shots_sim_step(&om);
    check(launched == prior + 1 && latest_ack() == 1, "a mapping arriving in time allows exactly one replay");
    incoming(1010, &one, 1); shots_sim_step(&om);
    check(launched == prior + 1 && latest_ack() == 1, "retransmission after successful replay stays deduplicated");

    shots_reset(); acks.clear(); shots_test_wait_time(200);
    prior = launched; missing_description = 2;
    incoming(1011, &one, 1); shots_sim_step(&om);
    check(launched == prior && latest_ack() == 0, "unknown weapon is never substituted by a local numeric ID");
    shots_test_wait_time(10200); shots_sim_step(&om);
    check(launched == prior && latest_ack() == 1, "unknown weapon is explicitly refused at its deadline");
    missing_description = 0xffffffffu;
    incoming(1011, &one, 1); shots_sim_step(&om);
    check(launched == prior && latest_ack() == 1, "a timed-out shot cannot replay after a mapping appears");

    shots_reset(); acks.clear(); shots_test_wait_time(300);
    ShotRec with_third = one; with_third.desc3 = 3;
    prior = launched; missing_description = 3;
    incoming(1012, &with_third, 1); shots_sim_step(&om);
    check(launched == prior && latest_ack() == 0, "an unknown third description also blocks replay");
    shots_test_wait_time(10300); shots_sim_step(&om);
    check(launched == prior && latest_ack() == 1, "unknown third description is refused after the deadline");
    missing_description = 0xffffffffu;

    shots_reset(); acks.clear(); shots_test_wait_time(400);
    with_third.obj2 = known_key; with_third.obj5 = known_key;
    incoming(1013, &with_third, 1); shots_sim_step(&om);
    check(launched == prior + 1 && latest_ack() == 1 && last_desc3 == third_desc &&
          last_obj2 == &known_object && last_obj5 == &known_object,
          "matched third description and secondary objects reach the launch unchanged");
    ShotRec absent_obj = make_record(2); absent_obj.obj2 = known_key + 1;
    incoming(1013, &absent_obj, 1); shots_sim_step(&om);
    check(launched == prior + 1 && latest_ack() == 2, "an identified missing secondary object is refused, not replaced by null");
    absent_obj.seq = 3; absent_obj.obj2 = 0; absent_obj.obj5 = known_key + 1;
    incoming(1013, &absent_obj, 1); shots_sim_step(&om);
    check(launched == prior + 1 && latest_ack() == 3, "an identified missing fifth argument is refused, not replaced by null");
    absent_obj.seq = 4; absent_obj.obj5 = 0; absent_obj.shooter = known_key + 1;
    incoming(1013, &absent_obj, 1); shots_sim_step(&om);
    check(launched == prior + 1 && latest_ack() == 4, "an identified missing shooter is refused");
    absent_obj.seq = 5; absent_obj.shooter = known_key; unreadable_pointer = &known_object;
    incoming(1013, &absent_obj, 1); shots_sim_step(&om);
    check(launched == prior + 1 && latest_ack() == 5, "an unreadable cached shooter is refused");
    unreadable_pointer = nullptr;
    with_third.seq = 6; third_maps_to_null = true;
    incoming(1013, &with_third, 1); shots_sim_step(&om);
    check(launched == prior + 1 && latest_ack() == 6, "a non-null third description cannot map to the null sentinel");
    third_maps_to_null = false;

    shots_reset(); acks.clear(); shots_test_wait_time(500);
    ShotRec unreadable_ammo = one;
    memset(unreadable_ammo.ammo_check, 0, sizeof unreadable_ammo.ammo_check); unreadable_ammo.ammo_code = 0;
    unreadable_pointer = ammo; prior = launched;
    incoming(1014, &unreadable_ammo, 1); shots_sim_step(&om);
    check(launched == prior && latest_ack() == 1, "unreadable ammunition cannot pass an all-zero shallow check");
    unreadable_pointer = nullptr;

    shots_reset(); acks.clear(); shots_test_wait_time(600);
    prior = launched; vtbl[57] = nullptr;
    incoming(1015, &one, 1); shots_sim_step(&om);
    check(launched == prior && latest_ack() == 0, "missing description lookup waits instead of calling a null vtable slot");
    vtbl[57] = (void *)lookup; shots_sim_step(&om);
    check(launched == prior + 1 && latest_ack() == 1, "description lookup can recover within the deadline");

    // One deadline follows the head even when the reason changes, and a new epoch must not
    // inherit the prior stream's time. Fake time avoids slow or nondeterministic sleeps.
    shots_reset(); acks.clear(); shots_test_engine(&om, nullptr, launch); shots_test_wait_time(700);
    prior = launched; mapping_ready = false;
    incoming(1016, &one, 1); shots_sim_step(&om);
    shots_test_engine(&om, (char *)ammo, launch); shots_test_wait_time(10699); shots_sim_step(&om);
    check(launched == prior && latest_ack() == 0, "a changed waiting reason does not replay or restart the head");
    shots_test_wait_time(10700); shots_sim_step(&om);
    check(launched == prior && latest_ack() == 1, "the changed reason still expires ten seconds after the head first blocked");
    mapping_ready = true;

    shots_reset(); acks.clear(); shots_test_engine(&om, nullptr, launch); shots_test_wait_time(800);
    incoming(1017, &one, 1); shots_sim_step(&om);
    unsigned old_hit_resets = hit_resets, old_anim_resets = anim_wait_resets, old_table_resets = table_resets, old_trench_forgets = trench_forgets;
    shots_test_wait_time(10800); incoming(1018, &one, 1); shots_sim_step(&om);
    check(latest_ack() == 0, "a new epoch starts a fresh deadline even with the same sequence");
    check(hit_resets == old_hit_resets + 1 && anim_wait_resets == old_anim_resets &&
          table_resets == old_table_resets + 1 && trench_forgets == old_trench_forgets + 1,
          "shot stream restart invalidates its tables/hit wait, not the independent animation channel");
    shots_test_wait_time(20799); shots_sim_step(&om);
    check(latest_ack() == 0, "the new epoch still waits just before its own deadline");
    shots_test_wait_time(20800); shots_sim_step(&om);
    check(latest_ack() == 1 && launched == prior, "missing shell manager is refused at the new epoch's deadline");
    shots_test_engine(&om, (char *)ammo, launch);

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

    shots_reset(); acks.clear(); shots_test_wait_time(1000);
    hit_ready = false; anim_ready = false; before_launch = launched;
    ShotRec stalled_handlers[3] = { make_record(1, 7), make_record(2, 8), make_record(3) };
    incoming(2003, stalled_handlers, 3); shots_sim_step(&om);
    check(latest_ack() == 0 && launched == before_launch, "an indefinitely waiting handler holds later gameplay");
    shots_test_wait_time(11000); shots_sim_step(&om);
    check(latest_ack() == 3 && launched == before_launch + 1 && anim_calls == 0,
          "stuck hit times out; animation records are not dispatched on the shot channel");
    incoming(2003, stalled_handlers, 3); shots_sim_step(&om);
    check(anim_calls == 0 && launched == before_launch + 1, "retransmission cannot replay the shot or dispatch animation");
    hit_ready = true; anim_ready = true;

    // A purported control bypass cannot skip an unreceived sequence: it is rejected and must
    // actually be retransmitted once the gap has arrived, even if the gameplay head is blocked.
    shots_reset(); acks.clear(); shots_test_engine(&om, nullptr, launch); shots_test_wait_time(22000);
    unsigned table_before = tables;
    ShotRec later_table = make_record(3, 3);
    incoming(2004, &one, 1); shots_sim_step(&om);
    incoming(2004, &later_table, 1); shots_sim_step(&om);
    check(tables == table_before && latest_ack() == 0, "a control record beyond a missing sequence is rejected, not buffered");
    incoming(2004, &two, 1); shots_sim_step(&om);
    check(tables == table_before && latest_ack() == 0, "filling the gap alone does not recover the discarded control record");
    incoming(2004, &later_table, 1); shots_sim_step(&om);
    check(tables == table_before + 1 && latest_ack() == 0, "a retried contiguous table is processed once while the head still waits");
    incoming(2004, &later_table, 1); shots_sim_step(&om);
    check(tables == table_before + 1, "a duplicate control record is not processed twice");
    shots_test_engine(&om, (char *)ammo, launch); shots_sim_step(&om);
    check(latest_ack() == 3, "after both shots replay, the cumulative ACK includes the completed table");

    unsigned forget_before_reset = trench_forgets;
    shots_reset();
    check(trench_forgets == forget_before_reset + 1, "a full event reset retires handed-off trenches");
    ShotRec bounded = make_record(0, 7);
    check(!shots_try_queue(bounded, 0) && shots_test_outstanding() == 0 && bounded.seq == 0,
          "zero limit refuses without consuming a sequence");
    check(shots_try_queue(bounded, 1) && bounded.seq == 1, "bounded queue accepts within its limit");
    ShotRec blocked = make_record(0, 7);
    check(!shots_try_queue(blocked, 1) && blocked.seq == 0 && shots_test_outstanding() == 1,
          "full bounded queue refuses without creating a sequence gap");

    // Animation admission/retry is covered by animation_channel_test; it never occupies this queue.
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

    // The unchanged 60-event window cannot send a table at 61 while the head is blocked.
    // The independent head deadline trades that event for progress, not lossless delivery.
    shots_reset(); acks.clear(); sent_packets.clear(); hit_ready = false; shots_test_wait_time(30000);
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
    unsigned before_unblock = launched;
    shots_test_wait_time(40000); shots_sim_step(&om);
    check(latest_ack() == 60 && launched == before_unblock + 59,
          "after terminal refusal the following received shots complete and the ACK can free the window");
    blocked_ack.seq = latest_ack();
    shots_on_packet((const unsigned char *)&blocked_ack, sizeof blocked_ack, capture);
    check(shots_test_outstanding() == 1, "cumulative ACK retires the sixty sent records only");
    sent_packets.clear(); shots_frame(capture);
    bool sent_after_refusal = false;
    for (const auto &packet : sent_packets) {
        const ShotHead *head = (const ShotHead *)packet.data();
        const ShotRec *records = (const ShotRec *)(head + 1);
        for (unsigned i = 0; i < head->h.count; ++i) sent_after_refusal |= records[i].seq == 61 && records[i].kind == 3;
    }
    check(sent_after_refusal, "the existing sender can send table 61 only after the refused head frees window space");
    printf("shots window: no bypass; deadline progresses by explicitly losing the blocked event\n");
    hit_ready = true; shots_test_wait_real_time();

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

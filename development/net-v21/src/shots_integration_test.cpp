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
u32 shots_test_epoch();

char g_out_dir[MAX_PATH] = ".";
char g_program[32] = "tacgame";
static unsigned char ammo[0xc8], weapon[0xc8];
static unsigned launched, tables, trenches;
static std::vector<ShotAck> acks;
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

static void *lookup(ObjMan *, u32 id) { return id == 1 ? ammo : id == 2 ? weapon : nullptr; }
static Object *find(ObjMan *, Dep *) { return nullptr; }
static void launch(char *, char *, u64, float *, void *, u32) { ++launched; }
static void capture(const void *data, int size)
{
    if (size < (int)sizeof(Head)) return;
    const Head *h = (const Head *)data;
    if (h->type == PKT_SHOT_ACK && size == (int)sizeof(ShotAck)) acks.push_back(*(const ShotAck *)data);
    if (h->type == PKT_SHOT) ++datagrams;
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
static void incoming(u32 epoch, const ShotRec *records, unsigned count)
{
    unsigned char packet[sizeof(ShotHead) + 5 * sizeof(ShotRec)] = {};
    ShotHead h = { make_head(PKT_SHOT, count), epoch };
    memcpy(packet, &h, sizeof h);
    memcpy(packet + sizeof h, records, count * sizeof(ShotRec));
    shots_on_packet(packet, (int)(sizeof h + count * sizeof(ShotRec)), capture);
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

    shots_reset();
    ShotRec outbound = make_record(0);
    shots_queue(outbound);
    ShotAck ack = { make_head(PKT_SHOT_ACK), shots_test_epoch(), 1 };
    shots_on_packet((const unsigned char *)&ack, sizeof ack, capture);
    check(shots_test_outstanding() == 1, "an ACK cannot remove an event never sent");
    shots_frame(capture);
    check(datagrams > 0, "outbound event is sent");
    ack.seq = 2;
    shots_on_packet((const unsigned char *)&ack, sizeof ack, capture);
    check(shots_test_outstanding() == 1, "an ACK beyond the latest sequence is rejected");
    ack.seq = 1;
    shots_on_packet((const unsigned char *)&ack, sizeof ack, capture);
    check(shots_test_outstanding() == 0, "a valid ACK retires the sent event");

    printf("shots integration selftest: %d checks, %d failed\n", checks, failures);
    return failures ? 1 : 0;
}

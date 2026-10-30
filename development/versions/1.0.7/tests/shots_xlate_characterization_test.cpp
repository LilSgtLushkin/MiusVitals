// Exercise the shot/translation interaction using both real production modules.
// The fake engine is synthetic; no game or real Steam is involved. A completed mapping is
// still a heuristic and this test cannot prove the identity of real engine descriptions.
#include "common.h"
#include "netlog.h"
#include "protocol.h"
#include <stdio.h>
#include <string.h>

using TestLaunch = void (*)(char *, char *, u64, float *, void *, u32);
void shots_test_engine(ObjMan *, char *, TestLaunch);
void shots_test_wait_time(DWORD);
u32 xlate_test_fingerprint(const void *);

char g_out_dir[MAX_PATH] = ".";
char g_program[32] = "tacgame";
static unsigned char correct_ammo[0xc8], wrong_ammo[0xc8], correct_weapon[0xc8], wrong_weapon[0xc8];
static void *last_ammo, *last_weapon, *last_desc3;
static unsigned launches, checks, failed, ack_sequence;

void mp_log(const char *, ...) {}
bool readable(const void *p, size_t) { return p != nullptr; }
void netlog_count(const char *, long long) {}
bool session_connected() { return true; }
int session_role() { return ROLE_HOST; }
bool session_owns(Object *) { return true; }
bool session_unit_key(Object *, u64 *) { return false; }
Object *session_unit(u64) { return nullptr; }
u64 session_battle_ms() { return 0; }
bool state_remote_loading() { return false; }
void trench_received(const ShotRec &) {}
void trench_forget_peer() {}
void trench_resend() {}
u32 vitals_epoch() { return 101; }
u32 vitals_peer_epoch() { return 202; }
bool hits_received(const ShotRec &, ObjMan *) { return true; }
bool anim_received(const ShotRec &, ObjMan *) { return true; }
void hits_reset_peer() {}
void anim_reset_wait() {}
void anim_reset_peer() {}

static void *lookup(ObjMan *, u32 id)
{
    switch (id) {
    case 7: return correct_ammo;
    case 8: return correct_weapon;
    case 9: return wrong_ammo;
    case 10: return wrong_weapon;
    case 41: return wrong_ammo;
    case 42: case 43: return wrong_weapon;
    default: return nullptr;
    }
}
static void launch(char *, char *rec, u64, float *, void *, u32)
{
    memcpy(&last_ammo, rec + 0x38, sizeof last_ammo);
    memcpy(&last_weapon, rec + 0x40, sizeof last_weapon);
    memcpy(&last_desc3, rec + 0x60, sizeof last_desc3);
    ++launches;
}
static void capture(const void *data, int size)
{
    if (size == (int)sizeof(ShotAck) && ((const Head *)data)->type == PKT_SHOT_ACK)
        ack_sequence = ((const ShotAck *)data)->seq;
}
static void check(bool ok, const char *label)
{
    ++checks;
    if (!ok) { ++failed; printf("FAIL: %s\n", label); }
}
static void incoming(const ShotRec &record, u32 epoch = 1234)
{
    struct Packet { ShotHead head; ShotRec record; } packet = {};
    packet.head = { make_head(PKT_SHOT, 1), epoch, 202, 101 };
    packet.record = record;
    shots_on_packet((const unsigned char *)&packet, sizeof packet, capture);
}
static ShotRec table(unsigned char kind, const u32 *pairs, u32 count)
{
    ShotRec rec = {};
    rec.kind = kind;
    memcpy(rec.launch, &count, 4);
    memcpy(rec.launch + 4, &count, 4);
    memcpy(rec.launch + 8, pairs, (size_t)count * 8);
    return rec;
}

int main()
{
    alignas(8) unsigned char manager[0x1800] = {}, rows[4 * 0x10] = {};
    void *vtbl[58] = {};
    vtbl[57] = (void *)lookup;
    ObjMan *om = (ObjMan *)manager;
    om->vtbl = vtbl;
    Object local_object = {};
    strcpy_s(local_object.full, "i_human:test");
    Object *objects[] = { &local_object };
    *(Object ***)(manager + 0x150) = objects;
    *(u32 *)(manager + 0x15c) = 1;

    float numbers[] = { 1.25f, 2.5f };
    u32 code = 0x12345678;
    memcpy(correct_ammo, numbers, sizeof numbers); memcpy(wrong_ammo, numbers, sizeof numbers);
    memcpy(correct_ammo + 0xc4, &code, 4); memcpy(wrong_ammo + 0xc4, &code, 4);
    correct_ammo[0x20] = 0x55; wrong_ammo[0x20] = 0x66; correct_weapon[0] = 0x77;
    u32 local_a = 7, local_w = 8, other_a = 9, other_w = 10;
    void *p = correct_ammo;
    memcpy(rows, &local_a, 4); memcpy(rows + 8, &p, sizeof p);
    p = correct_weapon;
    memcpy(rows + 0x10, &local_w, 4); memcpy(rows + 0x18, &p, sizeof p);
    p = wrong_ammo;
    memcpy(rows + 0x20, &other_a, 4); memcpy(rows + 0x28, &p, sizeof p);
    p = wrong_weapon;
    memcpy(rows + 0x30, &other_w, 4); memcpy(rows + 0x38, &p, sizeof p);
    *(void **)(manager + 0x1748) = rows;
    *(u32 *)(manager + 0x1754) = 4;

    shots_reset();
    shots_test_engine(om, (char *)om, launch);
    shots_test_wait_time(1000);
    u32 local = 0;
    check(!xlate_desc_checked(41, &local), "the new peer has not supplied a mapping");
    ShotRec shot = {};
    shot.seq = 1; shot.kind = 0; shot.ammo = 41; shot.weapon = 42; shot.desc3 = 0xffffffffu;
    memcpy(shot.ammo_check, numbers, sizeof numbers); shot.ammo_code = code;
    incoming(shot);
    shots_sim_step(om);
    // In 1.0.7 unchecked xlate_desc() returned 41/42. The wrong local ammunition has
    // the same shallow check, so that version launched the wrong pointers and ACKed it.
    check(launches == 0 && ack_sequence == 0,
          "unmapped remote IDs do not fall back to wrong local descriptions or receive an ACK");

    const u32 descriptions[] = { 41, xlate_test_fingerprint(correct_ammo),
                                 42, xlate_test_fingerprint(correct_weapon) };
    const u32 objects_table[] = { 0, 123 };
    ShotRec desc = table(3, descriptions, 2), obj = table(4, objects_table, 1);
    desc.seq = 2; obj.seq = 3;
    incoming(desc); incoming(obj);
    shots_sim_step(om);
    check(xlate_desc_checked(41, &local) && local == 7, "real table maps remote ammunition to the correct local ID");
    check(xlate_desc_checked(42, &local) && local == 8, "real table maps remote weapon to the correct local ID");
    check(launches == 1 && last_ammo == correct_ammo && last_weapon == correct_weapon && !last_desc3 && ack_sequence == 3,
          "contiguous tables unblock the shot and the null third-description sentinel is preserved");
    incoming(shot); shots_sim_step(om);
    check(launches == 1 && ack_sequence == 3, "the unblocked shot is not replayed on a duplicate");

    shot.seq = 4; shot.desc3 = 43; // local 43 exists but the remote table did not match it
    incoming(shot); shots_sim_step(om);
    check(launches == 1 && ack_sequence == 3, "an unknown third description cannot use a coincident local ID");
    shots_test_wait_time(11000); shots_sim_step(om);
    check(launches == 1 && ack_sequence == 4, "a persistently unknown third description is refused after ten seconds");
    incoming(shot); shots_sim_step(om);
    check(launches == 1 && ack_sequence == 4, "a refused shot remains retired on retransmission");

    // A restarted event stream can reuse the same remote numeric IDs for different descriptions.
    // Until its new tables arrive the old peer's dictionary must not be used for a new shot.
    shots_test_wait_time(12000);
    ShotRec new_shot = shot; new_shot.seq = 1; new_shot.desc3 = 0xffffffffu;
    incoming(new_shot, 2222); shots_sim_step(om);
    check(launches == 1 && ack_sequence == 0 && !xlate_desc_checked(41, &local),
          "a new stream epoch discards old mappings before attempting replay");
    const u32 new_descriptions[] = { 41, xlate_test_fingerprint(wrong_ammo),
                                     42, xlate_test_fingerprint(wrong_weapon) };
    ShotRec new_desc = table(3, new_descriptions, 2), new_obj = table(4, objects_table, 1);
    new_desc.seq = 2; new_obj.seq = 3;
    incoming(new_desc, 2222); incoming(new_obj, 2222); shots_sim_step(om);
    check(launches == 2 && last_ammo == wrong_ammo && last_weapon == wrong_weapon && ack_sequence == 3,
          "fresh tables from the new stream select its own descriptions");
    incoming(shot, 1234); shots_sim_step(om);
    check(launches == 2 && xlate_desc_checked(41, &local) && local == 9,
          "a late retired stream cannot roll back the new mappings");

    // Characterize a different, still OPEN limitation of the actual matcher and replay path.
    // The synthetic remote descriptors and local decoys share the first 0x60 bytes, whereas
    // the fixture's true local equivalents differ in those bytes as process data can. The decoy
    // ammunition also passes the wire's two-float/code check. In the supplied game binary the
    // launch function uses ammunition +0xc0 and weapon +0x88, outside that fingerprint/check.
    // This fixture does not assert that these particular descriptions occur in a real battle.
    alignas(8) unsigned char remote_ammo[0xc8] = {}, remote_weapon[0xc8] = {};
    memset(correct_ammo, 0, sizeof correct_ammo); memset(wrong_ammo, 0, sizeof wrong_ammo);
    memset(correct_weapon, 0, sizeof correct_weapon); memset(wrong_weapon, 0, sizeof wrong_weapon);
    code = 0x54414548; // HEAT is one category used by the supplied launch routine, not a unique name.
    void *ammo_fixture[] = { remote_ammo, correct_ammo, wrong_ammo };
    for (void *ammo : ammo_fixture) {
        memcpy(ammo, numbers, sizeof numbers);
        memcpy((char *)ammo + 0xc4, &code, sizeof code);
    }
    correct_ammo[0x20] = 0x55;
    remote_ammo[0x20] = wrong_ammo[0x20] = 0x66;
    u32 different_flags = 0x80000000u;
    memcpy(wrong_ammo + 0xc0, &different_flags, sizeof different_flags);
    correct_weapon[0] = 0x77;
    remote_weapon[0] = wrong_weapon[0] = 0x4b;
    wrong_weapon[0x88] = 1;
    check(xlate_test_fingerprint(remote_ammo) == xlate_test_fingerprint(wrong_ammo) &&
          xlate_test_fingerprint(remote_ammo) != xlate_test_fingerprint(correct_ammo) &&
          xlate_test_fingerprint(remote_weapon) == xlate_test_fingerprint(wrong_weapon) &&
          xlate_test_fingerprint(remote_weapon) != xlate_test_fingerprint(correct_weapon),
          "fixture gives the wrong local descriptions unique matching prefixes");
    check(memcmp(remote_ammo, wrong_ammo, 8) == 0 &&
          memcmp(remote_ammo + 0xc4, wrong_ammo + 0xc4, 4) == 0 &&
          memcmp(remote_ammo + 0xc0, wrong_ammo + 0xc0, 4) != 0 &&
          remote_weapon[0x88] != wrong_weapon[0x88],
          "shallow ammunition check misses game-used fields outside the fingerprint");
    shots_reset();
    shots_test_engine(om, (char *)om, launch);
    shots_test_wait_time(13000);
    const u32 ambiguous_descriptions[] = { 41, xlate_test_fingerprint(remote_ammo),
                                           42, xlate_test_fingerprint(remote_weapon) };
    ShotRec ambiguous_desc = table(3, ambiguous_descriptions, 2), fresh_obj = table(4, objects_table, 1);
    ambiguous_desc.seq = 1; fresh_obj.seq = 2;
    ShotRec ambiguous_shot = {};
    ambiguous_shot.seq = 3; ambiguous_shot.kind = 0; ambiguous_shot.ammo = 41;
    ambiguous_shot.weapon = 42; ambiguous_shot.desc3 = 0xffffffffu;
    memcpy(ambiguous_shot.ammo_check, numbers, sizeof numbers); ambiguous_shot.ammo_code = code;
    unsigned before = launches;
    incoming(ambiguous_desc, 3333); incoming(fresh_obj, 3333); incoming(ambiguous_shot, 3333);
    shots_sim_step(om);
    check(xlate_desc_checked(41, &local) && local == 9 && launches == before + 1 &&
          last_ammo == wrong_ammo && last_weapon == wrong_weapon && ack_sequence == 3,
          "KNOWN LIMITATION: unique anchors can replay the fixture's wrong descriptions and ACK");

    printf("shots/xlate integration: %u checks, %u failed; mapping heuristics remain unproven\n", checks, failed);
    return failed ? 1 : 0;
}

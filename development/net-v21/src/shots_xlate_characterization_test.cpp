// Characterize the 1.0.7 shot/translation interaction using both real production modules.
// This intentionally demonstrates a KNOWN UNSAFE FALLBACK, not a successful safety fix.
// The fake engine is synthetic; no game or real Steam is involved.
#include "common.h"
#include "netlog.h"
#include "protocol.h"
#include <stdio.h>
#include <string.h>

using TestLaunch = void (*)(char *, char *, u64, float *, void *, u32);
void shots_test_engine(ObjMan *, char *, TestLaunch);
u32 xlate_test_fingerprint(const void *);

char g_out_dir[MAX_PATH] = ".";
char g_program[32] = "tacgame";
static unsigned char correct_ammo[0xc8], wrong_ammo[0xc8], correct_weapon[0xc8], wrong_weapon[0xc8];
static void *last_ammo, *last_weapon;
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
void trench_resend() {}
u32 vitals_epoch() { return 101; }
u32 vitals_peer_epoch() { return 202; }
bool hits_received(const ShotRec &, ObjMan *) { return true; }
bool anim_received(const ShotRec &, ObjMan *) { return true; }
void hits_reset_peer() {}
void anim_reset_peer() {}

static void *lookup(ObjMan *, u32 id)
{
    switch (id) {
    case 7: return correct_ammo;
    case 8: return correct_weapon;
    case 41: return wrong_ammo;
    case 42: return wrong_weapon;
    default: return nullptr;
    }
}
static void launch(char *, char *rec, u64, float *, void *, u32)
{
    memcpy(&last_ammo, rec + 0x38, sizeof last_ammo);
    memcpy(&last_weapon, rec + 0x40, sizeof last_weapon);
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
static void incoming(const ShotRec &record)
{
    struct Packet { ShotHead head; ShotRec record; } packet = {};
    packet.head = { make_head(PKT_SHOT, 1), 1234, 202, 101 };
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
    alignas(8) unsigned char manager[0x1800] = {}, rows[2 * 0x10] = {};
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
    u32 local_a = 7, local_w = 8;
    void *p = correct_ammo;
    memcpy(rows, &local_a, 4); memcpy(rows + 8, &p, sizeof p);
    p = correct_weapon;
    memcpy(rows + 0x10, &local_w, 4); memcpy(rows + 0x18, &p, sizeof p);
    *(void **)(manager + 0x1748) = rows;
    *(u32 *)(manager + 0x1754) = 2;

    shots_reset();
    shots_test_engine(om, (char *)om, launch);
    u32 local = 0;
    check(!xlate_desc_checked(41, &local), "the new peer has not supplied a mapping");
    ShotRec shot = {};
    shot.seq = 1; shot.kind = 0; shot.ammo = 41; shot.weapon = 42; shot.desc3 = 0xffffffffu;
    memcpy(shot.ammo_check, numbers, sizeof numbers); shot.ammo_code = code;
    incoming(shot);
    shots_sim_step(om);
    // The real unchecked xlate_desc() returns remote IDs 41/42. The wrong local ammunition has
    // the same two floats/type code, so its shallow check passes and the engine gets wrong pointers.
    check(launches == 1 && last_ammo == wrong_ammo && last_weapon == wrong_weapon && ack_sequence == 1,
          "KNOWN RISK reproduced: unknown IDs fall back and a wrong local description can be launched");

    const u32 descriptions[] = { 41, xlate_test_fingerprint(correct_ammo),
                                 42, xlate_test_fingerprint(correct_weapon) };
    const u32 objects_table[] = { 0, 123 };
    ShotRec desc = table(3, descriptions, 2), obj = table(4, objects_table, 1);
    xlate_received(desc, om); xlate_received(obj, om);
    check(xlate_desc_checked(41, &local) && local == 7, "real table maps remote ammunition to the correct local ID");
    check(xlate_desc_checked(42, &local) && local == 8, "real table maps remote weapon to the correct local ID");
    shot.seq = 2;
    incoming(shot);
    shots_sim_step(om);
    check(launches == 2 && last_ammo == correct_ammo && last_weapon == correct_weapon && ack_sequence == 2,
          "the same shot uses the correct pointers when the actual table is available");

    printf("shots/xlate characterization: %u checks, %u failed; unsafe fallback remains OPEN\n", checks, failed);
    return failed ? 1 : 0;
}

// Experimental name matching through the real shots producer/receiver, with a fake engine.
// This is not a game, Steam, or a proof that the registry layout is safe in a running process.
#include "common.h"
#include "netlog.h"
#include "protocol.h"
#include "named_shots.h"
#include <stdio.h>
#include <string.h>
#include <vector>

using TestLaunch = void (*)(char *, char *, u64, float *, void *, u32);
void shots_test_engine(ObjMan *, char *, TestLaunch);
void shots_test_name_registry(void *);
void shots_test_queue_launch(const char *);
void shots_test_wait_time(DWORD);

char g_out_dir[MAX_PATH] = ".";
char g_program[32] = "tacgame";
alignas(8) static unsigned char registry[0x148], shells[2 * 0x170], weapons[2 * 0x130];
alignas(8) static unsigned char shell_rows[2 * 0x44], weapon_rows[2 * 0x44];
static unsigned launches, ack, checks, failures;
static bool unregistered;
static void *last_ammo, *last_weapon;
static std::vector<unsigned char> packet;
void mp_log(const char *, ...) {}
bool readable(const void *p, size_t) { return p != nullptr; }
void netlog_count(const char *, long long) {}
bool session_connected() { return true; }
int session_role() { return ROLE_HOST; }
bool session_owns(Object *) { return true; }
bool session_unit_key(Object *, u64 *) { return false; }
Object *session_unit(u64) { return nullptr; }
u64 session_battle_ms() { return 0; }
void trench_received(const ShotRec &) {}
void trench_forget_peer() {}
void trench_resend() {}
void xlate_received(const ShotRec &, ObjMan *) {}
void xlate_resend() {}
u32 xlate_desc(u32 n) { return n; }
bool xlate_desc_checked(u32 n, u32 *out) {
    if (n == 0xffffffffu) { *out = n; return true; }
    if (n == 41) { *out = 7; return true; } // intentionally wrong legacy mapping
    if (n == 42) { *out = 8; return true; }
    return false;
}
u32 vitals_epoch() { return 101; }
u32 vitals_peer_epoch() { return 202; }
bool hits_received(const ShotRec &, ObjMan *) { return true; }
bool anim_received(const ShotRec &, ObjMan *) { return true; }
void hits_reset_peer() {}
void anim_reset_wait() {}
void anim_reset_peer() {}
static void *lookup(ObjMan *, u32 id) {
    return id == 7 ? shells + 0x170 : id == 8 ? weapons + 0x130 :
           id == 41 ? shells : id == 42 ? weapons : nullptr;
}
static u32 id_for(ObjMan *, void *p) { return unregistered ? 0xffffffffu : p == shells ? 41 : p == weapons ? 42 : 0xffffffffu; }
static void launch(char *, char *rec, u64, float *, void *, u32) {
    memcpy(&last_ammo, rec + 0x38, sizeof last_ammo);
    memcpy(&last_weapon, rec + 0x40, sizeof last_weapon);
    ++launches;
}
static void capture(const void *data, int size) {
    if (size < (int)sizeof(Head)) return;
    const Head *h = (const Head *)data;
    if (h->type == PKT_SHOT_ACK && size == (int)sizeof(ShotAck)) ack = ((const ShotAck *)data)->seq;
    else if (h->type == PKT_SHOT) {
        const unsigned char *p = (const unsigned char *)data;
        packet.assign(p, p + size);
    }
}
static void check(bool ok, const char *what) { ++checks; if (!ok) { ++failures; printf("FAIL: %s\n", what); } }
static void ptr(size_t off, const void *p) { memcpy(registry + off, &p, sizeof p); }
static void count(size_t off, u32 n) { memcpy(registry + off, &n, 4); }
static void row(unsigned char *rows, unsigned at, const char *name, u32 index) {
    unsigned char *r = rows + at * 0x44; memset(r, 0, 0x44);
    uint16_t n = (uint16_t)strlen(name); memcpy(r, &n, 2); memcpy(r + 2, name, n); memcpy(r + 0x40, &index, 4);
}
static void receive(std::vector<unsigned char> data, ObjMan *om) {
    ShotHead h; memcpy(&h, data.data(), sizeof h); h.owner = 202; h.target = 101;
    memcpy(data.data(), &h, sizeof h);
    shots_on_packet(data.data(), (int)data.size(), capture); shots_sim_step(om);
}

int main() {
    void *vt[58] = {}; vt[56] = (void *)id_for; vt[57] = (void *)lookup;
    ObjMan om = { vt };
    ptr(0xf0, weapons); count(0xfc, 2); ptr(0x120, weapon_rows); count(0x12c, 2);
    ptr(0x108, shells); count(0x114, 2); ptr(0x138, shell_rows); count(0x144, 2);
    row(shell_rows, 0, "ammo", 0); row(shell_rows, 1, "otherammo", 1);
    row(weapon_rows, 0, "gun", 0); row(weapon_rows, 1, "othergun", 1);
    float numbers[2] = { 1.25f, 2.5f }; u32 code = 0x54414548, flags = 0x400;
    memcpy(shells, numbers, 8); memcpy(shells + 0x170, numbers, 8);
    memcpy(shells + 0xc4, &code, 4); memcpy(shells + 0x170 + 0xc4, &code, 4);
    memcpy(shells + 0xc0, &flags, 4);
    weapons[0x88] = 3; u32 scalar = 0x3f800000; memcpy(weapons + 0x90, &scalar, 4);
    named_shots::Name a = {}, w = {};
    check(named_shots::name_for(registry, named_shots::AMMO, shells, &a) && a.size == 4,
          "name_for finds an exact shell array element");
    check(named_shots::name_for(registry, named_shots::WEAPON, weapons, &w) && w.size == 3,
          "name_for finds an exact weapon array element");
    check(!named_shots::name_for(registry, named_shots::AMMO, shells + 1, &a), "interior pointers rejected");
    check(named_shots::pointer_for(registry, named_shots::AMMO, a) == shells, "exact shell name resolves");
    row(shell_rows, 1, "ammo", 1);
    check(!named_shots::name_for(registry, named_shots::AMMO, shells, &a) &&
          !named_shots::pointer_for(registry, named_shots::AMMO, a), "duplicate name refused");
    row(shell_rows, 1, "otherammo", 1);
    row(shell_rows, 1, "alias", 0);
    check(!named_shots::name_for(registry, named_shots::AMMO, shells, &a), "multiple names for one pointer refused");
    row(shell_rows, 1, "otherammo", 1);
    char long_name[62]; memset(long_name, 'x', 61); long_name[61] = 0;
    row(shell_rows, 0, long_name, 0);
    check(!named_shots::name_for(registry, named_shots::AMMO, shells, &a), "61-byte potentially truncated name refused");
    row(shell_rows, 0, "ammo", 0);
    check(named_shots::name_for(registry, named_shots::AMMO, shells, &a), "normal shell name restored");

    char rec[0x90] = {}; void *p = shells; memcpy(rec + 0x38, &p, 8); p = weapons; memcpy(rec + 0x40, &p, 8);
    shots_reset(); shots_test_engine(&om, (char *)&om, launch); shots_test_name_registry(registry);
    shots_test_wait_time(1000); shots_test_queue_launch(rec); shots_frame(capture);
    check(packet.size() == sizeof(ShotHead) + sizeof(ShotRec), "producer emits one normal-sized shot packet");
    ShotRec wire = {}; memcpy(&wire, packet.data() + sizeof(ShotHead), sizeof wire);
    named_shots::Proof proof = {};
    check(wire.pad == named_shots::k_marker && named_shots::decode(wire, &proof), "producer emits a valid exact-name proof");
    check(named_shots::fields_match(proof, shells, weapons), "proof preserves selected game-used fields");
    std::vector<unsigned char> named_packet = packet;
    shots_reset(); ack = 99; receive(named_packet, &om);
    check(launches == 1 && ack == 1 && last_ammo == shells && last_weapon == weapons,
          "actual receiver uses names rather than the deliberately wrong numeric mapping");
    receive(named_packet, &om);
    check(launches == 1 && ack == 1, "named retransmission is not replayed twice");

    shots_reset(); ack = 99; shells[0xc0] ^= 1; receive(named_packet, &om);
    check(launches == 1 && ack == 1, "different named ammo flags are explicitly refused");
    shells[0xc0] ^= 1;
    shots_reset(); ack = 99; weapons[0x88] ^= 1; receive(named_packet, &om);
    check(launches == 1 && ack == 1, "different named weapon mode is explicitly refused");
    weapons[0x88] ^= 1;
    shots_reset(); ack = 99; weapons[0x90] ^= 1; receive(named_packet, &om);
    check(launches == 1 && ack == 1, "different named weapon scalar is explicitly refused");
    weapons[0x90] ^= 1;
    std::vector<unsigned char> corrupt = named_packet; corrupt[sizeof(ShotHead) + offsetof(ShotRec, launch) + 0x38] ^= 1;
    shots_reset(); ack = 99; receive(corrupt, &om);
    check(launches == 1 && ack == 1, "malformed name proof is refused without legacy fallback");
    shots_reset(); ack = 99; shots_test_wait_time(1000); row(shell_rows, 0, "missing", 0);
    receive(named_packet, &om);
    check(launches == 1 && ack == 0, "missing name waits without replay");
    shots_test_wait_time(11000); shots_sim_step(&om);
    check(launches == 1 && ack == 1, "missing name is explicitly refused at deadline");
    row(shell_rows, 0, "ammo", 0);
    shots_reset(); ack = 99; shots_test_wait_time(1000); unregistered = true;
    receive(named_packet, &om);
    check(launches == 1 && ack == 0, "a named pointer not registered in the local object manager waits");
    shots_test_wait_time(11000); shots_sim_step(&om);
    check(launches == 1 && ack == 1, "an unregistered named pointer is explicitly refused at the deadline");
    unregistered = false;
    // If both exact names do not fit, the producer deliberately sends a normal old-style shot;
    // this is a compatibility fallback, not proof that its numeric mapping is correct.
    row(shell_rows, 0, "twenty_four_characters___", 0);
    shots_reset(); packet.clear(); shots_test_engine(&om, (char *)&om, launch); shots_test_queue_launch(rec); shots_frame(capture);
    ShotRec fallback = {}; memcpy(&fallback, packet.data() + sizeof(ShotHead), sizeof fallback);
    check(fallback.pad == 0, "names too long for the reserved bytes use the unchanged legacy format");
    shots_reset(); ack = 99; receive(packet, &om);
    check(launches == 2 && last_ammo == shells + 0x170 && last_weapon == weapons + 0x130,
          "KNOWN LIMITATION: long-name fallback still uses the deliberately wrong legacy mapping");

    // The exact payload boundary is 25 bytes across both complete names.
    row(shell_rows, 0, "1234567890123456789012", 0); // 22 + 3 = 25
    shots_reset(); packet.clear(); shots_test_queue_launch(rec); shots_frame(capture);
    ShotRec boundary = {}; memcpy(&boundary, packet.data() + sizeof(ShotHead), sizeof boundary);
    check(boundary.pad == named_shots::k_marker && named_shots::decode(boundary, &proof),
          "25 bytes of complete names fit the reserved payload");
    row(shell_rows, 0, "12345678901234567890123", 0); // 23 + 3 = 26
    shots_reset(); packet.clear(); shots_test_queue_launch(rec); shots_frame(capture);
    memcpy(&boundary, packet.data() + sizeof(ShotHead), sizeof boundary);
    check(boundary.pad == 0, "26 bytes of names fall back rather than truncating");

    row(shell_rows, 0, "ammo", 0);
    corrupt = named_packet;
    corrupt[sizeof(ShotHead) + offsetof(ShotRec, launch) + 0x67] = 1; // wire[39], in the second pointer span
    shots_reset(); ack = 99; receive(corrupt, &om);
    check(launches == 2 && ack == 1, "nonzero unused proof bytes are refused");
    shots_reset(); ack = 99; shots_test_wait_time(1000); row(shell_rows, 0, "missing", 0);
    receive(named_packet, &om);
    check(launches == 2 && ack == 0, "temporarily missing name waits");
    row(shell_rows, 0, "ammo", 0); shots_test_wait_time(10999); shots_sim_step(&om);
    check(launches == 3 && ack == 1 && last_ammo == shells,
          "name arriving before the deadline replays exactly once");
    receive(named_packet, &om);
    check(launches == 3 && ack == 1, "retransmission after delayed resolution is not replayed");
    printf("named shots: %u checks, %u failures (synthetic only)\n", checks, failures);
    return failures ? 1 : 0;
}

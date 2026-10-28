// No-game, two-process compatibility fixture. Built once with the pinned 1.0.7 shots/xlate
// sources and once with the current sources. A file carries an actual packet and ACK; it is
// not a Steam/UDP/game test. The legacy unknown-ID case intentionally demonstrates a risk.
#include "common.h"
#include "netlog.h"
#include "protocol.h"
#include <chrono>
#include <stdio.h>
#include <string>
#include <string.h>
#include <thread>
#include <vector>

using TestLaunch = void (*)(char *, char *, u64, float *, void *, u32);
void shots_test_engine(ObjMan *, char *, TestLaunch);
size_t shots_test_outstanding();
#ifndef COMPAT_LEGACY
void shots_test_wait_time(DWORD);
#endif

char g_out_dir[MAX_PATH] = ".";
char g_program[32] = "tacgame";
static bool sender;
static unsigned launches;
static void *last_ammo, *last_weapon;
static unsigned char correct_ammo[0xc8], wrong_ammo[0xc8], correct_weapon[0xc8], wrong_weapon[0xc8];
static ShotAck last_ack;
static bool saw_ack;
static std::vector<unsigned char> last_packet;

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
u32 vitals_epoch() { return sender ? 101 : 202; }
u32 vitals_peer_epoch() { return sender ? 202 : 101; }
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
    if (size < (int)sizeof(Head)) return;
    const Head *head = (const Head *)data;
    if (head->type == PKT_SHOT) {
        const unsigned char *first = (const unsigned char *)data;
        last_packet.assign(first, first + size);
    } else if (head->type == PKT_SHOT_ACK && size == (int)sizeof(ShotAck)) {
        memcpy(&last_ack, data, sizeof last_ack);
        saw_ack = true;
    }
}

static FILE *open_file(const char *path, const char *mode)
{
#ifdef _MSC_VER
    FILE *f = nullptr;
    return fopen_s(&f, path, mode) ? nullptr : f;
#else
    return fopen(path, mode);
#endif
}
static bool write_atomic(const char *path, const void *data, size_t size)
{
    std::string tmp = std::string(path) + ".tmp";
    FILE *f = open_file(tmp.c_str(), "wb");
    if (!f) return false;
    bool ok = fwrite(data, 1, size, f) == size;
    if (fclose(f)) ok = false;
    return ok && rename(tmp.c_str(), path) == 0;
}
static bool read_file(const char *path, std::vector<unsigned char> &out)
{
    FILE *f = open_file(path, "rb");
    if (!f) return false;
    if (fseek(f, 0, SEEK_END)) { fclose(f); return false; }
    long size = ftell(f);
    if (size < 0 || size > 2048 || fseek(f, 0, SEEK_SET)) { fclose(f); return false; }
    out.resize((size_t)size);
    bool ok = fread(out.data(), 1, out.size(), f) == out.size();
    fclose(f);
    return ok;
}
static bool require(bool ok, const char *what)
{
    if (!ok) fprintf(stderr, "compatibility fixture failed: %s\n", what);
    return ok;
}
// These fixtures contain no pointer-like words, so the real xlate fingerprint is simply
// FNV over the 0x60 bytes. Both the 1.0.7 and current production matcher calculate it.
static u32 fingerprint(const unsigned char *data)
{
    u32 h = 2166136261u;
    for (size_t i = 0; i < 0x60; ++i) h = (h ^ data[i]) * 16777619u;
    return h ? h : 1;
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

int main(int argc, char **argv)
{
    if (argc != 5 || (strcmp(argv[1], "send") && strcmp(argv[1], "receive")) ||
        (strcmp(argv[4], "mapped") && strcmp(argv[4], "unknown"))) return 2;
    sender = !strcmp(argv[1], "send");
    bool mapped = !strcmp(argv[4], "mapped");

    alignas(8) unsigned char manager[0x1800] = {}, rows[2 * 0x10] = {};
    void *vtbl[58] = {}; vtbl[57] = (void *)lookup;
    ObjMan *om = (ObjMan *)manager; om->vtbl = vtbl;
    Object local_object = {};
    memcpy(local_object.full, "i_human:test", sizeof "i_human:test");
    Object *objects[] = { &local_object };
    *(Object ***)(manager + 0x150) = objects; *(u32 *)(manager + 0x15c) = 1;
    correct_ammo[0x20] = 0x55; wrong_ammo[0x20] = 0x66; correct_weapon[0] = 0x77;
    u32 code = 0x12345678;
    memcpy(correct_ammo + 0xc4, &code, 4); memcpy(wrong_ammo + 0xc4, &code, 4);
    u32 local_a = 7, local_w = 8;
    void *p = correct_ammo; memcpy(rows, &local_a, 4); memcpy(rows + 8, &p, sizeof p);
    p = correct_weapon; memcpy(rows + 0x10, &local_w, 4); memcpy(rows + 0x18, &p, sizeof p);
    *(void **)(manager + 0x1748) = rows; *(u32 *)(manager + 0x1754) = 2;
    shots_reset(); shots_test_engine(om, (char *)om, launch);

    if (sender) {
        if (mapped) {
            const u32 descriptions[] = { 41, fingerprint(correct_ammo), 42, fingerprint(correct_weapon) };
            const u32 objects_table[] = { 0, 123 };
            ShotRec desc = table(3, descriptions, 2), obj = table(4, objects_table, 1);
            shots_queue(desc); shots_queue(obj);
        }
        ShotRec shot = {};
        shot.kind = 0; shot.ammo = 41; shot.weapon = 42; shot.desc3 = 0xffffffffu; shot.ammo_code = code;
        shots_queue(shot); shots_frame(capture);
        if (!require(!last_packet.empty() && write_atomic(argv[2], last_packet.data(), last_packet.size()), "write real sender packet")) return 1;
        std::vector<unsigned char> ack;
        for (unsigned i = 0; i < 500 && !read_file(argv[3], ack); ++i)
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        if (!require(ack.size() == sizeof(ShotAck), "receive ACK from the other process")) return 1;
        shots_on_packet(ack.data(), (int)ack.size(), capture);
        return require(shots_test_outstanding() == 0, "real sender accepts the compatible cumulative ACK") ? 0 : 1;
    }

    std::vector<unsigned char> packet;
    if (!require(read_file(argv[2], packet) && packet.size() >= sizeof(ShotHead), "read actual sender packet")) return 1;
    shots_on_packet(packet.data(), (int)packet.size(), capture);
    if (!require(saw_ack && last_ack.seq == 0, "receipt alone is not an application ACK")) return 1;
#ifndef COMPAT_LEGACY
    shots_test_wait_time(1000);
#endif
    shots_sim_step(om);
    if (mapped) {
        if (!require(launches == 1 && last_ammo == correct_ammo && last_weapon == correct_weapon && last_ack.seq == 3,
                     "contiguous legacy/current tables and shot replay with the expected synthetic pointers")) return 1;
    } else {
#ifdef COMPAT_LEGACY
        if (!require(launches == 1 && last_ammo == wrong_ammo && last_weapon == wrong_weapon && last_ack.seq == 1,
                     "legacy receiver still reproduces the known unsafe fallback")) return 1;
#else
        if (!require(launches == 0 && last_ack.seq == 0, "current receiver holds unknown IDs without replay")) return 1;
        shots_test_wait_time(11000); shots_sim_step(om);
        if (!require(launches == 0 && last_ack.seq == 1, "current receiver explicitly refuses the timed-out event")) return 1;
#endif
    }
    shots_on_packet(packet.data(), (int)packet.size(), capture); shots_sim_step(om);
#ifdef COMPAT_LEGACY
    const unsigned expected_launches = 1;
#else
    const unsigned expected_launches = mapped ? 1u : 0u;
#endif
    if (!require(launches == expected_launches, "duplicate packet does not replay the retired event")) return 1;
    if (!require(write_atomic(argv[3], &last_ack, sizeof last_ack), "write actual receiver ACK")) return 1;
    printf("%s receiver: %s packet processed; no game/Steam involved\n",
#ifdef COMPAT_LEGACY
           "1.0.7",
#else
           "current",
#endif
           mapped ? "mapped" : "unknown-ID");
    return 0;
}

// native test of the logic in mp_vitals.c (no Windows needed): fake soldiers, a fake squad, a fake network
#define MP_TEST
#define __declspec(x)
#define DllMain DllMain_unused
#define memcpy mp_memcpy
#define memmove mp_memmove
#define memset mp_memset
#define memcmp mp_memcmp
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include "../mp_vitals.c"
#include <assert.h>
DWORD fake_tick = 100000;
uintptr_t fake_vq_unmapped_from = UINTPTR_MAX;
const char *fake_env_idle, *fake_env_state, *fake_cmd;

#define NS 24
static char man[NS][0x2500];
static char squad[0x2000];
static char members[NS][M_SIZE];
static void *fake_vt[0x200];
#define NV 3
static char veh[NV][0x1000];
static void *objs[NS + NV];
static char om_buf[0x200];
static u8 conn = 1, tact = 1, have = 1;
static volatile u8 num_ready, num_flag; static u32 hmode = 2;
static u8 peer[16] = { 2, 0, 0x07, 0xa7, 1, 2, 3, 4 };
static unsigned char sent_pk[512][1200]; static int sent_len[512], sent_n;
static int auto_ack = 1;
static void fake_send(const void *d, u32 n)
{
    assert(sent_n < (int)(sizeof sent_pk / sizeof sent_pk[0]));
    memcpy(sent_pk[sent_n], d, n); sent_len[sent_n++] = (int)n;
    // The ordinary state tests model the healthy peer: it acknowledges a v7.1 data packet after accepting it.
    // The dedicated protocol test disables this to inspect retransmission and mailbox-full behaviour.
    if (auto_ack && n >= sizeof(VitalsHead)) {
        const VitalsHead *h = (const VitalsHead *)d;
        if (h->h.magic == MAGIC && h->h.type == PKT_VITALS) {
            VitalsHead ack = { { MAGIC, PKT_VITALS_ACK, 0, 0 }, h->epoch, h->seq };
            hook_packet(peer, (const u8 *)&ack, sizeof ack, fake_tick);
        }
    }
}
static void fake_log(const char *f, ...) { va_list a; va_start(a, f); printf("   LOG: "); vprintf(f, a); printf("\n"); va_end(a); }
static int stat_calls;
static void fake_stat(void *o, long long b, char c) { (void)o;(void)b;(void)c; stat_calls++; }
static int orig_pk_calls;
static void fake_orig_packet(const void *a, const unsigned char *b, int c, u32 d) { (void)a;(void)b;(void)c;(void)d; orig_pk_calls++; }
static char *fake_get_member(void *sq, u32 idx) { (void)sq; return idx < NS ? members[idx] : 0; }
static void (*during)(void);
static void fake_orig_frame(void *om, void *a2, u64 a3, void *a4) { (void)om;(void)a2;(void)a3;(void)a4; if (during) during(); }
static int fake_triangle_calls;
static int fake_render_triangle(void *query) { (void)query; fake_triangle_calls++; return 1; }

static float *selfhp(int i) { return (float *)(man[i] + H_SELF); }
static u32 *st(int i) { return (u32 *)(members[i] + M_STATE); }
static char *sq_of(int i) { return *(char **)(man[i] + H_SQUAD); }

static void setup(void)
{
    conn = tact = have = 1; hmode = 2;
    memset(man, 0, sizeof man); memset(members, 0, sizeof members); memset(squad, 0, sizeof squad);
    fake_vt[0x138 / 8] = (void *)fake_get_member;
    *(void ***)squad = fake_vt;
    for (int i = 0; i < NS; i++) {
        char *h = man[i];
        *(const char **)(h + 0x48) = "i_human";
        sprintf(h + 8, "human_%08x", 0x100 + i);
        *(char **)(h + H_SQUAD) = squad; *(char **)(h + H_SQUAD_COPY) = squad; *(int *)(h + H_INDEX) = i;
        *(char **)(members[i] + M_OWNER) = h;
        *selfhp(i) = 1.0f;
        objs[i] = h;
    }
    memset(veh, 0, sizeof veh);
    for (int i = 0; i < NV; i++) { *(const char **)(veh[i] + 0x48) = "i_techn"; sprintf(veh[i] + 8, "techn_%08x", 0x900 + i); objs[NS + i] = veh[i]; }
    *(u32 *)(om_buf + 0xfc) = NS + NV; *(void ***)(om_buf + 0xf0) = objs;
    g_connected = &conn; g_tactical = &tact; g_have_peer = &have; g_host_mode = &hmode; g_peer = peer;
    num_ready = 1; num_flag = 0;
    g_num_ready = &num_ready; g_num_flag = &num_flag;
    g_orig_frame = fake_orig_frame; during = 0; g_on = 0; g_broken = 0; g_send = fake_send; g_log = fake_log; g_orig_packet = fake_orig_packet;
    g_num_complete = g_num_peer_set = 0; g_idle_msg = -1; fake_env_idle = fake_env_state = fake_cmd = 0; g_operation_state_policy = 0;
    g_phys = 0; g_phys_size = 0; memset(g_why, 0, sizeof g_why);
    vitals_transport_reset(); vitals_transport_begin(fake_tick);
    g_s_vtx = g_s_vack = g_s_vretry = g_s_vreplace = g_s_vrx = g_s_vdup = g_s_vfull = g_s_vack_sent = g_s_vack_unknown = 0;
    g_box_n = g_box_head = 0; sent_n = 0; auto_ack = 1; fake_tick += 10000;
}
static void step(int ms) { fake_tick += (DWORD)ms; hook_frame(om_buf, 0, 0, 0); }
static u32 peer_seq;
static void from_peer(const Entry *e, int n, int delta)
{
    u8 pk[sizeof(VitalsHead) + 16 * 10]; VitalsHead h = { { MAGIC, PKT_VITALS, (u8)delta, (u16)n }, 0x7a11ce01u, ++peer_seq };
    memcpy(pk, &h, sizeof h); memcpy(pk + sizeof h, e, (size_t)n * sizeof(Entry));
    hook_packet(peer, pk, (int)(sizeof h + (size_t)n * sizeof(Entry)), fake_tick);
}
static int hurt_entries_sent(u32 id, float *self, float *rec)
{
    int c = 0;
    for (int p = 0; p < sent_n; p++) {
        VitalsHead *h = (VitalsHead *)sent_pk[p];
        if (sent_len[p] < (int)sizeof *h || h->h.magic != MAGIC || h->h.type != PKT_VITALS) continue;
        Entry *e = (Entry *)(sent_pk[p] + sizeof(VitalsHead));
        for (int i = 0; i < h->h.count; i++) if (e[i].id == id && (e[i].flags & E_HURT) && !(e[i].flags & E_VEH)) { c++; memcpy(self, &e[i].a, 4); memcpy(rec, &e[i].b, 4); }
    }
    return c;
}
static int veh_entries_sent(u32 id)
{
    int c = 0;
    for (int p = 0; p < sent_n; p++) {
        VitalsHead *h = (VitalsHead *)sent_pk[p];
        if (sent_len[p] < (int)sizeof *h || h->h.magic != MAGIC || h->h.type != PKT_VITALS) continue;
        Entry *e = (Entry *)(sent_pk[p] + sizeof(VitalsHead));
        for (int i = 0; i < h->h.count; i++) if ((e[i].flags & E_VEH) && e[i].id == id) c++;
    }
    return c;
}
static u32 fnv(const char *p) { u32 h = 2166136261u; for (; *p; p++) h = (h ^ (u8)*p) * 16777619u; return h; }
static Entry wounded(u32 id, float self, float rec) { Entry e = { id, E_HURT | E_REC, 0, 0 }; memcpy(&e.a, &self, 4); memcpy(&e.b, &rec, 4); return e; }
static float *rech(int i) { return (float *)(members[i] + M_HEALTH); }
static int dead_entries_sent(u32 id)
{
    int c = 0;
    for (int p = 0; p < sent_n; p++) {
        VitalsHead *h = (VitalsHead *)sent_pk[p];
        if (sent_len[p] < (int)sizeof *h || h->h.magic != MAGIC || h->h.type != PKT_VITALS) continue;
        Entry *e = (Entry *)(sent_pk[p] + sizeof(VitalsHead));
        for (int i = 0; i < h->h.count; i++) if (e[i].id == id && (e[i].flags & E_DEAD)) c++;
    }
    return c;
}
#define RUN(ms) do { for (int _t = 0; _t < (ms); _t += 60) step(60); } while (0)

int main(void)
{
    setvbuf(stdout, 0, _IONBF, 0);
    // ---- 0. Operation tactical launches default to the native position-only policy.  The recogniser
    // requires both the tactical program and the game's YYYY_MM_campaign:battle token; it cannot affect
    // opgame, a custom mission, or an explicit MP_SYNC_STATE user choice.
    assert(operation_tactical_command("starter_x64.exe root\\programs\\tacgame.progpack, data\\common\\shared\\config\\tacgame_image.cfgpack, users\\profiles, Zefir, 1942_07_Voronezh:04_06_07_shilovo_na_ger, 2, 13"));
    assert(operation_tactical_command("starter_x64.exe ROOT\\PROGRAMS\\TACGAME.PROGPACK, users\\profiles, Zefir, 1942_07_Voronezh:04_06_07_shilovo_na_ger, 2, 13"));
    assert(!operation_tactical_command("starter_x64.exe root\\programs\\opgame.progpack, Zefir, 1942_07_Voronezh:04_06_07_shilovo_na_ger, 2, 13"));
    assert(!operation_tactical_command("starter_x64.exe root\\programs\\notacgame.progpack, Zefir, 1942_07_Voronezh:04_06_07_shilovo_na_ger, 2, 13"));
    assert(!operation_tactical_command("starter_x64.exe root\\programs\\tacgame.progpack, data\\mission.cfgpack, users\\profiles, Zefir, custom_battle, 2, 13"));
    assert(!operation_tactical_command("starter_x64.exe root\\programs\\tacgame.progpack, "));
    fake_cmd = "starter_x64.exe root\\programs\\tacgame.progpack, data\\common\\shared\\config\\tacgame_image.cfgpack, users\\profiles, Zefir, 1942_07_Voronezh:04_06_07_shilovo_na_ger, 2, 13";
    fake_env_state = 0; assert(default_operation_state_policy() == 1 && str_eq(fake_env_state, "0"));
    fake_env_state = "2"; assert(default_operation_state_policy() == 0 && str_eq(fake_env_state, "2"));
    fake_cmd = "starter_x64.exe root\\programs\\tacgame.progpack, data\\mission.cfgpack, users\\profiles, Zefir, custom_battle, 2, 13";
    fake_env_state = 0; assert(default_operation_state_policy() == 0 && !fake_env_state);

    // ---- 1. a soldier dies here: told at once, once; listed again by the digest
    setup(); RUN(300);
    assert(sent_n == 0);
    *selfhp(1) = 0.2f;                                    // the hit code's result: health below 0.5
    RUN(120);
    printf("death here: %d message(s) for him\n", dead_entries_sent(0x101));
    assert(dead_entries_sent(0x101) == 1 && dead_entries_sent(0x100) == 0);
    sent_n = 0; RUN(2200);
    assert(dead_entries_sent(0x101) >= 1);               // the digest

    // ---- 2. the other game says a soldier is dead: he is killed here, the engine's way, nothing is sent back
    setup(); RUN(2000);
    Entry e2 = { 0x102, E_DEAD | E_HURT, 0, 0 }; { float z = 0.2f; memcpy(&e2.a, &z, 4); }
    from_peer(&e2, 1, 1); RUN(200);
    printf("killed: self=%.1f squad=%p copy=%p rec state=%u\n", *selfhp(2), (void *)sq_of(2), (void *)*(char **)(man[2] + H_SQUAD_COPY), *st(2));
    assert(*selfhp(2) == 0.2f && !sq_of(2) && *st(2) == STATE_KILLED && !*(char **)(man[2] + H_SQUAD_COPY));
    assert(*selfhp(3) == 1.0f && sq_of(3));               // the others are left alone
    assert(dead_entries_sent(0x102) == 0);                // no echo
    RUN(2500); assert(dead_entries_sent(0x102) == 0);     // not in the digest either (the other game has him)

    // ---- 3. nobody is brought back: not by silence, not by an alive entry
    Entry alive = { 0x102, 0, 0, 0 };
    from_peer(&alive, 1, 0); RUN(600);
    assert(*selfhp(2) == 0.2f && !sq_of(2));

    // ---- 4. dead on both sides: nothing to do; the first 1.5 s: nothing yet
    setup(); *selfhp(3) = 0.f; RUN(300);
    Entry e3 = { 0x103, E_DEAD, 0, 0 }; from_peer(&e3, 1, 1); RUN(200);
    assert(*selfhp(3) == 0.f && g_s_killed == 0);
    setup(); RUN(60); Entry e4 = { 0x104, E_DEAD, 0, 0 }; from_peer(&e4, 1, 1); RUN(300);
    assert(*selfhp(4) == 1.0f);                           // too early: the soldiers are still being set up
    RUN(1500); assert(*selfhp(4) == 0.f);

    // ---- 5. mode 1: only noticed
    setup(); hmode = 1; RUN(2000);                        // (default mode 1 outside operation battles)
    Entry e5 = { 0x105, E_DEAD, 0, 0 }; from_peer(&e5, 1, 1); RUN(300);
    assert(*selfhp(5) == 1.0f && g_s_notice >= 1);
    hmode = 2;

    // ---- 6. a soldier whose squad record cannot be found is killed all the same
    setup(); *(char **)(man[0] + H_SQUAD) = squad; *(int *)(man[0] + H_INDEX) = 99; RUN(2000);
    Entry e6 = { 0x100, E_DEAD, 0, 0 }; from_peer(&e6, 1, 1); RUN(200);
    assert(*selfhp(0) == 0.f && !sq_of(0) && g_why[6] >= 1);

    // ---- 7a. a soldier is wounded here: told once (not again and again), also the record's health; the digest lists him
    setup(); for (int i = 0; i < NS; i++) *rech(i) = 2.0f;
    RUN(1000); assert(sent_n == 0);
    *selfhp(4) = 0.8f; *rech(4) = 1.5f; RUN(120);
    { float a = 0, b = 0; assert(hurt_entries_sent(0x104, &a, &b) == 1); printf("wounded here: self %.2f rec %.2f\n", a, b); assert(a > 0.79f && a < 0.81f); }
    sent_n = 0; RUN(300); { float a, b; assert(hurt_entries_sent(0x104, &a, &b) == 0); }
    RUN(2200); { float a, b; assert(hurt_entries_sent(0x104, &a, &b) >= 1); }
    *selfhp(4) = 0.7f; sent_n = 0; RUN(120); { float a = 0, b = 0; assert(hurt_entries_sent(0x104, &a, &b) == 1 && a < 0.71f); }

    // ---- 7b. the other game has a soldier more wounded: the lower value wins; never a higher one; no echo
    setup(); for (int i = 0; i < NS; i++) *rech(i) = 2.0f;
    RUN(2000); *selfhp(6) = 0.9f;  RUN(120); sent_n = 0;
    Entry w = wounded(0x106, 0.6f, 1.2f); from_peer(&w, 1, 1); RUN(200);
    printf("merged: self %.2f rec %.2f\n", *selfhp(6), *rech(6));
    assert(*selfhp(6) == 0.6f && *rech(6) == 2.0f && sq_of(6));   // v5.1: the record's health is left to the engine
    { float a, b; assert(hurt_entries_sent(0x106, &a, &b) == 0); }   // no echo of what came from the other game
    Entry w2 = wounded(0x106, 0.95f, 1.9f); from_peer(&w2, 1, 1); RUN(200);
    assert(*selfhp(6) == 0.6f && *rech(6) == 2.0f);                 // a higher value from the other game changes nothing
    *selfhp(7) = 0.3f; Entry w3 = wounded(0x107, 0.4f, 1.0f); from_peer(&w3, 1, 1); RUN(300);   // 0.3 here: dead already
    assert(*selfhp(7) == 0.3f);
    Entry w4 = wounded(0x108, 0.3f, 0.5f); from_peer(&w4, 1, 1); RUN(300);                       // below 0.5 over there: killed
    assert(*selfhp(8) == 0.3f && !sq_of(8) && *st(8) == STATE_KILLED);   // killed with the other game's health, not 0
    // mode 1: only compared
    setup(); hmode = 1; RUN(2000); Entry w5 = wounded(0x109, 0.6f, 1.2f); from_peer(&w5, 1, 1); RUN(300);
    assert(*selfhp(9) == 1.0f); hmode = 2;

    // ---- 7c. vehicles: a status flag that comes on in this game is told once; one that comes on in the other game is switched on here once
    setup(); RUN(3600); sent_n = 0;
    veh[1][0xd08] = 1; RUN(300);
    assert(veh_entries_sent(fnv("techn_00000901")) == 1 && veh_entries_sent(fnv("techn_00000900")) == 0);
    sent_n = 0; RUN(300); assert(veh_entries_sent(fnv("techn_00000901")) == 0);
    RUN(2200); assert(veh_entries_sent(fnv("techn_00000901")) >= 1);
    sent_n = 0;
    { Entry v = { fnv("techn_00000902"), E_VEH, 0, 0 }; u8 bb[8] = { 0, 0, 1, 0, 0, 0, 0, 0 }; memcpy(&v.a, bb, 4); memcpy(&v.b, bb + 4, 4); v.flags |= (u32)1 << 8;
      from_peer(&v, 1, 1); RUN(400);
      printf("vehicle: +d09=%u +d10=%u\n", veh[2][0xd09], veh[2][0xd10]);
      assert(veh[2][0xd09] == 1 && veh[2][0xd10] == 1 && veh[2][0xd07] == 0 && veh[0][0xd09] == 0);
      assert(veh_entries_sent(fnv("techn_00000902")) == 0);          // no echo
      veh[2][0xd09] = 0; from_peer(&v, 1, 0); RUN(400);              // the same flag told again: not forced a second time
      assert(veh[2][0xd09] == 0); }
    // vehicles only noticed in mode 1
    setup(); hmode = 1; RUN(3600);
    { Entry v = { fnv("techn_00000900"), E_VEH, 1, 0 }; from_peer(&v, 1, 1); RUN(400); assert(veh[0][0xd07] == 0 && g_s_veh_note >= 1); }
    hmode = 2;
    // most vehicles with flags at the start: vehicles off, soldiers go on
    setup(); for (int i = 0; i < NV; i++) veh[i][0xd07] = 1;
    RUN(4000); assert(!g_broken);
    // (only NV = 3 vehicles here: below the 20 that the check needs)

    // ---- 7d. the dead: the lower exact health of the two games wins (killed or badly wounded in the statistics), nothing is sent when the other game has it
    setup(); *selfhp(10) = 0.4f; RUN(2000); sent_n = 0;
    { Entry d = { 0x10a, E_DEAD | E_HURT, 0, 0 }; float z = 0.05f; memcpy(&d.a, &z, 4); from_peer(&d, 1, 1); RUN(300);
      printf("dead both: %.3f\n", *selfhp(10)); assert(*selfhp(10) == 0.05f); assert(dead_entries_sent(0x10a) == 0);
      z = 0.3f; memcpy(&d.a, &z, 4); from_peer(&d, 1, 1); RUN(300); assert(*selfhp(10) == 0.05f); }
    setup(); *selfhp(11) = 0.05f; RUN(2000); sent_n = 0;
    { Entry d = { 0x10b, E_DEAD | E_HURT, 0, 0 }; float z = 0.45f; memcpy(&d.a, &z, 4); from_peer(&d, 1, 1); RUN(300);
      assert(*selfhp(11) == 0.05f); assert(dead_entries_sent(0x10b) >= 1);  }   // the other game has a higher value: it is told ours

    // ---- 7e. the statistics' dice
    setup(); RUN(300);
    { static char statobj[0x200]; *(u32 *)(statobj + STAT_SEED_OFF) = 0x1234; g_orig_stat = (FnStat)fake_stat; stat_calls = 0;
      hook_stat(statobj, 0, 0);
      printf("stat seed %08x\n", *(u32 *)(statobj + STAT_SEED_OFF));
      assert(*(u32 *)(statobj + STAT_SEED_OFF) == STAT_SEED && stat_calls == 1 && g_s_final_scans == 1); }

    // ---- 7f. Object numbers are a hard prerequisite: no Vitals state is sent, queued, or ACKed early.
    // When the native exchange completes, the local complete snapshot starts and a peer retry is accepted.
    setup(); num_ready = 0; num_flag = 0; RUN(300); *selfhp(12) = 0.2f; sent_n = 0; RUN(300);
    assert(sent_n == 0 && g_box_n == 0);
    { Entry early = { 0x10d, E_DEAD, 0, 0 }; from_peer(&early, 1, 1); assert(g_box_n == 0 && sent_n == 0); }
    RUN(3500); assert(num_flag == 1); RUN(12000); assert(num_flag == 1);
    num_flag = 0; num_ready = 1; RUN(300); assert(dead_entries_sent(0x10c) == 1);
    { Entry retry = { 0x10d, E_DEAD, 0, 0 }; from_peer(&retry, 1, 1); assert(g_box_n == 1 && sent_n == 2); }
    RUN(300); assert(*selfhp(13) == 0.f && !sq_of(13));
    RUN(9000); assert(num_flag == 0); num_ready = 0;

    // ---- 8. the health is not a health: off
    setup(); for (int i = 0; i < NS; i++) *selfhp(i) = 1e9f;
    RUN(1000); assert(g_broken);

    // ---- 9. too many dead at the start: off
    setup(); for (int i = 0; i < NS; i++) *selfhp(i) = 0.f;
    // (the sanity count of 40 samples is done with sensible numbers: the zero ones are sensible, the dead share then trips)
    RUN(4000); assert(g_broken);

    // ---- 10. v7.1 state packets are taken out, accepted once and ACKed only after the mailbox copy; strangers pass neither state nor ACKs.
    setup(); step(1); have = 1; orig_pk_calls = 0; sent_n = 0; auto_ack = 0;
    u8 other[16] = { 9, 9 }; u8 pk[sizeof(VitalsHead) + 16]; VitalsHead hh = { { MAGIC, PKT_VITALS, 1, 1 }, 0x55aa, 7 }; Entry ee = { 0x100, E_DEAD, 0, 0 };
    memcpy(pk, &hh, sizeof hh); memcpy(pk + sizeof hh, &ee, sizeof ee);
    hook_packet(other, pk, sizeof pk, fake_tick); assert(g_box_n == 0 && sent_n == 0 && orig_pk_calls == 0);
    hook_packet(peer, pk, sizeof pk, fake_tick); assert(g_box_n == 1 && sent_n == 1 && ((VitalsHead *)sent_pk[0])->h.type == PKT_VITALS_ACK && orig_pk_calls == 0);
    hook_packet(peer, pk, sizeof pk, fake_tick); assert(g_box_n == 1 && sent_n == 2 && g_s_vdup == 1);  // duplicate is ACKed but never queued twice
    g_box_n = BOX; hh.seq++; memcpy(pk, &hh, sizeof hh); hook_packet(peer, pk, sizeof pk, fake_tick); assert(sent_n == 2 && g_s_vfull == 1);
    Head h2 = { MAGIC, 7, 0, 0 }; u8 pk2[8]; memcpy(pk2, &h2, 8); hook_packet(peer, pk2, 8, fake_tick); assert(orig_pk_calls == 1);

    // ---- 11. v7.1 sender keeps an exact state packet until its matching ACK; the retry does not allocate a new sequence.
    {
        setup(); auto_ack = 0; sent_n = 0;
        Entry one = { 0x100, E_HURT, 0, 0 }; float hp = 0.75f; memcpy(&one.a, &hp, 4);
        assert(send_entries(&one, 1, 1, fake_tick) == 1 && sent_n == 1 && vitals_pending() == 1);
        VitalsHead sent = *(VitalsHead *)sent_pk[0];
        assert(sent.h.type == PKT_VITALS && sent.h.count == 1 && sent.epoch == g_v_epoch && sent.seq != 0);
        fake_tick += VITAL_RETRY_MS; vitals_transport_tick(fake_tick);
        assert(sent_n == 2 && g_s_vretry == 1 && !memcmp(sent_pk[0], sent_pk[1], (size_t)sent_len[0]) && vitals_pending() == 1);
        VitalsHead wrong = { { MAGIC, PKT_VITALS_ACK, 0, 0 }, sent.epoch, sent.seq + 1 };
        hook_packet(peer, (const u8 *)&wrong, sizeof wrong, fake_tick); assert(vitals_pending() == 1 && g_s_vack_unknown == 1);
        VitalsHead ack = { { MAGIC, PKT_VITALS_ACK, 0, 0 }, sent.epoch, sent.seq };
        hook_packet(peer, (const u8 *)&ack, sizeof ack, fake_tick); assert(vitals_pending() == 0 && g_s_vack == 1);
        printf("state ACK/retry: OK\n");
    }

    // ---- 12. Native MiusPlayer reliability types are transparent.  Vitals observes their totals only; it never changes
    // their bytes, peer state, record cursor, or type-11 acknowledgement handling.
    {
        setup(); orig_pk_calls = 0;
        Head native10 = { MAGIC, 10, 0x5a, 1 };
        Head native11 = { MAGIC, 11, 0xa5, 0 };
        hook_packet(peer, (const u8 *)&native10, sizeof native10, fake_tick);
        hook_packet(peer, (const u8 *)&native11, sizeof native11, fake_tick);
        assert(orig_pk_calls == 2);
        printf("native types 10/11: pass through unchanged\n");
    }
    // ---- 13. The native number-ready bit can pulse low after success. It is latched for this tactical
    // session, so a 3-second native transfer cycle cannot nudge/restart Vitals; a disconnect resets it.
    {
        setup(); num_ready = 0; num_flag = 0; g_num_complete = 0;
        step(1);
        assert(!g_num_complete && g_wait_numbers && num_flag == 1 && g_nudges == 1);
        num_ready = 1; step(60);
        assert(g_num_complete && !g_wait_numbers);
        num_ready = 0; num_flag = 0;
        u32 nudges = g_nudges; fake_tick += 3100; step(1);
        assert(g_num_complete && !g_wait_numbers && num_flag == 0 && g_nudges == nudges);
        conn = 0; step(1);
        assert(!g_on && !g_num_complete);
        conn = 1; num_ready = 0; num_flag = 0; fake_tick += 3100; step(1);
        assert(!g_num_complete && g_wait_numbers && num_flag == 1 && g_nudges == 1);
        printf("number-ready latch/reset: OK\n");
    }

    // ---- 14. The default leaves the native staggered idle cadence intact. An explicit override changes
    // exactly the two immediates, and only when the code is what it should be.
    {
        static u8 code[0x1a000]; u8 want[sizeof SIG_IDLE];
        memset(code, 0xcc, sizeof code); memcpy(code + RVA_IDLE, SIG_IDLE, sizeof SIG_IDLE);
        fake_env_idle = 0; patch_idle(code);
        assert(!memcmp(code + RVA_IDLE, SIG_IDLE, sizeof SIG_IDLE) && g_idle_msg == -1);
        fake_env_idle = "2000"; patch_idle(code);
        assert(!memcmp(code + RVA_IDLE, SIG_IDLE, sizeof SIG_IDLE) && g_idle_msg == -1);
        fake_env_idle = "0"; patch_idle(code);
        assert(code[RVA_IDLE + IDLE_IMM8] == 0 && !memcmp(code + RVA_IDLE + IDLE_IMM32, "\0\0\0\0", 4) && g_idle_msg == 0);
        memcpy(want, SIG_IDLE, sizeof want); want[IDLE_IMM8] = 0; memset(want + IDLE_IMM32, 0, 4);
        assert(!memcmp(code + RVA_IDLE, want, sizeof want) && code[RVA_IDLE + sizeof want] == 0xcc && code[RVA_IDLE - 1] == 0xcc);
        memset(code, 0xcc, sizeof code); memcpy(code + RVA_IDLE, SIG_IDLE, sizeof SIG_IDLE); g_idle_msg = -1;
        fake_env_idle = "500"; patch_idle(code); { u32 ms = 0; memcpy(&ms, code + RVA_IDLE + IDLE_IMM32, 4); assert(code[RVA_IDLE + IDLE_IMM8] == 0 && ms == 500 && g_idle_msg == 500); }
        memset(code, 0xcc, sizeof code); memcpy(code + RVA_IDLE, SIG_IDLE, sizeof SIG_IDLE); code[RVA_IDLE + 3] ^= 1; g_idle_msg = -1;
        fake_env_idle = "0"; patch_idle(code); assert(g_idle_msg == -1 && code[RVA_IDLE + IDLE_IMM8] == 0x1e);   // not the expected code: left alone
        fake_env_idle = 0;
        printf("idle policy: OK\n");
    }
    // ---- 15. Exact basic_render repair: malformed recursive children still return through their normal epilogues,
    // stale matrix-cache IDs go through the engine allocator, and corrupt geometry is rejected before the engine reads it.
    {
        static u8 render[0x3a000], triangle_query[0xc0], triangle_vertices[256]; static u32 triangle_indices[3];
        static u64 renderer[0x300 / 8], groups[8], record[0x200 / 8];
        EXCEPTION_RECORD er; CONTEXT cx; EXCEPTION_POINTERS xp = { &er, &cx };
        memset(&er, 0, sizeof er); memset(&cx, 0, sizeof cx); memset(renderer, 0, sizeof renderer); memset(groups, 0, sizeof groups); memset(record, 0, sizeof record);
        g_render = render; g_render_recoveries = g_render_cache_repairs = g_render_triangle_skips = 0; g_render_cache_armed = g_render_triangle_armed = 1;
        er.ExceptionCode = EXCEPTION_ACCESS_VIOLATION; er.NumberParameters = 2; er.ExceptionInformation[0] = 0;

        // Triangle preflight models the current crash exactly: its third stored index (+8), faaec63, is
        // multiplied by the 16-byte stride to make the observed invalid offset faaec630.  Good geometry
        // reaches the engine; malformed geometry gets only its documented no-hit result before a vertex load occurs.
        memset(triangle_query, 0, sizeof triangle_query); memset(triangle_vertices, 0, sizeof triangle_vertices);
        triangle_indices[0] = triangle_indices[1] = triangle_indices[2] = 0;
        *(u64 *)(triangle_query + 0x70) = (u64)(size_t)triangle_indices;
        *(u64 *)(triangle_query + 0x78) = (u64)(size_t)triangle_vertices;
        *(u32 *)(triangle_query + 0x80) = 3; *(u32 *)(triangle_query + 0x84) = 1; *(u32 *)(triangle_query + 0x88) = 16;
        *(u32 *)(triangle_query + 0x8c) = 64; *(u32 *)(triangle_query + 0x90) = 96;
        fake_vq_unmapped_from = UINTPTR_MAX; fake_triangle_calls = 0; g_orig_render_triangle = fake_render_triangle;
        assert(!render_triangle_bad_input(triangle_query) && hook_render_triangle(triangle_query) == 1 && fake_triangle_calls == 1);
        triangle_indices[2] = 0x0faaec63u; triangle_query[0xa4] = 1;
        fake_vq_unmapped_from = (uintptr_t)triangle_vertices + sizeof triangle_vertices;
        assert(render_triangle_bad_input(triangle_query) && hook_render_triangle(triangle_query) == 0 && fake_triangle_calls == 1 && !triangle_query[0xa4] && g_render_triangle_skips == 1);
        fake_vq_unmapped_from = UINTPTR_MAX;

        // The original material reader returns its valid parent material ID in EDI.
        cx.Rip = (u64)(render + RVA_RENDER_BAD_READ); cx.Rsi = 0x123456780ull; cx.R8 = 0x400; cx.Rdi = 0x3456;
        er.ExceptionAddress = render + RVA_RENDER_BAD_READ; er.ExceptionInformation[1] = cx.Rsi + 0x38;
        assert(render_fault_guard(&xp) == EXCEPTION_CONTINUE_EXECUTION && cx.Rip == (u64)(render + RVA_RENDER_FALLBACK) && g_render_recoveries == 1);
        // The newly confirmed node reader has the bad child pointer in RDI and returns void; its fallback restores RBX too.
        cx.Rip = (u64)(render + RVA_RENDER_WALK_BAD_READ); cx.Rdi = 0x123456780ull; cx.R8 = 0x3f800000u;
        er.ExceptionAddress = render + RVA_RENDER_WALK_BAD_READ; er.ExceptionInformation[1] = cx.Rdi + 0x38;
        assert(render_fault_guard(&xp) == EXCEPTION_CONTINUE_EXECUTION && cx.Rip == (u64)(render + RVA_RENDER_WALK_FALLBACK) && g_render_recoveries == 2);
        // A plausible child index, a write fault, a different instruction, or an unusable material parent must pass on unchanged.
        cx.Rip = (u64)(render + RVA_RENDER_WALK_BAD_READ); cx.R8 = 0x3ff; assert(render_fault_guard(&xp) == EXCEPTION_CONTINUE_SEARCH && cx.Rip == (u64)(render + RVA_RENDER_WALK_BAD_READ));
        cx.R8 = 0x400; er.ExceptionInformation[0] = 1; assert(render_fault_guard(&xp) == EXCEPTION_CONTINUE_SEARCH);
        er.ExceptionInformation[0] = 0; cx.Rip = (u64)(render + RVA_RENDER_WALK_BAD_READ + 1); assert(render_fault_guard(&xp) == EXCEPTION_CONTINUE_SEARCH);
        cx.Rip = (u64)(render + RVA_RENDER_BAD_READ); cx.Rsi = 0x123456780ull; cx.Rdi = 0x1000000; er.ExceptionAddress = render + RVA_RENDER_BAD_READ; er.ExceptionInformation[1] = cx.Rsi + 0x38;
        assert(render_fault_guard(&xp) == EXCEPTION_CONTINUE_SEARCH);

        // The preflight's tiny race backup accepts only the triangle routine's twelve read instructions and
        // still returns via its own no-hit cleanup.  An address mismatch remains the engine's exception.
        memset(&cx, 0, sizeof cx); cx.Rdx = (u64)(size_t)renderer; cx.Rsi = 0xc4f85f30ull; cx.Rax = 0xfaaec630ull;
        cx.Rip = (u64)(render + RVA_RENDER_TRIANGLE_VERTEX_1); er.ExceptionAddress = render + RVA_RENDER_TRIANGLE_VERTEX_1; er.ExceptionInformation[1] = cx.Rsi + cx.Rax;
        assert(render_fault_guard(&xp) == EXCEPTION_CONTINUE_EXECUTION && cx.Rip == (u64)(render + RVA_RENDER_TRIANGLE_FALLBACK) && g_render_triangle_skips == 2);
        memset(&cx, 0, sizeof cx); cx.Rdx = (u64)(size_t)renderer; cx.R8 = 0x1bfa72560ull;
        cx.Rip = (u64)(render + RVA_RENDER_TRIANGLE_INDEX_0); er.ExceptionAddress = render + RVA_RENDER_TRIANGLE_INDEX_0; er.ExceptionInformation[1] = cx.R8;
        assert(render_fault_guard(&xp) == EXCEPTION_CONTINUE_EXECUTION && cx.Rip == (u64)(render + RVA_RENDER_TRIANGLE_FALLBACK) && g_render_triangle_skips == 3);
        memset(&cx, 0, sizeof cx); cx.Rdx = (u64)(size_t)renderer; cx.Rax = 0x100000000ull; cx.R14 = 0x2340;
        cx.Rip = (u64)(render + RVA_RENDER_TRIANGLE_ATTR1_1); er.ExceptionAddress = render + RVA_RENDER_TRIANGLE_ATTR1_1; er.ExceptionInformation[1] = cx.Rax + cx.R14;
        assert(render_fault_guard(&xp) == EXCEPTION_CONTINUE_EXECUTION && cx.Rip == (u64)(render + RVA_RENDER_TRIANGLE_FALLBACK) && g_render_triangle_skips == 4);
        er.ExceptionInformation[1]++; assert(render_fault_guard(&xp) == EXCEPTION_CONTINUE_SEARCH);

        // The reported cache has an ordinary source node but an out-of-pool 24-bit matrix slot.  A valid destination leaves it alone;
        // an unmapped destination atomically becomes the engine's -2 cache-miss sentinel before the original function runs.
        *(u64 *)((u8 *)renderer + 0x240) = (u64)(size_t)groups; *(u64 *)groups = (u64)(size_t)record;
        *(u64 *)((u8 *)renderer + 0x168) = 0x7000000000000000ull; *(u32 *)((u8 *)record + 0x34) = 0x200;
        fake_vq_unmapped_from = UINTPTR_MAX; render_cache_preflight(renderer, 0); assert(*(u32 *)((u8 *)record + 0x34) == 0x200);
        fake_vq_unmapped_from = 0x7000000000008000ull; render_cache_preflight(renderer, 0);
        assert(*(u32 *)((u8 *)record + 0x34) == RENDER_CACHE_MISSING && g_render_cache_repairs == 1);
        fake_vq_unmapped_from = UINTPTR_MAX;

        // If a racing writer changes that field after the preflight, accept only the exact captured `movaps` write and send it to
        // basic_render's normal cache-rebuild branch; any address mismatch remains the game's own exception.
        *(u32 *)((u8 *)record + 0x34) = 0x00de741bu; memset(&cx, 0, sizeof cx); er.ExceptionInformation[0] = 1;
        cx.Rip = (u64)(render + RVA_RENDER_CACHE_WRITE); cx.Rax = 0x1000; cx.Rdx = 0x00de741bu; cx.Rbp = (u64)(size_t)record;
        er.ExceptionAddress = render + RVA_RENDER_CACHE_WRITE; er.ExceptionInformation[1] = cx.Rax + (((u64)(u32)cx.Rdx & 0xffffffu) << 6);
        { LONG guarded = render_fault_guard(&xp);
          assert(guarded == EXCEPTION_CONTINUE_EXECUTION && cx.Rip == (u64)(render + RVA_RENDER_CACHE_REBUILD) && g_render_cache_repairs == 2); }
        cx.Rip = (u64)(render + RVA_RENDER_CACHE_WRITE); er.ExceptionInformation[1]++; assert(render_fault_guard(&xp) == EXCEPTION_CONTINUE_SEARCH);
        fake_vq_unmapped_from = UINTPTR_MAX; g_render_cache_armed = 0; g_render = 0;
        printf("render cache repair and crash guards: OK\n");
    }
    printf("ALL OK\n");
    return 0;
}

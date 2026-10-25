// mp_vitals.dll - "a kill is final on both sides" for the Steam release of the Mius-Front multiplayer mod
// (basic_network.codelib, built 2026-10-14, "v20").
//
// How it is loaded: patch_import.py adds ONE import entry (mp_vitals.dll!MpVitalsInit) to that
// basic_network.codelib. Windows then loads this DLL together with the module, DllMain() below checks that the
// module really is the build this code was made for and, only then, hooks two of its functions:
//     session_frame  - once per rendered frame (after the original has run)
//     on_packet      - every datagram that arrives (only the reserved Vitals types 0x34/0x35 are taken out; every native type passes unchanged)
// Nothing else in the module is touched: Steam P2P, UPnP, lobby, animation/world sync all run unchanged.
// If anything does not match, no hook is installed and the module behaves exactly as before.
//
// What it does: each game looks at every soldier (i_human; vehicle and gun crews are soldiers too). A soldier whose own
// health (+0x7c8) is below 0.5 is dead - that is the engine's own test: its hit code takes the soldier out of his squad
// at that moment. Every game tells the other game which soldiers are dead here (a message at once, and the whole list
// every two seconds). A soldier that is dead in the other game and not dead here is KILLED here, the way the engine does
// it. Nobody is ever brought back: a kill in either game is a kill in both. Without this nothing ever corrected a
// soldier that one game killed and the other did not (the module's hit replay works the outcome out again on each side).
//
// v4 adds the other states, with the one rule that a unit's state only ever gets worse and is the worse of the two games:
//   - a wounded soldier: his own health (+0x7c8) and his record's health (+0x60) become the lower of the two games' values
//     (nothing in the engine raises them again, so the lower value is never wrong; a kill is just the case below 0.5);
//   - a vehicle, gun or section (i_techn): the status flags the engine keeps at +0xd07..+0xd10 (+0xd0d is a view flag and left
//     out) - a flag that came on in the other game and is off here is switched on here, once, when it appears there.
//
// v5: the post-battle statistics. Their code (adv_game.codelib, the function that adds up the casualties at the end of a battle) sorts every
// soldier by his health (+0x7c8): below 0.1 killed, 0.1 to 0.5 badly wounded - and for those it throws a die (the random generator of the
// statistics object, +0x130) whether he is lost or comes back. So the exact health matters, also of the dead (v4 forced them to 0), and each
// game threw its own dice. v5 keeps the exact (lower) health of the two games also for the dead, kills with the other game's health instead of
// 0, and puts the statistics object's random generator at the same value in both games right before that function runs.
// Morale of the operation map is worked out from these numbers after the battle, so it follows them.
//
// v7: the unit-position sender normally repeats a stationary unit only every two seconds. MP_IDLE_MS is an explicit,
// diagnostic opt-in override for that native cadence; without it this DLL leaves the sender untouched.
//
// v7.0.1: two crash dumps, including one taken before this DLL was present, stop in the same basic_render recursive node walk. The child node
// number is corrupt there, and the engine reads outside the node table. On the exact checked Steam 3598 basic_render image, a tiny vectored
// exception guard turns only that read fault back into the already-valid parent material. It leaves every other exception alone.
//
// Environment: MP_VITALS 0 off, 1 only compare and log, 2 and 3 apply.
//              Default: apply in operation battles, compare and log only otherwise.   MP_LOG_VITALS=1 logs every event.
//              MP_VEHICLES 0 off, 1 only compare and log, 2 apply (default: as MP_VITALS).   MP_STATS 0 leaves the statistics' dice alone.
//              MP_RENDER_GUARD=0 disables the exact basic_render cache-lifetime, geometry-index and invalid-child guards.
//              MP_IDLE_MS selects a stationary-unit repeat period only when explicitly set (0 is every tick; default is native staggered 2 s).
//
// v7.1.0: Vitals data uses type 0x34 and an application ACK uses type 0x35, rather than the legacy unacknowledged type 0x32.
// Each data packet carries a nonzero epoch and sequence. It is retained for an ACK, retried at most 12 times, and applied once only.
// Native type 10 (reliable records) and type 11 (its ACK) are deliberately only observed in diagnostics and always pass to MiusPlayer.
//
// v7.1.1: the verified basic_render guard also skips the second observed recursive invalid-child read at RVA 0x12ffd.
// It runs only after exact signatures for both reader/epilogue pairs on the checked Steam 3598 image.
//
// v7.2: Vitals will neither send nor apply a tactical state before the native object-number exchange says
// it is complete. This prevents a Vitals repair from getting ahead of the very translation table that gives
// foreign object and description numbers their local meaning. Right before end-of-battle statistics, one
// forced final Vitals scan publishes the last monotonic casualties, and both statistic functions use the
// existing common RNG seed.
//
// v7.2.1: a new Steam 3598 crash dump proves that basic_render can retain a matrix-cache slot from a
// previous pool. The exact cache-sync function now tests pool ownership and asks the engine's allocator
// to rebuild a stale entry before the unchecked 64-byte matrix write; it does not invent a render object.
//
// Current release: the later dump is a different fault: unvalidated triangle index/stride arithmetic
// reaches basic_render+39b52. The exact triangle-query entry now verifies every index-derived
// vertex/optional-attribute address before the engine reads it, returning its documented no-hit result
// for malformed geometry. The VEH remains only an exact after-check race fallback for those loads.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

typedef unsigned long long u64;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;


// No C runtime at all (the DLL imports nothing but kernel32): the few helpers it needs are here.
// (compiled with -fno-builtin so that the compiler does not turn these loops back into calls of themselves)
void *memcpy(void *d, const void *s, size_t n) { unsigned char *a = (unsigned char *)d; const unsigned char *b = (const unsigned char *)s; while (n--) *a++ = *b++; return d; }
void *memmove(void *d, const void *s, size_t n)
{
    unsigned char *a = (unsigned char *)d; const unsigned char *b = (const unsigned char *)s;
    if (a < b) while (n--) *a++ = *b++; else { a += n; b += n; while (n--) *--a = *--b; }
    return d;
}
void *memset(void *d, int c, size_t n) { unsigned char *a = (unsigned char *)d; while (n--) *a++ = (unsigned char)c; return d; }
int memcmp(const void *x, const void *y, size_t n)
{
    const unsigned char *a = (const unsigned char *)x, *b = (const unsigned char *)y;
    for (; n; n--, a++, b++) if (*a != *b) return *a < *b ? -1 : 1;
    return 0;
}
static int str_eq(const char *a, const char *b) { while (*a && *a == *b) { a++; b++; } return *a == *b; }
static char ascii_lower(char c) { return c >= 'A' && c <= 'Z' ? (char)(c + ('a' - 'A')) : c; }
static int tactical_program_path(const char *s)
{
    static const char name[] = "tacgame.progpack";
    const char *p;
    for (p = s; *p; p++) {
        const char *a = p, *b = name;
        while (*a && *b && ascii_lower(*a) == ascii_lower(*b)) { a++; b++; }
        // A filename boundary prevents an unrelated argument merely containing tacgame.progpack from
        // opting a skirmish/custom launcher into the operation policy.
        if (!*b && (p == s || p[-1] == '\\' || p[-1] == '/') &&
            (!*a || *a == ',' || *a == ' ' || *a == '\t' || *a == '"')) return 1;
    }
    return 0;
}
static const char *last_us(const char *s) { const char *r = 0; for (; *s; s++) if (*s == '_') r = s; return r; }
static u32 hex_of(const char *s)
{
    u32 v = 0;
    for (;; s++) {
        int c = *s, d;
        if (c >= '0' && c <= '9') d = c - '0'; else if (c >= 'a' && c <= 'f') d = c - 'a' + 10; else if (c >= 'A' && c <= 'F') d = c - 'A' + 10; else break;
        v = v * 16 + (u32)d;
    }
    return v;
}
static int num_of(const char *s) { int v = 0; while (*s >= '0' && *s <= '9') v = v * 10 + (*s++ - '0'); return v; }

// A tactical operation is named by the game as YYYY_MM_campaign:battle.  The native module normally
// receives MP_HOST_MODE=operation from opgame; a direct tactical launch can lose that inherited setting
// and incorrectly activate full-object replication.  Its objects contain operation-local formation
// references, so this is both unsupported and enormously expensive.  Recognise only that exact tactical
// command-line form; skirmishes and custom missions retain their native state policy.
static int operation_tactical_command(const char *cmd)
{
    const char *p;
    if (!cmd || !tactical_program_path(cmd)) return 0;
    // Arguments are comma-delimited by starter_x64.  Advance full fields rather than scanning every
    // character, so malformed/truncated command lines cannot make a token cross a delimiter.
    for (p = cmd; *p;) {
        const char *q;
        while (*p == ' ' || *p == '\t' || *p == '"') p++;
        if (p[0] == '1' && p[1] == '9' && p[2] >= '0' && p[2] <= '9' && p[3] >= '0' && p[3] <= '9' && p[4] == '_' &&
            p[5] >= '0' && p[5] <= '9' && p[6] >= '0' && p[6] <= '9' && p[7] == '_') {
            for (q = p + 8; *q && *q != ','; q++) if (*q == ':') return 1;
        }
        while (*p && *p != ',') p++;
        if (*p == ',') p++;
    }
    return 0;
}

// Do not override an explicit diagnostic/user choice.  This function runs during DLL attach, before
// basic_network reads MP_SYNC_STATE for the tactical session.
static int default_operation_state_policy(void)
{
    if (!operation_tactical_command(GetCommandLineA())) return 0;
    if (GetEnvironmentVariableA("MP_SYNC_STATE", 0, 0)) return 0;
    return SetEnvironmentVariableA("MP_SYNC_STATE", "0") ? 1 : -1;
}

// ------------------------------------------------------------------ the build this is made for
#define V20_TIMESTAMP   0x6acfaa3du
enum {
    RVA_SESSION_FRAME = 0x19f88, RVA_ON_PACKET = 0x1623a, RVA_OWNS = 0x17cf0, RVA_SEND_PEER = 0x19a98, RVA_LOG = 0x27dc,
    RVA_NUM_READY = 0x5d490, RVA_NUM_FLAG = 0x5d492, RVA_CONNECTED = 0x5b61b, RVA_TACTICAL = 0x5b618, RVA_HAVE_PEER = 0x5b61a, RVA_HOST_MODE = 0x5b8e4, RVA_PEER = 0x5b8f0,
    RVA_IDLE = 0x19d44, IDLE_IMM8 = 9, IDLE_IMM32 = 19,
    PHYS_GET_MEMBER = 0x9c480,
    GAME_STAT = 0xf80c0, STAT_STEAL = 15, STAT_SEED_OFF = 0x130, STAT_SEED = 0x4d495553,
    // basic_render.codelib, Steam build 3598.  The renderer's transform-cache updater at 12925
    // stores matrices through a cached 24-bit pool slot; the two next functions walk material nodes.
    // 39a40 is the triangle-query routine: it returns false through 39de5 if none of its triangles hits.
    RVA_RENDER_CACHE_SYNC = 0x12925, RVA_RENDER_CACHE_WRITE = 0x129a2, RVA_RENDER_CACHE_REBUILD = 0x129c6,
    RVA_RENDER_WALK_NODE = 0x12fd0, RVA_RENDER_WALK_BAD_READ = 0x12ffd, RVA_RENDER_WALK_FALLBACK = 0x13038,
    RVA_RENDER_NODE = 0x13ab0, RVA_RENDER_BAD_READ = 0x13adb, RVA_RENDER_FALLBACK = 0x13bc9,
    RVA_RENDER_TRIANGLE = 0x39a40, RVA_RENDER_TRIANGLE_LOOP = 0x39b20,
    RVA_RENDER_TRIANGLE_INDEX_0 = 0x39b26, RVA_RENDER_TRIANGLE_INDEX_1 = 0x39b29, RVA_RENDER_TRIANGLE_INDEX_2 = 0x39b30,
    RVA_RENDER_TRIANGLE_VERTEX_0 = 0x39b4d, RVA_RENDER_TRIANGLE_VERTEX_1 = 0x39b52, RVA_RENDER_TRIANGLE_VERTEX_2 = 0x39b5b,
    RVA_RENDER_TRIANGLE_ATTR0 = 0x39cf2, RVA_RENDER_TRIANGLE_ATTR0_0 = 0x39cfb, RVA_RENDER_TRIANGLE_ATTR0_1 = 0x39cff, RVA_RENDER_TRIANGLE_ATTR0_2 = 0x39d04,
    RVA_RENDER_TRIANGLE_ATTR1 = 0x39d23, RVA_RENDER_TRIANGLE_ATTR1_0 = 0x39d2c, RVA_RENDER_TRIANGLE_ATTR1_1 = 0x39d30, RVA_RENDER_TRIANGLE_ATTR1_2 = 0x39d35,
    RVA_RENDER_TRIANGLE_FALLBACK = 0x39de5
};
#define RENDER_TIMESTAMP 0x66f2d710u
// adv_game.codelib: the end-of-battle statistics function (prologue; its body has three times imul reg,[reg+0x130],0x19660d)
static const u8 SIG_STAT[] = { 0x48,0x8b,0xc4, 0x44,0x88,0x40,0x18, 0x48,0x89,0x50,0x10, 0x48,0x89,0x48,0x08, 0x55,0x53,0x56,0x57, 0x41,0x54,0x41,0x55,0x41,0x56 };
static const u8 SIG_STAT_RNG[8] = { 0x30,0x01,0x00,0x00, 0x0d,0x66,0x19,0x00 };
static const u8 SIG_FRAME[15]  = { 0x48,0x8b,0xc4, 0x48,0x89,0x58,0x10, 0x48,0x89,0x70,0x18, 0x48,0x89,0x78,0x20 };
static const u8 SIG_PACKET[14] = { 0x48,0x8b,0xc4, 0x48,0x89,0x58,0x20, 0x55,0x56,0x57, 0x41,0x54, 0x41,0x55 };
static const u8 SIG_PACKET_HEAD[6] = { 0x41,0x83,0xf8,0x08, 0x0f,0x8c };
static const u8 SIG_OWNS[8]    = { 0x40,0x53,0x48,0x83,0xec,0x20,0x80,0x3d };
static const u8 SIG_SEND[7]    = { 0x48,0x83,0xec,0x28,0x48,0x83,0x3d };
// the transform sender's 'unit has not moved' test: and eax,0xf / imul ecx,eax,30 / mov eax,[rsp+24] / sub eax,[rsi+0x30] / add ecx,2000 / cmp eax,ecx / jb skip
static const u8 SIG_IDLE[27]   = { 0x8b,0x44,0x24,0x70, 0x83,0xe0,0x0f, 0x6b,0xc8,0x1e, 0x8b,0x44,0x24,0x24, 0x2b,0x46,0x30, 0x81,0xc1,0xd0,0x07,0x00,0x00, 0x3b,0xc1, 0x0f,0x82 };
static const u8 SIG_LOG[7]     = { 0x4c,0x8b,0xdc,0x49,0x89,0x4b,0x08 };
// adv_physic.codelib: squad->get_member(index) = cmp [rcx+0x190c],edx / jbe / lea eax*5 << 6 + [rcx+0x1900]
static const u8 SIG_GET_MEMBER[29] = { 0x39,0x91,0x0c,0x19,0,0, 0x76,0x12, 0x8b,0xc2, 0x48,0x8d,0x04,0x80, 0x48,0xc1,0xe0,0x06, 0x48,0x03,0x81,0x00,0x19,0,0, 0xc3, 0x33,0xc0, 0xc3 };
// The code surrounding both observed faults and their cleanup epilogues. All signatures are checked before
// the exception handler is registered; this is deliberately not a generic access-violation handler.
// RVA 12ffd walks a child node and returns void; its normal epilogue must restore RBX as well as RBP/RSI/RDI.
static const u8 SIG_RENDER_WALK_NODE[48] = {
    0x48,0x89,0x6c,0x24,0x10, 0x48,0x89,0x74,0x24,0x18, 0x57, 0x48,0x83,0xec,0x20,
    0x4c,0x8b,0x89,0x40,0x02,0,0, 0x43,0x8d,0x3c,0x80, 0xc1,0xe7,0x06, 0x48,0x8b,0xe9,
    0x44,0x8b,0xd2, 0x49,0xc1,0xe2,0x06, 0x8b,0xf2, 0x4b,0x03,0x3c,0x0a, 0x8b,0x4f,0x38
};
static const u8 SIG_RENDER_WALK_FALLBACK[21] = {
    0x48,0x8b,0x5c,0x24,0x30, 0x48,0x8b,0x6c,0x24,0x38, 0x48,0x8b,0x74,0x24,0x40,
    0x48,0x83,0xc4,0x20, 0x5f,0xc3
};
// RVA 13adb returns the already-valid parent material ID in EAX.
static const u8 SIG_RENDER_NODE[46] = {
    0x48,0x89,0x6c,0x24,0x10, 0x48,0x89,0x74,0x24,0x18, 0x57, 0x48,0x83,0xec,0x20,
    0x4c,0x8b,0x89,0x40,0x02,0,0, 0x43,0x8d,0x34,0x80, 0xc1,0xe6,0x06, 0x48,0x8b,0xe9,
    0x44,0x8b,0xd2, 0x49,0xc1,0xe2,0x06, 0x4b,0x03,0x34,0x0a, 0x8b,0x4e,0x38
};
static const u8 SIG_RENDER_FALLBACK[18] = { 0x48,0x8b,0x6c,0x24,0x38, 0x8b,0xc7, 0x48,0x8b,0x74,0x24,0x40, 0x48,0x83,0xc4,0x20, 0x5f,0xc3 };
// Render-cache updater observed in the current report.  The first 15 bytes are a whole
// prologue (safe to trampoline); the second signature reaches the unchecked matrix write.
static const u8 SIG_RENDER_CACHE_SYNC[15] = { 0x48,0x89,0x6c,0x24,0x10, 0x48,0x89,0x74,0x24,0x18, 0x57, 0x48,0x83,0xec,0x20 };
static const u8 SIG_RENDER_CACHE_BODY[114] = {
    0x8b,0xda, 0x48,0x8b,0xf9, 0x83,0xfa,0xff, 0x74,0x1b, 0x8b,0xca, 0x8b,0xc2, 0xc1,0xe9,0x0a,
    0x48,0xc1,0xe1,0x06, 0x48,0x03,0x8f,0x40,0x02,0,0, 0x25,0xff,0x03,0,0, 0x8b,0xf0, 0xeb,0x21,
    0x48,0x8b,0x89,0x40,0x02,0,0, 0x48,0xb8,0xc0,0xff,0xff,0xff,0x3f,0,0,0, 0x48,0x03,0xc8,
    0x8b,0xf3, 0x81,0xe6,0xff,0x03,0,0, 0xb8,0xff,0xff,0xff,0xff, 0x8d,0x2c,0x80, 0xc1,0xe5,0x06,
    0x48,0x03,0x29, 0x8b,0x55,0x34, 0x83,0xfa,0xfd, 0x77,0x3b, 0x41,0x0f,0x28,0x00,
    0x81,0xe2,0xff,0xff,0xff,0, 0x48,0x8b,0x87,0x68,0x01,0,0, 0x8b,0xca, 0x48,0xc1,0xe1,0x06,
    0x0f,0x29,0x04,0x01
};
static const u8 SIG_RENDER_CACHE_REBUILD[20] = { 0x48,0x8d,0x8f,0x38,0x01,0,0, 0x49,0x8b,0xd0, 0xe8,0xeb,0x69,0,0, 0x8b,0xd0, 0x89,0x55,0x34 };
// Triangle query observed in gt3_crashreport.zip: it reads three 32-bit indices, then three 16-byte vertices.
// Its two optional attribute buffers use the same indices; 39de5 is its proven existing no-hit cleanup.
static const u8 SIG_RENDER_TRIANGLE[40] = {
    0x48,0x8b,0xc4, 0x48,0x89,0x58,0x18, 0x48,0x89,0x70,0x20, 0x57,0x41,0x54,0x41,0x57,
    0x48,0x81,0xec,0xc0,0,0,0, 0x8b,0x99,0x80,0,0,0, 0x45,0x32,0xc9, 0x44,0x0f,0xb6,0xa1,0x80,0,0,0
};
static const u8 SIG_RENDER_TRIANGLE_LOOP[64] = {
    0x8b,0x8a,0x88,0,0,0, 0x41,0x8b,0x28, 0x41,0x8b,0x40,0x08, 0x41,0x03,0xea,
    0x45,0x8b,0x70,0x04, 0x41,0x03,0xc2, 0x0f,0x10,0x72,0x50, 0x45,0x03,0xf2,
    0x0f,0xaf,0xe9, 0x0f,0x28,0xee, 0x0f,0xaf,0xc1, 0x44,0x0f,0xaf,0xf1, 0x8b,0xc8,
    0x44,0x0f,0x10,0x24,0x2e, 0x44,0x0f,0x10,0x0c,0x30, 0x41,0x0f,0x5c,0xec, 0x45,0x0f,0x10,0x04,0x36
};
static const u8 SIG_RENDER_TRIANGLE_ATTR0[48] = {
    0x8b,0x82,0x8c,0,0,0, 0x48,0x03,0xc6, 0x0f,0x10,0x0c,0x28, 0x41,0x0f,0x10,0x14,0x06, 0x0f,0x10,0x04,0x01,
    0x0f,0x5c,0xd1, 0x0f,0x5c,0xc1, 0x0f,0x59,0xd4, 0x0f,0x59,0xc5, 0x0f,0x58,0xd1, 0x0f,0x58,0xd0, 0x0f,0x11,0x52,0x20, 0x40,0x84,0xff,0x74
};
static const u8 SIG_RENDER_TRIANGLE_ATTR1[48] = {
    0x8b,0x82,0x90,0,0,0, 0x48,0x03,0xc6, 0x0f,0x10,0x0c,0x28, 0x41,0x0f,0x10,0x14,0x06, 0x0f,0x10,0x04,0x01,
    0x0f,0x5c,0xd1, 0x0f,0x5c,0xc1, 0x0f,0x59,0xd4, 0x0f,0x59,0xc5, 0x0f,0x58,0xd1, 0x0f,0x58,0xd0, 0x0f,0x11,0x52,0x30, 0x84,0xdb,0x74,0x26
};
static const u8 SIG_RENDER_TRIANGLE_FALLBACK[18] = { 0x44,0x88,0x8a,0xa4,0,0,0, 0xeb,0x07, 0x44,0x88,0x89,0xa4,0,0,0, 0x4c,0x8d };

// engine layout (adv_physic.codelib; the same in the build of the mod's author and in the current Steam build, read from its code):
//   i_human +0x7c8 the soldier's own health (float, 1 = unhurt). The hit code: health below 0.5 -> the soldier leaves his squad
//           (+0x2388 squad pointer and its copy +0x7d0 become 0). +0x2320 / +0x1da4 squad and index while attached.
//   The squad is an i_techn (a vehicle, a gun or an infantry section): members at *(squad+0x1900), 0x140 bytes each, count at
//           +0x190c; vtable slot 0x138 = get_member(squad, index). Member: +0x2c state (5 = killed), +0x60 health (float),
//           +0xc8 the soldier it belongs to.
enum { H_SQUAD = 0x2388, H_SQUAD_COPY = 0x7d0, H_INDEX = 0x1da4, H_SELF = 0x7c8, H_OUT = 0x228d };
enum { M_STATE = 0x2c, M_HEALTH = 0x60, M_OWNER = 0xc8, M_SIZE = 0x140, STATE_KILLED = 5 };

// ------------------------------------------------------------------ wire format
#pragma pack(push, 1)
typedef struct { u32 magic; u8 type, flags; u16 count; } Head;               // the first 8 bytes are the module's common header
// v7.1's state channel is intentionally distinct from v7's type 0x32: an old DLL must not mistake the added
// sequence fields for an Entry. `epoch, seq` identify one idempotent state packet; the ACK repeats both fields.
typedef struct { Head h; u32 epoch, seq; } VitalsHead;
// 16 bytes. A soldier: flags E_DEAD (dead in the sender's game), E_HURT (a = his health, float), E_REC (b = his record's health, float).
// A vehicle: flags E_VEH, a and b = the status bytes +0xd07 +0xd08 +0xd09 +0xd0a / +0xd0b +0xd0c +0xd0e +0xd0f, bits 8..15 of flags: +0xd10.
typedef struct { u32 id; u32 flags; u32 a; u32 b; } Entry;
#pragma pack(pop)
enum { PKT_VITALS = 0x34, PKT_VITALS_ACK = 0x35, MAGIC = 0x504d5447, PER_PACKET = 70, F_DELTA = 1, E_DEAD = 1, E_HURT = 2, E_REC = 4, E_VEH = 8 };

// ------------------------------------------------------------------ the module's functions
typedef void (*FnFrame)(void *om, void *a2, u64 a3, void *a4);
typedef void (*FnPacket)(const void *from, const unsigned char *data, int size, u32 now);
typedef u64 (*FnOwns)(void *obj);
typedef void (*FnSend)(const void *data, u32 size);
typedef void (*FnLog)(const char *fmt, ...);
typedef void (*FnStat)(void *obj, long long a2, char a3);
typedef void (*FnRenderCache)(void *renderer, u32 node, const void *matrix);
typedef int (*FnRenderTriangle)(void *query);

static HMODULE g_mod, g_phys;
static u32 g_phys_size;
static FnFrame g_orig_frame;
static FnPacket g_orig_packet;
static FnOwns g_owns;
static FnSend g_send;
static FnLog g_log;
static FnStat g_orig_stat;
static HMODULE g_game;
static int g_stat_tried;
static u32 g_stat_seeded;
static volatile u8 *g_connected, *g_tactical, *g_have_peer;
// v20's raw `ready` flag is a transfer-cycle flag: it can briefly go low again after a successful exchange.
// It must never re-open Vitals' startup gate during this tactical session.
static volatile u8 *g_num_ready, *g_num_flag;   // native: the other game's numbers arrived / ask it to send ours at its next 3 s tick
static int g_num_complete;                       // sticky successful native exchange, reset only with the Vitals tactical session
static u8 g_num_peer[16];
static int g_num_peer_set;
static u32 g_nudges;
static DWORD g_num_since, g_num_last;
static volatile u32 *g_host_mode;
static const u8 *g_peer;
static int g_active;                // hooks installed
static int g_broken;                // something did not match: no more work
static u8 *g_render;
static int g_render_tried, g_render_cache_armed, g_render_triangle_armed;
static u32 g_render_recoveries, g_render_reported, g_render_cache_repairs, g_render_cache_reported;
static u32 g_render_triangle_skips, g_render_triangle_reported;
static PVOID g_render_handler;
static FnRenderCache g_orig_render_cache;
static FnRenderTriangle g_orig_render_triangle;

// ------------------------------------------------------------------ small helpers
static int page_has(const void *p, size_t n, int write, MEMORY_BASIC_INFORMATION *out)
{
    MEMORY_BASIC_INFORMATION mbi;
    if (!p || !n || !VirtualQuery(p, &mbi, sizeof mbi)) return 0;
    if (mbi.State != MEM_COMMIT || (mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD))) return 0;
    if (write) {
        DWORD pflags = mbi.Protect & 0xff;
        if (pflags != PAGE_READWRITE && pflags != PAGE_WRITECOPY && pflags != PAGE_EXECUTE_READWRITE && pflags != PAGE_EXECUTE_WRITECOPY) return 0;
    }
    u64 at = (u64)(size_t)p, base = (u64)(size_t)mbi.BaseAddress, end;
    if (at < base || mbi.RegionSize > ~(u64)0 - base) return 0;
    end = base + (u64)mbi.RegionSize;
    if ((u64)n > ~(u64)0 - at || at + (u64)n > end) return 0;
    if (out) *out = mbi;
    return 1;
}
static int readable(const void *p, size_t n) { return page_has(p, n, 0, 0); }
static int writable(const void *p, size_t n) { return page_has(p, n, 1, 0); }

static int mode(void)
{
    static int forced = -2;
    if (forced == -2) {
        char v[8];
        forced = GetEnvironmentVariableA("MP_VITALS", v, sizeof v) ? num_of(v) : -1;
    }
    if (forced >= 0) return forced > 3 ? 3 : forced;
    return *g_host_mode == 2 ? 3 : 1;
}
static int log_events(void)
{
    static int on = -1;
    if (on < 0) on = GetEnvironmentVariableA("MP_LOG_VITALS", 0, 0) != 0;
    return on;
}

static int same(const u8 *at, const u8 *sig, size_t n) { return readable(at, n) && !memcmp(at, sig, n); }

// `basic_render+12925` has a transient cache in its per-node record.  A report proves that a
// valid input node (00052006) can carry a stale cache slot (00de741b): the renderer then writes
// 64 bytes through `renderer->matrix_pool + slot*64` without checking that this pool owns the slot.
// This is not a network identifier and does not appear in the tacgame image/save; it is renderer
// lifetime state.  Replacing an out-of-pool cache value with the engine's own -2 sentinel makes
// the normal allocator at 129c6 rebuild it before it can be used.
#define RENDER_CACHE_MISSING 0xfffffffeu
static void *install_detour(u8 *fn, size_t len, void *hook);

static int add_u64(u64 base, u64 off, u64 *out)
{
    if (base > ~(u64)0 - off) return 0;
    *out = base + off; return 1;
}
static u32 *render_cache_slot(void *renderer, u32 node)
{
    u64 groups, group_at, group, record_at;
    if (node == 0xffffffffu || !readable((const u8 *)renderer + 0x240, sizeof groups)) return 0;
    groups = *(const u64 *)((const u8 *)renderer + 0x240);
    if (!add_u64(groups, ((u64)node >> 10) << 6, &group_at) || !readable((const void *)(size_t)group_at, sizeof group)) return 0;
    group = *(const u64 *)(size_t)group_at;
    if (!add_u64(group, ((u64)(node & 0x3ffu)) * 320u, &record_at) || !add_u64(record_at, 0x34, &record_at) || !readable((const void *)(size_t)record_at, sizeof(u32))) return 0;
    return (u32 *)(size_t)record_at;
}
static int render_cache_destination(void *renderer, u32 slot, u64 *out)
{
    u64 base, target;
    if (slot >= RENDER_CACHE_MISSING || !readable((const u8 *)renderer + 0x168, sizeof base)) return 0;
    base = *(const u64 *)((const u8 *)renderer + 0x168);
    if (!add_u64(base, ((u64)(slot & 0xffffffu)) << 6, &target)) return 0;
    *out = target; return 1;
}
static int same_writable_allocation(const void *base, const void *target, size_t n)
{
    MEMORY_BASIC_INFORMATION a, b;
    if (!page_has(base, 1, 1, &a) || !page_has(target, n, 1, &b)) return 0;
    return a.AllocationBase == b.AllocationBase;
}
static void render_cache_preflight(void *renderer, u32 node)
{
    u32 *cache = render_cache_slot(renderer, node), old, now;
    u64 target;
    if (!cache || !writable(cache, sizeof *cache)) return;
    old = __atomic_load_n(cache, __ATOMIC_ACQUIRE);
    if (!render_cache_destination(renderer, old, &target) || same_writable_allocation((const void *)(size_t)(*(const u64 *)((const u8 *)renderer + 0x168)), (const void *)(size_t)target, 64)) return;
    now = old;
    if (__atomic_compare_exchange_n(cache, &now, RENDER_CACHE_MISSING, 0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE))
        __atomic_fetch_add(&g_render_cache_repairs, 1u, __ATOMIC_RELAXED);
}
static void hook_render_cache(void *renderer, u32 node, const void *matrix)
{
    render_cache_preflight(renderer, node);
    g_orig_render_cache(renderer, node, matrix);
}

// The current dump reached `movups xmm9,[rsi+rax]` at +39b52.  R10 was zero and RAX already
// held faaec630: that is an impossible scaled vertex offset from 32-bit triangle index/stride arithmetic,
// not a pointer failure.  The engine gives this query a vertex base (+78), index triples (+70, +84), stride
// (+88), two optional attribute offsets (+8c/+90), and the output's no-hit byte (+a4).  Check every
// address it will read before entering it.  On malformed geometry its documented result is simply no hit;
// no renderer object, index, or vertex data is repaired or fabricated.
static int triangle_vertex_readable(u64 base, u32 index, u32 stride)
{
    u64 at;
    u32 offset = index * stride;                           // exactly `imul r32,r32`: modulo 2^32
    return add_u64(base, (u64)offset, &at) && readable((const void *)(size_t)at, 16);
}
static int render_triangle_bad_input(void *query)
{
    const u8 *q = (const u8 *)query;
    u64 vertices, indices, attr0 = 0, attr1 = 0;
    u32 count, stride, base, flags;
    if (!q || !readable(q, 0x98) || !writable(q + 0xa4, 1)) return 0; // an unknown query remains the engine's fault
    vertices = *(const u64 *)(q + 0x78); indices = *(const u64 *)(q + 0x70);
    count = *(const u32 *)(q + 0x84); stride = *(const u32 *)(q + 0x88);
    base = *(const u32 *)(q + 0x94); flags = *(const u32 *)(q + 0x80);
    if (!count) return 0;                                  // the engine returns no hit without using +70
    if (count > 0xffffffffu / 12u || !readable((const void *)(size_t)indices, (size_t)count * 12u)) return 1;
    if ((flags & 1u) && !add_u64(vertices, *(const u32 *)(q + 0x8c), &attr0)) return 1;
    if ((flags & 2u) && !add_u64(vertices, *(const u32 *)(q + 0x90), &attr1)) return 1;
    for (u32 n = 0; n < count; n++) {
        const u32 *tri = (const u32 *)(size_t)(indices + (u64)n * 12u);
        u32 a = tri[0] + base, b = tri[1] + base, c = tri[2] + base; // exactly the engine's 32-bit additions
        if (!triangle_vertex_readable(vertices, a, stride) || !triangle_vertex_readable(vertices, b, stride) || !triangle_vertex_readable(vertices, c, stride)) return 1;
        if ((flags & 1u) && (!triangle_vertex_readable(attr0, a, stride) || !triangle_vertex_readable(attr0, b, stride) || !triangle_vertex_readable(attr0, c, stride))) return 1;
        if ((flags & 2u) && (!triangle_vertex_readable(attr1, a, stride) || !triangle_vertex_readable(attr1, b, stride) || !triangle_vertex_readable(attr1, c, stride))) return 1;
    }
    return 0;
}
static int hook_render_triangle(void *query)
{
    if (render_triangle_bad_input(query)) {
        *(u8 *)((u8 *)query + 0xa4) = 0;                  // the engine's no-hit result field
        __atomic_fetch_add(&g_render_triangle_skips, 1u, __ATOMIC_RELAXED);
        return 0;
    }
    return g_orig_render_triangle(query);
}

// A read may only race the preflight if another engine thread replaces geometry between the check and its
// corresponding instruction.  This is not a general exception handler: it recognises just those twelve
// loads and sends the already-framed query through its native no-hit epilogue.
static int render_triangle_fault(CONTEXT *ctx, const EXCEPTION_RECORD *er)
{
    u64 at, want, rip = ctx->Rip, out_at;
    if (!g_render_triangle_armed) return 0;
    if (rip == (u64)(g_render + RVA_RENDER_TRIANGLE_INDEX_0)) { at = ctx->R8; want = 0; }
    else if (rip == (u64)(g_render + RVA_RENDER_TRIANGLE_INDEX_1)) { at = ctx->R8; want = 8; }
    else if (rip == (u64)(g_render + RVA_RENDER_TRIANGLE_INDEX_2)) { at = ctx->R8; want = 4; }
    else if (rip == (u64)(g_render + RVA_RENDER_TRIANGLE_VERTEX_0)) { at = ctx->Rsi; want = ctx->Rbp; }
    else if (rip == (u64)(g_render + RVA_RENDER_TRIANGLE_VERTEX_1)) { at = ctx->Rsi; want = ctx->Rax; }
    else if (rip == (u64)(g_render + RVA_RENDER_TRIANGLE_VERTEX_2)) { at = ctx->Rsi; want = ctx->R14; }
    else if (rip == (u64)(g_render + RVA_RENDER_TRIANGLE_ATTR0_0) || rip == (u64)(g_render + RVA_RENDER_TRIANGLE_ATTR1_0)) { at = ctx->Rax; want = ctx->Rbp; }
    else if (rip == (u64)(g_render + RVA_RENDER_TRIANGLE_ATTR0_1) || rip == (u64)(g_render + RVA_RENDER_TRIANGLE_ATTR1_1)) { at = ctx->Rax; want = ctx->R14; }
    else if (rip == (u64)(g_render + RVA_RENDER_TRIANGLE_ATTR0_2) || rip == (u64)(g_render + RVA_RENDER_TRIANGLE_ATTR1_2)) { at = ctx->Rax; want = ctx->Rcx; }
    else return 0;
    if (er->ExceptionAddress != (void *)(size_t)rip || !add_u64(at, want, &at) ||
        (u64)er->ExceptionInformation[1] != at || !add_u64(ctx->Rdx, 0xa4, &out_at) || !writable((const void *)(size_t)out_at, 1)) return 0;
    ctx->Rip = (u64)(g_render + RVA_RENDER_TRIANGLE_FALLBACK);
    __atomic_fetch_add(&g_render_triangle_skips, 1u, __ATOMIC_RELAXED);
    return 1;
}

// basic_render's recursive node readers have no bound check on a child node number. The supplied dumps
// prove two exact read faults: 13adb has the valid parent material ID in EDI and returns it in EAX; 12ffd has an
// invalid child-node pointer in RDI and returns void. Both normal epilogues restore the non-volatile registers.
// The cache-write branch below is a last-resort race backup for the proactive cache reset above, not a generic AV handler.
static LONG WINAPI render_fault_guard(EXCEPTION_POINTERS *xp)
{
    EXCEPTION_RECORD *er;
    CONTEXT *ctx;
    if (!g_render || !xp || !(er = xp->ExceptionRecord) || !(ctx = xp->ContextRecord)) return EXCEPTION_CONTINUE_SEARCH;
    if (er->ExceptionCode != EXCEPTION_ACCESS_VIOLATION || er->NumberParameters < 2) return EXCEPTION_CONTINUE_SEARCH;
    if (er->ExceptionInformation[0] == 1 && g_render_cache_armed && ctx->Rip == (u64)(g_render + RVA_RENDER_CACHE_WRITE) &&
        er->ExceptionAddress == (void *)(g_render + RVA_RENDER_CACHE_WRITE)) {
        u64 target, cache_at, slot_offset = ((u64)((u32)ctx->Rdx & 0xffffffu)) << 6;
        u32 *cache;
        // This is precisely `movaps [rax + (edx & 0xffffff) * 64], xmm0`.  RBP
        // is the node record and its cache field is at +34; that field still
        // contains EDX, so branch to the engine's cache allocator, which
        // overwrites it with a fresh slot before continuing.
        if (!add_u64(ctx->Rbp, 0x34, &cache_at)) return EXCEPTION_CONTINUE_SEARCH;
        cache = (u32 *)(size_t)cache_at;
        if (add_u64(ctx->Rax, slot_offset, &target) && (u64)er->ExceptionInformation[1] == target &&
            readable(cache, sizeof *cache) && *cache == (u32)ctx->Rdx) {
            ctx->Rip = (u64)(g_render + RVA_RENDER_CACHE_REBUILD);
            __atomic_fetch_add(&g_render_cache_repairs, 1u, __ATOMIC_RELAXED);
            return EXCEPTION_CONTINUE_EXECUTION;
        }
        return EXCEPTION_CONTINUE_SEARCH;
    }
    if (er->ExceptionInformation[0] != 0) return EXCEPTION_CONTINUE_SEARCH;
    if (render_triangle_fault(ctx, er)) return EXCEPTION_CONTINUE_EXECUTION;
    if (ctx->R8 <= 0x3ff) return EXCEPTION_CONTINUE_SEARCH;       // a plausible child index is never hidden
    if (ctx->Rip == (u64)(g_render + RVA_RENDER_BAD_READ) && er->ExceptionAddress == (void *)(g_render + RVA_RENDER_BAD_READ)) {
        if (ctx->Rsi > ~(u64)0 - 0x38 || (u64)er->ExceptionInformation[1] != ctx->Rsi + 0x38 || ctx->Rdi > 0xffffff)
            return EXCEPTION_CONTINUE_SEARCH;
        ctx->Rip = (u64)(g_render + RVA_RENDER_FALLBACK);          // EDI is the valid parent material ID
    } else if (ctx->Rip == (u64)(g_render + RVA_RENDER_WALK_BAD_READ) && er->ExceptionAddress == (void *)(g_render + RVA_RENDER_WALK_BAD_READ)) {
        if (ctx->Rdi > ~(u64)0 - 0x38 || (u64)er->ExceptionInformation[1] != ctx->Rdi + 0x38)
            return EXCEPTION_CONTINUE_SEARCH;
        ctx->Rip = (u64)(g_render + RVA_RENDER_WALK_FALLBACK);     // skip only the invalid child branch
    } else {
        return EXCEPTION_CONTINUE_SEARCH;
    }
    __atomic_fetch_add(&g_render_recoveries, 1u, __ATOMIC_RELAXED);
    return EXCEPTION_CONTINUE_EXECUTION;
}

static void try_render_guard(void)
{
    if (g_render_tried) return;
    const u8 *b = (const u8 *)GetModuleHandleA("basic_render.codelib");
    if (!b) return;                                        // it can load after the first network frame
    g_render_tried = 1;
    char v[8];
    if (GetEnvironmentVariableA("MP_RENDER_GUARD", v, sizeof v) && num_of(v) == 0) {
        if (g_log) g_log("vitals: basic_render crash guard disabled by MP_RENDER_GUARD=0");
        return;
    }
    if (!readable(b, 0x400) || b[0] != 'M' || b[1] != 'Z') goto no;
    u32 pe = *(const u32 *)(b + 0x3c), stamp, image;
    if (pe > 0x300 || !readable(b + pe, 0x108) || *(const u32 *)(b + pe) != 0x4550u) goto no;
    stamp = *(const u32 *)(b + pe + 8);
    image = *(const u32 *)(b + pe + 24 + 56);
    if (stamp != RENDER_TIMESTAMP || image <= RVA_RENDER_FALLBACK + sizeof SIG_RENDER_FALLBACK ||
        image <= RVA_RENDER_WALK_FALLBACK + sizeof SIG_RENDER_WALK_FALLBACK || image <= RVA_RENDER_CACHE_REBUILD + sizeof SIG_RENDER_CACHE_REBUILD ||
        image <= RVA_RENDER_TRIANGLE_FALLBACK + sizeof SIG_RENDER_TRIANGLE_FALLBACK ||
        !same(b + RVA_RENDER_NODE, SIG_RENDER_NODE, sizeof SIG_RENDER_NODE) ||
        !same(b + RVA_RENDER_FALLBACK, SIG_RENDER_FALLBACK, sizeof SIG_RENDER_FALLBACK) ||
        !same(b + RVA_RENDER_WALK_NODE, SIG_RENDER_WALK_NODE, sizeof SIG_RENDER_WALK_NODE) ||
        !same(b + RVA_RENDER_WALK_FALLBACK, SIG_RENDER_WALK_FALLBACK, sizeof SIG_RENDER_WALK_FALLBACK) ||
        !same(b + RVA_RENDER_CACHE_SYNC, SIG_RENDER_CACHE_SYNC, sizeof SIG_RENDER_CACHE_SYNC) ||
        !same(b + RVA_RENDER_CACHE_SYNC + sizeof SIG_RENDER_CACHE_SYNC, SIG_RENDER_CACHE_BODY, sizeof SIG_RENDER_CACHE_BODY) ||
        !same(b + RVA_RENDER_CACHE_REBUILD, SIG_RENDER_CACHE_REBUILD, sizeof SIG_RENDER_CACHE_REBUILD) ||
        !same(b + RVA_RENDER_TRIANGLE, SIG_RENDER_TRIANGLE, sizeof SIG_RENDER_TRIANGLE) ||
        !same(b + RVA_RENDER_TRIANGLE_LOOP, SIG_RENDER_TRIANGLE_LOOP, sizeof SIG_RENDER_TRIANGLE_LOOP) ||
        !same(b + RVA_RENDER_TRIANGLE_ATTR0, SIG_RENDER_TRIANGLE_ATTR0, sizeof SIG_RENDER_TRIANGLE_ATTR0) ||
        !same(b + RVA_RENDER_TRIANGLE_ATTR1, SIG_RENDER_TRIANGLE_ATTR1, sizeof SIG_RENDER_TRIANGLE_ATTR1) ||
        !same(b + RVA_RENDER_TRIANGLE_FALLBACK, SIG_RENDER_TRIANGLE_FALLBACK, sizeof SIG_RENDER_TRIANGLE_FALLBACK)) goto no;
    g_render = (u8 *)b;
    g_render_handler = AddVectoredExceptionHandler(1, render_fault_guard);
    if (!g_render_handler) { g_render = 0; goto no; }
    g_orig_render_cache = (FnRenderCache)install_detour(g_render + RVA_RENDER_CACHE_SYNC, sizeof SIG_RENDER_CACHE_SYNC, (void *)hook_render_cache);
    if (g_orig_render_cache) g_render_cache_armed = 1;
    g_orig_render_triangle = (FnRenderTriangle)install_detour(g_render + RVA_RENDER_TRIANGLE, 14, (void *)hook_render_triangle);
    if (g_orig_render_triangle) g_render_triangle_armed = 1;
    if (g_log && g_render_cache_armed && g_render_triangle_armed)
        g_log("vitals: basic_render cache-lifetime, geometry-index and invalid-child guards armed (rvas %x, %x, %x, %x)",
              RVA_RENDER_CACHE_WRITE, RVA_RENDER_TRIANGLE, RVA_RENDER_WALK_BAD_READ, RVA_RENDER_BAD_READ);
    else if (g_log && g_render_triangle_armed)
        g_log("vitals: basic_render geometry-index and invalid-child guards armed; cache-lifetime repair could not be installed");
    else if (g_log && g_render_cache_armed)
        g_log("vitals: basic_render cache-lifetime and invalid-child guards armed; geometry-index preflight could not be installed");
    else if (g_log)
        g_log("vitals: basic_render invalid-child guards armed; cache-lifetime and geometry-index preflights could not be installed");
    return;
no:
    if (g_log) g_log("vitals: basic_render crash guard not armed (different build or handler unavailable)");
}

// find squad->get_member inside adv_physic.codelib: first where this build had it, otherwise by its bytes in the executable sections.
// Everything found (or not) goes to the log, so that a build that is not known can be looked at from the log alone.
static const u8 *locate_get_member(const u8 *base)
{
    const u8 *hit = 0; int hits = 0, loose = 0; u32 loose_rva[4] = {0, 0, 0, 0};
    u32 stamp = 0, size_img = 0;
    if (!readable(base, 0x400) || base[0] != 'M' || base[1] != 'Z') { if (g_log) g_log("vitals: adv_physic.codelib has no readable PE header - off"); return 0; }
    u32 pe = *(const u32 *)(base + 0x3c);
    if (pe > 0x300 || !readable(base + pe, 0x108) || *(const u32 *)(base + pe) != 0x4550u) { if (g_log) g_log("vitals: adv_physic.codelib has an odd PE header - off"); return 0; }
    stamp = *(const u32 *)(base + pe + 8);
    g_phys_size = *(const u32 *)(base + pe + 24 + 56);
    u32 nsec = *(const u16 *)(base + pe + 6), optsz = *(const u16 *)(base + pe + 20);
    size_img = *(const u32 *)(base + pe + 24 + 56);
    const u8 *sec = base + pe + 24 + optsz;
    if (nsec > 32) nsec = 32;
    if (same(base + PHYS_GET_MEMBER, SIG_GET_MEMBER, sizeof SIG_GET_MEMBER) && readable(base + PHYS_GET_MEMBER, sizeof SIG_GET_MEMBER)) { hit = base + PHYS_GET_MEMBER; hits = 1; }
    for (u32 i = 0; i < nsec && !hit; i++) {
        const u8 *h = sec + i * 40;
        if (!readable(h, 40)) break;
        u32 vsize = *(const u32 *)(h + 8), rva = *(const u32 *)(h + 12), chr = *(const u32 *)(h + 36);
        if (!(chr & 0x20000000u) || vsize < sizeof SIG_GET_MEMBER) continue;
        for (u32 o = 0; o + sizeof SIG_GET_MEMBER <= vsize; o++) {
            const u8 *p = base + rva + o;
            if ((o & 0xfff) == 0 && !readable(p, 0x1000 < vsize - o ? 0x1000 : vsize - o)) break;
            if (p[0] != 0x39 || p[1] != 0x91 || p[2] != 0x0c || p[3] != 0x19) continue;
            if (loose < 4) loose_rva[loose] = rva + o;
            loose++;
            if (same(p, SIG_GET_MEMBER, sizeof SIG_GET_MEMBER)) { if (!hits) hit = p; hits++; }
        }
    }
    if (g_log) {
        g_log("vitals: adv_physic.codelib stamp %x image %x; get_member by its bytes: %d found%s (loose matches %d: %x %x %x %x)",
              stamp, size_img, hits, hits == 1 ? "" : " - not used", loose, loose_rva[0], loose_rva[1], loose_rva[2], loose_rva[3]);
        if (hit) g_log("vitals: get_member at rva %x%s", (u32)(hit - base), (u32)(hit - base) == PHYS_GET_MEMBER ? " (as in the build this was made for)" : " (this build differs from the one this was made for)");
        else if (readable(base + PHYS_GET_MEMBER, 16)) { const u8 *q = base + PHYS_GET_MEMBER; g_log("vitals: bytes at the old place: %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x - off", q[0],q[1],q[2],q[3],q[4],q[5],q[6],q[7],q[8],q[9],q[10],q[11],q[12],q[13],q[14],q[15]); }
    }
    return hits == 1 ? hit : 0;
}

// ------------------------------------------------------------------ the soldier
#ifndef MP_TEST
static int in_phys(const void *p) { return g_phys && g_phys_size && (const char *)p >= (const char *)g_phys && (const char *)p < (const char *)g_phys + g_phys_size; }
#endif

static int soldier_id(char *o, const char **name)
{
    const char *iface = *(const char **)(o + 0x48);
    if (!iface || !str_eq(iface, "i_human")) return 0;
    *name = o + 8;
    return last_us(*name) ? 1 : 0;
}
static float self_of(const char *h) { float f; memcpy(&f, h + H_SELF, 4); return f; }
static int is_dead(float self) { return self < 0.5f; }                  // (false for a NaN)

// the life record of a soldier, asked of his squad the way the engine asks it (the squad's own get_member); 0 when there is none.
// why: 1 no squad, 2 no index, 3 squad not readable, 4 no vtable, 5 slot is not a get_member, 6 no record, 7 record not readable, 8 record is not his
enum { WHY_N = 9 };
static u32 g_why[WHY_N];
static char *rec_of(char *h, int *why)
{
    char *squad = *(char **)(h + H_SQUAD);
    int idx = *(int *)(h + H_INDEX);
    *why = 1; if (!squad) return 0;
    *why = 2; if (idx < 0) return 0;
    *why = 3; if (!readable(squad, 0x1a00)) return 0;
    void **vt = *(void ***)squad;
    *why = 4; if (!readable(vt, 0x140)) return 0;
    const u8 *fn = (const u8 *)vt[0x138 / 8];
#ifndef MP_TEST
    *why = 5; if (!fn || !in_phys(fn) || !same(fn, SIG_GET_MEMBER, sizeof SIG_GET_MEMBER)) return 0;
#endif
    char *rec = ((char *(*)(void *, u32))fn)(squad, (u32)idx);
    *why = 6; if (!rec) return 0;
    *why = 7; if (!readable(rec, M_SIZE)) return 0;
    *why = 8; if (*(char **)(rec + M_OWNER) != h) return 0;
    *why = 0; return rec;
}

// kill a soldier the way the engine does when his health falls below 0.5: the squad's record, his own health, out of the squad.
// health: his health in the other game (the statistics tell badly wounded from killed by it), rec_health: the same for the record (< 0: not known -> 0)
static void kill_soldier(char *h, float health, float rec_health)
{
    int why; char *rec = rec_of(h, &why);
    float zero = 0.f; u32 st = STATE_KILLED; void *none = 0;
    if (!(health >= 0.f && health < 0.5f)) health = 0.f;
    if (rec) {
        float rh = rec_health >= 0.f ? rec_health : zero, cur; memcpy(&cur, rec + M_HEALTH, 4);
        if (cur == cur && cur < rh) rh = cur;
        memcpy(rec + M_STATE, &st, 4); memcpy(rec + M_HEALTH, &rh, 4);
    }
    memcpy(h + H_SELF, &health, 4);
    memcpy(h + H_SQUAD, &none, 8);
    memcpy(h + H_SQUAD_COPY, &none, 8);
}

// ------------------------------------------------------------------ the tables: one row per soldier, one per vehicle (open addressing, no allocation)
enum { CAP = 8192, VCAP = 4096, NB = 9 };
typedef struct { u32 id; u8 used, dead_here, peer_dead, rec_base_set, told_dead, pad3[3]; float sent_self, sent_rec, peer_self, peer_rec, rec_base; } Unit;
static Unit g_u[CAP];
static u32 g_u_n;
static u32 slot_of(u32 id) { return (id * 2654435761u) >> 19; }          // 13 bits
static Unit *unit_for(u32 id)
{
    for (u32 i = slot_of(id), n = 0; n < CAP; i = (i + 1) & (CAP - 1), n++) {
        if (!g_u[i].used) {
            if (g_u_n >= CAP * 3 / 4) return 0;
            g_u[i].used = 1; g_u[i].id = id; g_u_n++;
            g_u[i].sent_self = 1.f; g_u[i].peer_self = 9.f; g_u[i].peer_rec = 1e30f; g_u[i].sent_rec = 1e30f; g_u[i].rec_base = 1e30f;
            return &g_u[i];
        }
        if (g_u[i].id == id) return &g_u[i];
    }
    return 0;
}
// a vehicle: sent_mask / peer_nz = which of the NB status bytes are not 0 (last sent / last told by the other game)
typedef struct { u32 id; u8 used, pad; u16 sent_mask, peer_nz, pending; u8 pend_val[NB]; u8 pad2[3]; } Veh;
static Veh g_v[VCAP];
static u32 g_v_n;
static Veh *veh_for(u32 id)
{
    for (u32 i = (id * 2654435761u) >> 20, n = 0; n < VCAP; i = (i + 1) & (VCAP - 1), n++) {
        if (!g_v[i].used) { if (g_v_n >= VCAP * 3 / 4) return 0; g_v[i].used = 1; g_v[i].id = id; g_v_n++; return &g_v[i]; }
        if (g_v[i].id == id) return &g_v[i];
    }
    return 0;
}
static const u16 VB[NB] = { 0xd07, 0xd08, 0xd09, 0xd0a, 0xd0b, 0xd0c, 0xd0e, 0xd0f, 0xd10 };
static int veh_id(char *o, const char **name, u32 *id)
{
    const char *iface = *(const char **)(o + 0x48);
    if (!iface || !str_eq(iface, "i_techn")) return 0;
    *name = o + 8;
    u32 h = 2166136261u;
    for (const char *p = *name; *p && p < *name + 64; p++) h = (h ^ (u8)*p) * 16777619u;
    *id = h;
    return 1;
}
static void veh_read(const char *o, u8 *b) { for (int i = 0; i < NB; i++) b[i] = (u8)o[VB[i]]; }

// ------------------------------------------------------------------ v7.1 state mailbox and acknowledged snapshot channel
// State entries are monotonic: retransmitting an accepted packet cannot revive or heal a unit.  The protocol still
// requires an application-level ACK, because the original sender only gives us a fire-and-forget P2P call.  A packet
// is acknowledged only after it was copied to this mailbox (not merely after its bytes reached Steam).
enum { BOX = 128, PKT_MAX = sizeof(VitalsHead) + PER_PACKET * sizeof(Entry), OUTCAP = 6144,
       VITAL_TX_CAP = 32, VITAL_SEEN_CAP = 128, VITAL_RETRY_MS = 400, VITAL_MAX_RETRIES = 12 };
static CRITICAL_SECTION g_cs;
static struct { int size; u8 data[PKT_MAX]; } g_box[BOX];
static int g_box_head, g_box_n;

typedef struct { u8 used, retries; u16 size; u32 epoch, seq; DWORD sent_at; u8 data[PKT_MAX]; } VitalsTx;
typedef struct { u32 epoch, seq; } VitalsSeen;
static VitalsTx g_vtx[VITAL_TX_CAP];
static VitalsSeen g_vseen[VITAL_SEEN_CAP];
static u32 g_vseen_at, g_v_epoch, g_v_seq, g_v_epoch_nonce;

static u32 g_s_hurt, g_s_hurt_peer, g_s_merged, g_s_merged_rec, g_s_differ, g_s_veh, g_s_veh_flag, g_s_veh_peer, g_s_veh_set, g_s_veh_note, g_veh_samples;
static int g_veh_broken;
static u32 g_s_seen, g_s_dead, g_s_peer_dead, g_s_sent_delta, g_s_sent_digest, g_s_rx, g_s_killed, g_s_notice, g_s_dropped, g_s_rec_ok, g_s_rec_bad;
static u32 g_s_vtx, g_s_vack, g_s_vretry, g_s_vreplace, g_s_vrx, g_s_vdup, g_s_vfull, g_s_vack_sent, g_s_vack_unknown;
static u32 g_s_final_scans;
static int g_san_seen, g_san_good, g_rec_samples, g_start_checked;
static int g_wait_numbers, g_force_scan;
static DWORD g_on_since;

static void vitals_transport_reset(void)
{
    memset(g_vtx, 0, sizeof g_vtx);
    memset(g_vseen, 0, sizeof g_vseen);
    g_vseen_at = g_v_epoch = g_v_seq = 0;
}
static void vitals_transport_begin(DWORD now)
{
    // An epoch distinguishes a new tactical connection from delayed packets of the previous one.  It is not a
    // security token; peer-address validation below remains the authority check.
    g_v_epoch_nonce += 0x9e3779b9u;
    g_v_epoch = now ^ g_v_epoch_nonce;
    if (!g_v_epoch) g_v_epoch = 1;
    g_v_seq = 0;
}
static int vitals_pending(void)
{
    int n = 0;
    for (int i = 0; i < VITAL_TX_CAP; i++) if (g_vtx[i].used) n++;
    return n;
}
static int vitals_tx_slot(void)
{
    int old = 0;
    for (int i = 0; i < VITAL_TX_CAP; i++) {
        if (!g_vtx[i].used) return i;
        if (g_vtx[i].sent_at < g_vtx[old].sent_at) old = i;
    }
    // A fresh complete digest will supersede an unacknowledged old delta.  Replacing the oldest is bounded and
    // prevents a dead peer from consuming unbounded process memory.
    g_vtx[old].used = 0;
    g_s_vreplace++;
    return old;
}
static void vitals_send_tx(VitalsTx *tx, DWORD now, int retry)
{
    g_send(tx->data, (u32)tx->size);
    tx->sent_at = now;
    if (retry) { tx->retries++; g_s_vretry++; }
    else g_s_vtx++;
}
static int send_entries(const Entry *e, int n, int delta, DWORD now)
{
    int done = 0;
    if (!g_v_epoch) vitals_transport_begin(now);
    for (int at = 0; at < n; at += PER_PACKET) {
        int c = n - at < PER_PACKET ? n - at : PER_PACKET;
        VitalsTx *tx = &g_vtx[vitals_tx_slot()];
        VitalsHead h = { { MAGIC, PKT_VITALS, (u8)(delta ? F_DELTA : 0), (u16)c }, g_v_epoch, ++g_v_seq };
        if (!h.seq) h.seq = ++g_v_seq;                   // reserve zero for malformed/empty headers
        tx->used = 1; tx->retries = 0; tx->epoch = h.epoch; tx->seq = h.seq;
        tx->size = (u16)(sizeof h + (size_t)c * sizeof(Entry));
        memcpy(tx->data, &h, sizeof h);
        memcpy(tx->data + sizeof h, e + at, (size_t)c * sizeof(Entry));
        vitals_send_tx(tx, now, 0);
        done += c;
    }
    return done;
}
static void vitals_transport_tick(DWORD now)
{
    for (int i = 0; i < VITAL_TX_CAP; i++) {
        VitalsTx *tx = &g_vtx[i];
        if (!tx->used || now - tx->sent_at < VITAL_RETRY_MS) continue;
        if (tx->retries >= VITAL_MAX_RETRIES) { tx->used = 0; g_s_vreplace++; continue; }
        vitals_send_tx(tx, now, 1);
    }
}
static int vitals_seen_before(u32 epoch, u32 seq)
{
    for (int i = 0; i < VITAL_SEEN_CAP; i++) if (g_vseen[i].epoch == epoch && g_vseen[i].seq == seq) return 1;
    return 0;
}
static void vitals_remember(u32 epoch, u32 seq)
{
    g_vseen[g_vseen_at % VITAL_SEEN_CAP].epoch = epoch;
    g_vseen[g_vseen_at % VITAL_SEEN_CAP].seq = seq;
    g_vseen_at++;
}
static void vitals_send_ack(u32 epoch, u32 seq)
{
    VitalsHead ack = { { MAGIC, PKT_VITALS_ACK, 0, 0 }, epoch, seq };
    g_send(&ack, sizeof ack);
    g_s_vack_sent++;
}
static void vitals_take_ack(const VitalsHead *ack)
{
    if (ack->epoch != g_v_epoch || !ack->seq) { g_s_vack_unknown++; return; }
    for (int i = 0; i < VITAL_TX_CAP; i++) {
        VitalsTx *tx = &g_vtx[i];
        if (tx->used && tx->epoch == ack->epoch && tx->seq == ack->seq) {
            tx->used = 0; g_s_vack++; return;
        }
    }
    // An ACK can race a local timeout or be a duplicate.  It must never acknowledge another packet by position.
    g_s_vack_unknown++;
}

static void reset_all(void)
{
    memset(g_u, 0, sizeof g_u); g_u_n = 0; memset(g_v, 0, sizeof g_v); g_v_n = 0; g_veh_broken = 0;
    g_num_since = g_num_last = g_nudges = 0; g_num_complete = g_num_peer_set = 0; g_wait_numbers = 0; g_force_scan = 0;
    vitals_transport_reset();
    EnterCriticalSection(&g_cs); g_box_n = 0; g_box_head = 0; LeaveCriticalSection(&g_cs);
}

static void drain_mailbox(void)
{
    for (;;) {
        u8 data[PKT_MAX];
        int size = 0;
        EnterCriticalSection(&g_cs);
        if (g_box_n) {
            size = g_box[g_box_head].size;
            memcpy(data, g_box[g_box_head].data, (size_t)size);
            g_box_head = (g_box_head + 1) % BOX;
            g_box_n--;
        }
        LeaveCriticalSection(&g_cs);
        if (!size) return;
        const VitalsHead *h = (const VitalsHead *)data;
        const Entry *e = (const Entry *)(data + sizeof(VitalsHead));
        g_s_rx += h->h.count;
        for (int i = 0; i < h->h.count; i++) {
            u32 fl = e[i].flags;
            if (fl & E_VEH) {
                Veh *v = veh_for(e[i].id);
                if (!v) { g_s_dropped++; continue; }
                u8 b[NB]; memcpy(b, &e[i].a, 4); memcpy(b + 4, &e[i].b, 4); b[8] = (u8)(fl >> 8);
                for (int k = 0; k < NB; k++) {
                    if (b[k] && !(v->peer_nz & (1u << k))) { v->pending |= (u16)(1u << k); v->pend_val[k] = b[k]; }
                    if (b[k]) v->peer_nz |= (u16)(1u << k); else v->peer_nz &= (u16)~(1u << k);
                }
                continue;
            }
            Unit *u = unit_for(e[i].id);
            if (!u) { g_s_dropped++; continue; }
            if (fl & E_DEAD) u->peer_dead = 1;
            if (fl & E_HURT) { float f; memcpy(&f, &e[i].a, 4); if (f == f && f >= 0.f && f < u->peer_self) u->peer_self = f; if (f < 0.5f) u->peer_dead = 1; }
            if (fl & E_REC) { float f; memcpy(&f, &e[i].b, 4); if (f == f && f >= 0.f && f < u->peer_rec) u->peer_rec = f; }
        }
    }
}

// ------------------------------------------------------------------ the frame (after the module's own frame, so its replayed hits are done)
static int g_on;
// 1 means attach selected the safe native position-only policy for a detected tactical operation;
// it becomes 2 once the normal module logger is available and the fact has been recorded.
static int g_operation_state_policy;
static void *g_om;
static int g_veh_checked;
static u16 g_vb_off;                // status bytes that are on in many vehicles from the start: not event flags, not synchronised
static u32 g_scan_no;

static int veh_mode(int m)
{
    static int forced = -2;
    if (forced == -2) {
        char v[8];
        forced = GetEnvironmentVariableA("MP_VEHICLES", v, sizeof v) ? num_of(v) : -1;
    }
    if (m == 0) return 0;
    if (forced >= 0) return forced > 2 ? 2 : forced;
    return m >= 2 ? 2 : 1;
}
static float fdiff(float a, float b) { float d = a - b; return d < 0.f ? -d : d; }
static int write_float_checked(char *p, float v) { memcpy(p, &v, 4); float r; memcpy(&r, p, 4); return r == v; }

static void try_hook_stat(void);
static DWORD g_last_on;
static volatile u32 g_rx_type[64], g_rx_size[64];

// Only Vitals types 0x34/0x35 have transport logic below. The pre-existing native reliable stream (types 10/11) is not
// inspected for recovery and its receive/ACK state is never written by this DLL.
static int g_idle_msg = -1;
static int wsprintfA_lite(char *out, u32 t, u32 c, u32 b)
{
    char tmp[40]; int k = 0, n = 0;
    u32 v[3] = { t, c, b }; const char sep[3] = { ':', '/', ' ' };
    for (int f = 0; f < 3; f++) {
        char d[12]; int dl = 0; u32 x = v[f];
        do { d[dl++] = (char)('0' + x % 10); x /= 10; } while (x);
        while (dl) tmp[k++] = d[--dl];
        tmp[k++] = sep[f];
    }
    for (int j = 0; j < k; j++) out[n++] = tmp[j];
    out[n] = 0; return n;
}

static void vitals_frame(void *om)
{
    static DWORD last_scan, last_digest, last_vdigest, last_report;
    if (g_broken) return;
    if (!g_phys && (g_phys = GetModuleHandleA("adv_physic.codelib")) != 0) {
        if (!locate_get_member((const u8 *)g_phys) && g_log) g_log("vitals: get_member not found - the squad's own record is not used, soldiers are killed without it");
    }
    try_hook_stat();
    try_render_guard();
    if (g_log) {
        if (g_operation_state_policy == 1) {
            g_log("vitals: tactical operation detected; full object-state replication is disabled (native position, shots, hits and Vitals remain)");
            g_operation_state_policy = 2;
        }
        if (g_idle_msg == 0) { g_log("net: units that stand still are sent on every tick (MP_IDLE_MS=0 override)"); g_idle_msg = -2; }
        else if (g_idle_msg > 0) { g_log("net: units that stand still are sent every %d ms (patched at start)", g_idle_msg); g_idle_msg = -2; }
        u32 render_recoveries = __atomic_load_n(&g_render_recoveries, __ATOMIC_RELAXED);
        if (render_recoveries != g_render_reported) {
            g_render_reported = render_recoveries;
            g_log("vitals: skipped an invalid recursive basic_render node (%u time%s); parent material used", g_render_reported, g_render_reported == 1 ? "" : "s");
        }
        u32 triangle_skips = __atomic_load_n(&g_render_triangle_skips, __ATOMIC_RELAXED);
        if (triangle_skips != g_render_triangle_reported) {
            g_render_triangle_reported = triangle_skips;
            g_log("vitals: skipped %u malformed basic_render geometry read%s through the engine's no-hit path", triangle_skips, triangle_skips == 1 ? "" : "s");
        }
        u32 cache_repairs = __atomic_load_n(&g_render_cache_repairs, __ATOMIC_RELAXED);
        if (cache_repairs != g_render_cache_reported) {
            g_render_cache_reported = cache_repairs;
            g_log("vitals: rebuilt %u stale basic_render transform cache entr%s through the engine allocator", cache_repairs, cache_repairs == 1 ? "y" : "ies");
        }
    }
    int on = *g_connected && *g_tactical && mode() > 0 && *g_have_peer;
    if (!on) { if (g_on) { reset_all(); g_on = 0; } return; }
    DWORD now = GetTickCount();
    g_last_on = now ? now : 1;
    // A new object manager or selected peer is a new tactical session. A connection loss is handled above.
    // Do not carry a completed number table into either case.
    if (!g_on || om != g_om || (g_num_peer_set && memcmp(g_num_peer, g_peer, sizeof g_num_peer))) {
        reset_all();
        vitals_transport_begin(now);
        memcpy(g_num_peer, g_peer, sizeof g_num_peer); g_num_peer_set = 1;
        g_om = om; g_on = 1; g_on_since = now; last_scan = 0; last_digest = last_vdigest = now;
        g_san_seen = g_san_good = g_rec_samples = g_start_checked = g_veh_checked = 0; g_vb_off = 0; g_veh_samples = 0; g_scan_no = 0;
        if (g_log) g_log("vitals: on (mode %d, vehicles %d)", mode(), veh_mode(mode()));
    }
    {   // The native exchange is a prerequisite, not a best-effort diagnostic. Its raw ready flag, however,
        // is deliberately reused by v20 for later transfer cycles and pulses low after the first success.
        // Latch the first completed exchange for *this* object-manager session instead of forcing another
        // full description/object-table walk every three seconds.
        if (!g_num_complete) {
            if (*g_num_ready) {
                g_num_complete = 1;
                g_num_since = 0;
            } else {
                if (!g_num_since) g_num_since = now;
                if (now - g_num_last >= 3000) {
                    *g_num_flag = 1; g_num_last = now; g_nudges++;
                    if (g_log && g_nudges <= 5) g_log("vitals: requested object-number exchange (waiting %u s)", (now - g_num_since) / 1000);
                }
            }
        }
    }
    // Every remote saved-state field is expressed in the sender's object/description-number space.
    // Do not let Vitals get ahead of the first exact translation handshake. On the receiver side a pre-ready
    // Vitals packet is deliberately not ACKed (see hook_packet), so a current peer retains it instead of
    // silently considering an unsafe state delivered.
    if (!g_num_complete) {
        if (!g_wait_numbers && g_log) g_log("vitals: waiting for object-number exchange before tactical state sync");
        g_wait_numbers = 1;
        return;
    }
    if (g_wait_numbers) {
        g_wait_numbers = 0;
        if (g_log) g_log("vitals: object-number exchange complete; tactical state sync enabled");
    }
    drain_mailbox();
    vitals_transport_tick(now);
    int m = mode(), vm = veh_mode(m);
    if (g_force_scan || now - last_scan >= 50) {
        g_force_scan = 0;
        last_scan = now; g_scan_no++;
        int digest = now - last_digest >= 2000;
        if (digest) last_digest = now;
        int grace = now - g_on_since >= 1500;
        int do_veh = vm > 0 && !g_veh_broken && (g_scan_no % 4) == 0;
        int vdigest = do_veh && now - last_vdigest >= 2000;
        if (vdigest) last_vdigest = now;
        static Entry out_delta[OUTCAP], out_digest[OUTCAP];
        int nd = 0, ng = 0;
        u32 count = *(u32 *)((char *)om + 0xfc);
        void **all = *(void ***)((char *)om + 0xf0);
        u32 seen = 0, dead = 0, hurt = 0, differ = 0, veh_n = 0, veh_nz = 0, vb_nz[NB] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        for (u32 i = 0; i < count; i++) {
            char *o = (char *)all[i];
            const char *name;
            if (!o) continue;
            if (!soldier_id(o, &name)) {
                u32 vid;
                if (!do_veh || !veh_id(o, &name, &vid) || !readable(o + 0xd00, 0x20)) continue;
                Veh *v = veh_for(vid);
                if (!v) { g_s_dropped++; continue; }
                u8 b[NB]; u16 mask = 0;
                veh_read(o, b);
                { int any = 0; for (int k = 0; k < NB; k++) if (b[k]) { vb_nz[k]++; any = 1; } veh_n++; if (any) veh_nz++; }
                if (!g_veh_checked) continue;                       // the first 3 s only look: which bytes are on from the start
                for (int k = 0; k < NB; k++) { if (g_vb_off & (1u << k)) b[k] = 0; if (b[k]) mask |= (u16)(1u << k); }
                if ((mask != v->sent_mask || (vdigest && mask)) && (nd < OUTCAP && ng < OUTCAP)) {
                    Entry en; en.id = vid; en.flags = E_VEH | ((u32)b[8] << 8); memcpy(&en.a, b, 4); memcpy(&en.b, b + 4, 4);
                    if (mask != v->sent_mask) { out_delta[nd++] = en; v->sent_mask = mask; if (g_veh_samples < 60 && g_log) { g_veh_samples++; g_log("vitals: vehicle %s: status bytes +d07.. now %02x %02x %02x %02x %02x %02x %02x %02x %02x here", name, b[0], b[1], b[2], b[3], b[4], b[5], b[6], b[7], b[8]); } }
                    else out_digest[ng++] = en;
                }
                if (v->pending && grace) {
                    for (int k = 0; k < NB; k++) {
                        if (!(v->pending & (1u << k))) continue;
                        v->pending &= (u16)~(1u << k);
                        if (b[k]) continue;                         // already on here
                        if (vm >= 2) {
                            o[VB[k]] = (char)v->pend_val[k];
                            if ((u8)o[VB[k]] != v->pend_val[k]) { g_veh_broken = 1; if (g_log) g_log("vitals: the status byte of %s is not written through - vehicles off", name); break; }
                            v->sent_mask |= (u16)(1u << k);
                            g_s_veh_set++;
                            if (g_veh_samples < 40 && g_log) { g_veh_samples++; g_log("vitals: vehicle %s: status byte +%x set to %u here, it is set in the other game", name, VB[k], v->pend_val[k]); }
                        } else {
                            g_s_veh_note++;
                            if (g_veh_samples < 40 && g_log) { g_veh_samples++; g_log("vitals: vehicle %s: status byte +%x is %u in the other game, 0 here", name, VB[k], v->pend_val[k]); }
                        }
                    }
                }
                continue;
            }
            u32 id = hex_of(last_us(name) + 1);
            float self = self_of(o);
            seen++;
            if (g_san_seen < 40) {                       // is +0x7c8 a health at all? (a float between 0 and 2)
                g_san_seen++; if (self >= 0.f && self <= 2.f) g_san_good++;
                if (g_san_seen == 40) {
                    if (g_san_good * 10 < g_san_seen * 9) { g_broken = 1; if (g_log) g_log("vitals: %d of %d soldiers have no sensible health where it is expected - off", g_san_seen - g_san_good, g_san_seen); return; }
                    if (g_log) g_log("vitals: soldier health found where expected (%d of %d)", g_san_good, g_san_seen);
                }
            }
            Unit *u = unit_for(id);
            if (!u) { g_s_dropped++; continue; }
            if (is_dead(self)) {
                dead++;
                if (!u->dead_here) {
                    u->dead_here = 1;
                    if (log_events() && g_log) g_log("vitals: dead here: %s%s", name, u->peer_dead ? " (the other game has said so already)" : "");
                }
                // the exact health decides killed or badly wounded in the statistics: the lower of the two games' values
                if (grace && m >= 2 && u->peer_self < self - 0.005f) {
                    if (!write_float_checked(o + H_SELF, u->peer_self)) { g_broken = 1; if (g_log) g_log("vitals: the health of %s is not written through - off", name); return; }
                    if (log_events() && g_log) g_log("vitals: %s: health of the dead %.3f -> %.3f here", name, self, u->peer_self);
                    self = u->peer_self; g_s_merged++;
                    if (self < u->sent_self) u->sent_self = self;
                }
                int need = !(u->peer_self < 8.f && u->peer_self <= self + 0.005f) && !(u->peer_dead && u->peer_self >= 8.f);        // the other game has this value or a lower one already: nothing to tell
                if (need && (self < u->sent_self - 0.005f || !u->told_dead) && nd < OUTCAP) {
                    out_delta[nd].id = id; out_delta[nd].flags = E_DEAD | E_HURT; memcpy(&out_delta[nd].a, &self, 4); out_delta[nd].b = 0; nd++;
                    u->sent_self = self; u->told_dead = 1;
                } else if (need && digest && ng < OUTCAP) {
                    out_digest[ng].id = id; out_digest[ng].flags = E_DEAD | E_HURT; memcpy(&out_digest[ng].a, &self, 4); out_digest[ng].b = 0; ng++;
                }
                continue;
            }
            u->dead_here = 0;
            if (u->peer_dead) {
                if (m >= 2 && grace) {
                    int why; char *rec = rec_of(o, &why);
                    if (rec) g_s_rec_ok++; else { g_s_rec_bad++; g_why[why]++; }
                    if (!rec && why != 1 && g_rec_samples < 6 && g_log) {
                        g_rec_samples++;
                        g_log("vitals: no squad record for %s (why %d): squad %x:%x index %d health %.2f", name, why, (u32)((u64)*(char **)(o + H_SQUAD) >> 32), (u32)(u64)*(char **)(o + H_SQUAD), *(int *)(o + H_INDEX), self);
                    }
                    kill_soldier(o, u->peer_self < 8.f ? u->peer_self : 0.f, u->peer_rec < 1e29f ? u->peer_rec : -1.f);
                    u->sent_self = self_of(o); u->told_dead = 1;
                    if (!is_dead(self_of(o))) { g_broken = 1; if (g_log) g_log("vitals: the health of %s is not written through - off", name); return; }
                    g_s_killed++;
                    if (log_events() && g_log) g_log("vitals: killed %s here, he is dead in the other game%s", name, rec ? "" : " (without a squad record)");
                } else if (m == 1) { g_s_notice++; if (log_events() && g_log) g_log("vitals: %s is dead in the other game, alive here", name); }
                continue;
            }
            // ---- alive here, alive in the other game as far as is known: the lower health of the two games wins
            int hurt_here = self < 0.999f;
            int need_rec = 0;           // v5.1: the record's health is not merged: the engine works it out anew every frame (the log of v5 showed ~4000 rewrites per 5 s)
            char *rec = 0; float rh = 0.f; int have_rec = 0;
            if (need_rec) {
                int why;
                rec = rec_of(o, &why);
                if (rec) { memcpy(&rh, rec + M_HEALTH, 4); have_rec = rh == rh && rh >= 0.f && rh < 1e6f; }
            }
            if (have_rec) {
                if (!u->rec_base_set) { u->rec_base = rh; u->rec_base_set = 1; }
                else if (rh > u->rec_base) u->rec_base = rh;
            }
            if (grace && m >= 2) {
                if (u->peer_self < self - 0.005f && u->peer_self >= 0.5f) {
                    if (!write_float_checked(o + H_SELF, u->peer_self)) { g_broken = 1; if (g_log) g_log("vitals: the health of %s is not written through - off", name); return; }
                    if (log_events() && g_log) g_log("vitals: %s: health %.3f -> %.3f here (that is what the other game has)", name, self, u->peer_self);
                    self = u->peer_self; u->sent_self = self; g_s_merged++;
                }
            }
            int rec_hurt = have_rec && rh < u->rec_base - 0.005f;
            if (u->peer_self < 8.f && fdiff(self, u->peer_self) > 0.05f) differ++;
            if (hurt_here || rec_hurt) {
                hurt++;
                u32 fl = E_HURT | (rec_hurt ? E_REC : 0);
                int changed = self < u->sent_self - 0.005f || (rec_hurt && rh < u->sent_rec - 0.005f);
                if (changed && nd < OUTCAP) {
                    out_delta[nd].id = id; out_delta[nd].flags = fl; memcpy(&out_delta[nd].a, &self, 4); memcpy(&out_delta[nd].b, &rh, 4); nd++;
                    u->sent_self = self; if (rec_hurt) u->sent_rec = rh;
                } else if (digest && ng < OUTCAP) {
                    out_digest[ng].id = id; out_digest[ng].flags = fl; memcpy(&out_digest[ng].a, &self, 4); memcpy(&out_digest[ng].b, &rh, 4); ng++;
                }
            }
        }
        g_s_seen = seen; g_s_dead = dead; g_s_hurt = hurt; g_s_differ = differ;
        if (do_veh) { g_s_veh = veh_n; g_s_veh_flag = veh_nz; }
        g_s_peer_dead = 0; g_s_hurt_peer = 0;
        for (u32 i = 0; i < CAP; i++) if (g_u[i].used) { if (g_u[i].peer_dead) g_s_peer_dead++; else if (g_u[i].peer_self < 8.f) g_s_hurt_peer++; }
        g_s_veh_peer = 0;
        for (u32 i = 0; i < VCAP; i++) if (g_v[i].used && g_v[i].peer_nz) g_s_veh_peer++;
        if (nd) g_s_sent_delta += (u32)send_entries(out_delta, nd, 1, now);
        if (ng) g_s_sent_digest += (u32)send_entries(out_digest, ng, 0, now);
        // a dead share this large at the start means that the health is not what it was taken for
        if (!g_start_checked && now - g_on_since >= 3000) {
            g_start_checked = 1;
            if (seen >= 20 && dead * 100 > seen * 30) { g_broken = 1; if (g_log) g_log("vitals: %u of %u soldiers look dead at the start - off", dead, seen); return; }
        }
        // likewise for the vehicles' status bytes: a byte that is on in more than a fifth of the vehicles from the start is a property, not an event
        if (do_veh && !g_veh_checked && now - g_on_since >= 3000) {
            g_veh_checked = 1;
            if (veh_n >= 10) for (int k = 0; k < NB; k++) if (vb_nz[k] * 100 > veh_n * 20) g_vb_off |= (u16)(1u << k);
            if (g_log) g_log("vitals: vehicles %u, status bytes on at the start (+d07 +d08 +d09 +d0a +d0b +d0c +d0e +d0f +d10): %u %u %u %u %u %u %u %u %u; not synchronised (mask): %x",
                             veh_n, vb_nz[0], vb_nz[1], vb_nz[2], vb_nz[3], vb_nz[4], vb_nz[5], vb_nz[6], vb_nz[7], vb_nz[8], g_vb_off);
        }
    }
    if (now - last_report >= 5000) {
        last_report = now;
        // Per-five-second counters are useful when diagnosing a desync, but writing three long lines through
        // the game's synchronous logger during normal play adds needless disk work.  Events and errors above
        // remain visible; MP_LOG_VITALS=1 opts into this aggregate telemetry.
        if (log_events() && g_log) {
            g_log("vitals: 5 s: mode %d, soldiers %u (dead here %u, dead in the other game %u; hurt here %u, in the other game %u, lowered here %u + record %u, still apart %u), "
                  "sent %u at once + %u in lists, received %u entries; killed here %u, only noticed %u, table full %u; squad records found %u, not %u (why 1..8: %u %u %u %u %u %u %u %u); "
                  "vehicles %u (flags on here %u, in the other game %u), switched on here %u, only noticed %u",
                  m, g_s_seen, g_s_dead, g_s_peer_dead, g_s_hurt, g_s_hurt_peer, g_s_merged, g_s_merged_rec, g_s_differ,
                  g_s_sent_delta, g_s_sent_digest, g_s_rx, g_s_killed, g_s_notice, g_s_dropped, g_s_rec_ok, g_s_rec_bad,
                  g_why[1], g_why[2], g_why[3], g_why[4], g_why[5], g_why[6], g_why[7], g_why[8],
                  g_s_veh, g_s_veh_flag, g_s_veh_peer, g_s_veh_set, g_s_veh_note);
            g_log("vitals: state channel v7.5: packets sent %u, acked %u, retried %u, replaced %u, pending %d; received %u, duplicates %u, inbox full %u, ACK sent %u, unmatched ACK %u; final scans %u",
                  g_s_vtx, g_s_vack, g_s_vretry, g_s_vreplace, vitals_pending(), g_s_vrx, g_s_vdup, g_s_vfull, g_s_vack_sent, g_s_vack_unknown, g_s_final_scans);
            char tl[400]; int n = 0; tl[0] = 0;
            for (int t = 0; t < 64 && n < 360; t++) if (g_rx_type[t]) n += wsprintfA_lite(tl + n, t, g_rx_type[t], g_rx_size[t]);
            if (n) g_log("rx: datagrams of the other game by type (type:count/bytes) %s", tl);
            g_log("vitals: object-number exchange latched: %s (requested %u times before first completion)", g_num_complete ? "yes" : "NO", g_nudges);
        }
        memset((void *)g_rx_type, 0, sizeof g_rx_type); memset((void *)g_rx_size, 0, sizeof g_rx_size);
        g_s_sent_delta = g_s_sent_digest = g_s_rx = g_s_killed = g_s_notice = g_s_dropped = g_s_rec_ok = g_s_rec_bad = g_s_merged = g_s_merged_rec = g_s_veh_set = g_s_veh_note = 0;
        g_s_vtx = g_s_vack = g_s_vretry = g_s_vreplace = g_s_vrx = g_s_vdup = g_s_vfull = g_s_vack_sent = g_s_vack_unknown = g_s_final_scans = 0;
        memset(g_why, 0, sizeof g_why);
    }
}

// ------------------------------------------------------------------ hooks
static void hook_frame(void *om, void *a2, u64 a3, void *a4)
{
    g_orig_frame(om, a2, a3, a4);
    if (om && !g_broken) vitals_frame(om);
}

static void hook_packet(const void *from, const unsigned char *data, int size, u32 now)
{
    if (size < 8) return;                               // what the module's own first check does (the hook sits behind it)
    const Head *h = (const Head *)data;
    if (h->magic == MAGIC && h->type < 64) { g_rx_type[h->type]++; g_rx_size[h->type] += (u32)size; }   // diagnostics only; native types below are passed through unchanged
    if (h->magic == MAGIC && h->type == PKT_VITALS_ACK) {
        if (*g_have_peer && !memcmp(from, g_peer, 16) && size == (int)sizeof(VitalsHead) && h->count == 0 && h->flags == 0) {
            const VitalsHead *ack = (const VitalsHead *)data;
            vitals_take_ack(ack);
        }
        return;                                           // our reserved packet is never handed to the original module
    }
    if (h->magic == MAGIC && h->type == PKT_VITALS) {
        // Only accept a whole v7.1 packet from the selected peer.  The ACK comes after a successful mailbox copy;
        // a full mailbox deliberately leaves the packet unacknowledged so the sender retries it.
        int queued = 0, duplicate = 0;
        if (*g_have_peer && !memcmp(from, g_peer, 16) && size >= (int)sizeof(VitalsHead) && h->count > 0 && h->count <= PER_PACKET &&
            !(h->flags & ~F_DELTA) && size == (int)(sizeof(VitalsHead) + (size_t)h->count * sizeof(Entry)) && mode() > 0 && g_num_complete) {
            const VitalsHead *vh = (const VitalsHead *)data;
            if (vh->epoch && vh->seq) {
                EnterCriticalSection(&g_cs);
                duplicate = vitals_seen_before(vh->epoch, vh->seq);
                if (!duplicate && g_box_n < BOX) {
                    int at = (g_box_head + g_box_n) % BOX;
                    g_box[at].size = size;
                    memcpy(g_box[at].data, data, (size_t)size);
                    g_box_n++;
                    vitals_remember(vh->epoch, vh->seq);
                    queued = 1;
                }
                LeaveCriticalSection(&g_cs);
                if (duplicate) { g_s_vdup++; vitals_send_ack(vh->epoch, vh->seq); }
                else if (queued) { g_s_vrx++; vitals_send_ack(vh->epoch, vh->seq); }
                else g_s_vfull++;
            }
        }
        return;
    }
    g_orig_packet(from, data, size, now);
}

// Replaces the first `len` bytes of fn with a jump to hook; returns the trampoline that runs them and continues.
static void *install_detour(u8 *fn, size_t len, void *hook)
{
    if (len < 14 || len > 32) return 0;
    u8 *tramp = (u8 *)VirtualAlloc(0, 64, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!tramp) return 0;
    static const u8 jmp[6] = { 0xff, 0x25, 0, 0, 0, 0 };
    void *back = fn + len;
    memcpy(tramp, fn, len);
    memcpy(tramp + len, jmp, 6);
    memcpy(tramp + len + 6, &back, 8);
    DWORD old;
    if (!VirtualProtect(fn, len, PAGE_EXECUTE_READWRITE, &old)) { VirtualFree(tramp, 0, MEM_RELEASE); return 0; }
    u8 patch[32];
    memset(patch, 0x90, sizeof patch);
    memcpy(patch, jmp, 6);
    memcpy(patch + 6, &hook, 8);
    memcpy(fn, patch, len);
    VirtualProtect(fn, len, old, &old);
    FlushInstructionCache(GetCurrentProcess(), fn, len);
    FlushInstructionCache(GetCurrentProcess(), tramp, 64);
    return tramp;
}

// ---- the statistics' dice: one value in both games right before the end-of-battle statistics are worked out
static int stats_mode(void)
{
    static int forced = -2;
    if (forced == -2) { char v[8]; forced = GetEnvironmentVariableA("MP_STATS", v, sizeof v) ? num_of(v) : -1; }
    if (forced >= 0) return forced != 0;
    return 1;
}
static void hook_stat(void *obj, long long a2, char a3)
{
    // This call is the last common point before operation casualties are rolled up.  Force the normal,
    // monotonic scan once more so a death or a lower health from the last combat frame is published before
    // the deterministic statistics seed is used.  It never bypasses the object-number prerequisite.
    if (!g_broken && g_on && g_om && g_num_complete) {
        g_force_scan = 1;
        vitals_frame(g_om);
        g_s_final_scans++;
        if (g_log) g_log("vitals: final tactical state scan before battle statistics");
    }
    if (!g_broken && g_last_on && GetTickCount() - g_last_on < 120000 && mode() >= 2 && stats_mode() && readable((char *)obj + STAT_SEED_OFF, 4)) {
        u32 *seed = (u32 *)((char *)obj + STAT_SEED_OFF);
        if (g_log) g_log("vitals: statistics start: their dice were at %08x, now %08x in both games", *seed, (u32)STAT_SEED);
        *seed = STAT_SEED; g_stat_seeded++;
    }
    else if (g_log) g_log("vitals: statistics start: dice left alone (%s)", g_last_on ? "the other game is gone or the mode does not apply" : "never was on");
    g_orig_stat(obj, a2, a3);
}
static void try_hook_stat(void)
{
    if (g_stat_tried) return;
    if (!g_game) g_game = GetModuleHandleA("adv_game.codelib");
    if (!g_game) return;
    g_stat_tried = 1;
    const u8 *b = (const u8 *)g_game;
    if (!readable(b, 0x400) || b[0] != 'M' || b[1] != 'Z') return;
    u32 pe = *(const u32 *)(b + 0x3c), img = 0;
    if (pe < 0x300 && readable(b + pe, 0x108) && *(const u32 *)(b + pe) == 0x4550u) img = *(const u32 *)(b + pe + 24 + 56);
    int ok = img > GAME_STAT + 11000 && same(b + GAME_STAT, SIG_STAT, sizeof SIG_STAT) && readable(b + GAME_STAT, 11000);
    int rng = 0;
    if (ok) for (u32 o = 0; o + sizeof SIG_STAT_RNG <= 11000; o++) if (b[GAME_STAT + o] == 0x30 && !memcmp(b + GAME_STAT + o, SIG_STAT_RNG, sizeof SIG_STAT_RNG)) rng++;
    if (ok && rng == 3) {
        g_orig_stat = (FnStat)install_detour((u8 *)b + GAME_STAT, STAT_STEAL, (void *)hook_stat);
        if (g_log) g_log("vitals: statistics function found in adv_game.codelib (rva %x), dice %s", GAME_STAT, g_orig_stat ? "will be put at one value" : "not hooked");
    } else if (g_log) g_log("vitals: statistics function not recognised in adv_game.codelib (signature %d, generator uses %d) - the dice stay as they are", ok, rng);
}

// The transform sender (called from session_frame, about 13 times a second) skips a unit that has moved less than a threshold unless its last
// transform is older than (id & 15) * 30 + 2000 ms. Preserve that staggered native cadence by default. An explicitly set MP_IDLE_MS can
// override it for diagnosis (0 = every unit on every tick); imm8 -> 0, imm32 -> T.
static void patch_idle(u8 *b)
{
    int T = 0;
    char v[12];
    DWORD n = GetEnvironmentVariableA("MP_IDLE_MS", v, sizeof v);
    if (!n || n >= sizeof v) return;                         // absent/too long: preserve native cadence
    for (DWORD i = 0; i < n; i++) {
        int d = v[i] - '0';
        if (d < 0 || d > 9 || T > 6000 || (T == 6000 && d > 0)) return;
        T = T * 10 + d;
    }
    if (T == 2000) return;                                   // the module's own value (the (id & 15) * 30 part is then left too)
    u8 *at = b + RVA_IDLE;
    if (!same(at, SIG_IDLE, sizeof SIG_IDLE)) return;
    DWORD old;
    if (!VirtualProtect(at, sizeof SIG_IDLE, PAGE_EXECUTE_READWRITE, &old)) return;
    at[IDLE_IMM8] = 0;
    u32 t32 = (u32)T;
    memcpy(at + IDLE_IMM32, &t32, 4);
    DWORD o2;
    VirtualProtect(at, sizeof SIG_IDLE, old, &o2);
    FlushInstructionCache(GetCurrentProcess(), at, sizeof SIG_IDLE);
    g_idle_msg = T;
}

static void attach(void)
{
    g_mod = GetModuleHandleA("basic_network.codelib");
    if (!g_mod) return;
    u8 *b = (u8 *)g_mod;
    IMAGE_DOS_HEADER *dos = (IMAGE_DOS_HEADER *)b;
    if (dos->e_magic != IMAGE_DOS_SIGNATURE) return;
    IMAGE_NT_HEADERS *nt = (IMAGE_NT_HEADERS *)(b + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE || nt->FileHeader.TimeDateStamp != V20_TIMESTAMP) return;
    if (!same(b + RVA_SESSION_FRAME, SIG_FRAME, sizeof SIG_FRAME) || !same(b + RVA_ON_PACKET - 10, SIG_PACKET_HEAD, sizeof SIG_PACKET_HEAD) ||
        !same(b + RVA_ON_PACKET, SIG_PACKET, sizeof SIG_PACKET) || !same(b + RVA_OWNS, SIG_OWNS, sizeof SIG_OWNS) ||
        !same(b + RVA_SEND_PEER, SIG_SEND, sizeof SIG_SEND) || !same(b + RVA_LOG, SIG_LOG, sizeof SIG_LOG)) return;
    // The module has now passed every v20 signature check, but session_start has not read MP_SYNC_STATE yet.
    // Correct only the missing operation default; an explicit caller value is never changed.
    g_operation_state_policy = default_operation_state_policy();
    g_owns = (FnOwns)(b + RVA_OWNS);
    g_send = (FnSend)(b + RVA_SEND_PEER);
    g_log = (FnLog)(b + RVA_LOG);
    g_num_ready = b + RVA_NUM_READY; g_num_flag = b + RVA_NUM_FLAG;
    g_connected = b + RVA_CONNECTED; g_tactical = b + RVA_TACTICAL; g_have_peer = b + RVA_HAVE_PEER;
    g_host_mode = (volatile u32 *)(b + RVA_HOST_MODE);
    g_peer = b + RVA_PEER;
    InitializeCriticalSection(&g_cs);
    g_orig_packet = (FnPacket)install_detour(b + RVA_ON_PACKET, sizeof SIG_PACKET, (void *)hook_packet);
    if (!g_orig_packet) return;
    g_orig_frame = (FnFrame)install_detour(b + RVA_SESSION_FRAME, sizeof SIG_FRAME, (void *)hook_frame);
    if (!g_orig_frame) return;      // (the packet hook that is already in only takes out our own packet type, which nobody sends then)
    g_active = 1;
    patch_idle(b);
}

// one absolute address in the data, so that the linker writes a relocation table and the loader may place the DLL anywhere
__attribute__((used)) static void *g_reloc_anchor = &g_mod;

__declspec(dllexport) int MpVitalsInit(void) { return g_active; }

BOOL WINAPI DllMain(HINSTANCE inst, DWORD reason, LPVOID reserved)
{
    (void)reserved;
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(inst);
        attach();
    }
    return TRUE;
}

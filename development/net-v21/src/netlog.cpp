// Implementation of netlog.h.  No dependencies beyond the C runtime and kernel32, so the same file
// can be linked into the module, into mp_join and into the self-test.
#include "netlog.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

// --------------------------------------------------------------------------- state
static FILE *g_file;
static CRITICAL_SECTION g_cs;
static bool g_cs_ready;
static char g_path[MAX_PATH];
static char g_program[32] = "";
static DWORD g_started;
static unsigned long long g_frames;

static unsigned long long g_rx_packets, g_rx_bytes, g_tx_packets, g_tx_bytes;
static unsigned long long g_rx_type[NETLOG_TYPES], g_tx_type[NETLOG_TYPES];
static unsigned long long g_rx_logged[NETLOG_TYPES], g_tx_logged[NETLOG_TYPES];
static DWORD g_last_summary;

static int g_phase = -1;
static unsigned g_sim_ms;
static int g_peers;
static unsigned g_sides;

// Named counters, fixed table: a fixed table cannot allocate, cannot fail and cannot grow without
// bound while a battle is running.
enum { NETLOG_NAMES = 16 };
static struct { char name[20]; long long value; } g_names[NETLOG_NAMES];
static int g_name_count;

// 8 MB is about a long battle at the rate this writes; beyond that the file is closed and a new one
// started, so an overnight session cannot fill a disk.
static const long k_max_bytes = 8 * 1024 * 1024;

// --------------------------------------------------------------------------- locking
static void lock()
{
    if (g_cs_ready)
        EnterCriticalSection(&g_cs);
}

static void unlock()
{
    if (g_cs_ready)
        LeaveCriticalSection(&g_cs);
}

// --------------------------------------------------------------------------- writing
static void prefix(const char *channel)
{
    SYSTEMTIME t;
    DWORD now = GetTickCount();
    GetLocalTime(&t);
    fprintf(g_file, "%02u:%02u:%02u.%03u [t%05lu f%06llu +%6.1fs] %-7s ",
            t.wHour, t.wMinute, t.wSecond, t.wMilliseconds, GetCurrentThreadId(), g_frames,
            (now - g_started) / 1000.0, channel);
}

static void write_va(const char *channel, const char *fmt, va_list args)
{
    if (!g_file)
        return;
    lock();
    if (g_file) {
        prefix(channel);
        vfprintf(g_file, fmt, args);
        fputc('\n', g_file);
        fflush(g_file);
    }
    unlock();
}

static void write_line(const char *channel, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    write_va(channel, fmt, args);
    va_end(args);
}

// --------------------------------------------------------------------------- identity helpers
static unsigned long long fnv1a(FILE *file, unsigned long long *size)
{
    unsigned long long hash = 1469598103934665603ULL;
    unsigned char buffer[65536];
    size_t got;
    *size = 0;
    while ((got = fread(buffer, 1, sizeof buffer, file)) > 0) {
        for (size_t i = 0; i < got; i++) {
            hash ^= buffer[i];
            hash *= 1099511628211ULL;
        }
        *size += got;
    }
    return hash;
}

static unsigned long pe_stamp(HMODULE self)
{
    // TimeDateStamp of our own image: the one number that changes with every rebuild, so a report
    // can be matched to the exact binary that produced it.
    const unsigned char *base = (const unsigned char *)self;
    if (!base)
        return 0;
    unsigned long offset = *(const unsigned long *)(base + 0x3C);
    if (offset < 0x40 || offset > 0x1000)
        return 0;
    return *(const unsigned long *)(base + offset + 8);
}

static const char *env_or(const char *name, const char *fallback)
{
    static char values[8][64];
    static int next;
    char *slot = values[next++ % 8];
    DWORD got = GetEnvironmentVariableA(name, slot, 64);
    if (!got || got >= 64)
        return fallback;
    return slot;
}

static int counter_slot(const char *name)
{
    for (int i = 0; i < g_name_count; i++)
        if (strcmp(g_names[i].name, name) == 0)
            return i;
    if (g_name_count == NETLOG_NAMES)
        return -1;
    strncpy_s(g_names[g_name_count].name, name, _TRUNCATE);
    g_names[g_name_count].value = 0;
    return g_name_count++;
}

// --------------------------------------------------------------------------- open / close
static bool open_file(const char *out_dir, const char *program)
{
    char previous[MAX_PATH];
    sprintf_s(g_path, sizeof g_path, "%s\\net_v21_%s.log", out_dir, program);
    sprintf_s(previous, sizeof previous, "%s\\net_v21_%s.prev.log", out_dir, program);
    DeleteFileA(previous);
    MoveFileA(g_path, previous);   // keep the previous run; overwriting it would lose the last crash
    return fopen_s(&g_file, g_path, "w") == 0 && g_file;
}

void netlog_open(HMODULE self, const char *program, const char *out_dir)
{
    if (g_file)
        return;
    if (!g_cs_ready) {
        InitializeCriticalSection(&g_cs);
        g_cs_ready = true;
    }
    if (!out_dir || !program)
        return;
    strncpy_s(g_program, program, _TRUNCATE);
    if (!open_file(out_dir, program))
        return;

    g_started = GetTickCount();
    g_last_summary = g_started;

    char path[MAX_PATH] = "";
    GetModuleFileNameA(self, path, MAX_PATH);
    FILE *image = nullptr;
    unsigned long long size = 0;
    unsigned long long hash = 0;
    if (fopen_s(&image, path, "rb") == 0 && image) {
        hash = fnv1a(image, &size);
        fclose(image);
    }

    write_line("boot", "=== net_v21: our own network module, v1.0.2-test ===");
#ifdef _MSC_VER
    write_line("boot", "build: compiled %s %s, MSVC %d", __DATE__, __TIME__, (int)_MSC_VER);
#else
    write_line("boot", "build: compiled %s %s, non-MSVC compiler", __DATE__, __TIME__);
#endif
    write_line("boot", "image: %s", path);
    write_line("boot", "image: %llu bytes, fnv1a64 %016llx, pe stamp %08lx",
               size, hash, pe_stamp(self));
    write_line("boot", "process: %s, pid %lu, program %s",
               env_or("COMPUTERNAME", "?"), GetCurrentProcessId(), program);
    write_line("boot", "command line: %s", GetCommandLineA());
    write_line("boot", "flags: MP_STEAM=%s MP_STEAM_FAKE=%s MP_SYNC_STATE=%s MP_VERSUS=%s MP_ROLE=%s "
                       "MP_SWAP=%s MP_MAP=%s",
               env_or("MP_STEAM", "<unset>"), env_or("MP_STEAM_FAKE", "<unset>"),
               env_or("MP_SYNC_STATE", "<unset>"), env_or("MP_VERSUS", "<unset>"),
               env_or("MP_ROLE", "<unset>"), env_or("MP_SWAP", "<unset>"),
               env_or("MP_MAP", "<unset>"));
    write_line("boot", "log: %s (previous run kept as .prev.log)", g_path);
}

void netlog_shutdown()
{
    lock();
    if (g_file) {
        prefix("bye");
        fprintf(g_file, "module going away after %.1f s, %llu frames\n",
                (GetTickCount() - g_started) / 1000.0, g_frames);
        fflush(g_file);
        fclose(g_file);
        g_file = nullptr;
    }
    unlock();
}

const char *netlog_path() { return g_path; }

// --------------------------------------------------------------------------- lines
void netlog_mirror(const char *text)
{
    write_line("module", "%s", text ? text : "");
}

void netlog_note(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    write_va("note", fmt, args);
    va_end(args);
}

void netlog_mark(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    write_va("battle", fmt, args);
    va_end(args);
}

void netlog_count(const char *name, long long delta)
{
    if (!g_file || !name)
        return;
    lock();
    int index = counter_slot(name);
    if (index >= 0)
        g_names[index].value += delta;
    unlock();
}

void netlog_state(int phase, unsigned sim_ms, int peers, unsigned sides)
{
    g_phase = phase;
    g_sim_ms = sim_ms;
    g_peers = peers;
    g_sides = sides;
}

// --------------------------------------------------------------------------- traffic
static const char *type_name(unsigned char type)
{
    switch (type) {
    case 1: return "HELLO";
    case 2: return "WELCOME";
    case 3: return "KEEPALIVE";
    case 4: return "IMAGE_REQ";
    case 5: return "IMAGE_DATA";
    case 6: return "STATE";
    case 7: return "BYE";
    case 8: return "OBJECT";
    case 9: return "OBJECT_ACK";
    case 10: return "SHOT";
    case 11: return "SHOT_ACK";
    default: return "?";
    }
}

void netlog_packet(const char *dir, const unsigned char *data, int size,
                   unsigned int peer_ip, unsigned short peer_port, unsigned short family)
{
    if (!g_file || !data || size <= 0)
        return;
    bool out = dir && dir[0] == 't';
    unsigned char type = size > 5 ? data[4] : 0;   // Head: magic(4), type(1)
    if (type >= NETLOG_TYPES)
        type = 0;

    lock();
    if (out) {
        g_tx_packets++;
        g_tx_bytes += (unsigned)size;
        g_tx_type[type]++;
    } else {
        g_rx_packets++;
        g_rx_bytes += (unsigned)size;
        g_rx_type[type]++;
    }
    // Rate limit: the first 20 of every type are written in full, after that every 100th.  The
    // five-second summary carries the true totals, so nothing is lost that matters.
    unsigned long long *seen = out ? &g_tx_logged[type] : &g_rx_logged[type];
    bool write = (*seen)++ < 20 || (*seen % 100) == 0;
    if (write) {
        char peer[64];
        if (family == 0x5354)
            sprintf_s(peer, "steam");
        else if (family == 2 /* AF_INET */)
            sprintf_s(peer, "%u.%u.%u.%u:%u", (peer_ip >> 24) & 255, (peer_ip >> 16) & 255,
                      (peer_ip >> 8) & 255, peer_ip & 255, peer_port);
        else
            sprintf_s(peer, "-");
        prefix(out ? "tx" : "rx");
        fprintf(g_file, "%s %-10s %5d B  peer %s  #%llu\n", out ? "->" : "<-", type_name(type),
                size, peer, *seen);
        fflush(g_file);
    }
    unlock();
}

// --------------------------------------------------------------------------- summary
static void summary_locked();

void netlog_test_summary()
{
    if (!g_file)
        return;
    lock();
    summary_locked();
    unlock();
}

// Called with the lock held: the caller decides whether the five seconds are up.
static void summary_locked()
{
    g_last_summary = GetTickCount();
    prefix("summary");
    fprintf(g_file, "%llu frames, rx %llu/%llu B, tx %llu/%llu B", g_frames, g_rx_packets,
            g_rx_bytes, g_tx_packets, g_tx_bytes);
    if (g_phase >= 0)
        fprintf(g_file, ", phase %d, sim %.1f s, %d peer(s), sides 0x%x",
                g_phase, g_sim_ms / 1000.0, g_peers, g_sides);
    fputc('\n', g_file);
    // Per-type split only for the types that actually moved: a line per type per five seconds is
    // noise, one line with the nonzero entries is a picture of the battle.
    prefix("summary");
    fprintf(g_file, "types:");
    for (int i = 0; i < NETLOG_TYPES; i++)
        if (g_rx_type[i] || g_tx_type[i])
            fprintf(g_file, " %s rx%llu/tx%llu", type_name((unsigned char)i), g_rx_type[i],
                    g_tx_type[i]);
    fputc('\n', g_file);
    if (g_name_count) {
        prefix("summary");
        fprintf(g_file, "battle:");
        for (int i = 0; i < g_name_count; i++)
            fprintf(g_file, " %s=%lld", g_names[i].name, g_names[i].value);
        fputc('\n', g_file);
    }
    fflush(g_file);
}

void netlog_frame()
{
    g_frames++;
    if (!g_file)
        return;
    if (GetTickCount() - g_last_summary < 5000)
        return;
    lock();
    summary_locked();
    unlock();
}

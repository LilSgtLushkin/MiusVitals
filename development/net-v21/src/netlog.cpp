// Implementation of netlog.h.  No dependencies beyond the C runtime and kernel32, so the same file
// can be linked into the module, into mp_join and into the self-test.
#include "netlog.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <share.h>
#include <atomic>
#include <limits.h>

// --------------------------------------------------------------------------- state
static FILE *g_file;
static CRITICAL_SECTION g_cs;
static INIT_ONCE g_cs_once = INIT_ONCE_STATIC_INIT;
static char g_path[MAX_PATH], g_part_path[MAX_PATH], g_identity[1024];
static char g_program[32] = "";
static DWORD g_started;
static std::atomic<unsigned long long> g_frames{0};

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

// Keep the current and previous 8 MiB segments plus the previous run; never truncate a segment
// unless its rename succeeded. The collector includes both current and .part/.prev files.
static const long k_max_bytes = 8 * 1024 * 1024;
static long g_rotate_at = k_max_bytes;

// --------------------------------------------------------------------------- locking
static BOOL CALLBACK init_cs(PINIT_ONCE, PVOID, PVOID *)
{
    InitializeCriticalSection(&g_cs);
    return TRUE;
}

static void lock()
{
    InitOnceExecuteOnce(&g_cs_once, init_cs, nullptr, nullptr);
    EnterCriticalSection(&g_cs);
}

static void unlock() { LeaveCriticalSection(&g_cs); }

// --------------------------------------------------------------------------- writing
static void prefix(const char *channel)
{
    SYSTEMTIME t;
    DWORD now = GetTickCount();
    GetLocalTime(&t);
    fprintf(g_file, "%02u:%02u:%02u.%03u [t%05lu f%06llu +%6.1fs] %-7s ",
            t.wHour, t.wMinute, t.wSecond, t.wMilliseconds, GetCurrentThreadId(),
            g_frames.load(std::memory_order_relaxed), (now - g_started) / 1000.0, channel);
}

// Caller holds g_cs. If rotation fails, append to the original and retry only after another MiB;
// a locked destination must never turn a failed rename into a truncated log.
static void rotate_locked(bool force = false)
{
    if (!g_file) return;
    long at = ftell(g_file);
    if (!force && (at < 0 || at < g_rotate_at)) return;
    fflush(g_file);
    fclose(g_file);
    g_file = nullptr;
    if (!MoveFileExA(g_path, g_part_path, MOVEFILE_REPLACE_EXISTING)) {
        DWORD error = GetLastError();
        g_file = _fsopen(g_path, "a", _SH_DENYNO);
        g_rotate_at = at >= 0 && at < LONG_MAX - 1024 * 1024 ? at + 1024 * 1024 : LONG_MAX;
        if (g_file) {
            prefix("note");
            fprintf(g_file, "log rotation failed (%lu); continuing the existing file\n", error);
            fflush(g_file);
        }
        return;
    }
    g_file = _fsopen(g_path, "w", _SH_DENYNO);
    g_rotate_at = k_max_bytes;
    if (g_file) {
        prefix("boot");
        fprintf(g_file, "continued after log rotation; previous segment is .part.log\n");
        prefix("boot");
        fprintf(g_file, "%s\n", g_identity[0] ? g_identity : "identity was not yet available");
        fflush(g_file);
    }
}

static void write_va(const char *channel, const char *fmt, va_list args)
{
    lock();
    if (g_file) {
        prefix(channel);
        vfprintf(g_file, fmt, args);
        fputc('\n', g_file);
        fflush(g_file);
        rotate_locked();
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
    char previous[MAX_PATH], previous_part[MAX_PATH];
    sprintf_s(g_path, sizeof g_path, "%s\\net_v21_%s.log", out_dir, program);
    sprintf_s(g_part_path, sizeof g_part_path, "%s\\net_v21_%s.part.log", out_dir, program);
    sprintf_s(previous, sizeof previous, "%s\\net_v21_%s.prev.log", out_dir, program);
    sprintf_s(previous_part, sizeof previous_part, "%s\\net_v21_%s.part.prev.log", out_dir, program);
    g_identity[0] = 0;
    g_rotate_at = k_max_bytes;
    if (GetFileAttributesA(g_path) != INVALID_FILE_ATTRIBUTES) {
        if (!MoveFileExA(g_path, previous, MOVEFILE_REPLACE_EXISTING)) {
            // A reader may have the old file open. Append rather than erase the only surviving log.
            g_file = _fsopen(g_path, "a", _SH_DENYNO);
            return g_file != nullptr;
        }
        if (GetFileAttributesA(g_part_path) != INVALID_FILE_ATTRIBUTES)
            MoveFileExA(g_part_path, previous_part, MOVEFILE_REPLACE_EXISTING);
    }
    g_file = _fsopen(g_path, "w", _SH_DENYNO);
    return g_file != nullptr;
}

void netlog_open(HMODULE self, const char *program, const char *out_dir)
{
    if (!out_dir || !program) return;
    lock();
    if (g_file) { unlock(); return; }
    strncpy_s(g_program, program, _TRUNCATE);
    if (!open_file(out_dir, program)) { unlock(); return; }

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

    sprintf_s(g_identity, sizeof g_identity, "net_v21 v1.0.3-test, %s %s, program %s; image %s, %llu bytes, fnv1a64 %016llx, pe stamp %08lx",
              __DATE__, __TIME__, program, path, size, hash, pe_stamp(self));
    write_line("boot", "=== net_v21: our own network module, v1.0.3-test ===");
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
    write_line("boot", "battle flags: MP_SYNC_SHOTS=%s MP_LOG_SHOTS=%s MP_SYNC_HITS=%s MP_LOG_HITS=%s MP_SYNC_ANIM=%s",
               env_or("MP_SYNC_SHOTS", "<unset>"), env_or("MP_LOG_SHOTS", "<unset>"),
               env_or("MP_SYNC_HITS", "<unset>"), env_or("MP_LOG_HITS", "<unset>"),
               env_or("MP_SYNC_ANIM", "<unset>"));
    write_line("boot", "feature status: hit and soldier animation replication are not implemented in this candidate");
    write_line("boot", "log: %s (previous run kept as .prev.log)", g_path);
    unlock();
}

void netlog_shutdown()
{
    lock();
    if (g_file) {
        prefix("bye");
        fprintf(g_file, "module going away after %.1f s, %llu frames\n",
                (GetTickCount() - g_started) / 1000.0, g_frames.load(std::memory_order_relaxed));
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
    if (!name) return;
    lock();
    if (g_file) {
        int index = counter_slot(name);
        if (index >= 0) g_names[index].value += delta;
    }
    unlock();
}

void netlog_state(int phase, unsigned sim_ms, int peers, unsigned sides)
{
    lock();
    g_phase = phase;
    g_sim_ms = sim_ms;
    g_peers = peers;
    g_sides = sides;
    unlock();
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
    case 12: return "END";
    default: return "?";
    }
}

void netlog_packet(const char *dir, const unsigned char *data, int size,
                   unsigned int peer_ip, unsigned short peer_port, unsigned short family)
{
    if (!data || size <= 0) return;
    bool out = dir && dir[0] == 't';
    unsigned char type = size > 5 ? data[4] : 0;   // Head: magic(4), type(1)
    if (type >= NETLOG_TYPES)
        type = 0;

    lock();
    if (!g_file) { unlock(); return; }
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
        rotate_locked();
    }
    unlock();
}

// --------------------------------------------------------------------------- summary
static void summary_locked();

void netlog_test_summary()
{
    lock();
    if (g_file) summary_locked();
    unlock();
}

void netlog_test_rotate()
{
    lock();
    rotate_locked(true);
    unlock();
}

// Called with the lock held: the caller decides whether the five seconds are up.
static void summary_locked()
{
    g_last_summary = GetTickCount();
    prefix("summary");
    fprintf(g_file, "%llu frames, rx %llu/%llu B, tx %llu/%llu B",
            g_frames.load(std::memory_order_relaxed), g_rx_packets, g_rx_bytes, g_tx_packets, g_tx_bytes);
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
    rotate_locked();
}

void netlog_frame()
{
    g_frames.fetch_add(1, std::memory_order_relaxed);
    lock();
    if (g_file && GetTickCount() - g_last_summary >= 5000) summary_locked();
    unlock();
}

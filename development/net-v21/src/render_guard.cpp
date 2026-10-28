// Narrow crash guards derived from the historical mp_vitals renderer fixes. Each code path and
// native recovery target is checked against the actual module before any handler or hook is installed.
// This is not a general access-violation handler. See docs/GITHUB_HISTORY_AUDIT_2026-10-27.md.
#include "common.h"
#include "hooks.h"
#include "render_cache_bounds.h"
#include <atomic>
#include <stdint.h>
#include <string.h>

using u8 = unsigned char;
static const unsigned char sig_walk_node[] = {
    0x48,0x89,0x6c,0x24,0x10,0x48,0x89,0x74,0x24,0x18,0x57,0x48,0x83,0xec,0x20,0x4c,
    0x8b,0x89,0x40,0x02,0x00,0x00,0x43,0x8d,0x3c,0x80,0xc1,0xe7,0x06,0x48,0x8b,0xe9,
    0x44,0x8b,0xd2,0x49,0xc1,0xe2,0x06,0x8b,0xf2,0x4b,0x03,0x3c,0x0a,0x8b,0x4f,0x38,
};
static const unsigned char sig_walk_fallback[] = {
    0x48,0x8b,0x5c,0x24,0x30,0x48,0x8b,0x6c,0x24,0x38,0x48,0x8b,0x74,0x24,0x40,0x48,
    0x83,0xc4,0x20,0x5f,0xc3,
};
static const unsigned char sig_node[] = {
    0x48,0x89,0x6c,0x24,0x10,0x48,0x89,0x74,0x24,0x18,0x57,0x48,0x83,0xec,0x20,0x4c,
    0x8b,0x89,0x40,0x02,0x00,0x00,0x43,0x8d,0x34,0x80,0xc1,0xe6,0x06,0x48,0x8b,0xe9,
    0x44,0x8b,0xd2,0x49,0xc1,0xe2,0x06,0x4b,0x03,0x34,0x0a,0x8b,0x4e,0x38,
};
static const unsigned char sig_cache_rebuild[] = {
    0x48,0x8d,0x8f,0x38,0x01,0x00,0x00,0x49,0x8b,0xd0,0xe8,0xeb,0x69,0x00,0x00,0x8b,
    0xd0,0x89,0x55,0x34,
};
// The allocator receives renderer+0x138. Its +0x30/+0x3c/+0x40 are therefore the
// renderer's pool pointer/live count/capacity at +0x168/+0x174/+0x178 respectively.
// Verify both fresh-slot and free-list paths, plus the vector-growth routine, before
// trusting those fields in the much cheaper inline guard.
static const unsigned char sig_cache_allocator[] = {
    0x48,0x89,0x5c,0x24,0x08,0x48,0x89,0x6c,0x24,0x10,0x48,0x89,0x74,0x24,0x18,0x57,
    0x48,0x83,0xec,0x20,0x83,0x79,0x24,0x00,0x48,0x8b,0xf2,0x48,0x8b,0xd9,0x75,0x28,
    0x8b,0x41,0x3c,0x3b,0x41,0x40,0x75,0x15,0x03,0xc0,0xba,0x01,0x00,0x00,0x00,0x3b,
    0xc2,0x0f,0x47,0xd0,0x48,0x83,0xc1,0x30,0xe8,0xa3,0x06,0x00,0x00,0x8b,0x6b,0x3c,
    0x8d,0x45,0x01,0x89,0x43,0x3c,0xeb,0x0f,0x8b,0x51,0x24,0x48,0x8b,0x41,0x18,0xff,
    0xca,0x8b,0x2c,0x90,0x89,0x51,0x24,
};
static const unsigned char sig_cache_growth[] = {
    0x48,0x89,0x5c,0x24,0x10,0x57,0x48,0x83,0xec,0x20,0x48,0x8b,0xd9,0x8b,0xfa,0x3b,
    0x51,0x10,0x76,0x51,0x8b,0xcf,0x48,0x89,0x74,0x24,0x30,0x48,0xc1,0xe1,0x06,0xba,
    0x10,0x00,0x00,0x00,0xff,0x15,0xbe,0x88,0x02,0x00,0x48,0x8b,0xf0,0x8b,0x43,0x0c,
    0x85,0xc0,0x74,0x18,0x83,0x7b,0x14,0x00,0x74,0x12,0x48,0x8b,0x13,0x44,0x8b,0xc0,
    0x49,0xc1,0xe0,0x06,0x48,0x8b,0xce,0xe8,0xaa,0x61,0x02,0x00,0x48,0x8b,0x0b,0x89,
    0x7b,0x10,0x48,0x85,0xc9,0x74,0x06,0xff,0x15,0xab,0x88,0x02,0x00,0x48,0x89,0x33,
    0x48,0x8b,0x74,0x24,
};
static const unsigned char sig_cache_inline_site[] = {
    0x81,0xe2,0xff,0xff,0xff,0x00,0x48,0x8b,0x87,0x68,0x01,0x00,0x00,0x8b,0xca,
};
static const unsigned char sig_triangle[] = {
    0x48,0x8b,0xc4,0x48,0x89,0x58,0x18,0x48,0x89,0x70,0x20,0x57,0x41,0x54,0x41,0x57,
    0x48,0x81,0xec,0xc0,0x00,0x00,0x00,0x8b,0x99,0x80,0x00,0x00,0x00,0x45,0x32,0xc9,
    0x44,0x0f,0xb6,0xa1,0x80,0x00,0x00,0x00,
};
static const unsigned char sig_triangle_loop[] = {
    0x8b,0x8a,0x88,0x00,0x00,0x00,0x41,0x8b,0x28,0x41,0x8b,0x40,0x08,0x41,0x03,0xea,
    0x45,0x8b,0x70,0x04,0x41,0x03,0xc2,0x0f,0x10,0x72,0x50,0x45,0x03,0xf2,0x0f,0xaf,
    0xe9,0x0f,0x28,0xee,0x0f,0xaf,0xc1,0x44,0x0f,0xaf,0xf1,0x8b,0xc8,0x44,0x0f,0x10,
    0x24,0x2e,0x44,0x0f,0x10,0x0c,0x30,0x41,0x0f,0x5c,0xec,0x45,0x0f,0x10,0x04,0x36,
};
static const unsigned char sig_triangle_attr0[] = {
    0x8b,0x82,0x8c,0x00,0x00,0x00,0x48,0x03,0xc6,0x0f,0x10,0x0c,0x28,0x41,0x0f,0x10,
    0x14,0x06,0x0f,0x10,0x04,0x01,0x0f,0x5c,0xd1,0x0f,0x5c,0xc1,0x0f,0x59,0xd4,0x0f,
    0x59,0xc5,0x0f,0x58,0xd1,0x0f,0x58,0xd0,0x0f,0x11,0x52,0x20,0x40,0x84,0xff,0x74,
};
static const unsigned char sig_triangle_attr1[] = {
    0x8b,0x82,0x90,0x00,0x00,0x00,0x48,0x03,0xc6,0x0f,0x10,0x0c,0x28,0x41,0x0f,0x10,
    0x14,0x06,0x0f,0x10,0x04,0x01,0x0f,0x5c,0xd1,0x0f,0x5c,0xc1,0x0f,0x59,0xd4,0x0f,
    0x59,0xc5,0x0f,0x58,0xd1,0x0f,0x58,0xd0,0x0f,0x11,0x52,0x30,0x84,0xdb,0x74,0x26,
};
static const unsigned char sig_triangle_fallback[] = {
    0x44,0x88,0x8a,0xa4,0x00,0x00,0x00,0xeb,0x07,0x44,0x88,0x89,0xa4,0x00,0x00,0x00,
    0x4c,0x8d,
};
static const unsigned char sig_cache_complete[] = {
    0x48,0x89,0x6c,0x24,0x10,0x48,0x89,0x74,0x24,0x18,0x57,0x48,0x83,0xec,0x20,0x8b,
    0xda,0x48,0x8b,0xf9,0x83,0xfa,0xff,0x74,0x1b,0x8b,0xca,0x8b,0xc2,0xc1,0xe9,0x0a,
    0x48,0xc1,0xe1,0x06,0x48,0x03,0x8f,0x40,0x02,0x00,0x00,0x25,0xff,0x03,0x00,0x00,
    0x8b,0xf0,0xeb,0x21,0x48,0x8b,0x89,0x40,0x02,0x00,0x00,0x48,0xb8,0xc0,0xff,0xff,
    0xff,0x3f,0x00,0x00,0x00,0x48,0x03,0xc8,0x8b,0xf3,0x81,0xe6,0xff,0x03,0x00,0x00,
    0xb8,0xff,0xff,0xff,0xff,0x8d,0x2c,0x80,0xc1,0xe5,0x06,0x48,0x03,0x29,0x8b,0x55,
    0x34,0x83,0xfa,0xfd,0x77,0x3b,0x41,0x0f,0x28,0x00,0x81,0xe2,0xff,0xff,0xff,0x00,
    0x48,0x8b,0x87,0x68,0x01,0x00,0x00,0x8b,0xca,0x48,0xc1,0xe1,0x06,0x0f,0x29,0x04,
    0x01,
};
static const unsigned char sig_node_fallback[] = {
    0x48,0x8b,0x6c,0x24,0x38,0x8b,0xc7,0x48,0x8b,0x74,0x24,0x40,0x48,0x83,0xc4,0x20,
    0x5f,0xc3,
};

namespace {
// The four function starts can move independently. All instruction and native-epilogue offsets
// below are relative to their own, uniquely resolved function, never to a guessed image base.
static u8 *g_walk, *g_node, *g_cache, *g_triangle;
static PVOID g_handler;
static std::atomic<bool> g_attempted(false);
static std::atomic<u32> g_node_repairs(0), g_cache_repairs(0), g_geometry_skips(0), g_preflight_faults(0);
// Updated by the x64 stub's LOCK INC only on a rejected slot. Interlocked reads below
// use Windows' 32-bit LONG interlocked contract; normal cache writes do not touch it.
static volatile LONG g_cache_bounds_repairs;
// One timing sample per 128 calls per renderer thread, excluding the native function. This reports
// guard cost without two high-resolution clock calls and contended atomics on every geometry call.
struct GuardSamples { std::atomic<u64> count{0}, ticks{0}; };
static GuardSamples g_cache_samples, g_triangle_samples;
static thread_local unsigned g_cache_sample_seq, g_triangle_sample_seq;
static bool sample_start(unsigned &sequence, LARGE_INTEGER *at)
{
    return (++sequence & 127u) == 0 && QueryPerformanceCounter(at) != 0;
}
static void sample_end(GuardSamples &samples, LARGE_INTEGER start)
{
    LARGE_INTEGER end;
    if (QueryPerformanceCounter(&end) && end.QuadPart >= start.QuadPart) {
        samples.ticks.fetch_add((u64)(end.QuadPart - start.QuadPart), std::memory_order_relaxed);
        samples.count.fetch_add(1, std::memory_order_relaxed);
    }
}
using CacheFn = void (*)(void *, u32, const void *);
using TriangleFn = int (*)(void *);
static std::atomic<CacheFn> g_original_cache(nullptr);
static std::atomic<TriangleFn> g_original_triangle(nullptr);
static bool g_cache_hooked, g_cache_bounds_hooked, g_triangle_hooked;
static const u32 cache_missing = 0xfffffffeu;

static bool add_u64(u64 base, u64 off, u64 *out)
{
    if (base > ~(u64)0 - off) return false;
    *out = base + off;
    return true;
}
#ifdef NETV21_TEST_HOOKS
static unsigned g_test_page_queries;
#endif
static bool page_has(const void *p, size_t n, bool write, MEMORY_BASIC_INFORMATION *out = nullptr)
{
    MEMORY_BASIC_INFORMATION mbi = {};
#ifdef NETV21_TEST_HOOKS
    if (p && n) ++g_test_page_queries;
#endif
    if (!p || !n || !VirtualQuery(p, &mbi, sizeof mbi) || mbi.State != MEM_COMMIT ||
        (mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD))) return false;
    DWORD flags = mbi.Protect & 0xff;
    bool readable_page = flags == PAGE_READONLY || flags == PAGE_READWRITE || flags == PAGE_WRITECOPY ||
                         flags == PAGE_EXECUTE_READ || flags == PAGE_EXECUTE_READWRITE || flags == PAGE_EXECUTE_WRITECOPY;
    bool writable_page = flags == PAGE_READWRITE || flags == PAGE_WRITECOPY ||
                         flags == PAGE_EXECUTE_READWRITE || flags == PAGE_EXECUTE_WRITECOPY;
    if (!readable_page || (write && !writable_page)) return false;
    uintptr_t at = (uintptr_t)p, base = (uintptr_t)mbi.BaseAddress;
    if (at < base || at - base > mbi.RegionSize || n > mbi.RegionSize - (at - base)) return false;
    if (out) *out = mbi;
    return true;
}
static bool same_code(HMODULE image, const u8 *at, const void *sig, size_t n)
{
    MEMORY_BASIC_INFORMATION mbi = {};
    if (!page_has(at, n, false, &mbi) || mbi.AllocationBase != image) return false;
    DWORD p = mbi.Protect & 0xff;
    return (p == PAGE_EXECUTE_READ || p == PAGE_EXECUTE_READWRITE || p == PAGE_EXECUTE_WRITECOPY) &&
           !memcmp(at, sig, n);
}
template <typename T> static bool load_at(u64 base, u64 offset, T *out)
{
    u64 address;
    if (!add_u64(base, offset, &address) || !page_has((const void *)(uintptr_t)address, sizeof(T), false)) return false;
    memcpy(out, (const void *)(uintptr_t)address, sizeof(T));
    return true;
}

// VirtualQuery regions are reusable only within ONE guard call: never cache them across
// renderer calls, where allocation/protection may change. Every read/write still gets the
// same bounds, protection and allocation checks, and the outer SEH handles unmap races.
struct CacheProbe {
    MEMORY_BASIC_INFORMATION regions[6] = {};
    unsigned count = 0;
    bool has(const void *p, size_t n, bool write, MEMORY_BASIC_INFORMATION *out = nullptr) {
        uintptr_t at = (uintptr_t)p;
        if (!p || !n) return false;
        for (unsigned i = 0; i < count; ++i) {
            const MEMORY_BASIC_INFORMATION &m = regions[i];
            uintptr_t base = (uintptr_t)m.BaseAddress;
            if (at < base || at - base > m.RegionSize || n > m.RegionSize - (at - base)) continue;
            DWORD flags = m.Protect & 0xff;
            bool writable_page = flags == PAGE_READWRITE || flags == PAGE_WRITECOPY ||
                                 flags == PAGE_EXECUTE_READWRITE || flags == PAGE_EXECUTE_WRITECOPY;
            if (write && !writable_page) return false;
            if (out) *out = m;
            return true;
        }
        MEMORY_BASIC_INFORMATION m = {};
        if (!page_has(p, n, write, &m)) return false;
        if (count < sizeof regions / sizeof regions[0]) regions[count++] = m;
        if (out) *out = m;
        return true;
    }
    template <typename T> bool load(u64 base, u64 offset, T *out) {
        u64 address;
        if (!add_u64(base, offset, &address) || !has((const void *)(uintptr_t)address, sizeof(T), false)) return false;
        memcpy(out, (const void *)(uintptr_t)address, sizeof(T));
        return true;
    }
};

static void render_cache_preflight(void *renderer, u32 node)
{
    CacheProbe p;
    u64 groups, group, at;
    if (node == 0xffffffffu || !p.load((u64)(uintptr_t)renderer, 0x240, &groups) ||
        !add_u64(groups, ((u64)node >> 10) << 6, &at) || !p.load(at, 0, &group) ||
        !add_u64(group, (u64)(node & 0x3ffu) * 320u, &at) || !add_u64(at, 0x34, &at) ||
        !p.has((void *)(uintptr_t)at, sizeof(u32), true) || (at & 3u)) return;
    u32 *slot = (u32 *)(uintptr_t)at;
    u32 old = (u32)InterlockedCompareExchange((volatile LONG *)slot, 0, 0);
    u64 pool, target;
    if (old >= cache_missing || !p.load((u64)(uintptr_t)renderer, 0x168, &pool) ||
        !add_u64(pool, ((u64)(old & 0xffffffu)) << 6, &target)) return;
    MEMORY_BASIC_INFORMATION a = {}, b = {};
    if (p.has((const void *)(uintptr_t)pool, 1, true, &a) &&
        p.has((const void *)(uintptr_t)target, 64, true, &b) && a.AllocationBase == b.AllocationBase) return;
    if ((u32)InterlockedCompareExchange((volatile LONG *)slot, (LONG)cache_missing, (LONG)old) == old)
        g_cache_repairs.fetch_add(1, std::memory_order_relaxed);
}

#ifdef NETV21_TEST_HOOKS
// Cache memory is local to the renderer, not a network id. Only an out-of-allocation cached slot
// is changed, atomically, to the engine's own 'allocate a fresh slot' sentinel.
static u32 *render_cache_slot_reference(void *renderer, u32 node)
{
    u64 groups, group, at;
    if (node == 0xffffffffu || !load_at((u64)(uintptr_t)renderer, 0x240, &groups) ||
        !add_u64(groups, ((u64)node >> 10) << 6, &at) || !load_at(at, 0, &group) ||
        !add_u64(group, (u64)(node & 0x3ffu) * 320u, &at) || !add_u64(at, 0x34, &at) ||
        !page_has((void *)(uintptr_t)at, sizeof(u32), true) || (at & 3u)) return nullptr;
    return (u32 *)(uintptr_t)at;
}
static bool same_writable_allocation_reference(u64 base, u64 target, size_t n)
{
    MEMORY_BASIC_INFORMATION a = {}, b = {};
    if (!page_has((const void *)(uintptr_t)base, 1, true, &a)) return false;
    // If the target is already covered by the same writable VirtualQuery region, querying it again
    // cannot add protection or allocation information. Do not cache this across renderer calls.
    uintptr_t at = (uintptr_t)target, start = (uintptr_t)a.BaseAddress;
    if ((u64)at == target && at >= start && at - start <= a.RegionSize &&
        n && n <= a.RegionSize - (at - start)) return true;
    return page_has((const void *)at, n, true, &b) && a.AllocationBase == b.AllocationBase;
}
static void render_cache_preflight_reference(void *renderer, u32 node)
{
    u32 *cache = render_cache_slot_reference(renderer, node);
    if (!cache) return;
    u32 old = (u32)InterlockedCompareExchange((volatile LONG *)cache, 0, 0);
    u64 pool, target;
    if (old >= cache_missing || !load_at((u64)(uintptr_t)renderer, 0x168, &pool) ||
        !add_u64(pool, ((u64)(old & 0xffffffu)) << 6, &target) ||
        same_writable_allocation_reference(pool, target, 64)) return;
    if ((u32)InterlockedCompareExchange((volatile LONG *)cache, (LONG)cache_missing, (LONG)old) == old)
        g_cache_repairs.fetch_add(1, std::memory_order_relaxed);
}

#endif

// Cache a few readable regions for the duration of a single triangle query. This avoids issuing a
// VirtualQuery for every vertex in the same allocation; the narrow VEH below handles engine reads if
// another thread frees or replaces that region after the check.
struct ReadCache {
    struct Region { uintptr_t start, end; } regions[4] = {};
    unsigned next = 0;
    bool has(u64 address, size_t n) {
        uintptr_t at = (uintptr_t)address;
        if ((u64)at != address || !n || n > ~(uintptr_t)0 - at) return false;
        for (const Region &r : regions) if (at >= r.start && at + n <= r.end && r.end) return true;
        MEMORY_BASIC_INFORMATION mbi = {};
        if (!page_has((const void *)at, n, false, &mbi)) return false;
        uintptr_t base = (uintptr_t)mbi.BaseAddress;
        if (mbi.RegionSize > ~(uintptr_t)0 - base) return false;
        regions[next++ % 4] = { base, base + mbi.RegionSize };
        return true;
    }
};
static bool triangle_vertex_readable(ReadCache &cache, u64 base, u32 index, u32 stride)
{
    u64 at;
    u32 offset = index * stride;               // the engine's 32-bit multiplication is modulo 2^32
    return add_u64(base, offset, &at) && cache.has(at, 16);
}
static bool render_triangle_bad_input(void *query)
{
    u64 q = (u64)(uintptr_t)query, output, vertices, indices, attr0 = 0, attr1 = 0;
    u32 count, stride, base, flags, attr0_offset, attr1_offset;
    if (!query || !page_has(query, 0x98, false) || !add_u64(q, 0xa4, &output) ||
        !page_has((void *)(uintptr_t)output, 1, true)) return false; // unknown input stays the engine's fault
    // page_has(query, 0x98) already covered all these fields in one readable region. Re-querying
    // each one issued eight redundant VirtualQuery calls per triangle, without eliminating the
    // unmap race; the enclosing SEH still handles a region changed by another thread.
    const u8 *fields = (const u8 *)query;
    memcpy(&vertices, fields + 0x78, sizeof vertices); memcpy(&indices, fields + 0x70, sizeof indices);
    memcpy(&count, fields + 0x84, sizeof count); memcpy(&stride, fields + 0x88, sizeof stride);
    memcpy(&base, fields + 0x94, sizeof base); memcpy(&flags, fields + 0x80, sizeof flags);
    memcpy(&attr0_offset, fields + 0x8c, sizeof attr0_offset); memcpy(&attr1_offset, fields + 0x90, sizeof attr1_offset);
    if (!count) return false;                   // native no-hit path never touches indices
    if (count > 0xffffffffu / 12u || !page_has((const void *)(uintptr_t)indices, (size_t)count * 12u, false)) return true;
    if ((flags & 1u) && !add_u64(vertices, attr0_offset, &attr0)) return true;
    if ((flags & 2u) && !add_u64(vertices, attr1_offset, &attr1)) return true;
    ReadCache cache;
    for (u32 n = 0; n < count; n++) {
        u32 tri[3];
        memcpy(tri, (const void *)(uintptr_t)(indices + (u64)n * 12u), sizeof tri);
        u32 a = tri[0] + base, b = tri[1] + base, c = tri[2] + base;
        if (!triangle_vertex_readable(cache, vertices, a, stride) || !triangle_vertex_readable(cache, vertices, b, stride) ||
            !triangle_vertex_readable(cache, vertices, c, stride)) return true;
        if ((flags & 1u) && (!triangle_vertex_readable(cache, attr0, a, stride) ||
                            !triangle_vertex_readable(cache, attr0, b, stride) || !triangle_vertex_readable(cache, attr0, c, stride))) return true;
        if ((flags & 2u) && (!triangle_vertex_readable(cache, attr1, a, stride) ||
                            !triangle_vertex_readable(cache, attr1, b, stride) || !triangle_vertex_readable(cache, attr1, c, stride))) return true;
    }
    return false;
}
static void hook_render_cache(void *renderer, u32 node, const void *matrix)
{
    LARGE_INTEGER started = {};
    bool sampled = sample_start(g_cache_sample_seq, &started);
    // A page can be unmapped between VirtualQuery and a preflight load. Do not let a guard-created
    // read crash the game; the original code and narrowly scoped exception handler still get control.
    __try { render_cache_preflight(renderer, node); }
    __except (GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
        g_preflight_faults.fetch_add(1, std::memory_order_relaxed);
    }
    if (sampled) sample_end(g_cache_samples, started);
    CacheFn original = g_original_cache.load(std::memory_order_acquire);
    if (original) original(renderer, node, matrix);
}
static int hook_render_triangle(void *query)
{
    bool bad = false;
    LARGE_INTEGER started = {};
    bool sampled = sample_start(g_triangle_sample_seq, &started);
    __try { bad = render_triangle_bad_input(query); }
    __except (GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
        g_preflight_faults.fetch_add(1, std::memory_order_relaxed);
    }
    if (sampled) sample_end(g_triangle_samples, started);
    if (bad) {
        __try {
            *(u8 *)((u8 *)query + 0xa4) = 0;    // engine's own no-hit result field
            g_geometry_skips.fetch_add(1, std::memory_order_relaxed);
            return 0;
        } __except (GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
            g_preflight_faults.fetch_add(1, std::memory_order_relaxed);
        }
    }
    TriangleFn original = g_original_triangle.load(std::memory_order_acquire);
    return original ? original(query) : 0;
}

#define WALK(rva)     (g_walk + ((rva) - 0x12fd0u))
#define NODE(rva)     (g_node + ((rva) - 0x13ab0u))
#define CACHE(rva)    (g_cache + ((rva) - 0x12925u))
#define TRIANGLE(rva) (g_triangle + ((rva) - 0x39a40u))
static bool triangle_fault(CONTEXT *ctx, const EXCEPTION_RECORD *er)
{
    u64 at, off, rip = ctx->Rip, output;
    if (!g_triangle) return false;
    if (rip == (u64)(uintptr_t)TRIANGLE(0x39b26)) { at = ctx->R8;  off = 0; }
    else if (rip == (u64)(uintptr_t)TRIANGLE(0x39b29)) { at = ctx->R8;  off = 8; }
    else if (rip == (u64)(uintptr_t)TRIANGLE(0x39b30)) { at = ctx->R8;  off = 4; }
    else if (rip == (u64)(uintptr_t)TRIANGLE(0x39b4d)) { at = ctx->Rsi; off = ctx->Rbp; }
    else if (rip == (u64)(uintptr_t)TRIANGLE(0x39b52)) { at = ctx->Rsi; off = ctx->Rax; }
    else if (rip == (u64)(uintptr_t)TRIANGLE(0x39b5b)) { at = ctx->Rsi; off = ctx->R14; }
    else if (rip == (u64)(uintptr_t)TRIANGLE(0x39cfb) || rip == (u64)(uintptr_t)TRIANGLE(0x39d2c)) { at = ctx->Rax; off = ctx->Rbp; }
    else if (rip == (u64)(uintptr_t)TRIANGLE(0x39cff) || rip == (u64)(uintptr_t)TRIANGLE(0x39d30)) { at = ctx->Rax; off = ctx->R14; }
    else if (rip == (u64)(uintptr_t)TRIANGLE(0x39d04) || rip == (u64)(uintptr_t)TRIANGLE(0x39d35)) { at = ctx->Rax; off = ctx->Rcx; }
    else return false;
    if (er->ExceptionAddress != (void *)(uintptr_t)rip || !add_u64(at, off, &at) ||
        (u64)er->ExceptionInformation[1] != at || !add_u64(ctx->Rdx, 0xa4, &output) ||
        !page_has((void *)(uintptr_t)output, 1, true)) return false;
    ctx->Rip = (u64)(uintptr_t)TRIANGLE(0x39de5);  // native no-hit path
    g_geometry_skips.fetch_add(1, std::memory_order_relaxed);
    return true;
}
static LONG WINAPI render_fault_guard(EXCEPTION_POINTERS *xp)
{
    if (!g_walk || !g_node || !g_cache || !g_triangle || !xp || !xp->ExceptionRecord || !xp->ContextRecord)
        return EXCEPTION_CONTINUE_SEARCH;
    EXCEPTION_RECORD *er = xp->ExceptionRecord;
    CONTEXT *ctx = xp->ContextRecord;
    if (er->ExceptionCode != EXCEPTION_ACCESS_VIOLATION || er->NumberParameters < 2) return EXCEPTION_CONTINUE_SEARCH;
    if (er->ExceptionInformation[0] == 1 && ctx->Rip == (u64)(uintptr_t)CACHE(0x129a2) && er->ExceptionAddress == CACHE(0x129a2)) {
        u64 target, record;
        u32 slot;
        u64 slot_offset = ((u64)((u32)ctx->Rdx & 0xffffffu)) << 6;
        if (!add_u64(ctx->Rbp, 0x34, &record) || !add_u64(ctx->Rax, slot_offset, &target) ||
            (u64)er->ExceptionInformation[1] != target || !load_at(record, 0, &slot) || slot != (u32)ctx->Rdx)
            return EXCEPTION_CONTINUE_SEARCH;
        ctx->Rip = (u64)(uintptr_t)CACHE(0x129c6); // native cache allocator updates the record
        g_cache_repairs.fetch_add(1, std::memory_order_relaxed);
        return EXCEPTION_CONTINUE_EXECUTION;
    }
    if (er->ExceptionInformation[0] != 0) return EXCEPTION_CONTINUE_SEARCH;
    if (triangle_fault(ctx, er)) return EXCEPTION_CONTINUE_EXECUTION;
    if (ctx->R8 <= 0x3ff) return EXCEPTION_CONTINUE_SEARCH; // never hide a plausible child index
    if (ctx->Rip == (u64)(uintptr_t)NODE(0x13adb) && er->ExceptionAddress == NODE(0x13adb)) {
        if (ctx->Rsi > ~(u64)0 - 0x38 || (u64)er->ExceptionInformation[1] != ctx->Rsi + 0x38 || ctx->Rdi > 0xffffff)
            return EXCEPTION_CONTINUE_SEARCH;
        ctx->Rip = (u64)(uintptr_t)NODE(0x13bc9);  // valid parent material remains in EDI
    } else if (ctx->Rip == (u64)(uintptr_t)WALK(0x12ffd) && er->ExceptionAddress == WALK(0x12ffd)) {
        if (ctx->Rdi > ~(u64)0 - 0x38 || (u64)er->ExceptionInformation[1] != ctx->Rdi + 0x38)
            return EXCEPTION_CONTINUE_SEARCH;
        ctx->Rip = (u64)(uintptr_t)WALK(0x13038);  // skip exactly the bad child branch
    } else return EXCEPTION_CONTINUE_SEARCH;
    g_node_repairs.fetch_add(1, std::memory_order_relaxed);
    return EXCEPTION_CONTINUE_EXECUTION;
}

// Publish the trampoline before the code can branch to the hook. This closes the null-original
// window of a conventional 'original = install_detour(...)' on another renderer thread. Like the
// existing detour mechanism this does not suspend all engine threads; actual game timing remains
// an explicit validation limitation.
template <typename Fn> static bool install_published_detour(u8 *fn, size_t len, void *hook, std::atomic<Fn> &original)
{
    if (!fn || !hook || len < 14 || len > 32) return false;
    u8 *trampoline = (u8 *)VirtualAlloc(nullptr, 64, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!trampoline) return false;
    static const u8 jump[6] = { 0xff, 0x25, 0, 0, 0, 0 };
    void *back = fn + len;
    memcpy(trampoline, fn, len);
    memcpy(trampoline + len, jump, 6);
    memcpy(trampoline + len + 6, &back, 8);
    FlushInstructionCache(GetCurrentProcess(), trampoline, 64);
    DWORD previous;
    if (!VirtualProtect(fn, len, PAGE_EXECUTE_READWRITE, &previous)) {
        VirtualFree(trampoline, 0, MEM_RELEASE);
        return false;
    }
    u8 patch[32];
    memset(patch, 0x90, sizeof patch);
    memcpy(patch, jump, 6);
    memcpy(patch + 6, &hook, 8);
    original.store((Fn)trampoline, std::memory_order_release);
    memcpy(fn, patch, len);
    DWORD ignored;
    VirtualProtect(fn, len, previous, &ignored);
    FlushInstructionCache(GetCurrentProcess(), fn, len);
    return true;
}

// Install the narrow inline patch only after the caller verifies the exact image and allocator.
// The original first matrix write stays at +0x129a2, so the exact write-AV recovery remains valid.
// Like the existing entry detours, patch publication does not stop other renderer threads.
static bool install_cache_bounds_patch(u8 *site, u8 *next, u8 *allocate, volatile LONG *counter)
{
    const size_t len = sizeof sig_cache_inline_site;
    if (!site || !next || !allocate || !counter || len != 15 ||
        memcmp(site, sig_cache_inline_site, len)) return false;
    u8 *stub = (u8 *)VirtualAlloc(nullptr, render_cache_bounds::k_size,
                                 MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!stub) return false;
    if (!render_cache_bounds::build(stub, render_cache_bounds::k_size,
                                    (uintptr_t)next, (uintptr_t)allocate, (uintptr_t)counter)) {
        VirtualFree(stub, 0, MEM_RELEASE);
        return false;
    }
    DWORD stub_old;
    if (!VirtualProtect(stub, render_cache_bounds::k_size, PAGE_EXECUTE_READ, &stub_old) ||
        !FlushInstructionCache(GetCurrentProcess(), stub, render_cache_bounds::k_size)) {
        VirtualFree(stub, 0, MEM_RELEASE);
        return false;
    }
    DWORD previous;
    if (!VirtualProtect(site, len, PAGE_EXECUTE_READWRITE, &previous)) {
        VirtualFree(stub, 0, MEM_RELEASE);
        return false;
    }
    // Never overwrite a newly installed third-party patch.
    if (memcmp(site, sig_cache_inline_site, len)) {
        DWORD ignored;
        VirtualProtect(site, len, previous, &ignored);
        VirtualFree(stub, 0, MEM_RELEASE);
        return false;
    }
    u8 patch[15] = { 0xff,0x25,0,0,0,0 };
    memcpy(patch + 6, &stub, sizeof stub);
    patch[14] = 0x90;
    memcpy(site, patch, len);
    DWORD ignored;
    if (!VirtualProtect(site, len, previous, &ignored))
        mp_log("render guard: warning: failed to restore cache code protection");
    if (!FlushInstructionCache(GetCurrentProcess(), site, len))
        mp_log("render guard: warning: cache instruction flush failed");
    return true; // the stub must remain allocated while the patch exists
}

static bool install_cache_bounds(HMODULE image, u8 *cache)
{
    if (!image || cache != (u8 *)image + 0x12925 ||
        !hook_image_identity("basic_render.codelib", 0x66f2d710, 0x64000) ||
        !same_code(image, cache, sig_cache_complete, sizeof sig_cache_complete) ||
        !same_code(image, cache + (0x1298fu - 0x12925u), sig_cache_inline_site, sizeof sig_cache_inline_site) ||
        !same_code(image, cache + (0x193c0u - 0x12925u), sig_cache_allocator, sizeof sig_cache_allocator) ||
        !same_code(image, cache + (0x19aa0u - 0x12925u), sig_cache_growth, sizeof sig_cache_growth)) return false;
    return install_cache_bounds_patch(cache + (0x1298fu - 0x12925u),
                                      cache + (0x1299eu - 0x12925u),
                                      cache + (0x129c6u - 0x12925u), &g_cache_bounds_repairs);
}

// Strict preflight is an explicit opt-in only. In particular, an invalid value must never
// turn expensive per-call VirtualQuery checks back on silently at the start of a battle.
static bool strict_cache_requested()
{
    char value[24] = {};
    DWORD n = GetEnvironmentVariableA("MP_RENDER_CACHE_GUARD", value, (DWORD)sizeof value);
    if (!n) return false;
    if (n < sizeof value && !lstrcmpiA(value, "strict")) return true;
    if (n >= sizeof value || lstrcmpiA(value, "fast"))
        mp_log("render guard: unknown MP_RENDER_CACHE_GUARD value; using fast mode, not strict preflight");
    return false;
}

using CacheInstaller = bool (*)(HMODULE, u8 *);
enum CacheInstallMode { cache_exact_av_only, cache_bounds, cache_strict };
static bool install_cache_strict(HMODULE image, u8 *cache)
{
    return same_code(image, cache, sig_cache_complete, sizeof sig_cache_complete) &&
           install_published_detour(cache, 15, (void *)hook_render_cache, g_original_cache);
}
// Never try strict after a fast-install failure. In that case the already installed, narrow
// exception handler is the only cache recovery; writable wrong addresses are NOT caught.
static CacheInstallMode install_selected_cache(bool strict, HMODULE image, u8 *cache,
                                              CacheInstaller fast, CacheInstaller slow)
{
    if (strict) return slow(image, cache) ? cache_strict : cache_exact_av_only;
    return fast(image, cache) ? cache_bounds : cache_exact_av_only;
}

static bool checked_layout(HMODULE image, u8 *walk, u8 *node, u8 *cache, u8 *tri)
{
    return walk && node && cache && tri &&
           same_code(image, walk + (0x13038u - 0x12fd0u), sig_walk_fallback, sizeof sig_walk_fallback) &&
           same_code(image, node + (0x13bc9u - 0x13ab0u), sig_node_fallback, sizeof sig_node_fallback) &&
           same_code(image, cache + (0x129c6u - 0x12925u), sig_cache_rebuild, sizeof sig_cache_rebuild) &&
           same_code(image, tri + (0x39b20u - 0x39a40u), sig_triangle_loop, sizeof sig_triangle_loop) &&
           same_code(image, tri + (0x39cf2u - 0x39a40u), sig_triangle_attr0, sizeof sig_triangle_attr0) &&
           same_code(image, tri + (0x39d23u - 0x39a40u), sig_triangle_attr1, sizeof sig_triangle_attr1) &&
           same_code(image, tri + (0x39de5u - 0x39a40u), sig_triangle_fallback, sizeof sig_triangle_fallback);
}
} // namespace

// Call only from the tactical network session. Missing modules are retried; an unsupported or
// ambiguous build is logged once and is never patched.
void render_guard_install()
{
    HMODULE image = GetModuleHandleA("basic_render.codelib");
    if (!image) return;
    bool expected = false;
    if (!g_attempted.compare_exchange_strong(expected, true)) return;
    // This module does not import the old sidecar. If something else loads it, its separate
    // renderer hooks can conflict or add overhead; we cannot safely unload an external DLL.
    if (GetModuleHandleA("mp_vitals.dll"))
        mp_log("render guard: WARNING: legacy mp_vitals.dll is loaded by another component; independent hooks may conflict or add FPS cost");
    const HookSig w[] = { { 0x12fd0, sig_walk_node, sizeof sig_walk_node } };
    const HookSig n[] = { { 0x13ab0, sig_node, sizeof sig_node } };
    const HookSig c[] = { { 0x12925, sig_cache_complete, sizeof sig_cache_complete } };
    const HookSig t[] = { { 0x39a40, sig_triangle, sizeof sig_triangle } };
    u8 *walk = (u8 *)hook_resolve_checked("basic_render.codelib", w, 1, "renderer child traversal");
    u8 *node = (u8 *)hook_resolve_checked("basic_render.codelib", n, 1, "renderer material lookup");
    u8 *cache = (u8 *)hook_resolve_checked("basic_render.codelib", c, 1, "renderer matrix cache");
    u8 *tri = (u8 *)hook_resolve_checked("basic_render.codelib", t, 1, "renderer triangle query");
    if (!checked_layout(image, walk, node, cache, tri)) {
        mp_log("render guard: code or recovery target mismatch; renderer left unchanged");
        return;
    }
    g_walk = walk; g_node = node; g_cache = cache; g_triangle = tri;
    g_handler = AddVectoredExceptionHandler(1, render_fault_guard);
    if (!g_handler) {
        g_walk = g_node = g_cache = g_triangle = nullptr;
        mp_log("render guard: could not register exception handler; renderer left unchanged");
        return;
    }
    // Choose once per process. The fast inline path and the old entry detour cannot coexist.
    // An unavailable fast patch leaves exact-AV recovery only; it NEVER turns strict on behind
    // the user's back. Strict's per-call VirtualQuery cost is incurred only on explicit request.
    const bool strict = strict_cache_requested();
    CacheInstallMode mode = install_selected_cache(strict, image, cache, install_cache_bounds, install_cache_strict);
    g_cache_bounds_hooked = mode == cache_bounds;
    g_cache_hooked = mode == cache_strict;
    if (mode == cache_exact_av_only)
        mp_log("render guard: requested cache preflight unavailable; exact write-AV recovery only; strict preflight not auto-enabled");
    if (same_code(image, tri, sig_triangle, sizeof sig_triangle))
        g_triangle_hooked = install_published_detour(tri, 14, (void *)hook_render_triangle, g_original_triangle);
    mp_log("render guard: verified exact exception paths active; cache %s, geometry preflight %s",
           g_cache_bounds_hooked ? "experimental inline bounds" : g_cache_hooked ? "strict preflight" : "exact-AV only",
           g_triangle_hooked ? "installed" : "unavailable");
    if (g_cache_bounds_hooked)
        mp_log("render guard: set MP_RENDER_CACHE_GUARD=strict before game start to opt in to expensive strict allocation checks");
}

void render_guard_report(DWORD now)
{
    static DWORD last;
    static u32 node, cache, bounds, triangle, preflight;
    static u64 cache_count, cache_ticks, triangle_count, triangle_ticks;
    if (!g_handler || now - last < 5000) return;
    last = now;
    u64 cc = g_cache_samples.count.load(std::memory_order_relaxed), ct = g_cache_samples.ticks.load(std::memory_order_relaxed);
    u64 tc = g_triangle_samples.count.load(std::memory_order_relaxed), tt = g_triangle_samples.ticks.load(std::memory_order_relaxed);
    LARGE_INTEGER freq = {};
    if ((cc != cache_count || tc != triangle_count) && QueryPerformanceFrequency(&freq) && freq.QuadPart > 0) {
        u64 dc = cc - cache_count, dt = tc - triangle_count;
        double cache_us = dc ? (double)(ct - cache_ticks) * 1000000.0 / (double)freq.QuadPart / (double)dc : 0.0;
        double tri_us = dt ? (double)(tt - triangle_ticks) * 1000000.0 / (double)freq.QuadPart / (double)dt : 0.0;
        mp_log("render guard: 1/128 preflight samples: cache %llu avg %.2f us, geometry %llu avg %.2f us (wall time, excludes engine)",
               (unsigned long long)dc, cache_us, (unsigned long long)dt, tri_us);
    }
    cache_count = cc; cache_ticks = ct; triangle_count = tc; triangle_ticks = tt;
    u32 n = g_node_repairs.load(std::memory_order_relaxed), c = g_cache_repairs.load(std::memory_order_relaxed);
    u32 b = (u32)InterlockedCompareExchange(&g_cache_bounds_repairs, 0, 0);
    u32 t = g_geometry_skips.load(std::memory_order_relaxed), p = g_preflight_faults.load(std::memory_order_relaxed);
    if (n != node || c != cache || b != bounds || t != triangle || p != preflight) {
        mp_log("render guard: total child branches recovered %u, cache strict/fault repairs %u, bounds repairs %u, geometry queries skipped %u, preflight races %u", n, c, b, t, p);
        node = n; cache = c; bounds = b; triangle = t; preflight = p;
    }
}

#ifdef NETV21_TEST_HOOKS
// The previous field-by-field implementation, kept only as a semantic reference for synthetic
// valid, invalid, protected-boundary and overflow cases; never shipped in the game module.
static bool triangle_reference(void *query)
{
    u64 q = (u64)(uintptr_t)query, output, vertices, indices, attr0 = 0, attr1 = 0;
    u32 count, stride, base, flags, attr0_offset, attr1_offset;
    if (!query || !page_has(query, 0x98, false) || !add_u64(q, 0xa4, &output) ||
        !page_has((void *)(uintptr_t)output, 1, true)) return false;
    if (!load_at(q, 0x78, &vertices) || !load_at(q, 0x70, &indices) || !load_at(q, 0x84, &count) ||
        !load_at(q, 0x88, &stride) || !load_at(q, 0x94, &base) || !load_at(q, 0x80, &flags) ||
        !load_at(q, 0x8c, &attr0_offset) || !load_at(q, 0x90, &attr1_offset)) return false;
    if (!count) return false;
    if (count > 0xffffffffu / 12u || !page_has((const void *)(uintptr_t)indices, (size_t)count * 12u, false)) return true;
    if ((flags & 1u) && !add_u64(vertices, attr0_offset, &attr0)) return true;
    if ((flags & 2u) && !add_u64(vertices, attr1_offset, &attr1)) return true;
    ReadCache cache;
    for (u32 n = 0; n < count; n++) {
        u32 tri[3]; memcpy(tri, (const void *)(uintptr_t)(indices + (u64)n * 12u), sizeof tri);
        u32 a = tri[0] + base, b = tri[1] + base, c = tri[2] + base;
        if (!triangle_vertex_readable(cache, vertices, a, stride) || !triangle_vertex_readable(cache, vertices, b, stride) ||
            !triangle_vertex_readable(cache, vertices, c, stride)) return true;
        if ((flags & 1u) && (!triangle_vertex_readable(cache, attr0, a, stride) ||
                            !triangle_vertex_readable(cache, attr0, b, stride) || !triangle_vertex_readable(cache, attr0, c, stride))) return true;
        if ((flags & 2u) && (!triangle_vertex_readable(cache, attr1, a, stride) ||
                            !triangle_vertex_readable(cache, attr1, b, stride) || !triangle_vertex_readable(cache, attr1, c, stride))) return true;
    }
    return false;
}
void render_test_reset_page_queries() { g_test_page_queries = 0; }
unsigned render_test_page_queries() { return g_test_page_queries; }
bool render_test_triangle_reference(void *p) { return triangle_reference(p); }
// Real guard logic with a synthetic layout. Never compiled into the game module.
void render_test_layout(void *base) {
    g_walk = (u8 *)base + 0x12fd0; g_node = (u8 *)base + 0x13ab0;
    g_cache = (u8 *)base + 0x12925; g_triangle = (u8 *)base + 0x39a40;
}
LONG render_test_fault(EXCEPTION_POINTERS *p) { return render_fault_guard(p); }
bool render_test_triangle(void *p) { return render_triangle_bad_input(p); }
void render_test_cache(void *p, u32 node) { render_cache_preflight(p, node); }
void render_test_cache_reference(void *p, u32 node) { render_cache_preflight_reference(p, node); }
bool render_test_install_cache_bounds(void *site, void *next, void *allocate, volatile LONG *counter) {
    return install_cache_bounds_patch((u8 *)site, (u8 *)next, (u8 *)allocate, counter);
}
bool render_test_cache_strict_requested() { return strict_cache_requested(); }
int render_test_select_cache(bool strict, bool (*fast)(HMODULE, unsigned char *),
                             bool (*slow)(HMODULE, unsigned char *)) {
    return (int)install_selected_cache(strict, nullptr, nullptr, fast, slow);
}
using TestFn = int (*)();
static std::atomic<TestFn> g_test_original(nullptr);
bool render_test_install(void *target, void *hook) {
    return install_published_detour((u8 *)target, 14, hook, g_test_original);
}
int render_test_original() {
    TestFn fn = g_test_original.load(std::memory_order_acquire);
    return fn ? fn() : -1;
}
#endif

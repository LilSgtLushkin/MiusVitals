// No-game tests call the production preflights and exception handler with synthetic Windows memory.
#include "render_guard.h"
#include "detour.h"
#include "common.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

void mp_log(const char *, ...) {}
static int checks, failed;
static void check(bool ok, const char *name) {
    ++checks;
    if (!ok) { ++failed; printf("FAIL: %s\n", name); }
}
static int test_hook() { return render_test_original() + 2; }
using PublishedFn = int (*)();
static std::atomic<PublishedFn> published_original(nullptr);
static int published_hook() {
    PublishedFn original = published_original.load(std::memory_order_acquire);
    return original ? original() + 5 : -1;
}

template<typename T> static void store(void *base, size_t off, T value) { memcpy((char *)base + off, &value, sizeof value); }
int main()
{
    SYSTEM_INFO info = {}; GetSystemInfo(&info);
    size_t page = info.dwPageSize;
    char *code = (char *)VirtualAlloc(nullptr, 0x50000, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!code) return 2;
    render_test_layout(code);
    CONTEXT ctx = {};
    EXCEPTION_RECORD er = {};
    EXCEPTION_POINTERS xp = { &er, &ctx };
    er.ExceptionCode = EXCEPTION_ACCESS_VIOLATION; er.NumberParameters = 2;
    er.ExceptionInformation[0] = 0;
    ctx.R8 = 0x400; ctx.Rsi = 0x12340000; ctx.Rdi = 42;
    ctx.Rip = (DWORD64)(uintptr_t)(code + 0x13adb);
    er.ExceptionAddress = code + 0x13adb; er.ExceptionInformation[1] = ctx.Rsi + 0x38;
    check(render_test_fault(&xp) == EXCEPTION_CONTINUE_EXECUTION && ctx.Rip == (DWORD64)(uintptr_t)(code + 0x13bc9), "invalid material child uses native parent return");
    ctx.Rip = (DWORD64)(uintptr_t)(code + 0x13adb); ctx.R8 = 0x3ff;
    check(render_test_fault(&xp) == EXCEPTION_CONTINUE_SEARCH, "plausible child index is never hidden");
    ctx.R8 = 0x400; er.ExceptionInformation[1]++;
    check(render_test_fault(&xp) == EXCEPTION_CONTINUE_SEARCH, "unrelated material address is not hidden");
    ctx.Rip = (DWORD64)(uintptr_t)(code + 0x12ffd); ctx.Rdi = 0x56780000;
    er.ExceptionAddress = code + 0x12ffd; er.ExceptionInformation[1] = ctx.Rdi + 0x38;
    check(render_test_fault(&xp) == EXCEPTION_CONTINUE_EXECUTION && ctx.Rip == (DWORD64)(uintptr_t)(code + 0x13038), "invalid walk branch uses native epilogue");

    alignas(8) unsigned char record[320] = {};
    const u32 stale = 0x00de741b;
    store(record, 0x34, stale);
    ctx.Rip = (DWORD64)(uintptr_t)(code + 0x129a2); ctx.Rbp = (DWORD64)(uintptr_t)record;
    ctx.Rax = 0x100000; ctx.Rdx = stale;
    er.ExceptionInformation[0] = 1; er.ExceptionAddress = code + 0x129a2;
    er.ExceptionInformation[1] = ctx.Rax + ((u64)(stale & 0xffffff) << 6);
    check(render_test_fault(&xp) == EXCEPTION_CONTINUE_EXECUTION && ctx.Rip == (DWORD64)(uintptr_t)(code + 0x129c6), "exact stale cache write uses native allocator");
    ctx.Rip = (DWORD64)(uintptr_t)(code + 0x129a2); er.ExceptionInformation[1]++;
    check(render_test_fault(&xp) == EXCEPTION_CONTINUE_SEARCH, "other write address is not hidden");

    alignas(8) unsigned char query[0xa8] = {};
    er.ExceptionInformation[0] = 0;
    ctx.Rip = (DWORD64)(uintptr_t)(code + 0x39b52); ctx.Rsi = 0x12300000; ctx.Rax = 0x2000;
    ctx.Rdx = (DWORD64)(uintptr_t)query;
    er.ExceptionAddress = code + 0x39b52; er.ExceptionInformation[1] = ctx.Rsi + ctx.Rax;
    check(render_test_fault(&xp) == EXCEPTION_CONTINUE_EXECUTION && ctx.Rip == (DWORD64)(uintptr_t)(code + 0x39de5), "specific bad geometry read uses native no-hit path");
    ctx.Rip = (DWORD64)(uintptr_t)(code + 0x39b52); er.ExceptionInformation[1]++;
    check(render_test_fault(&xp) == EXCEPTION_CONTINUE_SEARCH, "unrelated geometry fault is not hidden");
    er.ExceptionCode = EXCEPTION_ILLEGAL_INSTRUCTION;
    check(render_test_fault(&xp) == EXCEPTION_CONTINUE_SEARCH, "non-access-violation is not hidden");

    u32 indices[3] = { 0, 1, 2 };
    float vertices[12] = {};
    store(query, 0x70, (u64)(uintptr_t)indices); store(query, 0x78, (u64)(uintptr_t)vertices);
    store(query, 0x84, (u32)1); store(query, 0x88, (u32)16);
    render_test_reset_page_queries();
    check(!render_test_triangle(query), "valid triangle passes preflight");
    unsigned optimized_queries = render_test_page_queries();
    render_test_reset_page_queries();
    check(!render_test_triangle_reference(query), "reference accepts the same triangle");
    unsigned reference_queries = render_test_page_queries();
    check(optimized_queries == 4 && reference_queries == 12, "remove eight redundant field queries while retaining other checks");
    char *pages = (char *)VirtualAlloc(nullptr, page * 2, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!pages) return 2;
    DWORD old = 0;
    if (!VirtualProtect(pages + page, page, PAGE_NOACCESS, &old)) return 2;
    store(query, 0x78, (u64)(uintptr_t)(pages + page - 16));
    check(render_test_triangle(query), "triangle vertex in inaccessible page rejected");
    store(query, 0x84, (u32)0);
    check(!render_test_triangle(query), "empty query takes native no-hit path");
    check(!render_test_triangle(nullptr), "unknown query left to engine");
    // 512 stable synthetic inputs compare with the old field-by-field implementation, including
    // modulo-32-bit offsets, unreadable vertices, flags/attributes, and a protected page boundary.
    u32 rng = 0x13579bdf;
    bool same = true;
    for (int i = 0; i < 512; ++i) {
        rng = rng * 1664525u + 1013904223u;
        store(query, 0x78, (u64)(uintptr_t)((rng & 1) ? (void *)vertices : (void *)(pages + page - 16)));
        store(query, 0x84, (u32)((rng >> 1) & 1));
        store(query, 0x88, (u32)((rng & 4) ? 0xfffffff0u : 16u));
        store(query, 0x94, (u32)((rng >> 3) & 3));
        store(query, 0x80, (u32)((rng >> 5) & 3));
        store(query, 0x8c, (u32)((rng & 128) ? 0xfffffff0u : 0u));
        store(query, 0x90, (u32)((rng & 256) ? 16u : 0u));
        if (render_test_triangle(query) != render_test_triangle_reference(query)) same = false;
    }
    check(same, "optimized and original preflight agree on 512 synthetic geometries");
    void *boundary = pages + page - 0x98; // fields readable but no-hit output is not writable
    check(!render_test_triangle(boundary) && !render_test_triangle_reference(boundary),
          "unwritable output remains the engine's path");
    VirtualFree(pages, 0, MEM_RELEASE);

    alignas(8) unsigned char renderer[0x248] = {};
    u64 groups[8] = {};
    void *pool = VirtualAlloc(nullptr, page * 2, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!pool) return 2;
    groups[0] = (u64)(uintptr_t)record;
    store(renderer, 0x240, (u64)(uintptr_t)groups); store(renderer, 0x168, (u64)(uintptr_t)pool);
    store(record, 0x34, (u32)1);
    render_test_reset_page_queries();
    render_test_cache(renderer, 0);
    u32 value = 0; memcpy(&value, record + 0x34, 4);
    check(value == 1, "cache entry inside renderer allocation preserved");
    unsigned cached_queries = render_test_page_queries();
    render_test_reset_page_queries();
    render_test_cache_reference(renderer, 0);
    unsigned original_queries = render_test_page_queries();
    check(cached_queries < original_queries && original_queries == 5, "per-call region cache avoids redundant VirtualQuery calls");
    LARGE_INTEGER frequency, begin, middle, end; QueryPerformanceFrequency(&frequency);
    const unsigned iterations = 10000;
    QueryPerformanceCounter(&begin);
    for (unsigned i = 0; i < iterations; ++i) render_test_cache(renderer, 0);
    QueryPerformanceCounter(&middle);
    for (unsigned i = 0; i < iterations; ++i) render_test_cache_reference(renderer, 0);
    QueryPerformanceCounter(&end);
    printf("cache preflight synthetic: queries cached=%u previous=%u; mean us cached=%.2f previous=%.2f (not FPS)\n",
           cached_queries, original_queries, (middle.QuadPart - begin.QuadPart) * 1000000.0 / frequency.QuadPart / iterations,
           (end.QuadPart - middle.QuadPart) * 1000000.0 / frequency.QuadPart / iterations);
    DWORD previous = 0;
    if (!VirtualProtect((char *)pool + page, page, PAGE_EXECUTE_READWRITE, &previous)) return 2;
    store(record, 0x34, (u32)(page / 64));
    render_test_cache(renderer, 0); memcpy(&value, record + 0x34, 4);
    check(value == (u32)(page / 64), "different writable region in same allocation is preserved");
    if (!VirtualProtect((char *)pool + page, page, PAGE_READONLY, &previous)) return 2;
    render_test_cache(renderer, 0); memcpy(&value, record + 0x34, 4);
    check(value == 0xfffffffeu, "non-writable region in the same allocation is still rejected");
    store(record, 0x34, stale);
    render_test_cache(renderer, 0);
    memcpy(&value, record + 0x34, 4);
    check(value == 0xfffffffeu, "out-of-pool cache entry set to native missing sentinel");
    bool cache_same = true;
    for (unsigned i = 0; i < 512; ++i) {
        u32 input = (i % 5 == 0) ? stale : (i % 5 == 1) ? 0xfffffffeu :
                    (i % 5 == 2) ? (u32)(page / 64) : (i % 5 == 3) ? 1u : 0u;
        store(record, 0x34, input); render_test_cache(renderer, 0);
        u32 fast = 0; memcpy(&fast, record + 0x34, 4);
        store(record, 0x34, input); render_test_cache_reference(renderer, 0);
        u32 reference = 0; memcpy(&reference, record + 0x34, 4);
        if (fast != reference) cache_same = false;
    }
    check(cache_same, "cached and original preflights agree for 512 valid, protected and stale entries");
    VirtualFree(pool, 0, MEM_RELEASE);
    VirtualFree(code, 0, MEM_RELEASE);

    unsigned char *function = (unsigned char *)VirtualAlloc(nullptr, page, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!function) return 2;
    memset(function, 0x90, 14); function[14] = 0xb8; function[15] = 7;
    function[16] = function[17] = function[18] = 0; function[19] = 0xc3;
    FlushInstructionCache(GetCurrentProcess(), function, 20);
    using Fn = int (*)();
    Fn call = (Fn)function;
    check(call() == 7, "synthetic original function");
    check(render_test_install(function, (void *)test_hook), "synthetic detour installed");
    check(call() == 9 && render_test_original() == 7, "hook sees published callable original");
    VirtualFree(function, 0, MEM_RELEASE);

    // The operational hooks use detour.h's published trampoline, not the renderer's
    // private helper. Exercise that exact implementation on executable synthetic code.
    function = (unsigned char *)VirtualAlloc(nullptr, page, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!function) return 2;
    memset(function, 0x90, 14); function[14] = 0xb8; function[15] = 7;
    function[16] = function[17] = function[18] = 0; function[19] = 0xc3;
    FlushInstructionCache(GetCurrentProcess(), function, 20); call = (Fn)function;
    check(install_published_detour(function, 14, published_hook, published_original), "operation published detour installed");
    check(call() == 12 && published_original.load() && published_original.load()() == 7, "operation hook calls published original");
    VirtualFree(function, 0, MEM_RELEASE);

    printf("render guard selftest: %d checks, %d failed\n", checks, failed);
    return failed ? 1 : 0;
}

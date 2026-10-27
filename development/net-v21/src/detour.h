// In-memory detour of an engine function: its first `len` bytes (whole instructions, none of them
// position dependent, at least 14) are replaced by a jump to `hook`; the returned trampoline runs those
// bytes and continues in the original, so the hook can still call it. Nothing on disk changes, and the
// caller checks the function's bytes against a known signature first, so an unknown build is left alone.
#pragma once
#include <windows.h>
#include <string.h>
#include <atomic>

// The engine's functions are reached through typed function pointers; a hook handed over by name is
// one of those types. This overload does the conversion once, so every call site reads the same way
// under both compilers the module has been built with (MSVC allows the implicit conversion, clang not).
template <class Hook>
inline void *install_detour(void *target, size_t len, Hook hook)
{
    return install_detour(target, len, (void *)hook);
}

inline void *install_detour(void *target, size_t len, void *hook)
{
    if (len < 14 || len > 32) return 0;
    unsigned char *fn = (unsigned char *)target;
    unsigned char *tramp = (unsigned char *)VirtualAlloc(0, 64, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!tramp) return 0;
    static const unsigned char jmp[6] = { 0xff, 0x25, 0, 0, 0, 0 };     // jmp [rip+0], address follows
    void *back = fn + len;
    memcpy(tramp, fn, len);
    memcpy(tramp + len, jmp, 6);
    memcpy(tramp + len + 6, &back, 8);
    DWORD old;
    if (!VirtualProtect(fn, len, PAGE_EXECUTE_READWRITE, &old)) { VirtualFree(tramp, 0, MEM_RELEASE); return 0; }
    unsigned char patch[32];
    memset(patch, 0x90, sizeof patch);
    memcpy(patch, jmp, 6);
    memcpy(patch + 6, &hook, 8);
    memcpy(fn, patch, len);
    VirtualProtect(fn, len, old, &old);
    FlushInstructionCache(GetCurrentProcess(), fn, len);
    FlushInstructionCache(GetCurrentProcess(), tramp, 64);
    return tramp;
}

// Publish the trampoline before exposing the hook to other threads. This reduces the
// null-original race, but does not suspend engine threads while target bytes are patched.
template <class Fn, class Hook>
inline bool install_published_detour(void *target, size_t len, Hook hook, std::atomic<Fn> &original)
{
    if (!target || !hook || len < 14 || len > 32) return false;
    unsigned char *fn = (unsigned char *)target;
    unsigned char *tramp = (unsigned char *)VirtualAlloc(nullptr, 64, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!tramp) return false;
    static const unsigned char jmp[6] = { 0xff, 0x25, 0, 0, 0, 0 };
    void *back = fn + len, *destination = (void *)hook;
    memcpy(tramp, fn, len); memcpy(tramp + len, jmp, 6); memcpy(tramp + len + 6, &back, 8);
    FlushInstructionCache(GetCurrentProcess(), tramp, 64);
    DWORD old;
    if (!VirtualProtect(fn, len, PAGE_EXECUTE_READWRITE, &old)) { VirtualFree(tramp, 0, MEM_RELEASE); return false; }
    unsigned char patch[32]; memset(patch, 0x90, sizeof patch);
    memcpy(patch, jmp, 6); memcpy(patch + 6, &destination, 8);
    original.store((Fn)tramp, std::memory_order_release);
    memcpy(fn, patch, len);
    DWORD ignored; VirtualProtect(fn, len, old, &ignored);
    FlushInstructionCache(GetCurrentProcess(), fn, len);
    return true;
}

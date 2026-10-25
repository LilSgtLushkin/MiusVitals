// minimal stand-in for <windows.h> so that mp_vitals.c can be exercised natively (test only)
#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#define WINAPI
#define CALLBACK
#define TRUE 1
#define MEM_COMMIT 0x1000
#define MEM_RESERVE 0x2000
#define MEM_RELEASE 0x8000
#define PAGE_NOACCESS 1
#define PAGE_READWRITE 4
#define PAGE_WRITECOPY 8
#define PAGE_GUARD 0x100
#define PAGE_EXECUTE_READWRITE 0x40
#define PAGE_EXECUTE_WRITECOPY 0x80
#define DLL_PROCESS_ATTACH 1
#define IMAGE_DOS_SIGNATURE 0x5a4d
#define IMAGE_NT_SIGNATURE 0x4550
typedef unsigned long DWORD; typedef long LONG; typedef uintptr_t ULONG_PTR; typedef int BOOL; typedef void *LPVOID, *HMODULE, *HINSTANCE, *PVOID; typedef const char *LPCSTR;
typedef struct _EXCEPTION_RECORD { DWORD ExceptionCode, ExceptionFlags; struct _EXCEPTION_RECORD *ExceptionRecord; void *ExceptionAddress; DWORD NumberParameters; ULONG_PTR ExceptionInformation[15]; } EXCEPTION_RECORD;
typedef struct _CONTEXT { uint64_t Rip, Rsp, Rax, Rcx, Rdx, Rbp, Rsi, Rdi, R8, R14; } CONTEXT;
typedef struct _EXCEPTION_POINTERS { EXCEPTION_RECORD *ExceptionRecord; CONTEXT *ContextRecord; } EXCEPTION_POINTERS;
typedef LONG (WINAPI *PVECTORED_EXCEPTION_HANDLER)(EXCEPTION_POINTERS *);
#define EXCEPTION_ACCESS_VIOLATION ((DWORD)0xc0000005u)
#define EXCEPTION_CONTINUE_EXECUTION (-1)
#define EXCEPTION_CONTINUE_SEARCH 0
typedef struct { void *BaseAddress, *AllocationBase; DWORD AllocationProtect; size_t RegionSize; DWORD State, Protect, Type; } MEMORY_BASIC_INFORMATION;
typedef struct { int x; } CRITICAL_SECTION;
typedef struct { unsigned short e_magic; int e_lfanew; } IMAGE_DOS_HEADER;
typedef struct { DWORD TimeDateStamp; } FH; typedef struct { DWORD SizeOfImage; } OH;
typedef struct { DWORD Signature; FH FileHeader; OH OptionalHeader; } IMAGE_NT_HEADERS;
extern DWORD fake_tick;
extern uintptr_t fake_vq_unmapped_from;
// Test-only command line/environment control. A null value means the variable is absent.
extern const char *fake_env_idle, *fake_env_state, *fake_cmd;
static inline int fake_name_eq(const char *a, const char *b) { while (*a && *a == *b) { a++; b++; } return !*a && !*b; }
static inline size_t VirtualQuery(const void *p, MEMORY_BASIC_INFORMATION *m, size_t n) {
    (void)n; m->BaseAddress = (void*)p; m->AllocationBase = (void*)1; m->RegionSize = (size_t)1 << 40;
    m->State = (uintptr_t)p >= fake_vq_unmapped_from ? MEM_RESERVE : MEM_COMMIT; m->Protect = PAGE_READWRITE; return 1;
}
static inline DWORD GetTickCount(void) { return fake_tick; }
static inline const char *GetCommandLineA(void) { return fake_cmd ? fake_cmd : ""; }
static inline DWORD GetEnvironmentVariableA(const char *n, char *b, DWORD s) {
    const char *p = fake_name_eq(n, "MP_IDLE_MS") ? fake_env_idle : fake_name_eq(n, "MP_SYNC_STATE") ? fake_env_state : 0;
    int k = 0;
    if (!p) return 0;
    while (p[k]) k++;
    if (b && s) {
        if ((DWORD)k >= s) return (DWORD)k + 1;
        for (int i = 0; i <= k; i++) b[i] = p[i];
    }
    return (DWORD)k;
}
static inline BOOL SetEnvironmentVariableA(const char *n, const char *v) {
    if (!fake_name_eq(n, "MP_SYNC_STATE") || !v) return 0;
    fake_env_state = v;
    return 1;
}
static inline void InitializeCriticalSection(CRITICAL_SECTION *c) { (void)c; }
static inline void EnterCriticalSection(CRITICAL_SECTION *c) { (void)c; }
static inline void LeaveCriticalSection(CRITICAL_SECTION *c) { (void)c; }
static inline HMODULE GetModuleHandleA(const char *n) { (void)n; return 0; }
static inline BOOL DisableThreadLibraryCalls(HMODULE m) { (void)m; return 1; }
static inline PVOID AddVectoredExceptionHandler(unsigned long first, PVECTORED_EXCEPTION_HANDLER h) { (void)first; (void)h; return (PVOID)1; }
static inline void *VirtualAlloc(void *a, size_t s, DWORD t, DWORD p) { (void)a;(void)s;(void)t;(void)p; return 0; }
static inline BOOL VirtualFree(void *a, size_t s, DWORD t) { (void)a;(void)s;(void)t; return 1; }
static inline BOOL VirtualProtect(void *a, size_t s, DWORD n, DWORD *o) { (void)a;(void)s;(void)n;(void)o; return 1; }
static inline void *GetCurrentProcess(void) { return 0; }
static inline BOOL FlushInstructionCache(void *p, const void *a, size_t s) { (void)p;(void)a;(void)s; return 1; }

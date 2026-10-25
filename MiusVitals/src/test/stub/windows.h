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
#define PAGE_GUARD 0x100
#define PAGE_EXECUTE_READWRITE 0x40
#define DLL_PROCESS_ATTACH 1
#define IMAGE_DOS_SIGNATURE 0x5a4d
#define IMAGE_NT_SIGNATURE 0x4550
typedef unsigned long DWORD; typedef int BOOL; typedef void *LPVOID, *HMODULE, *HINSTANCE; typedef const char *LPCSTR;
typedef struct { void *BaseAddress; DWORD State, Protect; size_t RegionSize; } MEMORY_BASIC_INFORMATION;
typedef struct { int x; } CRITICAL_SECTION;
typedef struct { unsigned short e_magic; int e_lfanew; } IMAGE_DOS_HEADER;
typedef struct { DWORD TimeDateStamp; } FH; typedef struct { DWORD SizeOfImage; } OH;
typedef struct { DWORD Signature; FH FileHeader; OH OptionalHeader; } IMAGE_NT_HEADERS;
extern DWORD fake_tick;
static inline size_t VirtualQuery(const void *p, MEMORY_BASIC_INFORMATION *m, size_t n) { (void)n; m->BaseAddress = (void*)p; m->State = MEM_COMMIT; m->Protect = 4; m->RegionSize = (size_t)1 << 40; return 1; }
static inline DWORD GetTickCount(void) { return fake_tick; }
static inline DWORD GetEnvironmentVariableA(const char *n, char *b, DWORD s) { (void)n; (void)b; (void)s; return 0; }
static inline void InitializeCriticalSection(CRITICAL_SECTION *c) { (void)c; }
static inline void EnterCriticalSection(CRITICAL_SECTION *c) { (void)c; }
static inline void LeaveCriticalSection(CRITICAL_SECTION *c) { (void)c; }
static inline HMODULE GetModuleHandleA(const char *n) { (void)n; return 0; }
static inline BOOL DisableThreadLibraryCalls(HMODULE m) { (void)m; return 1; }
static inline void *VirtualAlloc(void *a, size_t s, DWORD t, DWORD p) { (void)a;(void)s;(void)t;(void)p; return 0; }
static inline BOOL VirtualFree(void *a, size_t s, DWORD t) { (void)a;(void)s;(void)t; return 1; }
static inline BOOL VirtualProtect(void *a, size_t s, DWORD n, DWORD *o) { (void)a;(void)s;(void)n;(void)o; return 1; }
static inline void *GetCurrentProcess(void) { return 0; }
static inline BOOL FlushInstructionCache(void *p, const void *a, size_t s) { (void)p;(void)a;(void)s; return 1; }

#pragma once
#include <windows.h>

// Tactical-only renderer guards. No patch is attempted unless all required code/recovery bytes match.
void render_guard_install();
void render_guard_report(DWORD now);

#ifdef NETV21_TEST_HOOKS
void render_test_layout(void *base);
LONG render_test_fault(EXCEPTION_POINTERS *p);
bool render_test_triangle(void *p);
void render_test_cache(void *p, unsigned node);
bool render_test_install(void *target, void *hook);
int render_test_original();
#endif

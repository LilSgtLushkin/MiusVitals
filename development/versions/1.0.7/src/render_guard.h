#pragma once
#include <windows.h>

// Tactical-only renderer guards. No patch is attempted unless all required code/recovery bytes match.
void render_guard_install();
void render_guard_report(DWORD now);

#ifdef NETV21_TEST_HOOKS
void render_test_layout(void *base);
LONG render_test_fault(EXCEPTION_POINTERS *p);
bool render_test_triangle(void *p);
bool render_test_triangle_reference(void *p);
void render_test_reset_page_queries();
unsigned render_test_page_queries();
void render_test_cache(void *p, unsigned node);
void render_test_cache_reference(void *p, unsigned node);
bool render_test_install_cache_bounds(void *site, void *next, void *allocate, volatile LONG *counter);
bool render_test_cache_strict_requested();
// 0 = exact-AV only; 1 = fast bounds; 2 = explicit strict preflight.
int render_test_select_cache(bool strict, bool (*fast)(HMODULE, unsigned char *),
                             bool (*slow)(HMODULE, unsigned char *));
bool render_test_install(void *target, void *hook);
int render_test_original();
#endif

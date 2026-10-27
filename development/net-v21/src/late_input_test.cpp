// Windows-only no-game test of the real IAT patcher, including a module loaded after the first scan.
#include "input_imports.h"
#include <stdio.h>

static int checks, failures;
static void test(bool condition, const char *name) {
    ++checks;
    if (!condition) { ++failures; printf("FAIL %s (Windows %lu)\n", name, GetLastError()); }
}
static SHORT WINAPI fake_key(int) { return (SHORT)0x1234; }
typedef SHORT (*Probe)();

int main() {
    input_imports::Tracker<1> tracker;
    test(!tracker.needs(0, nullptr), "no module on initial scan");
    HMODULE user = LoadLibraryA("user32.dll");
    void *real = user ? (void *)GetProcAddress(user, "GetAsyncKeyState") : nullptr;
    test(real != nullptr, "resolve real key function");
    test(input_imports::replace(nullptr, real, (void *)fake_key) < 0, "reject null module");
    for (int attempt = 0; attempt < 2; ++attempt) {
        HMODULE fixture = LoadLibraryA("late_input_fixture.dll");
        test(fixture != nullptr, "load late module");
        if (!fixture || !real) return 1;
        test(tracker.needs(0, fixture), "late module requires patch");
        int n = input_imports::replace(fixture, real, (void *)fake_key);
        test(n > 0, "replace real import address");
        Probe probe = (Probe)GetProcAddress(fixture, "late_input_probe");
        test(probe && probe() == (SHORT)0x1234, "late module calls patched input function");
        tracker.complete(0, fixture);
        test(!tracker.needs(0, fixture), "do not patch same loaded module repeatedly");
        FreeLibrary(fixture);
        test(!tracker.needs(0, nullptr), "forget unloaded module");
    }
    printf("late input module selftest: %d checks, %d failed\n", checks, failures);
    return failures ? 1 : 0;
}

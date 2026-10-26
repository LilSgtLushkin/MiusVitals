// Self-test for the run log: no game, no network, no Steam.
//
// The log is the only thing in this module that a player is ever asked to send, so it is tested the
// same way the Steam layer is: open it, write through every entry point, and read the file back to
// see that what arrived is what a bug report needs.
#include "netlog.h"

#include <stdio.h>
#include <string.h>

static int g_checks;
static int g_failures;

static void check(bool condition, const char *what)
{
    ++g_checks;
    if (!condition)
        ++g_failures;
    printf("%s %s\n", condition ? "OK  " : "FAIL", what);
}

static bool file_contains(const char *path, const char *needle)
{
    FILE *file = nullptr;
    if (fopen_s(&file, path, "rb") != 0 || !file)
        return false;
    static char text[65536];
    size_t got = fread(text, 1, sizeof text - 1, file);
    text[got] = 0;
    fclose(file);
    return strstr(text, needle) != nullptr;
}

int main()
{
    char directory[MAX_PATH];
    GetTempPathA(MAX_PATH, directory);
    strcat_s(directory, "netlog_selftest");
    CreateDirectoryA(directory, nullptr);

    netlog_open(GetModuleHandleA(nullptr), "opgame", directory);
    const char *path = netlog_path();
    check(path[0] != 0, "the log opens");
    check(file_contains(path, "net_v21: our own network module"),
          "and says which module wrote it");
    check(file_contains(path, "build: compiled"), "with the build stamp");
    check(file_contains(path, "fnv1a64"), "and the hash of the image it runs from");

    netlog_mirror("a line from mp_log");
    check(file_contains(path, "a line from mp_log"), "module lines are mirrored into it");

    netlog_packet("tx", (const unsigned char *)"\x47\x54\x4d\x50\x06\x00", 6, 0x0a000001, 1943, 2);
    netlog_packet("rx", (const unsigned char *)"\x47\x54\x4d\x50\x0a\x00", 6, 0x0a000002, 1943, 2);
    check(file_contains(path, "-> STATE"), "an outgoing datagram is written with its type");
    check(file_contains(path, "10.0.0.2:1943"), "and an incoming one with its peer");

    netlog_count("shots_sent", 3);
    netlog_count("shots_sent", 2);
    netlog_count("craters");
    netlog_state(2, 125500, 1, 0x52475353);
    netlog_test_summary();
    check(file_contains(path, "shots_sent=5"), "counters are summed");
    check(file_contains(path, "craters=1"), "and default to one");
    check(file_contains(path, "phase 2, sim 125.5 s, 1 peer(s)"), "the battle state is in the summary");
    check(file_contains(path, "HELLO rx0/tx0") == false, "types that never moved are left out of it");

    netlog_mark("ceasefire accepted at sim %.1f s", 125.5);
    check(file_contains(path, "battle  ceasefire accepted at sim 125.5 s"),
          "a battle milestone is written as a battle line");

    netlog_open(GetModuleHandleA(nullptr), "opgame", directory);
    long before = 0;
    {
        FILE *file = nullptr;
        if (fopen_s(&file, path, "rb") == 0 && file) {
            fseek(file, 0, SEEK_END);
            before = ftell(file);
            fclose(file);
        }
    }
    netlog_note("second open must not truncate the file");
    check(before > 0 && file_contains(path, "second open must not truncate"),
          "opening twice keeps writing into the same file");

    netlog_shutdown();
    netlog_open(GetModuleHandleA(nullptr), "opgame", directory);
    char kept[MAX_PATH];
    sprintf_s(kept, sizeof kept, "%s\\net_v21_opgame.prev.log", directory);
    check(GetFileAttributesA(kept) != INVALID_FILE_ATTRIBUTES,
          "the previous run is kept as .prev.log");
    check(file_contains(path, "net_v21: our own network module"), "and the new run starts clean");
    netlog_shutdown();

    printf("%d checks, %d failures\n", g_checks, g_failures);
    return g_failures ? 1 : 0;
}

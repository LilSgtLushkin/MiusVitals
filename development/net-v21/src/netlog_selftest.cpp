// Self-test for the run log: no game, no network, no Steam.
//
// The log is the only thing in this module that a player is ever asked to send, so it is tested the
// same way the Steam layer is: write through every entry point, close the log the way a finished
// process does, and read the file back to see that what arrived is what a bug report needs.
//
// The file is read only after netlog_shutdown(): while the module holds it open for writing, a second
// handle on it is not guaranteed, and a test that depends on that would test the CRT rather than the
// log.
#include "netlog.h"

#include <stdio.h>
#include <string.h>
#include <thread>

static int g_checks;
static int g_failures;
static char g_text[65536];

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
    g_text[0] = 0;
    if (fopen_s(&file, path, "rb") != 0 || !file) {
        printf("        (cannot open %s for reading)\n", path);
        return false;
    }
    size_t got = fread(g_text, 1, sizeof g_text - 1, file);
    g_text[got] = 0;
    fclose(file);
    return strstr(g_text, needle) != nullptr;
}

static void dump(const char *path, int lines)
{
    FILE *file = nullptr;
    if (fopen_s(&file, path, "rb") != 0 || !file)
        return;
    char line[512];
    printf("        ---- %s ----\n", path);
    for (int i = 0; i < lines && fgets(line, sizeof line, file); i++)
        printf("        %s", line);
    printf("        --------------\n");
    fclose(file);
}

static void expect(const char *path, const char *needle, const char *what)
{
    bool found = file_contains(path, needle);
    check(found, what);
    if (!found)
        dump(path, 12);
}

int main()
{
    char directory[MAX_PATH];
    DWORD got = GetTempPathA(MAX_PATH, directory);
    if (!got) {
        printf("FAIL the temporary folder is unknown\n1 check, 1 failures\n");
        return 1;
    }
    strcat_s(directory, "netlog_selftest");
    CreateDirectoryA(directory, nullptr);

    // ---- 1. write, then close the way a process that exits normally does
    netlog_open(GetModuleHandleA(nullptr), "opgame", directory);
    const char *path = netlog_path();
    check(path[0] != 0, "the log opens");

    netlog_mirror("a line from mp_log");
    netlog_packet("tx", (const unsigned char *)"\x47\x54\x4d\x50\x06\x00", 6, 0x0a000001, 1943, 2);
    netlog_packet("rx", (const unsigned char *)"\x47\x54\x4d\x50\x0a\x00", 6, 0x0a000002, 1943, 2);
    netlog_count("shots_sent", 3);
    netlog_count("shots_sent", 2);
    netlog_count("craters");
    std::thread workers[4];
    for (auto &worker : workers) worker = std::thread([] {
        for (int i = 0; i < 1000; ++i) { netlog_count("worker_events"); netlog_frame(); }
    });
    for (auto &worker : workers) worker.join();
    netlog_state(2, 125500, 1, 0x52475353);
    netlog_test_summary();
    netlog_mark("ceasefire accepted at sim %.1f s", 125.5);
    netlog_open(GetModuleHandleA(nullptr), "opgame", directory);   // must be a no-op
    netlog_note("second open must not truncate the file");
    netlog_shutdown();

    // ---- 2. read it back
    expect(path, "net_v21: our own network module", "and says which module wrote it");
    expect(path, "build: compiled", "with the build stamp");
    expect(path, "fnv1a64", "and the hash of the image it runs from");
    // Independently hash the running EXE and compare to the log, not just to the label.
    char image_path[MAX_PATH] = {}, expected[64] = {};
    GetModuleFileNameA(nullptr, image_path, MAX_PATH);
    FILE *image = nullptr;
    unsigned long long image_hash = 14695981039346656037ULL;
    if (fopen_s(&image, image_path, "rb") == 0 && image) {
        int c;
        while ((c = fgetc(image)) != EOF) image_hash = (image_hash ^ (unsigned char)c) * 1099511628211ULL;
        fclose(image);
        sprintf_s(expected, "fnv1a64 %016llx", image_hash);
        check(file_contains(path, expected), "image identity is the standard FNV-1a-64 hash");
    } else check(false, "the running selftest image can be read for identity verification");
    expect(path, "a line from mp_log", "module lines are mirrored into it");
    expect(path, "-> STATE", "an outgoing datagram is written with its type");
    expect(path, "10.0.0.2:1943", "and an incoming one with its peer");
    expect(path, "shots_sent=5", "counters are summed");
    expect(path, "craters=1", "and default to one");
    expect(path, "worker_events=4000", "simultaneous worker counters are not lost");
    expect(path, "phase 2, sim 125.5 s, 1 peer(s)", "the battle state is in the summary");
    check(!file_contains(path, "HELLO rx0/tx0"), "types that never moved are left out of it");
    expect(path, "battle  ceasefire accepted at sim 125.5 s",
           "a battle milestone is written as a battle line");
    expect(path, "second open must not truncate", "opening twice keeps writing into the same file");

    // ---- 3. a second run keeps the first one
    netlog_open(GetModuleHandleA(nullptr), "opgame", directory);
    netlog_shutdown();
    char kept[MAX_PATH];
    sprintf_s(kept, sizeof kept, "%s\\net_v21_opgame.prev.log", directory);
    check(GetFileAttributesA(kept) != INVALID_FILE_ATTRIBUTES,
          "the previous run is kept as .prev.log");
    check(file_contains(kept, "a line from mp_log"), "and it is the run that just ended");
    check(file_contains(path, "net_v21: our own network module"), "the new run starts clean");
    check(!file_contains(path, "a line from mp_log"), "with none of the old lines in it");

    // If a run ended without a current file, starting another must not erase the older .prev file.
    DeleteFileA(path);
    netlog_open(GetModuleHandleA(nullptr), "opgame", directory);
    netlog_shutdown();
    check(file_contains(kept, "a line from mp_log"), "a missing current file does not delete the previous run");

    netlog_open(GetModuleHandleA(nullptr), "opgame", directory);
    netlog_note("before rotation sentinel");
    netlog_test_rotate();
    netlog_note("after rotation sentinel");
    netlog_shutdown();
    char part[MAX_PATH];
    sprintf_s(part, sizeof part, "%s\\net_v21_opgame.part.log", directory);
    check(file_contains(part, "before rotation sentinel"), "rotation retains the completed segment");
    check(file_contains(path, "after rotation sentinel"), "logging continues into a new segment");
    check(file_contains(path, "fnv1a64"), "the continuation includes build identity");

    // Force the destination rename to fail and verify that the original is appended, not truncated.
    netlog_open(GetModuleHandleA(nullptr), "opgame", directory);
    DeleteFileA(part);
    CreateDirectoryA(part, nullptr);
    netlog_note("before blocked rotation");
    netlog_test_rotate();
    netlog_note("after blocked rotation");
    netlog_shutdown();
    check(file_contains(path, "before blocked rotation") && file_contains(path, "after blocked rotation"),
          "a failed rotation does not truncate the log");
    check(file_contains(path, "log rotation failed"), "a failed rotation is visible in the log");
    RemoveDirectoryA(part);

    printf("%d checks, %d failures\n", g_checks, g_failures);
    return g_failures ? 1 : 0;
}

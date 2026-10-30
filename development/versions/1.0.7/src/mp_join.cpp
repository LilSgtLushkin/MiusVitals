// mp_join.exe <host address>[:port]
// Optional standalone launcher; the in-game menu is the primary Steam path. This process initializes
// Steam explicitly because it does not inherit a loaded DLL or Steam interfaces from the game.
#include "joiner.h"

struct StandaloneSteam {
    HMODULE library = nullptr;
    void (*shutdown_api)() = nullptr;
    bool initialised = false;
    bool start(const char *root) {
        char fake[32];
        if (GetEnvironmentVariableA("MP_STEAM_FAKE", fake, sizeof fake)) return true; // isolated CI stand-in
        char path[MAX_PATH];
        if (sprintf_s(path, "%s\\steam_api64.dll", root) > 0)
            library = LoadLibraryExA(path, nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
        if (!library && sprintf_s(path, "%s\\bin\\rel\\x64\\steam_api64.dll", root) > 0)
            library = LoadLibraryExA(path, nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
        if (!library) return false;
        typedef bool (*Init)();
        Init init = (Init)GetProcAddress(library, "SteamAPI_Init");
        shutdown_api = (void (*)())GetProcAddress(library, "SteamAPI_Shutdown");
        if (!init || !shutdown_api || !init()) return false;
        initialised = true;
        return true;
    }
    ~StandaloneSteam() {
        steam_shutdown();
        if (initialised && shutdown_api) shutdown_api();
        if (library) FreeLibrary(library);
    }
};

int main(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: mp_join <host address>[:port] [germans | soviets]\n");
        return 2;
    }
    char root[MAX_PATH];
    if (!GetModuleFileNameA(0, root, MAX_PATH)) return 1;
    if (char *s = strrchr(root, '\\')) *s = 0;
    if (!SetCurrentDirectoryA(root)) { printf("Cannot open the game folder.\n"); return 1; }
    printf("net_v21 launcher %s\n", k_wire_tag);
    StandaloneSteam steam;
    if (!_strnicmp(argv[1], "steam:", 6) && !steam.start(root)) {
        printf("Steam could not be initialized in the standalone launcher. Start Steam and use the in-game lobby, or use IP.\n");
        return 1;
    }
    Joiner j;
    j.lobby_ready = true;
    if (argc > 2) j.prefer_side = !_stricmp(argv[2], "soviets") ? k_side_soviets : k_side_germans;
    if (!j.start(argv[1])) { printf("%s\n", j.error); return 1; }
    printf("Joining %s...\n", argv[1]);
    Joiner::State shown = Joiner::IDLE;
    unsigned noted = 0;
    for (;;) {
        steam_frame();
        j.poll();
        if (j.state != shown) {
            shown = j.state;
            if (shown == Joiner::WAITING_FOR_BATTLE) printf("Connected. Waiting for the host...\n");
            if (shown == Joiner::RECEIVING) printf("Receiving a snapshot (%zu KB)...\n", j.image.size() / 1024);
        }
        if (j.state == Joiner::CONNECTING && j.waited_seconds() >= noted + 10) {
            noted = j.waited_seconds();
            printf("  no answer yet (%u s) - check the address and transport\n", noted);
        }
        if (j.state == Joiner::FAILED) { printf("%s\n", j.error); return 1; }
        if (j.state == Joiner::DONE) break;
        Sleep(10);
    }
    if (!(j.want_map ? j.launch_map(root) : j.launch(root))) { printf("%s\n", j.error); return 1; }
    printf("Snapshot received. Starting the game...\n");
    return 0;
}

// A harmless CreateProcess target used by standalone-launcher tests, not the game.
#include <windows.h>
#include <stdio.h>
#include <string.h>
static bool equals(const char *name, const char *expected) {
    char value[128] = {};
    DWORD n = GetEnvironmentVariableA(name, value, sizeof value);
    return n && n < sizeof value && !strcmp(value, expected);
}
int main() {
    char autojoin[4];
    bool tactical = strstr(GetCommandLineA(), "tacgame.progpack") != nullptr;
    bool ok = equals("MP_ROLE", "join") && equals("MP_HOST_MODE", "operation") &&
              GetEnvironmentVariableA("MP_PEER", nullptr, 0) > 0;
    if (tactical) {
        // A battle offered by the host must not inherit the spectator marker: otherwise
        // session_start correctly isolates an unhosted local launch from the network.
        ok = ok && !GetEnvironmentVariableA("MP_MAP", autojoin, sizeof autojoin) &&
             GetEnvironmentVariableA("MP_AUTOJOIN", nullptr, 0) > 0 && equals("MP_VERSUS", "1");
    } else {
        ok = ok && equals("MP_MAP", "1") && equals("MP_MAP_EPOCH", "102") &&
             equals("MP_MAP_REV", "2") && !GetEnvironmentVariableA("MP_AUTOJOIN", autojoin, sizeof autojoin) &&
             strstr(GetCommandLineA(), "opgame.progpack");
    }
    FILE *f = nullptr;
    if (fopen_s(&f, "starter_fixture_result.tmp", "wb") || !f) return 2;
    if (ok) fprintf(f, "ok:%lu", GetCurrentProcessId());
    else fputs("invalid environment or command", f);
    if (fclose(f) || !MoveFileExA("starter_fixture_result.tmp", "starter_fixture_result.txt",
                                 MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) return 2;
    return ok ? 0 : 1;
}

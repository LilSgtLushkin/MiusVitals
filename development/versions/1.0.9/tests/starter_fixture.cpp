// A harmless CreateProcess target used by standalone-launcher tests, not the game.
//
// It reports which part of the inherited environment was wrong, not merely that something was:
// a launcher test that only prints "invalid" costs a full CI run per guess.
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include "battle_identity.h"
#include "protocol.h"
static bool equals(const char *name, const char *expected) {
    char value[128] = {};
    DWORD n = GetEnvironmentVariableA(name, value, sizeof value);
    return n && n < sizeof value && !strcmp(value, expected);
}
// Compare against the identity the stand's host actually offered, supplied by the test, and name
// both sides when they differ: a bare "wrong epoch" costs a full CI run to pin down.
static bool equals_expected(const char *name, const char *expected_name, std::string &bad) {
    char expected[128] = {}, got[128] = {};
    DWORD en = GetEnvironmentVariableA(expected_name, expected, sizeof expected);
    DWORD gn = GetEnvironmentVariableA(name, got, sizeof got);
    bool ok = en && en < sizeof expected && gn && gn < sizeof got && !strcmp(got, expected);
    if (!ok) {
        bad += name;
        bad += "=";
        bad += gn && gn < sizeof got ? got : "<unset>";
        bad += " want ";
        bad += en && en < sizeof expected ? expected : "<unset>";
        bad += ", ";
    }
    return ok;
}
int main() {
    char autojoin[4];
    bool tactical = strstr(GetCommandLineA(), "tacgame.progpack") != nullptr;
    bool operation = strstr(GetCommandLineA(), "opgame.progpack") != nullptr;
    char peer[128] = {}, requested[128] = {};
    DWORD pn = GetEnvironmentVariableA("MP_PEER", peer, sizeof peer);
    DWORD an = GetEnvironmentVariableA("MP_AUTOJOIN", requested, sizeof requested);
    bool same_host = pn && pn < sizeof peer && an && an < sizeof requested && !strcmp(peer, requested);
    std::string bad;
    auto want = [&bad](bool ok, const char *name) {
        if (!ok) { if (!bad.empty()) bad += ", "; bad += name; }
    };
    want(equals("MP_HOST_MODE", "operation"), "MP_HOST_MODE");
    want(pn && pn < sizeof peer, "MP_PEER");
    char identity[64] = {};
    if (tactical) {
        // A battle offered by the host must not inherit the spectator marker: otherwise
        // session_start correctly isolates an unhosted local launch from the network.
        want(equals("MP_ROLE", "join"), "MP_ROLE");
        want(!GetEnvironmentVariableA("MP_MAP", autojoin, sizeof autojoin), "MP_MAP set");
        want(same_host, "MP_AUTOJOIN");
        want(equals("MP_VERSUS", "1"), "MP_VERSUS");
        // Validate the actual child's inherited ticket against the image it was launched with.
        DWORD n = GetEnvironmentVariableA(battle_identity::k_environment, identity, sizeof identity);
        FILE *image = nullptr; std::vector<unsigned char> prepared;
        if (!fopen_s(&image, k_image_path, "rb") && image) {
            fseek(image, 0, SEEK_END); long len = ftell(image); rewind(image);
            if (len > 0) { prepared.resize((size_t)len); if (fread(prepared.data(), 1, prepared.size(), image) != prepared.size()) prepared.clear(); }
            fclose(image);
        }
        unsigned source = 0;
        want(n && n < sizeof identity, "battle ticket");
        want(!prepared.empty(), "prepared image");
        want(!prepared.empty() && battle_identity::resolve(identity, prepared.data(), prepared.size(), &source),
             "battle ticket does not match the image");
    } else if (operation) {
        want(equals("MP_ROLE", "join"), "MP_ROLE");
        want(equals("MP_MAP", "1"), "MP_MAP");
        equals_expected("MP_MAP_EPOCH", "MIUS_EXPECT_EPOCH", bad);
        equals_expected("MP_MAP_REV", "MIUS_EXPECT_REV", bad);
        want(!an, "MP_AUTOJOIN set");
        want(!GetEnvironmentVariableA(battle_identity::k_environment, autojoin, sizeof autojoin), "battle ticket set");
    } else {
        // The actual map->lobby CreateProcess must clear the map/role and battle ticket,
        // and retain the host address.
        want(!GetEnvironmentVariableA("MP_ROLE", autojoin, sizeof autojoin), "MP_ROLE set");
        want(!GetEnvironmentVariableA("MP_MAP", autojoin, sizeof autojoin), "MP_MAP set");
        want(!GetEnvironmentVariableA(battle_identity::k_environment, autojoin, sizeof autojoin), "battle ticket set");
        want(same_host, "MP_AUTOJOIN");
    }
    FILE *f = nullptr;
    if (fopen_s(&f, "starter_fixture_result.tmp", "wb") || !f) return 2;
    while (bad.size() >= 2 && bad.compare(bad.size() - 2, 2, ", ") == 0) bad.erase(bad.size() - 2);
    if (bad.empty()) fprintf(f, "ok:%lu:%s", GetCurrentProcessId(), identity);
    else fprintf(f, "fail:%s", bad.c_str());
    if (fclose(f) || !MoveFileExA("starter_fixture_result.tmp", "starter_fixture_result.txt",
                                 MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) return 2;
    return bad.empty() ? 0 : 1;
}

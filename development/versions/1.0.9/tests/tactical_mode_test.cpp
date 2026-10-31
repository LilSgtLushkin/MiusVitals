#include "tactical_mode.h"
#include <stdio.h>

static int checks, failed;
static void check(bool ok, const char *name) {
    ++checks;
    if (!ok) { ++failed; printf("FAIL: %s\n", name); }
}
int main()
{
    using namespace tactical_mode;
    check(operation_battle("starter_x64.exe root\\programs\\tacgame.progpack, data\\common\\shared\\config\\tacgame_image.cfgpack, users\\profiles, operation_profile, 1942_07_Voronezh:04_06_07_shilovo_na_ger, 2, 13"), "direct tactical operation from recorded command line");
    check(operation_battle("starter.exe ROOT\\PROGRAMS\\TACGAME.PROGPACK, profile, 1942_07_Voronezh:04_06_07"), "case-insensitive path");
    check(operation_battle("\"tacgame.progpack\", profile, \"1942_07_Voronezh:04\""), "quoted tokens");
    check(!operation_battle("starter.exe root\\programs\\opgame.progpack, 1942_07_Voronezh:04"), "not the operational program");
    check(!operation_battle("starter.exe root\\programs\\notacgame.progpack, 1942_07_Voronezh:04"), "not a filename suffix");
    check(!operation_battle("starter.exe root\\programs\\tacgame.progpack.extra, 1942_07_Voronezh:04"), "not a filename prefix");
    check(!operation_battle("starter.exe root\\programs\\tacgame.progpack, data\\mission.cfgpack, custom_battle, 2"), "not a custom mission");
    check(!operation_battle("starter.exe root\\programs\\tacgame.progpack, 1942_07_"), "truncated token");
    check(!operation_battle("starter.exe root\\programs\\tacgame.progpack, 1942_07_:04"), "no campaign name");
    check(!operation_battle("starter.exe root\\programs\\tacgame.progpack, 1942_07_Campaign:"), "no battle name");
    check(!operation_battle(nullptr), "missing command line");
    check(!default_full_state(HOST_MODE_OPERATION, false), "operation defaults to position only");
    check(!default_full_state(HOST_MODE_SKIRMISH, true), "direct operation defaults to position only");
    check(default_full_state(HOST_MODE_SKIRMISH, false), "skirmish keeps full state");
    check(default_full_state(HOST_MODE_MISSION, false), "mission keeps full state");
    printf("tactical mode selftest: %d checks, %d failed\n", checks, failed);
    return failed ? 1 : 0;
}

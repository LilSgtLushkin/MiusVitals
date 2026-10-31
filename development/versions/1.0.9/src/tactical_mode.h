// Recognise the tactical operation launched directly by starter_x64 when its parent did not set
// MP_HOST_MODE. Require the tactical progpack and the game's comma-delimited 19YY_MM_campaign:battle
// token; an unrelated custom mission must keep its normal full-state policy.
#pragma once
#include "protocol.h"

namespace tactical_mode {
inline char lower_ascii(char c) { return c >= 'A' && c <= 'Z' ? (char)(c + ('a' - 'A')) : c; }
inline bool tactical_program(const char *line)
{
    if (!line) return false;
    static const char name[] = "tacgame.progpack";
    for (const char *p = line; *p; ++p) {
        if (p != line && p[-1] != '\\' && p[-1] != '/' && p[-1] != ' ' && p[-1] != '"') continue;
        const char *a = p, *b = name;
        while (*a && *b && lower_ascii(*a) == *b) { ++a; ++b; }
        if (!*b && (!*a || *a == ',' || *a == ' ' || *a == '\t' || *a == '"')) return true;
    }
    return false;
}
inline bool operation_battle(const char *line)
{
    if (!tactical_program(line)) return false;
    for (const char *p = line; *p;) {
        while (*p == ' ' || *p == '\t' || *p == '"') ++p;
        const char *end = p;
        while (*end && *end != ',') ++end;
        // Check the field length before accessing the date prefix; malformed short fields are safe.
        if (end - p >= 10 && p[0] == '1' && p[1] == '9' &&
            p[2] >= '0' && p[2] <= '9' && p[3] >= '0' && p[3] <= '9' && p[4] == '_' &&
            p[5] >= '0' && p[5] <= '9' && p[6] >= '0' && p[6] <= '9' && p[7] == '_') {
            for (const char *q = p + 9; q < end; ++q)
                if (*q == ':' && q + 1 < end && q[1] != '"' && q[1] != ' ') return true;
        }
        p = *end ? end + 1 : end;
    }
    return false;
}
// An explicit MP_SYNC_STATE overrides this default exactly as before.
inline bool default_full_state(int host_mode, bool direct_operation_battle)
{
    return host_mode != HOST_MODE_OPERATION && !direct_operation_battle;
}
}

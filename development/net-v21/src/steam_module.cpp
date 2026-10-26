// Glue between the Steam layer and the rest of the module.
//
// steam.cpp deliberately knows nothing about this codebase: it only needs `steam_log`.  This file
// supplies that by forwarding to `mp_log`, so Steam lines land in the same log, and it is the single
// place the module touches to switch the layer on.
#include "steam.h"
#include "common.h"

#include <stdarg.h>
#include <stdio.h>

void steam_log(const char *fmt, ...)
{
    char text[1024];
    va_list args;
    va_start(args, fmt);
    vsprintf_s(text, sizeof(text), fmt, args);
    va_end(args);
    mp_log("%s", text);
}

// Called once from session_start(): reads the command line for a join request, then brings the
// layer up.  Cheap enough to call more than once - steam_init() memoises its answer.
void steam_install()
{
    steam_note_command_line(GetCommandLineA());
    steam_init();
}

// Called once per frame from session_frame(): delivers Steam callbacks and P2P packets.
void steam_tick()
{
    if (!steam_ready())
        return;
    steam_frame();
}

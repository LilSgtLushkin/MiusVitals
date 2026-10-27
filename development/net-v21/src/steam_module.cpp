// Glue between the Steam layer and the rest of the module.
//
// steam.cpp deliberately knows nothing about this codebase: it only needs `steam_log`.  This file
// supplies that by forwarding to `mp_log`, so Steam lines land in the same log, and it is the single
// place the module touches to switch the layer on.
#include "steam.h"
#include "common.h"
#include "netlog.h"

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

// The five-second Steam totals, for the run log's summary line.
void steam_traffic_note(unsigned long long sent, unsigned long long received)
{
    netlog_count("steam_sent", (long long)sent);
    netlog_count("steam_received", (long long)received);
}

// Called from session_start() and ui_frame(): read the join request once, then bring the layer up.
// If the game's Steam DLL is loaded late, steam_init() retries at a bounded rate.
void steam_install()
{
    // ui_frame calls this every frame. Re-reading the command line there re-queues the same
    // Join Game request forever and floods the log. The API may still need a later retry if the
    // game's Steam library has not been loaded/initialised yet, so only the parsing is once-only.
    static bool command_line_noted;
    if (!command_line_noted) {
        command_line_noted = true;
        steam_note_command_line(GetCommandLineA());
    }
    steam_init();
}

// Called once per frame from session_frame(): delivers Steam callbacks and P2P packets.
void steam_tick()
{
    if (!steam_ready() && !steam_init()) return;
    steam_frame();
}

// Nothing in the module calls the layer yet: the session code that will (it is part of M3) is not
// written, and /O1 puts every function in its own COMDAT, so /OPT:REF drops unreachable code.  The
// whole layer would silently vanish from basic_network.codelib - and then "M2 is in the build"
// would be a claim nobody could check, and the CI comparison would be comparing the baseline to
// v20 while calling it M2.
//
// This exported table is the anchor: the loader references it, so everything it points at stays in
// the image and shows up in the .map and in the comparison.  It doubles as a manifest - `dumpbin
// /exports basic_network.codelib` lists it, which is how a build can be asked whether the Steam
// layer is really there.
extern "C" __declspec(dllexport) const void *const gtmf_steam_layer[] = {
    reinterpret_cast<const void *>(&steam_init),
    reinterpret_cast<const void *>(&steam_shutdown),
    reinterpret_cast<const void *>(&steam_ready),
    reinterpret_cast<const void *>(&steam_is_fake),
    reinterpret_cast<const void *>(&steam_user_id),
    reinterpret_cast<const void *>(&steam_user_name),
    reinterpret_cast<const void *>(&steam_app_id),
    reinterpret_cast<const void *>(&steam_friend_count),
    reinterpret_cast<const void *>(&steam_frame),
    reinterpret_cast<const void *>(&steam_send),
    reinterpret_cast<const void *>(&steam_recv),
    reinterpret_cast<const void *>(&steam_presence),
    reinterpret_cast<const void *>(&steam_presence_value),
    reinterpret_cast<const void *>(&steam_presence_check),
    reinterpret_cast<const void *>(&steam_broadcast),
    reinterpret_cast<const void *>(&steam_connect_text),
    reinterpret_cast<const void *>(&steam_note_command_line),
    reinterpret_cast<const void *>(&steam_join_take),
    reinterpret_cast<const void *>(&steam_join_source),
    reinterpret_cast<const void *>(&steam_install),
    reinterpret_cast<const void *>(&steam_tick),
};

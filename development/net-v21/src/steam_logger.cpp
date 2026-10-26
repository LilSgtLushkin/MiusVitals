// steam_log for the two programs that have no mp_log: the self-test and the join launcher.
//
// The module forwards Steam's lines into its own log (steam_module.cpp); a console program has no log
// to forward to, so it prints them.  Keeping this in its own file means steam.cpp never needs to know
// which of the two worlds it is in.
#include "steam.h"

#include <stdarg.h>
#include <stdio.h>

void steam_log(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf("\n");
    fflush(stdout);
}

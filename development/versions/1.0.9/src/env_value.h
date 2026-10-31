// Bounded access to Windows environment variables. GetEnvironmentVariableA returns the required
// size on an oversized value without initializing the destination; never parse that buffer.
#pragma once
#include <windows.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>

inline bool env_text(const char *name, char *out, size_t capacity)
{
    if (!name || !out || !capacity || capacity > 0xffffffffu) return false;
    out[0] = 0;
    DWORD n = GetEnvironmentVariableA(name, out, (DWORD)capacity);
    if (!n || n >= capacity) { out[0] = 0; return false; }
    return true;
}

inline bool env_integer(const char *name, int *out)
{
    char text[32];
    if (!out || !env_text(name, text, sizeof text)) return false;
    char *end = nullptr;
    errno = 0;
    long value = strtol(text, &end, 10);
    if (end == text || *end || errno == ERANGE || value < INT_MIN || value > INT_MAX) return false;
    *out = (int)value;
    return true;
}

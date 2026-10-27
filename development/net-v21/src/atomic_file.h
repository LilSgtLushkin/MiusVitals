// Same-directory Windows replacement for M4 battle descriptions and recovery markers.
// On failure the destination is never opened for writing or truncated. A unique temporary file is
// fully written and flushed before a single replacement rename; failed temporaries are removed.
#pragma once
#include <windows.h>
#include <stdint.h>
#include <stddef.h>
#include <string>

namespace atomic_file {
inline bool replace(const char *path, const void *data, size_t size)
{
    if (!path || !*path || (!data && size)) { SetLastError(ERROR_INVALID_PARAMETER); return false; }
    static volatile LONG serial = 0;
    HANDLE file = INVALID_HANDLE_VALUE;
    std::string temporary;
    for (int attempt = 0; attempt < 32; ++attempt) {
        const unsigned long ticket = (unsigned long)InterlockedIncrement(&serial);
        temporary = std::string(path) + ".mp_tmp_" + std::to_string((unsigned long)GetCurrentProcessId()) +
                    "_" + std::to_string(ticket);
        file = CreateFileA(temporary.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (file != INVALID_HANDLE_VALUE) break;
        DWORD e = GetLastError();
        if (e != ERROR_FILE_EXISTS && e != ERROR_ALREADY_EXISTS) return false;
    }
    if (file == INVALID_HANDLE_VALUE) { SetLastError(ERROR_FILE_EXISTS); return false; }
    DWORD error = ERROR_SUCCESS;
    const unsigned char *bytes = (const unsigned char *)data;
    size_t at = 0;
    while (at < size) {
        size_t left = size - at;
        DWORD wanted = left > 0x7fffffffu ? 0x7fffffffu : (DWORD)left;
        DWORD written = 0;
        if (!WriteFile(file, bytes + at, wanted, &written, nullptr)) {
            error = GetLastError(); if (!error) error = ERROR_WRITE_FAULT; break;
        }
        if (!written || written > wanted) { error = ERROR_WRITE_FAULT; break; }
        at += written;
    }
    if (!error && !FlushFileBuffers(file)) { error = GetLastError(); if (!error) error = ERROR_WRITE_FAULT; }
    if (!CloseHandle(file) && !error) { error = GetLastError(); if (!error) error = ERROR_INVALID_HANDLE; }
    if (!error && !MoveFileExA(temporary.c_str(), path, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
        error = GetLastError(); if (!error) error = ERROR_WRITE_FAULT;
    }
    if (error) {
        DeleteFileA(temporary.c_str());
        SetLastError(error);
        return false;
    }
    return true;
}
}

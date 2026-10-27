// Windows adapter for map_file's injectable transaction.
#pragma once
#include "map_file.h"
#include "atomic_file.h"
#include <windows.h>
#include <string>
#include <vector>

namespace map_file_windows {
struct Context { const char *directory; std::string command; DWORD launch_error = 0; };
inline bool exists(void *, const char *name, bool *out) {
    DWORD attr = GetFileAttributesA(name);
    if (attr != INVALID_FILE_ATTRIBUTES) { *out = true; return !(attr & FILE_ATTRIBUTE_DIRECTORY); }
    DWORD e = GetLastError();
    if (e == ERROR_FILE_NOT_FOUND || e == ERROR_PATH_NOT_FOUND) { *out = false; return true; }
    return false;
}
inline bool read(void *, const char *name, std::vector<unsigned char> *out) {
    HANDLE h = CreateFileA(name, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return false;
    LARGE_INTEGER size = {};
    bool ok = GetFileSizeEx(h, &size) && size.QuadPart > 0 && size.QuadPart <= (64ll << 20);
    std::vector<unsigned char> data;
    if (ok) {
        data.resize((size_t)size.QuadPart);
        size_t at = 0;
        while (at < data.size()) {
            DWORD n = 0, wanted = (DWORD)(data.size() - at);
            if (!ReadFile(h, data.data() + at, wanted, &n, nullptr) || !n || n > wanted) { ok = false; break; }
            at += n;
        }
    }
    if (!CloseHandle(h)) ok = false;
    if (ok) out->swap(data);
    return ok;
}
inline bool create(void *, const char *name, const void *data, size_t size) { return atomic_file::create_new(name, data, size); }
inline bool write(void *, const char *name, const void *data, size_t size) { return atomic_file::replace(name, data, size); }
inline bool remove(void *, const char *name) { return DeleteFileA(name) != 0; }
inline bool launch(void *p) {
    Context &ctx = *(Context *)p;
    std::vector<char> cmd(ctx.command.begin(), ctx.command.end()); cmd.push_back(0);
    STARTUPINFOA si = { sizeof si }; PROCESS_INFORMATION pi = {};
    if (!CreateProcessA(nullptr, cmd.data(), nullptr, nullptr, FALSE, 0, nullptr, ctx.directory, &si, &pi)) {
        ctx.launch_error = GetLastError(); return false;
    }
    CloseHandle(pi.hProcess); CloseHandle(pi.hThread); return true;
}
inline std::string backup_name(const char *path) {
    static volatile LONG serial = 0;
    return std::string(path) + ".before_mp_" + std::to_string((unsigned long)GetCurrentProcessId()) +
           "_" + std::to_string((unsigned long)InterlockedIncrement(&serial));
}
inline map_file::Result replace_and_launch(const char *directory, const std::string &command, const char *path,
                                           const std::vector<unsigned char> &data, std::string *backup = nullptr,
                                           DWORD *launch_error = nullptr) {
    Context ctx = { directory, command, 0 };
    std::string keep = backup_name(path);
    map_file::Io io = { &ctx, exists, read, create, write, remove, launch };
    map_file::Result result = map_file::replace_and_launch(io, path, keep.c_str(), data);
    if (backup) *backup = keep;
    if (launch_error) *launch_error = ctx.launch_error;
    return result;
}

// Restore the parent process environment on failed launch; the new process inherits it on success.
struct Environment {
    struct Entry { std::string name, value; bool present; };
    std::vector<Entry> entries;
    bool committed = false;
    void set(const char *name, const char *value) {
        DWORD n = GetEnvironmentVariableA(name, nullptr, 0);
        Entry e; e.name = name; e.present = n != 0;
        if (n) { std::vector<char> b(n); DWORD got = GetEnvironmentVariableA(name, b.data(), n); if (got && got < n) e.value.assign(b.data(), got); else e.present = false; }
        entries.push_back(e);
        SetEnvironmentVariableA(name, value);
    }
    void commit() { committed = true; }
    ~Environment() {
        if (!committed) for (auto i = entries.rbegin(); i != entries.rend(); ++i)
            SetEnvironmentVariableA(i->name.c_str(), i->present ? i->value.c_str() : nullptr);
    }
};
}

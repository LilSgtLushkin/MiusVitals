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

// Temporarily set the parent environment only while CreateProcess takes its copy. Every failed
// setter stops launch, and both successful and failed launches restore the parent's prior values.
// As with Win32 process environment generally, another thread must not change these keys concurrently.
struct Environment {
    struct Entry { std::string name, value; bool present; };
    std::vector<Entry> entries;
    bool set(const char *name, const char *value) {
        if (!name || !*name) { SetLastError(ERROR_INVALID_PARAMETER); return false; }
        SetLastError(ERROR_SUCCESS);
        DWORD n = GetEnvironmentVariableA(name, nullptr, 0), error = GetLastError();
        Entry e; e.name = name; e.present = n != 0 || error == ERROR_SUCCESS;
        if (!n && error != ERROR_SUCCESS && error != ERROR_ENVVAR_NOT_FOUND) return false;
        if (n) {
            std::vector<char> b(n);
            SetLastError(ERROR_SUCCESS);
            DWORD got = GetEnvironmentVariableA(name, b.data(), n);
            if (got >= n || (!got && GetLastError() != ERROR_SUCCESS)) return false; // changed or system failure
            e.value.assign(b.data(), got);
        }
        if (!value && !e.present) return true; // deleting an absent variable is already satisfied
        if (!SetEnvironmentVariableA(name, value)) return false;
        entries.push_back(e);
        return true;
    }
    bool restore() {
        bool ok = true;
        for (auto i = entries.rbegin(); i != entries.rend(); ++i)
            if (!SetEnvironmentVariableA(i->name.c_str(), i->present ? i->value.c_str() : nullptr)) ok = false;
        entries.clear();
        return ok;
    }
    ~Environment() { restore(); }
};
}

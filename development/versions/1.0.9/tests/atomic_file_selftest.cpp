// Real Windows filesystem checks for same-directory replacement; no game or Steam required.
#include "atomic_file.h"
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>

static int checks, failed;
static void check(bool b, const char *what) { ++checks; if (!b) { ++failed; printf("FAIL: %s (winerr=%lu)\n", what, GetLastError()); } }
static std::vector<unsigned char> read_all(const char *path)
{
    std::vector<unsigned char> data;
    FILE *f = nullptr;
    if (fopen_s(&f, path, "rb") || !f) return data;
    unsigned char buf[256];
    for (;;) {
        size_t n = fread(buf, 1, sizeof buf, f);
        data.insert(data.end(), buf, buf + n);
        if (n < sizeof buf) break;
    }
    fclose(f);
    return data;
}
static unsigned temporary_count(const std::string &path)
{
    WIN32_FIND_DATAA info = {};
    HANDLE h = FindFirstFileA((path + ".mp_tmp_*").c_str(), &info);
    if (h == INVALID_HANDLE_VALUE) return 0;
    unsigned count = 0;
    do { ++count; } while (FindNextFileA(h, &info));
    FindClose(h);
    return count;
}
int main()
{
    char dir[MAX_PATH] = {}, name[MAX_PATH] = {};
    DWORD n = GetTempPathA(MAX_PATH, dir);
    if (!n || n >= MAX_PATH || !GetTempFileNameA(dir, "mft", 0, name)) return 2;
    const unsigned char old[] = {1,2,3,4}, newer[] = {9,8,7,6,5};
    const std::vector<unsigned char> old_bytes(old, old + sizeof old), new_bytes(newer, newer + sizeof newer);
    check(atomic_file::replace(name, old, sizeof old) && read_all(name) == old_bytes, "complete initial write");
    HANDLE lock = CreateFileA(name, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    check(lock != INVALID_HANDLE_VALUE, "test opens destination without delete sharing");
    if (lock != INVALID_HANDLE_VALUE) {
        check(!atomic_file::replace(name, newer, sizeof newer), "locked destination refuses replacement");
        check(read_all(name) == old_bytes, "failed replacement keeps every original byte");
        CloseHandle(lock);
    }
    check(temporary_count(name) == 0, "failed replacement removes temporary file");
    check(atomic_file::replace(name, newer, sizeof newer) && read_all(name) == new_bytes, "unlocked replacement is complete");
    check(temporary_count(name) == 0, "successful replacement leaves no temporary file");
    check(!atomic_file::create_new(name, old, sizeof old) && read_all(name) == new_bytes,
          "create-new never replaces an existing backup");
    std::string fresh = std::string(name) + ".new";
    check(atomic_file::create_new(fresh.c_str(), old, sizeof old) && read_all(fresh.c_str()) == old_bytes,
          "create-new writes a complete new backup");
    DeleteFileA(fresh.c_str());
    check(!atomic_file::replace("", newer, sizeof newer), "empty destination refused");
    DeleteFileA(name);
    printf("atomic file selftest: %d checks, %d failed\n", checks, failed);
    return failed ? 1 : 0;
}

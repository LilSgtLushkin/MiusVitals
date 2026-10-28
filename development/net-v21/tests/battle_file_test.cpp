// M4 description transaction with an injected filesystem; no game and no actual file writes.
#include "battle_file.h"
#include <map>
#include <string>
#include <vector>
#include <stdio.h>
#include <string.h>

using Bytes = std::vector<unsigned char>;
struct Fake {
    std::map<std::string, Bytes> files;
    std::vector<std::string> operations;
    int writes = 0;
    bool fail_copy = false, fail_marker = false, fail_image = false, fail_remove = false;
};
static bool copy(void *v, const char *from, const char *to)
{
    Fake &f = *(Fake *)v;
    f.operations.push_back("copy");
    auto it = f.files.find(from);
    if (f.fail_copy || it == f.files.end()) return false;
    f.files[to] = it->second;
    return true;
}
static bool write(void *v, const char *to, const void *bytes, size_t size)
{
    Fake &f = *(Fake *)v;
    f.operations.push_back("write");
    ++f.writes;
    if ((f.writes == 1 && f.fail_marker) || (f.writes == 2 && f.fail_image)) return false;
    const unsigned char *p = (const unsigned char *)bytes;
    f.files[to] = Bytes(p, p + size);
    return true;
}
static bool remove_file(void *v, const char *path)
{
    Fake &f = *(Fake *)v;
    f.operations.push_back("remove");
    if (f.fail_remove) return false;
    f.files.erase(path);
    return true;
}
static int checks, failed;
static void check(bool b, const char *what) { ++checks; if (!b) { ++failed; printf("FAIL: %s\n", what); } }
static battle_file::Io io(Fake &f) { return { &f, copy, write, remove_file }; }
int main()
{
    const char *path = "battle.cfgpack", *backup = "battle.cfgpack.before_mp", *marker = "battle.cfgpack.mp_combined";
    const Bytes old = {1,2,3}, newer = {4,5,6,7};
    const uint32_t hash = 0x78563412u;
    {
        Fake f; f.files[path] = old;
        check(battle_file::install(io(f), path, newer, hash) == battle_file::Result::ok, "success");
        check(f.files[path] == newer && f.files[backup] == old, "replacement and backup are complete");
        check(f.files[marker].size() == sizeof hash && !memcmp(f.files[marker].data(), &hash, sizeof hash), "marker contains the full hash");
        check(f.operations == std::vector<std::string>({"copy","write","write"}), "backup then marker then image");
    }
    {
        Fake f; f.files[path] = old; f.fail_copy = true;
        check(battle_file::install(io(f), path, newer, hash) == battle_file::Result::backup, "backup failure reported");
        check(f.files[path] == old && !f.files.count(marker) && f.operations.size() == 1, "backup failure leaves image and marker alone");
    }
    {
        Fake f; f.files[path] = old; f.fail_marker = true;
        check(battle_file::install(io(f), path, newer, hash) == battle_file::Result::marker, "marker failure reported");
        check(f.files[path] == old && f.files[backup] == old && !f.files.count(marker), "marker failure preserves image and backup");
    }
    {
        Fake f; f.files[path] = old; f.fail_image = true;
        check(battle_file::install(io(f), path, newer, hash) == battle_file::Result::image, "image failure reported");
        check(f.files[path] == old && f.files[backup] == old && !f.files.count(marker), "failed replacement removes marker and leaves image intact");
        check(f.operations == std::vector<std::string>({"copy","write","write","remove"}), "cleanup follows failed replacement");
    }
    {
        Fake f; f.files[path] = old; f.fail_image = f.fail_remove = true;
        check(battle_file::install(io(f), path, newer, hash) == battle_file::Result::image_and_marker_cleanup, "failed marker cleanup reported");
        check(f.files[path] == old && f.files[backup] == old && f.files.count(marker), "cleanup failure still preserves original");
    }
    {
        Fake f; f.files[path] = old;
        check(battle_file::install(io(f), "", newer, hash) == battle_file::Result::invalid, "empty path rejected");
        check(battle_file::install(io(f), path, Bytes(), hash) == battle_file::Result::invalid, "empty image rejected");
        check(f.operations.empty() && f.files[path] == old, "invalid requests cause no writes");
    }
    printf("battle file transaction selftest: %d checks, %d failed\n", checks, failed);
    return failed ? 1 : 0;
}

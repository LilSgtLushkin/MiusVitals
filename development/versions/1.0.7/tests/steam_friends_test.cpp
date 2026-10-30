// No Steam client or game: exercise the same friend selection as the in-game list.
#include "steam_friends.h"
#include <stdio.h>
#include <string.h>
#include <vector>

struct FakeFriend { uint64_t id, game; const char *name, *note; bool playing; };
struct Fake {
    std::vector<FakeFriend> rows;
    int requests = 0;
    int count() { return (int)rows.size(); }
    uint64_t id(int i) { return rows[(size_t)i].id; }
    FakeFriend *get(uint64_t id) {
        for (auto &f : rows) if (f.id == id) return &f;
        return nullptr;
    }
    bool game(uint64_t id, uint64_t *out) {
        FakeFriend *f = get(id); if (!f || !f->playing) return false;
        *out = f->game; return true;
    }
    const char *name(uint64_t id) { return get(id)->name; }
    const char *note(uint64_t id) { return get(id)->note; }
    void request(uint64_t) { ++requests; }
};
struct Many {
    int seen = 0, requests = 0;
    int count() { return 9000; }
    uint64_t id(int i) { ++seen; return (uint64_t)i + 1; }
    bool game(uint64_t, uint64_t *out) { *out = 312980; return true; }
    const char *name(uint64_t) { return "friend"; }
    const char *note(uint64_t) { return ""; }
    void request(uint64_t) { ++requests; }
};
static int checks, failures;
static void check(bool value, const char *what) {
    ++checks; if (!value) { ++failures; printf("FAIL: %s\n", what); }
}

int main()
{
    using namespace lobby_friends;
    const uint32_t app = 312980;
    Fake source;
    source.rows = {
        { 11, app, "host", "0 game", true },
        { 12, app, "playing", "", true },
        { 13, 999, "other game", "0 game", true },
        { 14, app, "offline", "0 game", false },
        { 15, (uint64_t(7) << 32) | app, "game mod", "42 game", true },
        { 10, app, "self", "0 game", true },
        { 0,  app, "invalid", "0 game", true },
    };
    Friend list[4] = {};
    size_t n = collect(source, app, 10, list, 4, true);
    check(n == 3 && source.requests == 3, "only other friends running this app are requested and listed");
    check(list[0].id == 11 && !strcmp(list[0].name, "host") && list[0].hosting, "advertised host");
    check(list[1].id == 12 && !list[1].hosting, "playing friend remains selectable before presence arrives");
    check(list[2].id == 15 && list[2].hosting, "CGameID upper bits do not change application ID");
    Friend one[1] = {};
    n = collect(source, app, 10, one, 1, false);
    check(n == 3 && one[0].id == 11 && source.requests == 3, "capacity limits writes but not the total");
    collect(source, app, 10, one, 1, true);
    check(source.requests == 4, "presence requests are bounded by displayed capacity");
    check(collect(source, 0, 10, list, 4, true) == 0, "unknown app id is not matched");
    check(collect(source, app, 10, nullptr, 1, true) == 0, "null output with nonzero capacity is rejected");
    Many many;
    check(collect(many, app, 0, list, 4, true) == MAX_SCAN && many.seen == MAX_SCAN && many.requests == 4,
          "scan and asynchronous requests remain bounded for large friend lists");
    check(hosting_note("0 game") && hosting_note("17 game") && !hosting_note("game") &&
          !hosting_note("0 games") && !hosting_note("-1 game") && !hosting_note(nullptr), "strict host note");
    char long_name[96]; memset(long_name, 'x', 60);
    // A 4-byte character would be cut at the buffer limit: discard it whole.
    memcpy(long_name + 60, "\xF0\x9F\x98\x80", 4); long_name[64] = 0;
    char name[NAME_BYTES]; copy_name(name, long_name);
    check(strlen(name) == 60, "UTF-8 truncation never leaves a partial character");
    copy_name(name, "a\nb\tc");
    check(!strcmp(name, "a b c"), "controls cannot break a label");
    char short_name[5];
    copy_name(short_name, "ab\xE2\x82\xAC");
    check(!strcmp(short_name, "ab"), "short UI labels truncate on a UTF-8 boundary");
    copy_name(name, nullptr);
    check(!name[0], "missing persona names are safe");
    printf("steam friends: %d checks, %d failures\n", checks, failures);
    return failures ? 1 : 0;
}

// Platform-independent selection of friends who are running this game.  Steam's flat API is
// adapted in steam.cpp; the same filtering and bounded name handling are exercised by no-game tests.
#pragma once
#include <stddef.h>
#include <stdint.h>
#include <string.h>

namespace lobby_friends {

enum { NAME_BYTES = 64, MAX_SCAN = 8192 };
struct Friend {
    uint64_t id;
    char name[NAME_BYTES];            // UTF-8, suitable for display only, never used as a path
    bool hosting;                     // friend advertised our gtmf "<number> game" note
};

inline bool hosting_note(const char *text)
{
    if (!text || *text < '0' || *text > '9') return false;
    while (*text >= '0' && *text <= '9') ++text;
    return strcmp(text, " game") == 0;
}

template<size_t N> inline void copy_name(char (&dst)[N], const char *src)
{
    if (!src) src = "";
    size_t n = 0;
    while (n + 1 < sizeof dst && src[n]) ++n;
    // Steam names are UTF-8. When truncating, do not leave half of a multibyte character.
    if (src[n]) while (n && ((unsigned char)src[n] & 0xc0) == 0x80) --n;
    for (size_t i = 0; i < n; ++i) {
        unsigned char c = (unsigned char)src[i];
        dst[i] = (c < 0x20 || c == 0x7f) ? ' ' : (char)c;
    }
    dst[n] = 0;
}

// Provider supplies count(), id(index), game(id, &game_id), name(id), note(id), request(id).
// CGameID's low 24 bits are the application ID; a mod/shortcut may use its upper bits.
// Returns the total matching friends, with at most capacity entries written.
template<class Provider>
size_t collect(Provider &source, uint32_t app_id, uint64_t self_id, Friend *out, size_t capacity,
               bool request_presence)
{
    if (!app_id || (!out && capacity)) return 0;
    int count = source.count();
    if (count < 0) return 0;
    if (count > MAX_SCAN) count = MAX_SCAN;
    size_t found = 0;
    for (int i = 0; i < count; ++i) {
        uint64_t id = source.id(i), game_id = 0;
        if (!id || id == self_id || !source.game(id, &game_id) || (game_id & 0xffffffu) != app_id)
            continue;
        if (found < capacity) {
            // Only ask Steam about entries the UI can actually show; the total is still counted.
            if (request_presence) source.request(id);
            Friend &entry = out[found];
            entry.id = id;
            entry.hosting = hosting_note(source.note(id));
            copy_name(entry.name, source.name(id));
        }
        ++found;
    }
    return found;
}
}

// Experimental, bounded name identity for kind-0 shot records (protocol 17).
//
// One supplied 3598/1 game path obtains weapon/shell pointers from i_weapons. Its two
// name indexes are validated and read as data; no game lookup method is called here. This
// is not a universal descriptor identity and matching names do not prove identical configs.
// A name of length 61 is intentionally ineligible: the game's lookup truncates longer names
// to that length. The complete, shorter names must fit in the unused launch-pointer bytes.
#pragma once
#include "common.h"
#include "protocol.h"
#include <stdint.h>
#include <string.h>

namespace named_shots {

enum { k_name_limit = 60, k_wire_size = 40, k_header_size = 15, k_marker = 0xe7 };
static const unsigned char k_magic[4] = { 'N', 'M', 'S', '1' };
struct Name { unsigned char size; unsigned char text[k_name_limit]; };
struct Proof { Name ammo, weapon; unsigned char ammo_flags[4], weapon_mode, weapon_value[4]; };
enum Kind { AMMO, WEAPON };

// The caller supplies the build- and vtable-verified i_weapons object. `readable` comes from
// common.h; these checks cannot prove object lifetime or the absence of concurrent mutation.
struct View {
    const unsigned char *array, *rows;
    uint32_t count, row_count;
    size_t stride;
};
inline bool view(const void *registry, Kind kind, View *out)
{
    if (!registry || !out || !readable(registry, 0x148)) return false;
    const unsigned char *p = (const unsigned char *)registry;
    const size_t arr_off = kind == AMMO ? 0x108 : 0xf0;
    const size_t map_off = kind == AMMO ? 0x138 : 0x120;
    View v = {};
    memcpy(&v.array, p + arr_off, sizeof v.array);
    memcpy(&v.count, p + arr_off + 0xc, sizeof v.count);
    memcpy(&v.rows, p + map_off, sizeof v.rows);
    memcpy(&v.row_count, p + map_off + 0xc, sizeof v.row_count);
    v.stride = kind == AMMO ? 0x170 : 0x130;
    // Existing local_tables uses this same generous ceiling for all descriptions. Avoid
    // overflow and unreasonable scans; do not infer identity from an unavailable table.
    if (!v.count || v.count >= 100000 || !v.row_count || v.row_count >= 100000) return false;
    size_t array_bytes = (size_t)v.count * v.stride, row_bytes = (size_t)v.row_count * 0x44;
    if (!v.array || !v.rows || (uintptr_t)v.array > UINTPTR_MAX - array_bytes ||
        (uintptr_t)v.rows > UINTPTR_MAX - row_bytes ||
        !readable(v.array, array_bytes) || !readable(v.rows, row_bytes)) return false;
    *out = v;
    return true;
}

inline bool row(const View &v, uint32_t at, uint32_t *index, const unsigned char **text, uint16_t *size)
{
    if (at >= v.row_count) return false;
    const unsigned char *r = v.rows + (size_t)at * 0x44;
    memcpy(size, r, sizeof *size);
    memcpy(index, r + 0x40, sizeof *index);
    if (*index >= v.count || !*size || *size > 61 || memchr(r + 2, 0, *size)) return false;
    *text = r + 2;
    return true;
}

// Return a full untruncated name only if the pointer is an exact array element, has exactly
// one name, and that name occurs exactly once in the verified index. Never guess from +0x80.
inline bool name_for(const void *registry, Kind kind, const void *pointer, Name *out)
{
    View v;
    if (!pointer || !out || !view(registry, kind, &v)) return false;
    uintptr_t base = (uintptr_t)v.array, address = (uintptr_t)pointer;
    if (address < base) return false;
    uintptr_t distance = address - base;
    if (distance % v.stride || distance / v.stride >= v.count) return false;
    uint32_t want = (uint32_t)(distance / v.stride), matches = 0;
    Name found = {};
    for (uint32_t i = 0; i < v.row_count; ++i) {
        uint32_t index; const unsigned char *text; uint16_t size;
        if (!row(v, i, &index, &text, &size)) return false;
        if (index != want) continue;
        if (size > k_name_limit || ++matches != 1) return false;
        found.size = (unsigned char)size;
        memcpy(found.text, text, size);
    }
    if (matches != 1) return false;
    matches = 0;
    for (uint32_t i = 0; i < v.row_count; ++i) {
        uint32_t index; const unsigned char *text; uint16_t size;
        if (!row(v, i, &index, &text, &size)) return false;
        if (size == found.size && !memcmp(text, found.text, size) && ++matches != 1) return false;
    }
    if (matches != 1) return false;
    *out = found;
    return true;
}

// Resolve a name without calling the engine's unsafe missing-name lookup. Reject duplicate keys
// and malformed rows. Pointers are based on the category-specific array and checked bounds.
inline const void *pointer_for(const void *registry, Kind kind, const Name &name)
{
    View v;
    if (!name.size || name.size > k_name_limit || memchr(name.text, 0, name.size) ||
        !view(registry, kind, &v)) return nullptr;
    uint32_t matches = 0, found = 0;
    for (uint32_t i = 0; i < v.row_count; ++i) {
        uint32_t index; const unsigned char *text; uint16_t size;
        if (!row(v, i, &index, &text, &size)) return nullptr;
        if (size == name.size && !memcmp(text, name.text, size)) { found = index; ++matches; }
    }
    if (matches != 1) return nullptr;
    const void *result = v.array + (size_t)found * v.stride;
    return readable(result, v.stride) ? result : nullptr;
}

// The old receiver ignores pad and overwrites all five pointer positions before calling launch.
// New receivers treat this marker as requiring valid metadata rather than falling back on a bad
// proof. An ordinary old 1.0.7 sender initializes pad and these pointer positions to zero.
inline bool encode(ShotRec &s, const Name &ammo, const Name &weapon,
                   const void *ammo_ptr, const void *weapon_ptr)
{
    if (!ammo.size || !weapon.size || ammo.size > k_name_limit || weapon.size > k_name_limit ||
        (size_t)ammo.size + weapon.size > k_wire_size - k_header_size ||
        memchr(ammo.text, 0, ammo.size) || memchr(weapon.text, 0, weapon.size) ||
        !readable(ammo_ptr, 0xc8) || !readable(weapon_ptr, 0x94)) return false;
    unsigned char wire[k_wire_size] = {};
    memcpy(wire, k_magic, sizeof k_magic);
    wire[4] = ammo.size; wire[5] = weapon.size;
    memcpy(wire + 6, (const unsigned char *)ammo_ptr + 0xc0, 4);
    wire[10] = ((const unsigned char *)weapon_ptr)[0x88];
    memcpy(wire + 11, (const unsigned char *)weapon_ptr + 0x90, 4);
    memcpy(wire + k_header_size, ammo.text, ammo.size);
    memcpy(wire + k_header_size + ammo.size, weapon.text, weapon.size);
    memcpy(s.launch + 0x38, wire, 0x20);
    memcpy(s.launch + 0x60, wire + 0x20, 8);
    s.pad = k_marker;
    return true;
}

inline bool decode(const ShotRec &s, Proof *out)
{
    if (!out || s.pad != k_marker) return false;
    unsigned char wire[k_wire_size];
    memcpy(wire, s.launch + 0x38, 0x20);
    memcpy(wire + 0x20, s.launch + 0x60, 8);
    if (memcmp(wire, k_magic, sizeof k_magic)) return false;
    const unsigned a = wire[4], w = wire[5];
    if (!a || !w || a > k_name_limit || w > k_name_limit || a + w > k_wire_size - k_header_size ||
        memchr(wire + k_header_size, 0, a + w)) return false;
    for (unsigned i = k_header_size + a + w; i < k_wire_size; ++i) if (wire[i]) return false;
    Proof p = {};
    p.ammo.size = (unsigned char)a; p.weapon.size = (unsigned char)w;
    memcpy(p.ammo_flags, wire + 6, 4); p.weapon_mode = wire[10];
    memcpy(p.weapon_value, wire + 11, 4);
    memcpy(p.ammo.text, wire + k_header_size, a);
    memcpy(p.weapon.text, wire + k_header_size + a, w);
    *out = p;
    return true;
}

inline bool fields_match(const Proof &p, const void *ammo_ptr, const void *weapon_ptr)
{
    if (!readable(ammo_ptr, 0xc8) || !readable(weapon_ptr, 0x94)) return false;
    return !memcmp(p.ammo_flags, (const unsigned char *)ammo_ptr + 0xc0, 4) &&
           p.weapon_mode == ((const unsigned char *)weapon_ptr)[0x88] &&
           !memcmp(p.weapon_value, (const unsigned char *)weapon_ptr + 0x90, 4);
}

} // namespace named_shots

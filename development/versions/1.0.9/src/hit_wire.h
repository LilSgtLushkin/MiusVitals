// Pointer-free envelope for an experimental target-owner hit on the existing ordered shot stream.
// The first 0x68 bytes have the same meaning as the captured engine record. The three pointer
// fields at 0x68/0x70/0x78 are never sent: they are reconstructed only after local validation.
#pragma once
#include "protocol.h"
#include <stdint.h>
#include <string.h>

namespace hit_wire {
static const unsigned kind = 7;
static const size_t copied = 0x68;
static const size_t engine_size = 0x80;
static const size_t queue_limit = 4096;
static const uint32_t max_parts = 4096, max_groups = 6;
struct PartHint { uint32_t count, ordinal; uint64_t shape; };
static_assert(sizeof(PartHint) == 16, "hit part metadata");

// v20 matched equal-length component lists by low ten bits, and mapped at most six distinct
// high-bit groups. Hash that same normalized group/low-bit sequence; raw process-specific
// group numbers never have to be guessed or sent as addresses. Callers validate the row span.
inline bool shape(const unsigned char *rows, uint32_t count, uint64_t *out)
{
    if (!rows || !out || !count || count > max_parts) return false;
    uint32_t groups[max_groups] = {}, used = 0;
    uint64_t h = 14695981039346656037ull;
    for (uint32_t i = 0; i < count; ++i) {
        uint32_t id = 0; memcpy(&id, rows + (size_t)i * 0x60 + 0x48, 4);
        uint32_t high = id >> 10, group = 0;
        while (group < used && groups[group] != high) ++group;
        if (group == used) { if (used == max_groups) return false; groups[used++] = high; }
        uint32_t low = id & 1023u;
        const unsigned char b[3] = {(unsigned char)group, (unsigned char)low, (unsigned char)(low >> 8)};
        for (unsigned char x : b) h = (h ^ x) * 1099511628211ull;
    }
    *out = h ? h : 1;
    return true;
}
inline bool make_hint(const unsigned char *rows, uint32_t count, uint32_t part, PartHint *hint)
{
    if (!hint || part == 0xffffffffu || !shape(rows, count, &hint->shape)) return false;
    hint->count = count; hint->ordinal = count;
    for (uint32_t i = 0; i < count; ++i) {
        uint32_t id = 0; memcpy(&id, rows + (size_t)i * 0x60 + 0x48, 4);
        if ((id >> 10) == (part >> 10)) { hint->ordinal = i; break; }
    }
    return hint->ordinal < count;
}
inline bool translate(const unsigned char *rows, uint32_t count, uint32_t part,
                      const PartHint &hint, uint32_t *translated)
{
    uint64_t local_shape = 0;
    if (!translated || !hint.shape || hint.count != count || hint.ordinal >= count ||
        !shape(rows, count, &local_shape) || local_shape != hint.shape) return false;
    uint32_t id = 0; memcpy(&id, rows + (size_t)hint.ordinal * 0x60 + 0x48, 4);
    for (uint32_t i = 0; i < hint.ordinal; ++i) {
        uint32_t before = 0; memcpy(&before, rows + (size_t)i * 0x60 + 0x48, 4);
        if ((before >> 10) == (id >> 10)) return false; // sender always chooses the first row in a group
    }
    *translated = (id & ~1023u) | (part & 1023u);
    return true;
}

inline bool unit_key(uint64_t key, bool nullable)
{
    if (!key) return nullable;
    uint64_t type = key >> 32;
    return type == 1 || type == 2;
}
inline bool valid(const ShotRec &s)
{
    if (s.kind != kind || s.flags > 1 || s.has_pos > 1 || s.pad > 1 || !unit_key(s.shooter, false) ||
        !unit_key(s.obj2, true) || !unit_key(s.obj5, true) ||
        (s.shooter >> 32) != (s.flags ? 1u : 2u)) return false;
    // Part metadata occupies the otherwise unused position bytes, never a pointer.
    uint32_t part = 0; memcpy(&part, s.launch + 0x60, 4);
    PartHint hint = {}; memcpy(&hint, s.pos, sizeof hint);
    // Human hit paths sampled in the supplied 3598/1 image do not use this renderer component
    // handle. Never transfer a process-local handle for humans; only vehicles use translation.
    if (!s.flags && part != 0xffffffffu) return false;
    if (part == 0xffffffffu) {
        if (s.pad || hint.count || hint.ordinal || hint.shape) return false;
    } else if (s.pad != 1 || !hint.count || hint.count > max_parts || hint.ordinal >= hint.count || !hint.shape) return false;
    const unsigned char *checks = (const unsigned char *)s.ammo_check;
    for (size_t i = 0; i < sizeof s.ammo_check; ++i) if (checks[i]) return false;
    for (size_t i = copied; i < sizeof s.launch; ++i) if (s.launch[i]) return false;
    return true;
}
inline void encode(ShotRec &s, bool vehicle, uint64_t target, uint64_t source, uint64_t object,
                   uint32_t desc1, uint32_t desc2, uint32_t argument, uint32_t argument5,
                   bool argument6, uint32_t seed, const void *record)
{
    memset(&s, 0, sizeof s);
    s.kind = kind; s.flags = vehicle ? 1 : 0; s.has_pos = argument6 ? 1 : 0;
    s.shooter = target; s.obj2 = source; s.obj5 = object;
    s.ammo = desc1; s.weapon = desc2; s.arg6 = argument; s.ammo_code = argument5; s.desc3 = seed;
    memcpy(s.launch, record, copied);
    if (!vehicle) { const uint32_t no_component = 0xffffffffu; memcpy(s.launch + 0x60, &no_component, 4); }
}
inline void put_hint(ShotRec &s, const PartHint &hint) {
    s.pad = 1; memcpy(s.pos, &hint, sizeof hint);
}
} // namespace hit_wire

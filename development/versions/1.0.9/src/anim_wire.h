// Selected scalar animation values. The 0x140-byte engine instance contains pointers, so never
// put it on the wire wholesale. This layout follows the v20 snapshot fields, not its wire format.
#pragma once
#include "protocol.h"
#include <stdint.h>
#include <string.h>

namespace anim_wire {
static const unsigned kind = 8;
static const size_t snapshot_size = 104;
static const size_t instance_size = 0x140;
static const size_t queue_limit = 2048;
static const unsigned max_index = 100000;

inline bool finite_field(const unsigned char *p)
{
    float v; memcpy(&v, p, sizeof v);
    return v >= -1000000.f && v <= 1000000.f; // also rejects NaN and infinity
}
inline bool valid(const ShotRec &s)
{
    if (s.kind != kind || s.flags || s.has_pos || s.pad || (s.shooter >> 32) != 2 ||
        s.ammo || s.weapon || s.desc3 || s.arg6 || s.ammo_code || s.obj2 || s.obj5) return false;
    const unsigned char *p = s.launch;
    uint32_t id = 0; memcpy(&id, p, sizeof id);
    if (id != (uint32_t)s.shooter || p[50] > 1 || p[51]) return false;
    uint32_t active = 0; memcpy(&active, p + 8, sizeof active);
    if (active > 1) return false;
    const size_t floats[] = {24,28,32,36,40,44,84,88,92,96,100};
    for (size_t off : floats) if (!finite_field(p + off)) return false;
    if (p[50]) {
        size_t n = 0;
        while (n < 28 && p[52 + n]) {
            if (p[52 + n] < 0x20 || p[52 + n] > 0x7e) return false;
            ++n;
        }
        if (!n || n >= 28) return false;
        for (size_t i = n + 1; i < 28; ++i) if (p[52 + i]) return false;
    } else for (size_t i = 52; i < 80; ++i) if (p[i]) return false;
    const unsigned char *checks = (const unsigned char *)s.ammo_check;
    for (size_t i = 0; i < sizeof s.ammo_check; ++i) if (checks[i]) return false;
    const unsigned char *position = (const unsigned char *)s.pos;
    for (size_t i = 0; i < sizeof s.pos; ++i) if (position[i]) return false;
    for (size_t i = snapshot_size; i < sizeof s.launch; ++i) if (p[i]) return false;
    return true;
}

// Only scalar fields are copied. Group names and soldier extras are supplied separately after
// the caller has independently validated the relevant engine memory.
inline void encode(ShotRec &s, uint64_t key, const unsigned char *instance,
                   const char *group, const unsigned char *human)
{
    memset(&s, 0, sizeof s);
    s.kind = kind; s.shooter = key;
    unsigned char *p = s.launch;
    uint32_t id = (uint32_t)key; memcpy(p, &id, 4);
    memcpy(p + 4, instance + 0xa8, 4);
    memcpy(p + 8, instance + 0x104, 4);
    memcpy(p + 12, instance + 0x10c, 4);
    memcpy(p + 16, instance + 0x100, 4);
    memcpy(p + 20, instance + 0x114, 4);
    memcpy(p + 24, instance + 0xd4, 4);
    memcpy(p + 28, instance + 0x11c, 12);
    memcpy(p + 40, instance + 0x128, 8);
    p[48] = instance[0x136]; p[49] = instance[0x139];
    if (group) {
        size_t n = 0; while (n < 28 && group[n]) ++n;
        if (n && n < 28) { p[50] = 1; memcpy(p + 52, group, n); }
    }
    memcpy(p + 80, human + 0x1e70, 4);
    memcpy(p + 84, human + 0x1210, 12);
    memcpy(p + 96, human + 0x1eac, 4);
    memcpy(p + 100, human + 0x1ef8, 4);
}
} // namespace anim_wire

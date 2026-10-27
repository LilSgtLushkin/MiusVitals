// Shape validation for the existing full-state wire protocol, before allocating or copying.
#pragma once
#include "protocol.h"
#include <stddef.h>
#include <stdint.h>

namespace state_fragments {
static const uint32_t fragment_bytes = 1100;
static const uint32_t maximum_bytes = 16u << 20;
// Nonzero versions advance in a wrapping 32-bit serial number space.
inline bool newer(uint32_t value, uint32_t previous) {
    uint32_t delta = value - previous;
    return delta != 0 && delta < 0x80000000u;
}
inline bool valid(const ObjectFrag &f, size_t packet_bytes)
{
    uint32_t len = f.h.count;
    if (f.h.type != PKT_OBJECT || !f.version || f.raw_size > maximum_bytes || f.delta_size > maximum_bytes ||
        !f.nfrags || f.frag >= f.nfrags || len > fragment_bytes ||
        packet_bytes < sizeof(ObjectFrag) || len > packet_bytes - sizeof(ObjectFrag)) return false;
    uint32_t count = f.delta_size ? (f.delta_size + fragment_bytes - 1) / fragment_bytes : 1;
    if (f.nfrags != count) return false;
    uint32_t offset = (uint32_t)f.frag * fragment_bytes;
    uint32_t remaining = f.delta_size - offset;
    uint32_t expected = remaining < fragment_bytes ? remaining : fragment_bytes;
    return len == expected;
}
}

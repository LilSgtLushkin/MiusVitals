// Validate an image/map fragment before it marks a piece received or touches the destination buffer.
#pragma once
#include "protocol.h"
#include <stddef.h>

namespace image_chunks {
inline bool valid(const ImageData *piece, size_t packet_bytes, size_t total, unsigned flags,
                  unsigned epoch = 0, unsigned revision = 0) {
    const size_t header = sizeof(ImageData) - k_chunk;
    if (!piece || packet_bytes < header || piece->h.magic != k_magic || piece->h.type != PKT_IMAGE_DATA ||
        piece->h.flags != flags || !total || total > (64u << 20)) return false;
    if (flags == 1 && (!epoch || !revision || piece->map_epoch != epoch || piece->map_revision != revision)) return false;
    const size_t index = piece->index;
    if (index >= (total + k_chunk - 1) / k_chunk) return false;
    size_t offset = index * (size_t)k_chunk;
    size_t left = total - offset;
    size_t expected = left < k_chunk ? left : k_chunk;
    return piece->size == expected && packet_bytes >= header + expected;
}
}

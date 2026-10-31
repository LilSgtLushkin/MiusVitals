// Carry the host's transfer checksum across a local process launch.
// Neither FNV nor the inherited environment authenticates the data. The prepared-image
// fingerprint rejects ordinary stale tickets for different bytes, but collisions and an
// old ticket for identical prepared bytes remain possible.
#pragma once
#include <stddef.h>
#include <stdint.h>

namespace battle_identity {
static const char k_environment[] = "MP_BATTLE_IDENTITY";
static const size_t k_text_size = 26; // 8 hex digits, ':', 16 hex digits, NUL

inline uint64_t fingerprint(const unsigned char *bytes, size_t size)
{
    uint64_t h = UINT64_C(14695981039346656037);
    for (size_t i = 0; i < size; ++i) h = (h ^ bytes[i]) * UINT64_C(1099511628211);
    return h;
}
inline void format(unsigned source_hash, uint64_t prepared_hash, char out[k_text_size])
{
    static const char hex[] = "0123456789abcdef";
    for (unsigned i = 0; i < 8; ++i) out[i] = hex[(source_hash >> (28 - i * 4)) & 15u];
    out[8] = ':';
    for (unsigned i = 0; i < 16; ++i) out[9 + i] = hex[(prepared_hash >> (60 - i * 4)) & 15u];
    out[25] = 0;
}
inline int digit(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}
inline bool resolve(const char *text, const unsigned char *prepared, size_t size, unsigned *source_hash)
{
    if (!text || !prepared || !size || !source_hash) return false;
    // Read only up to the first terminator, including for malformed/short environment values.
    unsigned source = 0; uint64_t local = 0;
    for (unsigned i = 0; i < 8; ++i) {
        int x = digit(text[i]); if (x < 0) return false;
        source = (source << 4) | (unsigned)x;
    }
    if (text[8] != ':') return false;
    for (unsigned i = 0; i < 16; ++i) {
        int x = digit(text[9 + i]); if (x < 0) return false;
        local = (local << 4) | (unsigned)x;
    }
    if (text[25] || local != fingerprint(prepared, size)) return false;
    *source_hash = source;
    return true;
}
}

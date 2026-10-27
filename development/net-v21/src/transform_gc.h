// Discard cached transforms for objects no longer present in the current object manager. The key
// set is rebuilt before calling this. Surviving keys and their cached data are left unchanged.
#pragma once
#include <stddef.h>

template <class Cache, class Index>
size_t prune_absent(Cache &cache, const Index &live)
{
    size_t removed = 0;
    for (auto it = cache.begin(); it != cache.end();) {
        if (live.find(it->first) == live.end()) { it = cache.erase(it); ++removed; }
        else ++it;
    }
    return removed;
}

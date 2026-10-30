// Object-list and transform-cache lifetime helpers. Object counts alone are not a generation:
// a removal and an insertion can leave the count unchanged while a cached pointer is obsolete.
#pragma once
#include <stddef.h>
#include <string.h>
#include <vector>

// Compare the exact ordered pointer list, not a hash: this avoids both collisions and the cost of
// rebuilding the unit index on every simulation step. A different manager always requires a
// rebuild. This detects pointer replacement/reordering, not destruction and reuse at the same
// address between two observations; the engine does not expose a generation here.
template <class Object>
class ObjectListSnapshot {
    const void *manager_ = nullptr;
    std::vector<Object *> entries_;
    bool initialized_ = false;
public:
    bool matches(const void *manager, Object *const *entries, size_t count) const {
        if (!initialized_ || manager_ != manager || entries_.size() != count) return false;
        return !count || (entries && !memcmp(entries_.data(), entries, count * sizeof(Object *)));
    }
    void remember(const void *manager, Object *const *entries, size_t count) {
        if (count) entries_.assign(entries, entries + count);
        else entries_.clear();
        manager_ = manager;
        initialized_ = true;
    }
    const std::vector<Object *> &entries() const { return entries_; }
    void clear() { entries_.clear(); manager_ = nullptr; initialized_ = false; }
};

// Discard cached transforms for objects no longer present in the current object manager. The key
// set is rebuilt before calling this. Surviving keys and their cached data are left unchanged.

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

// A key reused by a different object is not the same cached unit. Discard its held/sent state as
// well as state for keys that disappeared; preserve only an identical key and object pointer.
template <class Cache, class Index>
size_t prune_replaced(Cache &cache, const Index &previous, const Index &current)
{
    size_t removed = 0;
    for (auto it = cache.begin(); it != cache.end();) {
        auto old = previous.find(it->first), live = current.find(it->first);
        if (old == previous.end() || live == current.end() || old->second != live->second) {
            it = cache.erase(it); ++removed;
        } else ++it;
    }
    return removed;
}

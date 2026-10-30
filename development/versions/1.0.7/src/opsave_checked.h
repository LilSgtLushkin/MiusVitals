// Protocol 15 spectator mode uses validation/identity helpers only; ordered guest-turn
// routines below are retained for historical tests and are NOT connected to the session.
// Conservative checks for the ordered operation handoff. The guest receives the host's *updated*
// save, so a checked guest save can be kept whole: no foreign absolute pointers are copied into a
// different file. Unknown structures are rejected rather than silently losing either player's moves.
#pragma once
#include "opsave.h"
#include <stdint.h>
#include <limits>

namespace opsave_checked {
using opsave::byte;
using opsave::NodeRef;
using opsave::Sidecar;

inline bool reject(std::string *why, const std::string &text) { if (why) *why = text; return false; }
inline bool span(size_t size, size_t at, size_t count, size_t width) {
    return at <= size && width && count <= (size - at) / width;
}
inline bool safe_profile_id(const char *name) {
    if (!name || !*name || strlen(name) > 63) return false;
    for (const char *p = name; *p; ++p) {
        unsigned char c = (unsigned char)*p;
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
              (c >= '0' && c <= '9') || c == '_' || c == '-')) return false;
    }
    return true;
}

// Validate every node/cell before calling the older convenience readers, which assume bounded offsets.
// Type 0 is the empty root seen in the game's progpack files; 1 is a struct, 2 a table. A numeric
// list's meaning is not understood here, so it is not followed as a pointer or relocated.
inline bool validate(const std::vector<byte> &b, std::string *why = nullptr) {
    if (b.size() < 0x40 || b.size() > (64u << 20) || memcmp(b.data(), "AZCF", 4))
        return reject(why, "not a bounded AZCF file");
    const uint32_t count = opsave::u32_at(b, 4), table = opsave::u32_at(b, 8);
    if (!count || !span(b.size(), table, count, 0x80)) return reject(why, "node table is out of bounds");
    for (uint32_t i = 0; i < count; ++i) {
        size_t o = table + (size_t)i * 0x80;
        uint32_t type = opsave::u32_at(b, o + 0x20), cols = opsave::u32_at(b, o + 0x24);
        uint32_t rows = opsave::u32_at(b, o + 0x28), data = opsave::u32_at(b, o + 0x30);
        if (type > 2) return reject(why, "unknown node type");
        if (type == 0) {
            if (cols || rows || data > b.size()) return reject(why, "invalid root node");
            continue;
        }
        if (type == 2 && rows && !cols) return reject(why, "table has rows but no columns");
        size_t cells = rows;
        if (type == 2) {
            if (cols && cells > (std::numeric_limits<size_t>::max)() / cols) return reject(why, "table cell count overflow");
            cells *= cols;
        }
        size_t width = type == 1 ? 0x40 : 0x20;
        if (!span(b.size(), data, cells, width)) return reject(why, "node data is out of bounds");
        for (size_t j = 0; j < cells; ++j) {
            size_t cell = data + j * width;
            uint32_t kind = opsave::u32_at(b, cell + 16);
            if (kind > 6) return reject(why, "unknown cell kind");
            if (kind == 2) {
                uint32_t len = opsave::u32_at(b, cell), off = opsave::u32_at(b, cell + 4);
                if (off >= b.size() || len >= b.size() - off || b[(size_t)off + len] != 0)
                    return reject(why, "invalid text reference");
            }
        }
    }
    return true;
}

inline bool same_cell(const std::vector<byte> &a, size_t ca, const std::vector<byte> &b, size_t cb) {
    uint32_t kind = opsave::u32_at(a, ca + 16);
    if (kind != opsave::u32_at(b, cb + 16) || memcmp(a.data() + ca + 16, b.data() + cb + 16, 16)) return false;
    if (kind != 2) return memcmp(a.data() + ca, b.data() + cb, 16) == 0;
    uint32_t an = opsave::u32_at(a, ca), bn = opsave::u32_at(b, cb);
    uint32_t ao = opsave::u32_at(a, ca + 4), bo = opsave::u32_at(b, cb + 4);
    return an == bn && memcmp(a.data() + ao, b.data() + bo, an) == 0 &&
           memcmp(a.data() + ca + 8, b.data() + cb + 8, 8) == 0;
}

inline bool mirror(std::vector<byte> &save, Sidecar *side, unsigned *changed, std::string *why = nullptr) {
    if (!side || !validate(save, why)) return false;
    std::vector<opsave::Tagged> tags = opsave::tagged(save);
    std::set<std::string> names;
    unsigned host_side = 0, enemy_side = 0;
    for (const auto &t : tags) {
        if (t.name.empty() || !names.insert(t.name).second) return reject(why, "duplicate or empty tagged group name");
        unsigned ai = opsave::u32_at(save, t.ai);
        if (!ai) continue;
        if (!t.side || !opsave::u32_at(save, t.side)) return reject(why, "tagged group has no side");
        unsigned s = opsave::u32_at(save, t.side);
        if (ai == opsave::k_user) {
            if (host_side && host_side != s) return reject(why, "host controls multiple sides");
            host_side = s;
        }
    }
    if (!host_side) return reject(why, "host side is unknown");
    for (const auto &t : tags) {
        if (!opsave::u32_at(save, t.ai)) continue;
        unsigned s = opsave::u32_at(save, t.side);
        if (s != host_side) {
            if (enemy_side && enemy_side != s) return reject(why, "more than two sides in save");
            enemy_side = s;
        }
    }
    if (!enemy_side) return reject(why, "opposing side is unknown");
    Sidecar prepared;
    if (!opsave::mirror(save, &prepared, changed)) return reject(why, "save could not be mirrored");
    *side = prepared;
    return true;
}

// Bounded binary sidecar preserves BOTH ai and visibility. The caller persists these bytes with the
// same-directory atomic-file helper; decode rejects truncation, duplicates, extra bytes and unknown flags.
inline void put32(std::vector<byte> &b, uint32_t n) {
    b.push_back((byte)n); b.push_back((byte)(n >> 8)); b.push_back((byte)(n >> 16)); b.push_back((byte)(n >> 24));
}
inline uint32_t get32(const std::vector<byte> &b, size_t at) {
    return (uint32_t)b[at] | (uint32_t)b[at + 1] << 8 | (uint32_t)b[at + 2] << 16 | (uint32_t)b[at + 3] << 24;
}
inline bool encode_sidecar(const Sidecar &side, std::vector<byte> *out) {
    if (!out || !side.code || side.ai.empty() || side.ai.size() > 100000) return false;
    std::vector<byte> result;
    const byte magic[8] = {'M','V','S','I','D','E','2',0};
    result.insert(result.end(), magic, magic + sizeof magic);
    put32(result, side.code); put32(result, (uint32_t)side.ai.size());
    for (const auto &it : side.ai) {
        if (it.first.empty() || it.first.size() > 32 || it.first.find('\0') != std::string::npos || !it.second) return false;
        auto v = side.vis.find(it.first);
        put32(result, (uint32_t)it.first.size()); put32(result, it.second);
        put32(result, v == side.vis.end() ? 0 : 1); put32(result, v == side.vis.end() ? 0 : v->second);
        result.insert(result.end(), it.first.begin(), it.first.end());
    }
    for (const auto &it : side.vis) if (!side.ai.count(it.first)) return false;
    out->swap(result);
    return true;
}
inline bool decode_sidecar(const std::vector<byte> &data, Sidecar *side) {
    const byte magic[8] = {'M','V','S','I','D','E','2',0};
    if (!side || data.size() < 16 || data.size() > (8u << 20) || memcmp(data.data(), magic, sizeof magic)) return false;
    Sidecar result; result.code = get32(data, 8);
    uint32_t count = get32(data, 12);
    if (!result.code || !count || count > 100000) return false;
    size_t at = 16;
    for (uint32_t i = 0; i < count; ++i) {
        if (!span(data.size(), at, 1, 16)) return false;
        uint32_t length = get32(data, at), ai = get32(data, at + 4), flags = get32(data, at + 8), vis = get32(data, at + 12);
        at += 16;
        if (!length || length > 32 || !ai || flags > 1 || (!flags && vis) || !span(data.size(), at, length, 1)) return false;
        std::string name((const char *)data.data() + at, length); at += length;
        if (name.find('\0') != std::string::npos || !result.ai.emplace(name, ai).second) return false;
        if (flags) result.vis[name] = vis;
    }
    if (at != data.size()) return false;
    *side = result;
    return true;
}

using Key = std::pair<std::string, unsigned>;
inline std::map<Key, NodeRef> index_nodes(const std::vector<NodeRef> &nodes) {
    std::map<Key, NodeRef> result;
    std::map<std::string, unsigned> seen;
    for (const auto &n : nodes) result[{n.name, seen[n.name]++}] = n;
    return result;
}

// Compare re-serialized snapshots by value. Used to detect host-side changes after handoff;
// a pointer-like numeric cell that moved is conservatively treated as a change, never guessed at.
inline bool equivalent(const std::vector<byte> &a, const std::vector<byte> &b, std::string *why = nullptr) {
    if (!validate(a, why) || !validate(b, why) || opsave::operation_name(a) != opsave::operation_name(b))
        return reject(why, "host operation changed");
    auto left = index_nodes(opsave::nodes(a)), right = index_nodes(opsave::nodes(b));
    if (left.size() != right.size()) return reject(why, "host node count changed");
    for (const auto &it : left) {
        auto found = right.find(it.first);
        if (found == right.end()) return reject(why, "host node changed: " + it.first.first);
        const NodeRef &x = it.second, &y = found->second;
        if (x.type != y.type || x.cols != y.cols || x.rows != y.rows) return reject(why, "host structure changed: " + x.name);
        if (!x.type) continue;
        size_t cells = x.type == 1 ? x.rows : (size_t)x.rows * x.cols;
        size_t stride = x.type == 1 ? 0x40 : 0x20;
        for (size_t i = 0; i < cells; ++i) {
            size_t cx = x.data + i * stride, cy = y.data + i * stride;
            if ((x.type == 1 && memcmp(a.data() + cx + 0x20, b.data() + cy + 0x20, 0x20)) || !same_cell(a, cx, b, cy))
                return reject(why, "host value changed after handoff: " + x.name);
        }
    }
    return true;
}

// `base` is EXACTLY the mirrored handoff after the host finished; `guest` is the returned save.
// Existing orders must survive byte-for-value, including text. We never transplant cells from
// guest to base, so any absolute offsets in newly added rows keep pointing into their own file.
inline bool accept(const std::vector<byte> &base, const std::vector<byte> &guest, const Sidecar &side,
                   std::vector<byte> *out, unsigned *added_orders, std::string *why = nullptr) {
    if (!out || !validate(base, why) || !validate(guest, why)) return false;
    std::vector<byte> encoded;
    if (!encode_sidecar(side, &encoded)) return reject(why, "sidecar is missing or invalid");
    std::string name = opsave::operation_name(base), their_name = opsave::operation_name(guest);
    if (name.empty() || (their_name != name && their_name != opsave::counterpart(name)))
        return reject(why, "returned save belongs to another operation");
    auto base_nodes = index_nodes(opsave::nodes(base)), guest_nodes = index_nodes(opsave::nodes(guest));
    unsigned base_orders = 0, guest_orders = 0;
    auto ord = Key("user_orders", 0);
    if (base_nodes.count(Key("user_orders", 1)) || guest_nodes.count(Key("user_orders", 1)))
        return reject(why, "ambiguous user_orders tables");
    if (base_nodes.count(ord)) base_orders = base_nodes[ord].rows;
    if (guest_nodes.count(ord)) guest_orders = guest_nodes[ord].rows;
    if (guest_orders < base_orders || (base_nodes.count(ord) && !guest_nodes.count(ord)))
        return reject(why, "host orders were removed");

    std::map<std::string, opsave::Tagged> tags;
    unsigned host_side = 0;
    for (const auto &t : opsave::tagged(base)) {
        if (t.name.empty() || !tags.emplace(t.name, t).second) return reject(why, "ambiguous tagged group");
        auto it = side.ai.find(t.name);
        if (it == side.ai.end()) {
            if (opsave::u32_at(base, t.ai)) return reject(why, "sidecar misses a tagged group");
            continue;
        }
        if (!t.side || !opsave::u32_at(base, t.side)) return reject(why, "tagged group lacks a side");
        if (it->second == opsave::k_user) {
            unsigned s = opsave::u32_at(base, t.side);
            if (host_side && host_side != s) return reject(why, "ambiguous host side");
            host_side = s;
        }
        if ((t.vis != 0) != (side.vis.count(t.name) != 0)) return reject(why, "sidecar visibility is incomplete");
    }
    if (!host_side) return reject(why, "host side missing from sidecar");
    for (const auto &it : side.ai) if (!tags.count(it.first)) return reject(why, "sidecar has an unknown group");

    for (const auto &item : base_nodes) {
        auto found = guest_nodes.find(item.first);
        if (found == guest_nodes.end()) return reject(why, "a node was removed: " + item.first.first);
        const NodeRef &a = item.second, &b = found->second;
        if (a.type != b.type || a.cols != b.cols) return reject(why, "node type or width changed: " + a.name);
        bool orders = item.first == ord;
        if (orders) {
            if (a.type != 2 || b.rows < a.rows) return reject(why, "invalid orders table");
        } else if (a.rows != b.rows) return reject(why, "unsupported structural change: " + a.name);
        if (a.type == 0) continue;
        auto ti = tags.find(a.name);
        bool host_group = a.type == 1 && ti != tags.end() && ti->second.side &&
                          opsave::u32_at(base, ti->second.side) == host_side;
        bool guest_group = a.type == 1 && ti != tags.end() && ti->second.side &&
                           opsave::u32_at(base, ti->second.side) != host_side;
        size_t cells = a.type == 1 ? a.rows : (size_t)a.rows * a.cols;
        size_t stride = a.type == 1 ? 0x40 : 0x20;
        for (size_t i = 0; i < cells; ++i) {
            size_t ca = a.data + i * stride, cb = b.data + i * stride;
            if (opsave::u32_at(base, ca + 16) != opsave::u32_at(guest, cb + 16))
                return reject(why, "cell type changed: " + a.name);
            // Unknown global/table values cannot safely be attributed to the guest. Only guest
            // group fields and host visibility (which is restored from the sidecar) may differ.
            bool preserve = !guest_group;
            if (a.type == 1) {
                const char *fa = (const char *)base.data() + ca + 0x20;
                const char *fb = (const char *)guest.data() + cb + 0x20;
                if (memcmp(fa, fb, 0x20)) return reject(why, "struct fields changed: " + a.name);
                if (host_group && !strncmp(fa, "vis_mode", 0x20)) preserve = false;
                if (ti != tags.end() && (!strncmp(fa, "ai", 0x20) || !strncmp(fa, "side", 0x20))) preserve = true;
            }
            if (preserve && !same_cell(base, ca, guest, cb))
                return reject(why, orders ? "host order was changed" : "protected host/global value was changed: " + a.name);
        }
    }
    for (const auto &item : guest_nodes)
        if (!base_nodes.count(item.first) && !(item.first == ord && item.second.type == 2))
            return reject(why, "unsupported new node: " + item.first.first);

    std::vector<byte> result = guest;
    unsigned restored = 0;
    if (!opsave::unmirror(result, side, &restored) || !restored) return reject(why, "cannot restore host ownership");
    opsave::set_operation_name(result, name);
    if (!validate(result, why)) return false;
    if (added_orders) *added_orders = guest_orders - base_orders;
    out->swap(result);
    return true;
}
// After the engine has processed the joint turn, put the recorded ownership and the host's
// pre-handoff visibility back. Refuse unknown/ambiguous groups instead of inventing a tag.
// This conservative visibility is NOT proof of the game's fog-of-war semantics.
inline bool restore_after_turn(std::vector<byte> &save, const Sidecar &side, unsigned *changed, std::string *why = nullptr) {
    if (!validate(save, why)) return false;
    std::vector<byte> encoded;
    if (!encode_sidecar(side, &encoded)) return reject(why, "invalid sidecar after turn");
    std::set<std::string> seen;
    std::vector<byte> restored = save;
    unsigned n = 0;
    for (const auto &t : opsave::tagged(restored)) {
        if (t.name.empty() || !seen.insert(t.name).second) return reject(why, "ambiguous group after turn");
        auto it = side.ai.find(t.name);
        unsigned current = opsave::u32_at(restored, t.ai);
        if (it == side.ai.end()) {
            if (current) return reject(why, "unknown group after turn: " + t.name);
            continue;
        }
        if (!current || (t.vis != 0) != (side.vis.count(t.name) != 0))
            return reject(why, "changed group layout after turn: " + t.name);
        if (current != it->second) ++n;
        memcpy(restored.data() + t.ai, &it->second, 4);
        if (t.vis) { unsigned vis = side.vis.at(t.name); memcpy(restored.data() + t.vis, &vis, 4); }
    }
    for (const auto &it : side.ai) if (!seen.count(it.first)) return reject(why, "missing group after turn: " + it.first);
    if (changed) *changed = n;
    save.swap(restored);
    return true;
}
} // namespace opsave_checked

// Putting the other player's force into a battle (M4), as pure work on the battle description's bytes.
//
// A battle description is the CFG the engine starts a battle from (image.h describes the format). It holds,
// among everything else, two tables:
//
//   squads   one row per squad: id in column 1, a side code ('GER'/'USSR') and a command tag
//            ('USER' = the squads this game's player commands, 'ENEM' or a formation code for the rest)
//   units    one row per soldier or vehicle, named "i_techn:tec_<id>" / "i_human:hum_<id>"; a unit belongs
//            to the squad with id (unit id & ~0x1f)
//
// Each player picks his own force in his own Skirmish screen, so each game's description carries his own
// force on his side and whatever the setup gave the other side. A two-player battle needs both forces in
// both games, every unit keeping the id it has in its owner's description. That is what this file does: it
// takes the other player's description, lifts his own squads and units out of it, and writes them into this
// game's description on the side he commands.
//
// How the rows are written: the file's offsets are absolute, so nothing already in it may move. The new
// table (this game's rows plus the other player's) is therefore appended at the end of the file and the
// node's row count and data offset are pointed at it; the old table stays where it was, unreferenced. A
// row's text cell is (length, offset), so the other player's text is copied into the appended block and its
// offset rebased, while this game's own rows keep their offsets - their text is still where it was. Cells of
// a kind this file does not know (number lists that point somewhere) are refused rather than guessed at,
// and the result is parsed back and checked before anybody uses it.
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "image.h"

namespace force {

typedef unsigned char byte;

struct Table {
    bool ok = false;
    size_t header = 0, data = 0;
    unsigned cols = 0, rows = 0;
};

enum { k_kind_offset = 16, k_text = 2, k_numbers = 6 };

inline unsigned u32_at(const std::vector<byte> &b, size_t o)
{
    unsigned v = 0;
    if (o + 4 <= b.size()) memcpy(&v, b.data() + o, 4);
    return v;
}

inline void put_u32(std::vector<byte> &b, size_t o, unsigned v)
{
    if (o + 4 <= b.size()) memcpy(b.data() + o, &v, 4);
}

inline Table table(const std::vector<byte> &b, const char *name)
{
    Table t;
    if (b.size() < 0x40 || memcmp(b.data(), "AZCF", 4)) return t;
    unsigned count = u32_at(b, 4), table_at = u32_at(b, 8);
    for (unsigned i = 0; i < count; i++) {
        size_t o = table_at + (size_t)i * 0x80;
        if (o + 0x80 > b.size()) break;
        if (strncmp((const char *)b.data() + o, name, 0x20)) continue;
        unsigned type = u32_at(b, o + 0x20), cols = u32_at(b, o + 0x24), rows = u32_at(b, o + 0x28), data = u32_at(b, o + 0x30);
        if (type != 2 || !cols) return t;
        if (data + (size_t)rows * cols * 0x20 > b.size()) return t;
        t.ok = true;
        t.header = o;
        t.cols = cols;
        t.rows = rows;
        t.data = data;
        return t;
    }
    return t;
}

inline size_t cell_at(const Table &t, unsigned row, unsigned col) { return t.data + ((size_t)row * t.cols + col) * 0x20; }

// A text cell: kind 2, first word the length, second the offset of the text itself (NUL terminated).
inline bool text_at(const std::vector<byte> &b, size_t cell, std::string *out)
{
    if (cell + 0x20 > b.size() || u32_at(b, cell + k_kind_offset) != k_text) return false;
    unsigned len = u32_at(b, cell), off = u32_at(b, cell + 4);
    if (off >= b.size() || len >= b.size() - off) return false;
    if (b[off + len] != 0) return false;
    out->assign((const char *)b.data() + off, len);
    return true;
}

// id of a unit row: the hex number after the last '_' of "i_techn:tec_<id>" / "i_human:hum_<id>"
inline unsigned unit_id(const std::vector<byte> &b, const Table &units, unsigned row)
{
    std::string name;
    if (!units.ok || row >= units.rows || !text_at(b, cell_at(units, row, 0), &name)) return 0;
    if (name.compare(0, 8, "i_techn:") && name.compare(0, 8, "i_human:")) return 0;
    size_t us = name.rfind('_');
    if (us == std::string::npos || us + 1 >= name.size()) return 0;
    return (unsigned)strtoul(name.c_str() + us + 1, 0, 16);
}

inline unsigned squad_id(const std::vector<byte> &b, const Table &squads, unsigned row)
{
    return squads.ok && row < squads.rows ? u32_at(b, cell_at(squads, row, 1)) : 0;
}

// The side and command-tag columns of `squads`, worked out the way image.h does.
struct Columns { int side = -1, tag = -1; };

inline Columns columns(const std::vector<byte> &b)
{
    Columns c;
    image::SquadColumns sc = image::squad_columns(b, image::find(b, "squads"));
    c.side = sc.side;
    c.tag = sc.tag;
    return c;
}

struct Counts { unsigned squads = 0, units = 0; bool ok = false; };

// What this description's own player commands: the squads tagged 'USER' and their units.
inline Counts user_force(const std::vector<byte> &b)
{
    Counts out;
    Table s = table(b, "squads"), u = table(b, "units");
    Columns c = columns(b);
    if (!s.ok || !u.ok || c.tag < 0) return out;
    std::unordered_set<unsigned> ids;
    for (unsigned r = 0; r < s.rows; r++)
        if (u32_at(b, cell_at(s, r, (unsigned)c.tag)) == image::k_user) {
            unsigned id = squad_id(b, s, r);
            if (id) ids.insert(id);
            out.squads++;
        }
    for (unsigned r = 0; r < u.rows; r++) {
        unsigned id = unit_id(b, u, r);
        if (id && ids.count(id & ~0x1fu)) out.units++;
    }
    out.ok = true;
    return out;
}

// ---------------------------------------------------------------- writing rows
// A row that is being written: its cells as they will go into the file, plus the text cells whose text has
// to be copied in as well (those of the other player's rows).
struct Pending { size_t at; std::string text; };

// Build a table body out of `rows` of `src` (table `t`): the cells are copied, `patch_side_col` and
// `patch_tag_col` (when given) are overwritten, and text is either kept as it is (`keep_text`: it already
// lives in the file being written) or collected so the caller can copy it in.
inline bool build_rows(const std::vector<byte> &src, const Table &t, const std::vector<unsigned> &rows, unsigned cols,
                       bool keep_text, int patch_side_col, unsigned side, int patch_tag_col, unsigned tag,
                       std::vector<byte> *body, std::vector<Pending> *pending, std::string *why)
{
    if (!t.ok || t.cols != cols) { *why = "a table's width is not the one this file has"; return false; }
    for (unsigned r : rows) {
        if (r >= t.rows) { *why = "a row is past the end of the table"; return false; }
        size_t row_start = body->size();
        for (unsigned col = 0; col < cols; col++) {
            size_t cell = cell_at(t, r, col);
            if (cell + 0x20 > src.size()) { *why = "a cell is past the end of the file"; return false; }
            unsigned kind = u32_at(src, cell + k_kind_offset);
            size_t at = body->size();
            body->insert(body->end(), src.begin() + cell, src.begin() + cell + 0x20);
            if (keep_text) continue;                     // this game's own row: its offsets stay valid as they are
            if (kind == k_text) {
                std::string text;
                if (!text_at(src, cell, &text)) { *why = "a text cell does not point at text"; return false; }
                put_u32(*body, at, (unsigned)text.size());
                Pending p;
                p.at = at;
                p.text = text;
                pending->push_back(p);
            } else if (kind == k_numbers && (u32_at(src, cell) || u32_at(src, cell + 4))) {
                *why = "a row carries a number list";
                return false;
            }
        }
        if (patch_side_col >= 0) put_u32(*body, row_start + (size_t)patch_side_col * 0x20, side);
        if (patch_tag_col >= 0) put_u32(*body, row_start + (size_t)patch_tag_col * 0x20, tag);
    }
    return true;
}

// Write a table: `body` at the end of `dst`, the pending text after it, then point the node at it.
inline bool write_table(std::vector<byte> &dst, const Table &node, const std::vector<byte> &body,
                        std::vector<Pending> pending)
{
    if (!node.ok) return false;
    size_t base = dst.size();
    dst.insert(dst.end(), body.begin(), body.end());
    for (Pending &p : pending) {
        size_t at = dst.size();
        dst.insert(dst.end(), p.text.begin(), p.text.end());
        dst.push_back(0);
        put_u32(dst, base + p.at + 4, (unsigned)at);
    }
    unsigned rows = (unsigned)(body.size() / ((size_t)node.cols * 0x20));
    put_u32(dst, node.header + 0x28, rows);
    put_u32(dst, node.header + 0x30, (unsigned)base);
    return true;
}

// ---------------------------------------------------------------- the force itself
struct Result {
    bool ok = false;
    char note[200] = "";
    unsigned kept_squads = 0, kept_units = 0, added_squads = 0, added_units = 0;
    unsigned mine_side = 0, peer_side = 0;
};

// Everything the insert does, without writing anything: which rows stay and which come in.
struct Plan {
    std::vector<unsigned> keep_squads, keep_units, add_squads, add_units;
    unsigned mine_side = 0, peer_side = 0, other_side = 0, enemy_tag = 0;
    int side_col = -1, tag_col = -1;
    unsigned cols_s = 0, cols_u = 0;
};

inline bool plan(const std::vector<byte> &mine, const std::vector<byte> &peer, Plan *p, std::string *why)
{
    unsigned mu = 0, me = 0, pu = 0, pe = 0;
    if (!image::sides(mine, &mu, &me)) { *why = "this battle's description has no sides"; return false; }
    if (!image::sides(peer, &pu, &pe)) { *why = "the other player's force has no sides"; return false; }
    Table ms = table(mine, "squads"), mu_t = table(mine, "units");
    Table ps = table(peer, "squads"), pu_t = table(peer, "units");
    if (!ms.ok || !mu_t.ok) { *why = "this battle's description has no squads or no units"; return false; }
    if (!ps.ok || !pu_t.ok) { *why = "the other player's force has no squads or no units"; return false; }
    if (ms.cols != ps.cols || mu_t.cols != pu_t.cols) { *why = "the two descriptions lay their tables out differently"; return false; }
    Columns c = columns(mine), pc = columns(peer);
    if (c.side < 0 || c.tag < 0) { *why = "this battle's squads table has no side or command column"; return false; }
    if (pc.side < 0 || pc.tag < 0) { *why = "the other player's squads table has no side or command column"; return false; }
    if (pc.side != c.side || pc.tag != c.tag) { *why = "the two descriptions keep their side and command columns in different places"; return false; }

    p->mine_side = mu;
    p->other_side = me;
    p->peer_side = pu;                                  // the side his own force is on, in his description
    p->side_col = c.side;
    p->tag_col = c.tag;
    p->cols_s = ms.cols;
    p->cols_u = mu_t.cols;

    // The tag the computer's side carries here (what the other player's squads become).
    for (unsigned r = 0; r < ms.rows && !p->enemy_tag; r++)
        if (u32_at(mine, cell_at(ms, r, (unsigned)c.side)) == me) p->enemy_tag = u32_at(mine, cell_at(ms, r, (unsigned)c.tag));
    if (!p->enemy_tag) p->enemy_tag = image::k_enem;

    std::unordered_set<unsigned> kept, mine_ids;
    for (unsigned r = 0; r < ms.rows; r++) {
        unsigned side = u32_at(mine, cell_at(ms, r, (unsigned)c.side)), id = squad_id(mine, ms, r);
        if (side == me) continue;                        // the side the other player commands: replaced
        p->keep_squads.push_back(r);
        if (id) { kept.insert(id); mine_ids.insert(id); }
    }
    for (unsigned r = 0; r < mu_t.rows; r++) {
        unsigned id = unit_id(mine, mu_t, r);
        if (id && kept.count(id & ~0x1fu)) p->keep_units.push_back(r);
    }

    std::unordered_set<unsigned> added;
    for (unsigned r = 0; r < ps.rows; r++) {
        if (u32_at(peer, cell_at(ps, r, (unsigned)pc.side)) != pu) continue;
        unsigned id = squad_id(peer, ps, r);
        if (id && mine_ids.count(id)) {
            char text[120];
            sprintf_s(text, "the two forces use the same squad number %u", id);
            *why = text;
            return false;
        }
        p->add_squads.push_back(r);
        if (id) added.insert(id);
    }
    for (unsigned r = 0; r < pu_t.rows; r++) {
        unsigned id = unit_id(peer, pu_t, r);
        if (id && added.count(id & ~0x1fu)) p->add_units.push_back(r);
    }
    if (p->add_squads.empty()) { *why = "the other player's force has no squads of the side he commands"; return false; }
    if (p->add_units.empty()) { *why = "the other player's force has no units"; return false; }
    return true;
}

// Parse the result back and check that it is the battle it was plus the other player's force: the node
// table is untouched, both tables have the expected row counts, every text cell resolves, every unit's
// squad is there.
inline bool check(const std::vector<byte> &was, const std::vector<byte> &now, const Plan &p, std::string *why)
{
    if (now.size() < was.size()) { *why = "the file got shorter"; return false; }
    // Everything that was in the file has to be where it was, except the two node headers this wrote to.
    Table ws = table(was, "squads"), wu = table(was, "units");
    for (size_t i = 0; i < was.size(); i++) {
        bool in_header = (i >= ws.header + 0x28 && i < ws.header + 0x34) || (i >= wu.header + 0x28 && i < wu.header + 0x34);
        if (!in_header && now[i] != was[i]) { *why = "bytes that were there have changed"; return false; }
    }
    Table s = table(now, "squads"), u = table(now, "units");
    if (!s.ok || !u.ok) { *why = "the rewritten file has no squads or no units"; return false; }
    if (s.rows != p.keep_squads.size() + p.add_squads.size()) { *why = "the squads table did not take the force"; return false; }
    if (u.rows != p.keep_units.size() + p.add_units.size()) { *why = "the units table did not take the force"; return false; }
    std::unordered_set<unsigned> squads;
    for (unsigned r = 0; r < s.rows; r++) {
        unsigned id = squad_id(now, s, r);
        if (!id) { *why = "a squad row has no number"; return false; }
        squads.insert(id);
        for (unsigned col = 0; col < s.cols; col++)
            if (u32_at(now, cell_at(s, r, col) + k_kind_offset) == k_text) {
                std::string text;
                if (!text_at(now, cell_at(s, r, col), &text)) { *why = "a squad's text does not read back"; return false; }
            }
    }
    unsigned orphans = 0;
    for (unsigned r = 0; r < u.rows; r++) {
        unsigned id = unit_id(now, u, r);
        if (!id) { *why = "a unit row has no number"; return false; }
        if (!squads.count(id & ~0x1fu)) orphans++;
    }
    if (orphans) { *why = "the units table has units of squads that are not in it"; return false; }
    unsigned user = 0, enemy = 0;
    if (!image::sides(now, &user, &enemy)) { *why = "the rewritten file has no sides"; return false; }
    return true;
}

// The whole operation: this description keeps its own side and takes the other player's squads and units
// for the side he commands. `mine` is modified in place only when everything checks out.
inline Result insert(std::vector<byte> &mine, const std::vector<byte> &peer)
{
    Result r;
    Plan p;
    std::string why;
    if (!plan(mine, peer, &p, &why)) { strcpy_s(r.note, sizeof r.note, why.c_str()); return r; }

    Table ms = table(mine, "squads"), mu = table(mine, "units");
    Table ps = table(peer, "squads"), pu = table(peer, "units");
    std::vector<byte> squads, units;
    std::vector<Pending> squads_text, units_text;

    // this game's rows: text stays where it is; the side and tag columns are written as they are
    if (!build_rows(mine, ms, p.keep_squads, ms.cols, true, -1, 0, -1, 0, &squads, &squads_text, &why) ||
        !build_rows(mine, mu, p.keep_units, mu.cols, true, -1, 0, -1, 0, &units, &units_text, &why)) {
        strcpy_s(r.note, sizeof r.note, why.c_str());
        return r;
    }
    // the other player's rows: text is copied in, his side and 'USER' tag become this battle's other side
    if (!build_rows(peer, ps, p.add_squads, ps.cols, false, p.side_col, p.other_side, p.tag_col, p.enemy_tag,
                    &squads, &squads_text, &why) ||
        !build_rows(peer, pu, p.add_units, pu.cols, false, -1, 0, -1, 0, &units, &units_text, &why)) {
        strcpy_s(r.note, sizeof r.note, why.c_str());
        return r;
    }

    std::vector<byte> out = mine;
    if (!write_table(out, ms, squads, squads_text) || !write_table(out, mu, units, units_text)) {
        strcpy_s(r.note, sizeof r.note, "the tables could not be written");
        return r;
    }
    if (!check(mine, out, p, &why)) { strcpy_s(r.note, sizeof r.note, why.c_str()); return r; }

    mine.swap(out);
    r.ok = true;
    r.kept_squads = (unsigned)p.keep_squads.size();
    r.kept_units = (unsigned)p.keep_units.size();
    r.added_squads = (unsigned)p.add_squads.size();
    r.added_units = (unsigned)p.add_units.size();
    r.mine_side = p.mine_side;
    r.peer_side = p.peer_side;
    r.note[0] = 0;
    return r;
}

// Is this file a battle description this code can work with? (header, node table, a units and a squads
// table). Used before anything is sent or applied, so a wrong file costs a log line, not a battle.
inline bool is_description(const std::vector<byte> &b)
{
    if (b.size() < 0x100 || memcmp(b.data(), "AZCF", 4)) return false;
    unsigned count = u32_at(b, 4), table_at = u32_at(b, 8);
    if (table_at < 0x40 || table_at > b.size() || count > 100000) return false;
    if (table_at + (size_t)count * 0x80 > b.size()) return false;
    return table(b, "squads").ok && table(b, "units").ok;
}

} // namespace force

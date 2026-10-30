// The force exchange's own test: no game, no network, just the bytes of a battle description.
//
// It builds two small descriptions - one for the host, one for the joining player - and checks that putting
// the joining player's force into the host's battle keeps both forces, their numbers and their text; that a
// file which is not a battle description, a row carrying a number list or two forces sharing a squad number
// are refused instead of written; and that copying a real description's table through the same code gives
// the same bytes back (development/unified-kit-source/files/root/programs/*.progpack are such files).
//
// Build: build.bat selftest   Run: build\selftest\force_test.exe
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include "image.h"
#include "force.h"

typedef unsigned char byte;
static int g_failed, g_checks;

static void expect(bool ok, const char *what)
{
    g_checks++;
    if (!ok) { g_failed++; printf("FAIL %s\n", what); }
}

static void expect_eq(long got, long want, const char *what)
{
    g_checks++;
    if (got != want) { g_failed++; printf("FAIL %-52s got %ld, want %ld\n", what, got, want); }
}

// ---------------------------------------------------------------- a minimal CFG writer
struct Cell {
    unsigned kind = 4, value = 0;
    std::string text;
};

struct Node {
    std::string name, sig;
    unsigned type = 2, cols = 1;
    std::vector<std::string> fields;      // type 1: one name per field (row)
    std::vector<Cell> cells;              // type 2: rows * cols
    unsigned rows() const { return type == 1 ? (unsigned)fields.size() : cols ? (unsigned)(cells.size() / cols) : 0; }
};

static size_t align40(size_t n) { return (n + 0x3f) & ~(size_t)0x3f; }

static std::vector<byte> build(std::vector<Node> &nodes)
{
    size_t table = 0x40, data = align40(table + nodes.size() * 0x80);
    std::vector<size_t> at(nodes.size());
    for (size_t i = 0; i < nodes.size(); i++) {
        at[i] = data;
        data += align40((size_t)nodes[i].rows() * (nodes[i].type == 1 ? 0x40 : (size_t)nodes[i].cols * 0x20));
    }
    std::vector<byte> b(data, 0);
    memcpy(b.data(), "AZCF", 4);
    unsigned table_off = (unsigned)table, count = (unsigned)nodes.size();
    memcpy(b.data() + 4, &count, 4);
    memcpy(b.data() + 8, &table_off, 4);
    size_t strings = data;
    auto put_text = [&](const std::string &s, size_t cell_at, unsigned size_for) {
        // the cell's first word is the length, the second the offset; the text goes to the end of the file
        unsigned off = (unsigned)b.size(), len = (unsigned)s.size();
        b.insert(b.end(), s.begin(), s.end());
        b.push_back(0);
        memcpy(b.data() + cell_at, &len, 4);
        memcpy(b.data() + cell_at + 4, &off, 4);
        unsigned kind = 2;
        memcpy(b.data() + cell_at + 16, &kind, 4);
        (void)size_for;
    };
    (void)strings;
    for (size_t i = 0; i < nodes.size(); i++) {
        Node &n = nodes[i];
        size_t o = table + i * 0x80;
        memcpy(b.data() + o, n.name.c_str(), n.name.size() < 0x20 ? n.name.size() : 0x20);
        unsigned type = n.type, cols = n.cols, rows = n.rows(), sig_at = (unsigned)b.size(), data_at = (unsigned)at[i];
        memcpy(b.data() + o + 0x20, &type, 4);
        memcpy(b.data() + o + 0x24, &cols, 4);
        memcpy(b.data() + o + 0x28, &rows, 4);
        memcpy(b.data() + o + 0x2c, &sig_at, 4);
        memcpy(b.data() + o + 0x30, &data_at, 4);
        std::string sig = n.sig;
        if (sig.empty()) sig.assign(n.type == 1 ? n.rows() : n.cols, 'u');
        b.insert(b.end(), sig.begin(), sig.end());
        b.push_back(0);
        if (n.type == 1) {
            for (unsigned r = 0; r < n.rows(); r++) {
                size_t c = at[i] + (size_t)r * 0x40;
                unsigned v = n.cells.empty() ? 0 : n.cells[r].value;
                unsigned kind = n.cells.empty() ? 4 : n.cells[r].kind;
                memcpy(b.data() + c, &v, 4);
                memcpy(b.data() + c + 16, &kind, 4);
                memcpy(b.data() + c + 0x20, n.fields[r].c_str(), n.fields[r].size() < 0x20 ? n.fields[r].size() : 0x20);
            }
        } else {
            for (unsigned k = 0; k < n.rows() * n.cols; k++) {
                size_t c = at[i] + (size_t)k * 0x20;
                if (n.cells[k].kind == 2) { put_text(n.cells[k].text, c, 0); continue; }
                unsigned v = n.cells[k].value, kind = n.cells[k].kind;
                memcpy(b.data() + c, &v, 4);
                memcpy(b.data() + c + 16, &kind, 4);
            }
        }
    }
    return b;
}

// A description: the two side codes, its squads (id, side, tag) and its units (name).
struct SquadRow { unsigned id, side, tag; };
struct UnitRow { unsigned id; bool vehicle; std::string model; };

static std::vector<byte> description(unsigned user, unsigned enemy, const std::vector<SquadRow> &squads, const std::vector<UnitRow> &units)
{
    std::vector<Node> nodes(4);
    nodes[0].name = "i_gamespec:gamespec";
    nodes[0].type = 1;
    nodes[0].cols = 1;
    nodes[0].fields = { "user_side", "enemy_side" };
    nodes[0].cells = { { 4, user, "" }, { 4, enemy, "" } };

    nodes[1].name = "squads";
    nodes[1].type = 2;
    nodes[1].cols = 5;
    nodes[1].sig = "sssbb";
    for (const SquadRow &s : squads) {
        nodes[1].cells.push_back({ 4, 0, "" });
        nodes[1].cells.push_back({ 4, s.id, "" });
        nodes[1].cells.push_back({ 4, s.side, "" });
        nodes[1].cells.push_back({ 4, s.tag, "" });
        nodes[1].cells.push_back({ 2, 0, "line" });
    }

    nodes[2].name = "units";
    nodes[2].type = 2;
    nodes[2].cols = 3;
    nodes[2].sig = "sbs";
    for (const UnitRow &u : units) {
        char name[32];
        sprintf_s(name, "%s_%08x", u.vehicle ? "i_techn:tec" : "i_human:hum", u.id);
        nodes[2].cells.push_back({ 2, 0, name });
        nodes[2].cells.push_back({ 4, 0xBEEF, "" });
        nodes[2].cells.push_back({ 2, 0, u.model });
    }

    nodes[3].name = "pools";
    nodes[3].type = 2;
    nodes[3].cols = 3;
    nodes[3].cells.push_back({ 2, 0, "Germans" });
    nodes[3].cells.push_back({ 0, 0, "" });
    nodes[3].cells.push_back({ 6, 0, "" });
    nodes[3].cells.push_back({ 2, 0, "Soviets" });
    nodes[3].cells.push_back({ 0, 0, "" });
    nodes[3].cells.push_back({ 6, 0, "" });
    return build(nodes);
}

static const unsigned k_ger = 0x00524547, k_ussr = 0x52535355, k_user = 0x52455355, k_enem = 0x4d454e45;

static void test_insert()
{
    std::vector<byte> mine = description(k_ger, k_ussr, { { 0x100, k_ger, k_user }, { 0x200, k_ussr, k_enem } },
                                         { { 0x100, true, "pz3" }, { 0x101, false, "rifleman" }, { 0x200, true, "t34" } });
    std::vector<byte> peer = description(k_ussr, k_ger, { { 0x300, k_ussr, k_user }, { 0x400, k_ger, k_enem } },
                                         { { 0x300, true, "kv1" }, { 0x301, false, "soldier" }, { 0x400, true, "pz4" } });
    expect(force::is_description(mine), "the host's description is one");
    expect(force::is_description(peer), "the other player's description is one");
    force::Counts c = force::user_force(mine);
    expect_eq(c.squads, 1, "the host commands one squad");
    expect_eq(c.units, 2, "of two units");

    std::vector<byte> before = mine;
    force::Result r = force::insert(mine, peer);
    if (!r.ok) printf("  insert: %s\n", r.note);
    expect(r.ok, "the joining player's force goes into the battle");
    expect_eq(r.kept_squads, 1, "one squad stays");
    expect_eq(r.kept_units, 2, "two of its units stay");
    expect_eq(r.added_squads, 1, "one squad comes in");
    expect_eq(r.added_units, 2, "with two units");
    expect(mine.size() > before.size(), "the file grew");

    force::Table s = force::table(mine, "squads"), u = force::table(mine, "units");
    expect_eq(s.rows, 2, "two squads in the battle");
    expect_eq(u.rows, 4, "four units in the battle");
    force::Columns cols = force::columns(mine);
    int german = 0, soviet = 0, users = 0;
    for (unsigned row = 0; row < s.rows; row++) {
        unsigned side = force::u32_at(mine, force::cell_at(s, row, (unsigned)cols.side));
        unsigned tag = force::u32_at(mine, force::cell_at(s, row, (unsigned)cols.tag));
        if (side == k_ger) german++;
        if (side == k_ussr) soviet++;
        if (tag == k_user) users++;
    }
    expect_eq(german, 1, "one German squad (the host's)");
    expect_eq(soviet, 1, "one Soviet squad (the joining player's)");
    expect_eq(users, 1, "the host still commands exactly one squad");

    // every unit resolves to a squad that is in the table
    int orphans = 0, kept = 0, added = 0;
    for (unsigned row = 0; row < u.rows; row++) {
        unsigned id = force::unit_id(mine, u, row);
        bool found = false;
        for (unsigned k = 0; k < s.rows && !found; k++) found = force::squad_id(mine, s, k) == (id & ~0x1fu);
        if (!found) orphans++;
        if ((id & ~0x1fu) == 0x100) kept++;
        if ((id & ~0x1fu) == 0x300) added++;
    }
    expect_eq(orphans, 0, "no unit without its squad");
    expect_eq(kept, 2, "the host's two units are still there");
    expect_eq(added, 2, "the joining player's two units are there");
    // and the other player's text came across intact
    std::string model;
    for (unsigned row = 0; row < u.rows; row++)
        if (force::unit_id(mine, u, row) == 0x300) force::text_at(mine, force::cell_at(u, row, 2), &model);
    expect(model == "kv1", "the joining player's model name reads back");

    // the file still is what the game expects: node table untouched, sides readable
    expect(force::is_description(mine), "the result is still a description");
    unsigned side = 0, enemy = 0;
    expect(image::sides(mine, &side, &enemy) && side == k_ger && enemy == k_ussr, "the sides did not change");
}

// The second battle: the description still carries the last opponent's force on the other side. A new force
// must replace it, not add to it - and must never touch this player's own side.
static void test_second_battle()
{
    std::vector<byte> mine = description(k_ger, k_ussr, { { 0x100, k_ger, k_user } }, { { 0x100, true, "pz3" } });
    std::vector<byte> first = description(k_ussr, k_ger, { { 0x300, k_ussr, k_user } }, { { 0x300, true, "kv1" } });
    std::vector<byte> second = description(k_ussr, k_ger, { { 0x500, k_ussr, k_user } }, { { 0x500, true, "t70" } });
    expect(force::insert(mine, first).ok, "the first battle takes a force");
    std::vector<byte> was = mine;
    force::Result r = force::insert(mine, second);
    expect(r.ok, "the next battle takes the next force");
    expect_eq(r.added_squads, 1, "of one squad");
    force::Table s = force::table(mine, "squads"), u = force::table(mine, "units");
    expect_eq(s.rows, 2, "the squads table is not doubled");
    expect_eq(u.rows, 2, "nor is the units table");
    // the last opponent's squad is gone, the new one is there, this player's is untouched
    bool old = false, fresh = false, own = false;
    for (unsigned row = 0; row < s.rows; row++) {
        unsigned id = force::squad_id(mine, s, row);
        old |= id == 0x300;
        fresh |= id == 0x500;
        own |= id == 0x100;
    }
    expect(!old, "the previous opponent's squad is not still there");
    expect(fresh, "the new opponent's squad is there");
    expect(own, "this player's squad is there");
    force::Counts c = force::user_force(mine);
    expect_eq(c.squads, 1, "and this player still commands one squad");
    expect_eq(c.units, 1, "of one unit");
    expect(!force::is_description(was) || force::is_description(mine), "the battle is still readable");
}

static void test_refusals()
{
    std::vector<byte> mine = description(k_ger, k_ussr, { { 0x100, k_ger, k_user } }, { { 0x100, true, "pz3" } });

    // not a description at all
    std::vector<byte> junk(200, 0x41);
    force::Result r = force::insert(mine, junk);
    expect(!r.ok, "junk is refused");

    // a file that is a description but has no such tables
    std::vector<Node> nodes(1);
    nodes[0].name = "something";
    nodes[0].type = 2;
    nodes[0].cols = 1;
    nodes[0].cells = { { 4, 1, "" } };
    std::vector<byte> bare = build(nodes);
    expect(!force::insert(mine, bare).ok, "a file without squads or units is refused");

    // the same squad number on both sides
    std::vector<byte> clash = description(k_ussr, k_ger, { { 0x100, k_ussr, k_user } }, { { 0x100, true, "kv1" } });
    std::vector<byte> was = mine;
    r = force::insert(mine, clash);
    expect(!r.ok, "two forces sharing a squad number are refused");
    expect(mine == was, "and the battle is left untouched");

    // a row that carries a number list: not guessed at
    std::vector<byte> listed = description(k_ussr, k_ger, { { 0x300, k_ussr, k_user } }, { { 0x300, true, "kv1" } });
    force::Table u = force::table(listed, "units");
    // give the first unit a list cell that points at the file's start
    force::put_u32(listed, force::cell_at(u, 0, 1), 4);
    force::put_u32(listed, force::cell_at(u, 0, 1) + 4, 0x40);
    force::put_u32(listed, force::cell_at(u, 0, 1) + 16, force::k_numbers);
    r = force::insert(mine, listed);
    expect(!r.ok, "a row with a number list is refused");
}

static void test_copy_is_exact(const char *path)
{
    FILE *f = 0;
    if (fopen_s(&f, path, "rb") || !f) { printf("  (no %s, skipped)\n", path); return; }
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    std::vector<byte> b((size_t)(size > 0 ? size : 0));
    if (!b.empty() && fread(b.data(), 1, b.size(), f) != b.size()) b.clear();
    fclose(f);
    if (b.size() < 0x100) { printf("  (%s is not readable, skipped)\n", path); return; }
    expect(!force::is_description(b), "a program's CFG is not a battle description");
    force::Table t = force::table(b, "g_main");
    expect(t.ok, "g_main is a table");
    if (!t.ok) return;
    std::vector<unsigned> rows;
    for (unsigned r = 0; r < t.rows; r++) rows.push_back(r);
    std::vector<byte> body;
    std::vector<force::Pending> pending;
    std::string why;
    expect(force::build_rows(b, t, rows, t.cols, true, -1, 0, -1, 0, &body, &pending, &why), "its rows copy");
    expect_eq((long)pending.size(), 0, "with no text to move");
    expect_eq((long)body.size(), (long)((size_t)t.rows * t.cols * 0x20), "of the table's size");
    expect(!body.empty() && !memcmp(body.data(), b.data() + t.data, body.size()), "and byte for byte the same");
}

int main()
{
    test_insert();
    test_second_battle();
    test_refusals();
    test_copy_is_exact("development/unified-kit-source/files/root/programs/tacgame.progpack");
    test_copy_is_exact("development/unified-kit-source/files/root/programs/opgame.progpack");
    printf("force selftest: %d checks, %d failed\n", g_checks, g_failed);
    return g_failed ? 1 : 0;
}

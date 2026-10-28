// The game-side mirror changes command tags; applying it twice need not reproduce the host bytes.
#if !defined(_WIN32)
#include <stdio.h>
#include <errno.h>
static int fopen_s(FILE **f, const char *p, const char *m) { *f = fopen(p, m); return *f ? 0 : errno; }
// This fixture never uses the legacy text-sidecar reader.
static int fscanf_s(FILE *, const char *, ...) { return EOF; }
#endif
#include "image.h"
#include "battle_identity.h"
#include "opsave_fixture.h"
#include "protocol.h"
#include <stdio.h>

static int checks, failed;
static void check(bool ok, const char *what) {
    ++checks;
    if (!ok) { ++failed; printf("FAIL: %s\n", what); }
}
int main() {
    const unsigned ger = 0x00524547, ussr = 0x52535355, user = 0x52455355;
    const unsigned enem = 0x4d454e45, allied_formation = 0x314e4535;
    opsave_fixture::Node game; game.name = "i_gamespec:gamespec";
    game.cells = {opsave_fixture::num("user_side", ger), opsave_fixture::num("enemy_side", ussr)};
    opsave_fixture::Node squads; squads.name = "squads"; squads.type = 2; squads.cols = 4;
    squads.cells = {opsave_fixture::num("", 0), opsave_fixture::num("", 0x100), opsave_fixture::num("", ger), opsave_fixture::num("", user),
                     opsave_fixture::num("", 0), opsave_fixture::num("", 0x200), opsave_fixture::num("", ger), opsave_fixture::num("", allied_formation),
                     opsave_fixture::num("", 0), opsave_fixture::num("", 0x300), opsave_fixture::num("", ussr), opsave_fixture::num("", enem)};
    auto original = opsave_fixture::build({game, squads});
    check(opsave_checked::validate(original), "synthetic host image is structurally valid");
    auto mirrored = original;
    check(image::mirror(mirrored), "host image can be mirrored for guest");
    auto guest_owners = image::squad_owners(mirrored);
    check(guest_owners[0x100] == enem && guest_owners[0x200] == allied_formation && guest_owners[0x300] == user,
          "guest perspective changes command tags as intended");
    auto restored = mirrored;
    check(image::mirror(restored), "guest image can be mirrored a second time");
    auto restored_owners = image::squad_owners(restored);
    check(restored_owners[0x200] == user, "second mirror cannot recover the original allied formation tag");
    unsigned host_hash = fnv(original.data(), original.size());
    check(original != restored && host_hash != fnv(restored.data(), restored.size()),
          "reconstructed host hash can differ without a changed input file");

    char ticket[battle_identity::k_text_size];
    battle_identity::format(host_hash, battle_identity::fingerprint(mirrored.data(), mirrored.size()), ticket);
    unsigned recovered = 0;
    check(battle_identity::resolve(ticket, mirrored.data(), mirrored.size(), &recovered) && recovered == host_hash,
          "prepared image resolves to the identity of the verified host bytes");
    auto changed = mirrored; changed.back() ^= 1;
    recovered = 0xfeedbeef;
    check(!battle_identity::resolve(ticket, changed.data(), changed.size(), &recovered) && recovered == 0xfeedbeef,
          "stale identity cannot be used with different prepared bytes");
    check(!battle_identity::resolve("", mirrored.data(), mirrored.size(), &recovered) &&
          !battle_identity::resolve("01234567", mirrored.data(), mirrored.size(), &recovered) &&
          !battle_identity::resolve("01234567:000000000000000g", mirrored.data(), mirrored.size(), &recovered) &&
          !battle_identity::resolve("01234567:0000000000000000extra", mirrored.data(), mirrored.size(), &recovered),
          "empty, truncated, malformed and trailing identity values are rejected");
    check(!battle_identity::resolve(ticket, nullptr, 0, &recovered) &&
          !battle_identity::resolve(ticket, mirrored.data(), mirrored.size(), nullptr),
          "missing image and destination are rejected");
    check(battle_identity::fingerprint((const unsigned char *)"hello", 5) == UINT64_C(0xa430d84680aabd0b),
          "prepared-image fingerprint is standard FNV-1a-64");

    // A structurally valid synthetic file can contain more than one numeric column with USER.
    // There is no ground-truth engine schema in this fixture: it demonstrates ambiguity in the
    // heuristic, not that a real game uses either column or that this caused the reported symptom.
    opsave_fixture::Node ambiguous; ambiguous.name = "squads"; ambiguous.type = 2; ambiguous.cols = 5;
    ambiguous.cells = {opsave_fixture::num("", 0), opsave_fixture::num("", 0x100), opsave_fixture::num("", ger),
                       opsave_fixture::num("", enem), opsave_fixture::num("", user),
                       opsave_fixture::num("", 0), opsave_fixture::num("", 0x300), opsave_fixture::num("", ussr),
                       opsave_fixture::num("", user), opsave_fixture::num("", enem),
                       opsave_fixture::num("", 0), opsave_fixture::num("", 0x400), opsave_fixture::num("", ussr),
                       opsave_fixture::num("", enem), opsave_fixture::num("", enem)};
    opsave_fixture::Node units; units.name = "units"; units.type = 2; units.cols = 1;
    units.cells = {opsave_fixture::str("i_human:hum_00000100"), opsave_fixture::str("i_human:hum_00000300")};
    auto ambiguous_image = opsave_fixture::build({game, ambiguous, units});
    image::Node squad_table = image::find(ambiguous_image, "squads");
    image::SquadColumns columns = image::squad_columns(ambiguous_image, squad_table);
    check(opsave_checked::validate(ambiguous_image) && columns.side == 2 && columns.tag == 3,
          "characterization: first USER-bearing column wins even with a second candidate");
    auto ambiguous_owned = image::units_of(ambiguous_image, user);
    check(ambiguous_owned.count((2ull << 32) | 0x300) && !ambiguous_owned.count((2ull << 32) | 0x100),
          "characterization: that choice determines the network-owned set");
    check(image::mirror(ambiguous_image) &&
          image::u32_at(ambiguous_image.data(), image::cell(squad_table, 0, 4)) == user &&
          image::u32_at(ambiguous_image.data(), image::cell(squad_table, 1, 4)) == enem,
          "characterization: mirror edits the chosen column, leaving the other candidate untouched");

    opsave_fixture::Node lone; lone.name = "squads"; lone.type = 2; lone.cols = 4;
    lone.cells = {opsave_fixture::num("", 0), opsave_fixture::num("", 0x100),
                  opsave_fixture::num("", ger), opsave_fixture::num("", user)};
    auto cannot_mirror = opsave_fixture::build({game, lone});
    auto before = cannot_mirror;
    check(!image::mirror(cannot_mirror) && cannot_mirror == before,
          "a mirror with no convertible enemy fails without modifying any input bytes");
    printf("image mirror identity selftest: %d checks, %d failed\n", checks, failed);
    return failed ? 1 : 0;
}

// Portable checks for the experimental hit and human-animation wire envelopes.
#include "hit_wire.h"
#include "anim_wire.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <limits>

static int checks, failed;
static void expect(bool ok, const char *what) { ++checks; if (!ok) { ++failed; printf("FAIL %s\n", what); } }
static void put32(unsigned char *p, uint32_t x) { memcpy(p, &x, 4); }
static void putf(unsigned char *p, float x) { memcpy(p, &x, 4); }

int main()
{
    unsigned char record[hit_wire::engine_size];
    for (unsigned i = 0; i < sizeof record; ++i) record[i] = (unsigned char)(i ^ 0x5a);
    put32(record + 0x60, 0xffffffffu); // no component ID in this first envelope
    ShotRec h;
    hit_wire::encode(h, false, (2ull << 32) | 17, (1ull << 32) | 3, (2ull << 32) | 4,
                     33, 44, 55, 66, true, 77, record);
    expect(hit_wire::valid(h), "valid human hit");
    expect(!memcmp(h.launch, record, hit_wire::copied), "hit scalar prefix retained");
    put32(record + 0x60, 0x12345678u);
    hit_wire::encode(h, false, (2ull << 32) | 17, (1ull << 32) | 3, (2ull << 32) | 4,
                     33, 44, 55, 66, true, 77, record);
    uint32_t human_part = 0; memcpy(&human_part, h.launch + 0x60, 4);
    expect(hit_wire::valid(h) && human_part == 0xffffffffu, "human process-local component is omitted");
    ShotRec unsafe_human = h; put32(unsafe_human.launch + 0x60, 0x12345678u);
    expect(!hit_wire::valid(unsafe_human), "human envelope cannot smuggle a component handle");
    put32(record + 0x60, 0xffffffffu);
    bool zero_tail = true; for (size_t i = hit_wire::copied; i < sizeof h.launch; ++i) zero_tail &= h.launch[i] == 0;
    expect(zero_tail && h.ammo == 33 && h.weapon == 44 && h.arg6 == 55 && h.ammo_code == 66 &&
           h.desc3 == 77 && h.has_pos == 1, "hit pointer tail omitted, metadata retained");
    ShotRec bad = h; bad.shooter = 0; expect(!hit_wire::valid(bad), "missing target rejected");
    bad = h; bad.flags = 1; expect(!hit_wire::valid(bad), "vehicle flag does not match human key");
    bad = h; bad.obj2 = (3ull << 32) | 9; expect(!hit_wire::valid(bad), "invalid source key rejected");
    bad = h; bad.obj5 = (4ull << 32) | 2; expect(!hit_wire::valid(bad), "invalid object key rejected");
    bad = h; bad.has_pos = 2; expect(!hit_wire::valid(bad), "invalid final boolean rejected");
    bad = h; bad.pad = 1; expect(!hit_wire::valid(bad), "unknown hit extension rejected");
    bad = h; bad.launch[hit_wire::copied] = 1; expect(!hit_wire::valid(bad), "hit pointer region must remain blank");
    bad = h; bad.pos[0] = 1; expect(!hit_wire::valid(bad), "unused hit field rejected");
    hit_wire::encode(h, true, (1ull << 32), 0, 0, 0xffffffffu, 0xffffffffu, 0, 0, false, 0, record);
    expect(hit_wire::valid(h), "vehicle and nullable references accepted");
    unsigned char remote_parts[4 * 0x60] = {}, local_parts[4 * 0x60] = {};
    const uint32_t remote_high[4] = {10,10,20,10}, local_high[4] = {100,100,300,100};
    for (uint32_t i = 0; i < 4; ++i) {
        put32(remote_parts + i * 0x60 + 0x48, (remote_high[i] << 10) | i);
        put32(local_parts + i * 0x60 + 0x48, (local_high[i] << 10) | i);
    }
    uint32_t struck = (20u << 10) | 777, mapped = 0;
    hit_wire::PartHint hint = {};
    expect(hit_wire::make_hint(remote_parts, 4, struck, &hint) && hint.ordinal == 2, "sender identifies first row in the struck group");
    expect(hit_wire::translate(local_parts, 4, struck, hint, &mapped) && mapped == ((300u << 10) | 777),
           "component group translated and lower ten bits retained");
    put32(local_parts + 0x60 + 0x48, (100u << 10) | 7);
    expect(!hit_wire::translate(local_parts, 4, struck, hint, &mapped), "different component low bits refused");
    put32(local_parts + 0x60 + 0x48, (200u << 10) | 1);
    expect(!hit_wire::translate(local_parts, 4, struck, hint, &mapped), "different component grouping refused");
    expect(!hit_wire::make_hint(remote_parts, 4, (99u << 10), &hint), "unidentified sender component refused");
    expect(!hit_wire::shape(remote_parts, hit_wire::max_parts + 1, &hint.shape), "oversized component list refused");
    expect(hit_wire::make_hint(remote_parts, 4, struck, &hint), "component hint rebuilt");
    put32(record + 0x60, struck);
    hit_wire::encode(h, true, (1ull << 32), 0, 0, 0xffffffffu, 0xffffffffu, 0, 0, false, 0, record);
    expect(!hit_wire::valid(h), "component hit without hint refused");
    hit_wire::put_hint(h, hint);
    expect(hit_wire::valid(h), "component hit with checked hint accepted");
    bad = h; bad.pad = 0; expect(!hit_wire::valid(bad), "component hint requires its flag");
    bad = h; hint.ordinal = hint.count; hit_wire::put_hint(bad, hint);
    expect(!hit_wire::valid(bad), "out-of-range component ordinal refused");

    unsigned char instance[anim_wire::instance_size] = {}, human[0x1f00] = {};
    put32(instance + 0xa8, 11); put32(instance + 0x104, 1); put32(instance + 0x10c, 3);
    put32(instance + 0x100, 22); put32(instance + 0x114, 33);
    putf(instance + 0xd4, 0.5f); putf(instance + 0x11c, 1.f); putf(instance + 0x120, 2.f);
    putf(instance + 0x124, 3.f); putf(instance + 0x128, 4.f); putf(instance + 0x12c, 5.f);
    instance[0x136] = 1; instance[0x139] = 1;
    put32(human + 0x1e70, 2); putf(human + 0x1210, 6.f); putf(human + 0x1214, 7.f);
    putf(human + 0x1218, 8.f); putf(human + 0x1eac, 9.f); putf(human + 0x1ef8, 10.f);
    ShotRec a;
    anim_wire::encode(a, (2ull << 32) | 123, instance, "walk", human);
    expect(anim_wire::valid(a), "valid animation snapshot");
    uint32_t n = 0; memcpy(&n, a.launch, 4); expect(n == 123 && a.launch[50] == 1 && !strcmp((char *)a.launch + 52, "walk"), "identity and group encoded");
    memcpy(&n, a.launch + 4, 4); expect(n == 11, "animation scalar encoded");
    float f = 0; memcpy(&f, a.launch + 100, 4); expect(f == 10.f, "human animation extra encoded");
    expect(a.launch[104] == 0 && a.obj2 == 0 && a.obj5 == 0, "animation pointers never encoded");
    bad = a; bad.shooter = (1ull << 32) | 123; expect(!anim_wire::valid(bad), "vehicle animation key rejected");
    bad = a; bad.launch[0] ^= 1; expect(!anim_wire::valid(bad), "animation identity mismatch rejected");
    bad = a; bad.launch[51] = 1; expect(!anim_wire::valid(bad), "unknown animation extension rejected");
    bad = a; put32(bad.launch + 8, 2); expect(!anim_wire::valid(bad), "invalid active flag rejected");
    bad = a; bad.launch[50] = 2; expect(!anim_wire::valid(bad), "invalid group flag rejected");
    bad = a; bad.launch[52] = 1; expect(!anim_wire::valid(bad), "non-printable group rejected");
    bad = a; memset(bad.launch + 52, 'a', 28); expect(!anim_wire::valid(bad), "unterminated group rejected");
    bad = a; bad.launch[60] = 'x'; expect(!anim_wire::valid(bad), "bytes after group terminator rejected");
    bad = a; putf(bad.launch + 24, std::numeric_limits<float>::quiet_NaN()); expect(!anim_wire::valid(bad), "NaN rejected");
    bad = a; putf(bad.launch + 88, std::numeric_limits<float>::infinity()); expect(!anim_wire::valid(bad), "infinity rejected");
    bad = a; putf(bad.launch + 96, 1000001.f); expect(!anim_wire::valid(bad), "implausible animation value rejected");
    bad = a; bad.launch[104] = 1; expect(!anim_wire::valid(bad), "animation pointer tail rejected");
    anim_wire::encode(a, (2ull << 32), instance, nullptr, human);
    expect(anim_wire::valid(a) && !a.launch[50], "missing group represented without a pointer");
    bad = a; bad.launch[52] = 'x'; expect(!anim_wire::valid(bad), "group bytes require group flag");
    printf("hit/animation wire selftest: %d checks, %d failed\n", checks, failed);
    return failed ? 1 : 0;
}

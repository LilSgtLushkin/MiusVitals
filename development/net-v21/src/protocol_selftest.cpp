// Self-test of the wire format: the sizes both games agree on without ever saying so.
//
// The protocol is a packed struct in protocol.h, read by casting a datagram.  A field added, a pack
// pragma lost, a chunk size changed - none of that fails to compile, and all of it makes two games
// that "run the same version" disagree about what a packet means. Protocol 15 intentionally
// differs from v20: map fragments carry an epoch and revision; tactical record sizes are unchanged.
//
// Build: build.bat selftest   Run: build\selftest\protocol_selftest.exe
#include <stdio.h>
#include <string.h>
#include "protocol.h"
#include "image_chunks.h"
#include "spectator_policy.h"

static int g_failed, g_checks;

static void expect(long got, long want, const char *what)
{
    g_checks++;
    if (got != want) {
        g_failed++;
        printf("FAIL %-46s got %ld, want %ld\n", what, got, want);
    }
}

// What the map transfer does on the receiving side (session.cpp): a piece is taken once, a duplicate is
// ignored, and the blob is complete when every piece has arrived.
struct Assembly
{
    unsigned pieces, got;
    bool have[4096];
    explicit Assembly(unsigned size) : pieces((size + k_chunk - 1) / k_chunk), got(0)
    {
        memset(have, 0, sizeof have);
    }
    bool take(unsigned index, unsigned size)
    {
        if (index >= pieces || have[index] || size > k_chunk) return false;
        if ((size_t)index * k_chunk + size > (size_t)pieces * k_chunk) return false;
        have[index] = true;
        got++;
        return true;
    }
    bool complete() const { return got == pieces; }
};

int main()
{
    expect(sizeof(Head), 8, "Head");
    expect(sizeof(Hello), 84, "Hello (forces and map revision)");
    expect(sizeof(Welcome), 192, "Welcome");
    expect(sizeof(ImageRequest), 20, "ImageRequest");
    expect(sizeof(ImageData), 24 + k_chunk, "ImageData");
    expect(sizeof(StateHead), 28, "StateHead");
    expect(sizeof(EndState), 16, "EndState");
    expect(sizeof(UnitRec), 80, "UnitRec");
    expect(sizeof(ObjectFrag), 36, "ObjectFrag");
    expect(sizeof(ObjectAck), 20, "ObjectAck");
    expect(sizeof(ShotHead) + 5 * sizeof(ShotRec), 1112, "a five-shot datagram as the logs show it");
    expect(sizeof(StateHead) + 14 * sizeof(UnitRec), 1148, "a fourteen-unit state datagram");
    expect(k_chunk, 1000, "image piece size");

    Welcome welcome = {};
    welcome.h = make_head(PKT_WELCOME);
    memcpy(welcome.proto, k_protocol, sizeof k_protocol);
    welcome.version = k_version;
    expect(valid_welcome(&welcome, sizeof welcome), 1, "matching welcome is accepted");
    expect(valid_welcome(&welcome, sizeof welcome - 1), 0, "short welcome is rejected");
    expect(valid_welcome(nullptr, sizeof welcome), 0, "null welcome is rejected");
    welcome.version++;
    expect(valid_welcome(&welcome, sizeof welcome), 0, "other protocol version is rejected");
    welcome.version = k_version;
    welcome.proto[0] ^= 1;
    expect(valid_welcome(&welcome, sizeof welcome), 0, "other protocol id is rejected");
    welcome.proto[0] ^= 1;
    welcome.h.type = PKT_HELLO;
    expect(valid_welcome(&welcome, sizeof welcome), 0, "wrong packet type is rejected");
    welcome.h.type = PKT_WELCOME;
    welcome.h.magic = 0;
    expect(valid_welcome(&welcome, sizeof welcome), 0, "wrong magic is rejected");

    // Pieces of a real description: 1 115 295 bytes took 1116 requests in the 26.10 log.
    expect((1115295 + (int)k_chunk - 1) / (int)k_chunk, 1116, "pieces of a 1 115 295 byte description");
    expect((1000 + k_chunk - 1) / k_chunk, 1, "a blob of exactly one piece");
    expect((1001 + k_chunk - 1) / k_chunk, 2, "one byte over a piece");
    expect((0 + k_chunk - 1) / k_chunk, 0, "an empty blob has no pieces");

    // Reassembly: out of order, a duplicate, a piece built on a wrong index and a piece too large.
    Assembly a(1115295);
    expect(a.pieces, 1116, "assembly piece count");
    expect(a.take(1115, 1115295 - 1115 * k_chunk), 1, "the last piece is a short one");
    expect(a.take(0, k_chunk), 1, "the first piece");
    expect(a.take(0, k_chunk), 0, "a duplicate is not taken twice");
    expect(a.take(1116, 100), 0, "a piece past the end is refused");
    expect(a.take(500, k_chunk + 1), 0, "a piece larger than a chunk is refused");
    expect(a.complete(), 0, "not complete with holes");
    for (unsigned i = 1; i < a.pieces; i++) a.take(i, i + 1 == a.pieces ? 1115295 - i * k_chunk : k_chunk);
    expect(a.complete(), 1, "complete when every piece is in");

    // Production fragment guard: a same-epoch revision matters even if the file hash is unchanged.
    ImageData d = {};
    d.h = make_head(PKT_IMAGE_DATA); d.h.flags = 1;
    d.index = 1; d.size = 3; d.map_epoch = 123; d.map_revision = 2;
    size_t header = sizeof(ImageData) - k_chunk;
    expect(image_chunks::valid(&d, header + 3, k_chunk + 3, 1, 123, 2), 1, "matching final map fragment");
    expect(image_chunks::valid(&d, header + 3, k_chunk + 3, 1, 123, 1), 0, "stale map revision rejected");
    expect(image_chunks::valid(&d, header + 3, k_chunk + 3, 1, 122, 2), 0, "stale map epoch rejected");
    expect(image_chunks::valid(&d, header + 2, k_chunk + 3, 1, 123, 2), 0, "truncated datagram rejected");
    d.size = 2;
    expect(image_chunks::valid(&d, header + 2, k_chunk + 3, 1, 123, 2), 0, "short last piece rejected");
    d.index = 0; d.size = k_chunk - 1;
    expect(image_chunks::valid(&d, header + d.size, k_chunk + 3, 1, 123, 2), 0, "short interior piece rejected");
    d.size = k_chunk;
    expect(image_chunks::valid(&d, header + d.size, k_chunk + 3, 1, 123, 2), 1, "full first piece accepted");
    d.h.flags = 0; d.map_epoch = d.map_revision = 0;
    expect(image_chunks::valid(&d, header + d.size, k_chunk + 3, 0), 1, "battle pieces still use zero identity");

    expect(spectator_policy::local_battle_offline(true, true, true), 1, "inherited spectator battle stays offline");
    expect(spectator_policy::local_battle_offline(true, true, false), 0, "authorized battle without spectator marker allowed");
    expect(spectator_policy::incoming(true, false, PKT_STATE, 0), 0, "operation host ignores guest tactical state");
    expect(spectator_policy::incoming(true, false, PKT_SHOT, 0), 0, "operation host ignores guest tactical shots");
    expect(spectator_policy::incoming(true, false, PKT_IMAGE_DATA, 1), 0, "operation host never accepts guest map upload");
    expect(spectator_policy::incoming(true, false, PKT_IMAGE_REQUEST, 1), 1, "operation host serves its map");
    expect(spectator_policy::incoming(false, true, PKT_IMAGE_DATA, 1), 1, "spectator receives host map");
    expect(spectator_policy::incoming(false, true, PKT_SHOT, 0), 0, "spectator ignores tactical packets");
    expect(spectator_policy::incoming(false, false, PKT_SHOT, 0), 1, "tactical/other modes retain their packet path");
    expect(spectator_policy::accept_hello(true, 2), 0, "host map rejects local tactical hello");
    expect(spectator_policy::accept_hello(true, 0xfe), 1, "host map accepts a map observer");
    expect(spectator_policy::accept_hello(false, 2), 1, "tactical host accepts battle hello");

    // The acknowledgement window: everything up to and including seq is gone from the queue.
    unsigned long long seq[] = { 1, 2, 3, 7, 8 };
    unsigned left = 0;
    for (unsigned i = 0; i < 5; i++) if (seq[i] > 3) left++;
    expect(left, 2, "ack 3 leaves the two later launches waiting");

    printf("protocol selftest: %d checks, %d failed\n", g_checks, g_failed);
    return g_failed ? 1 : 0;
}

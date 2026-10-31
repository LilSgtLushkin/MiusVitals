// Reproducible no-game fault simulation of the production Vitals channel. Both endpoints use
// the real packet parser and retry window; the in-memory wire drops, delays, reorders and
// duplicates data and ACKs. This is not a substitute for Steam or engine integration tests.
#include "vitals_core.h"
#include <algorithm>
#include <stdint.h>
#include <stdio.h>
#include <vector>

using namespace vitals;
struct Datagram { bool to_b; std::vector<uint8_t> bytes; };
static std::vector<Datagram> network;
static void send_a(const void *p, int n) {
    network.push_back({true, {(const uint8_t *)p, (const uint8_t *)p + n}});
}
static void send_b(const void *p, int n) {
    network.push_back({false, {(const uint8_t *)p, (const uint8_t *)p + n}});
}
static uint32_t random_state;
static uint32_t next_random() {
    random_state ^= random_state << 13; random_state ^= random_state >> 17; random_state ^= random_state << 5;
    return random_state;
}
static void deliver(Channel &a, Channel &b, bool faults) {
    std::vector<Datagram> now; now.swap(network);
    for (size_t i = now.size(); i > 1; --i) std::swap(now[i - 1], now[next_random() % i]);
    for (const Datagram &p : now) {
        uint32_t choice = next_random() % 10;
        if (faults && choice < 2) continue;
        if (faults && choice == 2) { network.push_back(p); continue; }
        Channel &dest = p.to_b ? b : a; Send send = p.to_b ? send_b : send_a;
        dest.packet(p.bytes.data(), p.bytes.size(), true, send);
        if (faults && choice == 3) dest.packet(p.bytes.data(), p.bytes.size(), true, send);
    }
}
int main() {
    int failures = 0;
    unsigned long long events = 0, retries = 0;
    for (uint32_t seed = 1; seed <= 128; ++seed) {
        random_state = seed * 0x9e3779b9u; network.clear();
        Channel a, b; a.start(1000 + seed); b.start(2000 + seed);
        a.set_peer(2000 + seed); b.set_peer(1000 + seed);
        float expect_a[12], expect_b[12];
        for (int i = 0; i < 12; ++i) expect_a[i] = expect_b[i] = 9.f;
        for (uint32_t round = 0; round < 500; ++round) {
            uint32_t now = round * 100 + 1;
            if (round < 110) {
                for (int side = 0; side < 2; ++side) {
                    uint32_t id = next_random() % 12;
                    float health = (float)(next_random() % 33) / 16.f;
                    Entry entry = soldier(id, health);
                    Channel &ch = side ? b : a; Send send = side ? send_b : send_a;
                    if (ch.enqueue(&entry, 1, now, send)) {
                        float &expected = side ? expect_a[id] : expect_b[id];
                        expected = health; // latest retained owner update, independent of delivery order
                        ++events;
                    }
                }
            }
            a.tick(now, send_a); b.tick(now, send_b); deliver(a, b, round < 110);
            if (a.outstanding() > max_packets || b.outstanding() > max_packets) ++failures;
            if (round >= 110 && !a.outstanding() && !b.outstanding() && network.empty()) break;
        }
        retries += a.counters().retries + b.counters().retries;
        if (a.outstanding() || b.outstanding() || !network.empty()) ++failures;
        for (uint32_t id = 0; id < 12; ++id) {
            const Soldier *av = a.find_soldier(id), *bv = b.find_soldier(id);
            if ((expect_a[id] < 9.f && (!av || av->health != expect_a[id] || av->is_dead != (expect_a[id] < 0.5f))) ||
                (expect_b[id] < 9.f && (!bv || bv->health != expect_b[id] || bv->is_dead != (expect_b[id] < 0.5f)))) ++failures;
        }
    }
    if (events < 10000 || !retries) ++failures; // the test must actually exercise traffic and retries
    printf("vitals randomized selftest: 128 seeds, %llu retained events, %llu retries, %d failed\n", events, retries, failures);
    return failures ? 1 : 0;
}

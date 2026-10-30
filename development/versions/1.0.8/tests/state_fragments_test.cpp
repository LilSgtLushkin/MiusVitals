#include "state_fragments.h"
#include <stdio.h>
static int checks, failed;
static void check(bool b, const char *s) { ++checks; if (!b) { ++failed; printf("FAIL: %s\n", s); } }
static ObjectFrag frag(unsigned size, unsigned index, unsigned count, unsigned len) {
    ObjectFrag f = {}; f.h = make_head(PKT_OBJECT, len); f.version = 1; f.raw_size = size;
    f.delta_size = size; f.frag = (unsigned short)index; f.nfrags = (unsigned short)count; return f;
}
int main() {
    using state_fragments::valid;
    ObjectFrag f = frag(2201, 0, 3, 1100);
    check(valid(f, sizeof f + 1100), "first full fragment");
    f.frag = 1; check(valid(f, sizeof f + 1100), "middle fragment");
    f.frag = 2; f.h.count = 1; check(valid(f, sizeof f + 1), "last short fragment");
    check(!valid(f, sizeof f), "short packet rejected");
    f.h.count = 0; check(!valid(f, sizeof f), "missing last byte rejected");
    f.h.count = 2; check(!valid(f, sizeof f + 2), "oversized final fragment rejected");
    f = frag(2201, 0, 2, 1100); check(!valid(f, sizeof f + 1100), "wrong fragment count rejected");
    f = frag(2201, 0, 3, 1099); check(!valid(f, sizeof f + 1099), "short middle fragment rejected");
    f = frag(0, 0, 1, 0); check(valid(f, sizeof f), "empty state uses one empty fragment");
    f = frag(1100, 0, 1, 1100); check(valid(f, sizeof f + 1100), "exact fragment boundary");
    f.frag = 1; check(!valid(f, sizeof f + 1100), "fragment index out of range rejected");
    f = frag(16u << 20, 0, (unsigned)(((16u << 20) + 1099) / 1100), 1100);
    check(valid(f, sizeof f + 1100), "maximum state size");
    f.delta_size++; check(!valid(f, sizeof f + 1100), "delta above limit rejected");
    f.delta_size--; f.raw_size++; check(!valid(f, sizeof f + 1100), "raw size above limit rejected");
    f = frag(1, 0, 1, 1); f.h.type = PKT_STATE; check(!valid(f, sizeof f + 1), "wrong packet type");
    f = frag(1, 0, 1, 1); f.version = 0; check(!valid(f, sizeof f + 1), "zero is not a state version");
    check(state_fragments::newer(2, 1) && !state_fragments::newer(1, 2) && !state_fragments::newer(1, 1),
          "serial version order and duplicates");
    check(state_fragments::newer(1, 0xffffffffu) && !state_fragments::newer(0xffffffffu, 1),
          "serial version wrap");
    printf("state fragment selftest: %d checks, %d failed\n", checks, failed);
    return failed ? 1 : 0;
}

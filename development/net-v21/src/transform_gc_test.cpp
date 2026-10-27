#include "transform_gc.h"
#include <unordered_map>
#include <stdio.h>

static int checks, failed;
static void check(bool b, const char *s) { ++checks; if (!b) { ++failed; printf("FAIL: %s\n", s); } }
int main()
{
    std::unordered_map<unsigned long long, int> live = {{1, 10}, {3, 30}};
    std::unordered_map<unsigned long long, int> held = {{1, 100}, {2, 200}, {3, 300}};
    check(prune_absent(held, live) == 1, "removed stale object");
    check(held.size() == 2 && held.at(1) == 100 && held.at(3) == 300, "live cache values preserved");
    check(prune_absent(held, live) == 0, "stable set does not change");
    live.clear();
    check(prune_absent(held, live) == 2 && held.empty(), "empty manager clears cached units");
    held[9] = 42; live[9] = 0;
    check(prune_absent(held, live) == 0 && held.at(9) == 42, "replacement with same key is retained");
    printf("transform cache pruning selftest: %d checks, %d failed\n", checks, failed);
    return failed ? 1 : 0;
}

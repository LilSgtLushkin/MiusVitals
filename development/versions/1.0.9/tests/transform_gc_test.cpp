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
    check(prune_absent(held, live) == 0 && held.at(9) == 42, "key-only pruning retains a present key");

    struct Object { int value; } first = {1}, second = {2}, replacement = {3};
    Object *entries[] = { &first, &second };
    int manager_a = 1, manager_b = 2;
    ObjectListSnapshot<Object> snapshot;
    check(!snapshot.matches(&manager_a, entries, 2), "first observation requires a rebuild");
    snapshot.remember(&manager_a, entries, 2);
    check(snapshot.matches(&manager_a, entries, 2), "unchanged manager and exact list need no rebuild");
    entries[1] = &replacement;
    check(snapshot.entries()[1] == &second, "index rebuild uses a captured pointer sequence, not the mutable source array");
    check(!snapshot.matches(&manager_a, entries, 2), "same-count replacement invalidates the object index");
    snapshot.remember(&manager_a, entries, 2);
    check(snapshot.matches(&manager_a, entries, 2), "rebuilt index uses the new pointer list");
    check(!snapshot.matches(&manager_b, entries, 2), "manager change invalidates an otherwise identical list");
    Object *reordered[] = { &replacement, &first };
    check(!snapshot.matches(&manager_a, reordered, 2), "reordered list invalidates the object index");
    check(!snapshot.matches(&manager_a, entries, 1), "changed count invalidates the object index");
    snapshot.remember(&manager_a, nullptr, 0);
    check(snapshot.matches(&manager_a, nullptr, 0), "an empty object list is remembered without dereferencing it");
    snapshot.clear();
    check(!snapshot.matches(&manager_a, nullptr, 0), "reset invalidates even an empty remembered list");

    std::unordered_map<unsigned long long, Object *> previous = {{1, &first}, {2, &second}, {3, &second}};
    std::unordered_map<unsigned long long, Object *> current = {{1, &first}, {2, &replacement}, {4, &first}};
    held = {{1, 10}, {2, 20}, {3, 30}, {4, 40}};
    check(prune_replaced(held, previous, current) == 3, "changed pointer, absent key and previously unknown key are invalidated");
    check(held.size() == 1 && held.at(1) == 10, "only an unchanged key and pointer retains its cached state");
    check(prune_replaced(held, previous, current) == 0, "repeated pruning of the unchanged unit is a no-op");
    printf("transform cache pruning selftest: %d checks, %d failed\n", checks, failed);
    return failed ? 1 : 0;
}

// Exercise the real state_save() allocation failure path without a game or engine hooks.
#include "common.h"
#include <stdio.h>
#include <string.h>
#include <vector>

void state_test_fail_scratch_allocation(bool fail);
char g_out_dir[MAX_PATH] = ".";
char g_program[32] = "tacgame";
void mp_log(const char *, ...) {}
bool readable(const void *p, size_t) { return p != nullptr; }

struct FakeStream { unsigned char *begin, *end, *cap; u32 pos, underrun; };
struct FakeMessage { void **vtbl; const char *name; FakeStream *stream; };
static unsigned saves;
static void save(Object *, void *raw)
{
    ++saves;
    FakeStream *s = ((FakeMessage *)raw)->stream;
    if (s && s->begin && s->end == s->begin && s->cap > s->end) *s->end++ = 0x5a;
}
static u32 hash(const char **, u32) { return 0; }
static int checks, failed;
static void check(bool condition, const char *name)
{
    ++checks;
    if (!condition) { ++failed; printf("FAIL: %s\n", name); }
}

int main()
{
    Object object = {};
    const char *keys[] = { "save" };
    Handler values[] = { save };
    HashSlot slot[] = { { 0x80000000u, 0 } };
    object.cmds.keys = keys; object.cmds.vals = values;
    object.cmds.slots = slot; object.cmds.nslots = 1; object.cmds.hash = hash;
    std::vector<unsigned char> out(1, 0xcc);

    state_test_fail_scratch_allocation(true);
    check(!state_save(&object, out), "allocation failure is reported");
    check(saves == 0 && out.size() == 1 && out[0] == 0xcc,
          "failure does not call the engine or alter the existing output");
    state_test_fail_scratch_allocation(false);
    check(state_save(&object, out), "a later allocation can recover");
    check(saves == 1 && out.size() == 1 && out[0] == 0x5a,
          "successful save still reaches the engine and copies its data");
    printf("state memory selftest: %d checks, %d failed\n", checks, failed);
    return failed ? 1 : 0;
}

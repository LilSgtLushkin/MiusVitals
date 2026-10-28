// Exercise the real state_save() allocation failure path without a game or engine hooks.
#include "common.h"
#include <stdio.h>
#include <string.h>
#include <new>
#include <vector>

void state_test_fail_scratch_allocation(bool fail);
char g_out_dir[MAX_PATH] = ".";
char g_program[32] = "tacgame";
void mp_log(const char *, ...) {}
bool readable(const void *p, size_t) { return p != nullptr; }

static unsigned saves;
static void save(Object *, void *raw)
{
    ++saves;
    void *stream = nullptr;
    unsigned char *begin = nullptr, *end = nullptr, *cap = nullptr;
    memcpy(&stream, (char *)raw + 0x10, sizeof stream);
    if (!stream) return;
    memcpy(&begin, (char *)stream, sizeof begin);
    memcpy(&end, (char *)stream + 8, sizeof end);
    memcpy(&cap, (char *)stream + 16, sizeof cap);
    if (begin && end == begin && cap > end) {
        *end++ = 0x5a;
        memcpy((char *)stream + 8, &end, sizeof end);
    }
}
static u32 hash(const char **, u32) { return 0; }
static bool load_throws, saw_remote_mode;
static void load(Object *, void *)
{
    saw_remote_mode = state_remote_loading();
    if (load_throws) throw 42;
}
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

    // A throwing synthetic load handler represents C++ unwinding, not a real engine fault.
    // The unit fixture has enough room for the recovered motion/view/animation offsets.
    alignas(8) unsigned char unit_memory[0x3000] = {};
    Object *unit = new (unit_memory) Object{};
    unit->iface = "i_human";
    const char *load_keys[] = { "load" };
    Handler load_values[] = { load };
    HashSlot load_slot[] = { { 0x80000000u, 0 } };
    unit->cmds.keys = load_keys; unit->cmds.vals = load_values;
    unit->cmds.slots = load_slot; unit->cmds.nslots = 1; unit->cmds.hash = hash;
    unsigned char byte = 0;
    bool caught = false; load_throws = true; saw_remote_mode = false;
    try { state_load_keep_motion(unit, &byte, 1, false); }
    catch (int error) { caught = error == 42; }
    check(caught && saw_remote_mode && !state_remote_loading(),
          "exception unwinding clears remote-load mode after the real load path");
    load_throws = false; saw_remote_mode = false;
    check(state_load_keep_motion(unit, &byte, 1, false) == 0 && saw_remote_mode && !state_remote_loading(),
          "successful load still enables remote mode only around the engine handler");

    printf("state memory selftest: %d checks, %d failed\n", checks, failed);
    return failed ? 1 : 0;
}

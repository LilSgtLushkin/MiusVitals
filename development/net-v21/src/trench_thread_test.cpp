// Exercise the real trench dig hook from the owner thread and a worker without a game.
// This fixture supplies only the original dig function and the session/engine dependencies.
#include "common.h"
#include "netlog.h"
#include "protocol.h"
#include <atomic>
#include <stdio.h>
#include <string.h>

using TestDig = u64 (*)(char *, int, float *);
void trench_test_engine(TestDig original);
u64 trench_test_dig(char *trencher, int squad, float *pos);
char *trench_test_current();
bool trench_test_install(void *target);

static DWORD owner;
static std::atomic<unsigned> calls{0}, foreign_session_access{0};
static int checks, failed;

void mp_log(const char *, ...) {}
bool readable(const void *, size_t) { return false; }
static void session_access() {
    if (GetCurrentThreadId() != owner) foreign_session_access.fetch_add(1, std::memory_order_relaxed);
}
bool session_connected() { session_access(); return false; }
int session_role() { session_access(); return ROLE_NONE; }
bool session_owns_squad(u32) { session_access(); return false; }
bool session_unit_key(Object *, u64 *) { session_access(); return false; }
Object *session_unit(u64) { session_access(); return nullptr; }
void shots_queue(ShotRec &) { session_access(); }
void netlog_count(const char *, long long) {}

static u64 original(char *, int squad, float *) {
    calls.fetch_add(1, std::memory_order_relaxed);
    return (u64)squad + 100;
}
static DWORD WINAPI worker(void *object) {
    return trench_test_dig((char *)object, 23, nullptr) == 123 ? 0 : 1;
}
struct InstalledCall { TestDig function; char *manager; };
static DWORD WINAPI installed_worker(void *argument) {
    InstalledCall *call = (InstalledCall *)argument;
    return call->function(call->manager, 23, nullptr) == 123 ? 0 : 1;
}
static void check(bool condition, const char *message) {
    ++checks;
    if (!condition) { ++failed; printf("FAIL: %s\n", message); }
}

int main()
{
    owner = GetCurrentThreadId();
    trench_test_engine(original);
    char main_manager[1] = {}, worker_manager[1] = {};
    check(trench_test_dig(main_manager, 11, nullptr) == 111, "owner-thread call reaches original");
    check(trench_test_current() == main_manager, "owner thread records its trench manager");
    check(calls.load() == 1, "original called once on owner thread");

    HANDLE thread = CreateThread(nullptr, 0, worker, worker_manager, 0, nullptr);
    if (!thread) return 2;
    DWORD wait = WaitForSingleObject(thread, 5000), exit_code = 1;
    if (wait == WAIT_OBJECT_0) GetExitCodeThread(thread, &exit_code);
    CloseHandle(thread);
    check(wait == WAIT_OBJECT_0 && exit_code == 0, "worker call reaches original and returns its result");
    check(calls.load() == 2, "original called once on each thread");
    check(trench_test_current() == main_manager, "worker cannot overwrite owner-thread manager");
    check(foreign_session_access.load() == 0, "worker does not touch session state");

    // Install the production published trampoline on synthetic x64 code, not a game function.
    // The first 14 bytes are whole NOP instructions; the body returns the squad number + 100.
    unsigned char *code = (unsigned char *)VirtualAlloc(nullptr, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!code) return 2;
    memset(code, 0x90, 14);
    const unsigned char body[] = { 0x89, 0xd0, 0x83, 0xc0, 0x64, 0xc3 }; // mov eax,edx; add eax,100; ret
    memcpy(code + 14, body, sizeof body);
    FlushInstructionCache(GetCurrentProcess(), code, 14 + sizeof body);
    TestDig entry = (TestDig)code;
    check(entry(main_manager, 11, nullptr) == 111, "synthetic original returns expected result");
    bool installed = trench_test_install(code);
    check(installed, "published trench detour installs on synthetic function");
    if (installed) {
        check(entry(main_manager, 11, nullptr) == 111 && trench_test_current() == main_manager,
              "installed owner hook calls the published trampoline");
        InstalledCall call = { entry, worker_manager };
        thread = CreateThread(nullptr, 0, installed_worker, &call, 0, nullptr);
        if (!thread) return 2;
        wait = WaitForSingleObject(thread, 5000); exit_code = 1;
        if (wait == WAIT_OBJECT_0) GetExitCodeThread(thread, &exit_code);
        CloseHandle(thread);
        if (wait != WAIT_OBJECT_0) return 2;
        check(exit_code == 0 && trench_test_current() == main_manager && foreign_session_access.load() == 0,
              "installed worker hook passes through the published trampoline without touching session state");
    }
    VirtualFree(code, 0, MEM_RELEASE);
    printf("trench thread selftest: %d checks, %d failed\n", checks, failed);
    return failed ? 1 : 0;
}

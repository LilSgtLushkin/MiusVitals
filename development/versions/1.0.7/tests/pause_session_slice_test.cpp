// The generator includes actual static functions from session.cpp, with only the engine,
// transport, clock and global environment mocked. This is not a real detour or game test.
#include "pause_sync.h"
#include "transform_inbox.h"
#include <cmath>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>

using u32 = uint32_t;
using u64 = uint64_t;
using DWORD = uint32_t;
struct Object { unsigned char bytes[0x1000]; };
struct ObjMan { Object *ui; };
enum { ROLE_NONE, ROLE_HOST, ROLE_JOIN };
using StartAction = u64 (*)(Object *, u64, u64, u64);
using InputAction = void (*)(Object *, u64, const char *);
using BoxResult = void (*)(Object *, int, u64, u64);
using TimerStep = void (*)(char *, float, u32);
using SetPause = void (*)(char *, unsigned char);
static StartAction g_start_action;
static InputAction g_input_action;
static BoxResult g_box_result;
static TimerStep g_timer_step;
static SetPause g_set_pause;
static int g_role;
static bool g_tactical, g_connected, g_pause_ever_connected, g_have_host_clock, g_clock_adopted;
static unsigned g_peer_features;
static pause_sync::State g_pause;
static transforms::HeaderOrder g_state_headers;
static const int k_units_per_packet = 14;
static u32 g_phase, g_my_ready, g_peer_ready, g_host_phase, g_host_sim_ms, g_peer_phase, g_peer_sim_ms;
static u64 g_sim_ms;
static DWORD g_last_snapshot, g_last_hello, g_last_phase_step, g_peer_clock_at, g_waiting_since;
static unsigned char g_host_paused;
static float g_host_speed = 1, g_host_cur_speed = 1, g_clock_factor;
static double g_sim_step;
static DWORD fake_now = 10000;
static int start_calls, input_calls, timer_calls, pause_set_calls;
static unsigned char timer_observed;
static int checks, failures;

static DWORD GetTickCount() { return fake_now; }
static u32 vitals_epoch() { return 11; }
static u32 vitals_peer_epoch() { return 777; }
static void mp_log(const char *, ...) {}
static bool readable(const void *p, size_t n) { return p && n; }
static bool env_text(const char *, char *, size_t) { return false; }
static double seconds_now() { return 0; }
static Object *engine_find(ObjMan *om, const char *, const char *) { return om ? om->ui : nullptr; }
static u64 original_start(Object *, u64, u64, u64) { ++start_calls; return 77; }
static void original_input(Object *, u64, const char *) { ++input_calls; }
static void original_box(Object *, int, u64, u64) {}
static void original_timer(char *ct, float, u32) { ++timer_calls; timer_observed = (unsigned char)ct[0x15c]; }
static void original_set_pause(char *ct, unsigned char value) { ++pause_set_calls; ct[0x15c] = (char)value; }
static void expect(bool ok, const char *what) {
    ++checks;
    if (!ok) { ++failures; printf("FAIL: %s\n", what); }
}

#include "pause_session_extracted.inc"

int main() {
    Object ui = {};
    ObjMan manager = { &ui };
    alignas(8) char timer[0x180] = {};
    alignas(8) char event[0x10] = {};
    float release = -1, press = 1;
    memcpy(event + 8, &release, sizeof release);
    g_tactical = g_connected = true; g_role = ROLE_HOST; g_phase = 0;
    g_start_action = original_start; g_input_action = original_input; g_box_result = original_box;
    g_timer_step = original_timer; g_set_pause = original_set_pause;
    g_sim_step = 0; // exercise the timer's ordinary original-call branch with mock engine
    g_pause.reset(true);
    pause_sync::State guest; guest.reset(false);
    u32 host_sequence = 0, guest_sequence = 0;
    auto guest_header = [&](unsigned features) {
        StateHead sh = {};
        sh.h = make_head(PKT_STATE); sh.h.flags = (unsigned char)features;
        sh.owner = vitals_peer_epoch(); sh.target = vitals_epoch(); sh.snapshot = ++guest_sequence;
        sh.phase = (unsigned char)g_phase; sh.ready = (unsigned char)g_peer_ready;
        sh.speed = sh.cur_speed = 1; sh.pause = guest.message();
        return sh;
    };
    auto deliver = [&](const StateHead &sh, int size) {
        return valid_tactical_state(&sh, size) && apply_state_header(&sh, fake_now);
    };
    auto to_host = [&](unsigned features = PEER_PAUSE_SYNC) {
        StateHead sh = guest_header(features);
        return deliver(sh, sizeof sh);
    };
    auto to_guest = [&]() { return guest.receive(g_pause.message(), ++host_sequence); };
    auto timer_check = [&](unsigned char wanted, const char *what) {
        timer[0x15c] = (char)!wanted; timer_observed = 99;
        timer_step_hook(timer, 0.016f, 0);
        expect(timer_observed == wanted, what);
    };

    expect(pause_feature() == PEER_PAUSE_SYNC, "all four installed hooks advertise pause");
    g_box_result = nullptr;
    expect(!pause_feature(), "missing deployment-box hook is not advertised");
    g_box_result = original_box;
    expect(start_action_hook(&ui, 0, 0, 0) == 0 && g_my_ready == 1 && !start_calls,
           "phase zero button still marks readiness without stepping the engine");
    g_peer_ready = 1; advance_phase(&manager, 2000);
    expect(start_calls == 1, "phase zero advances after the original two ready votes");
    g_phase = 1; g_my_ready = 1; g_peer_ready = 2;
    input_action_hook(&ui, 0, event);
    expect(g_my_ready == 2 && !input_calls, "phase one keyboard still marks readiness");
    advance_phase(&manager, 4000);
    expect(start_calls == 1, "battle cannot start without verified pause capability");
    expect(to_guest() && to_host(), "first ordered peer control becomes active");
    StateHead stale = guest_header(PEER_PAUSE_SYNC); stale.snapshot = guest_sequence - 1;
    stale.ready = 0;
    expect(!deliver(stale, sizeof stale) && g_peer_ready == 2,
           "a later chunk with the same snapshot cannot roll back readiness or control");
    StateHead bad_epoch = guest_header(PEER_PAUSE_SYNC); bad_epoch.owner++;
    expect(!deliver(bad_epoch, sizeof bad_epoch), "retired peer epoch rejected before control dispatch");
    StateHead bad_flags = guest_header(PEER_PAUSE_SYNC); bad_flags.h.flags |= 0x80;
    expect(!deliver(bad_flags, sizeof bad_flags), "unknown state capability rejected");
    StateHead bad_count = guest_header(PEER_PAUSE_SYNC); bad_count.h.count = 14;
    expect(!deliver(bad_count, sizeof bad_count), "truncated multi-unit datagram rejected");
    struct Packet { StateHead head; UnitRec units[14]; } packet = {};
    static_assert(sizeof packet == sizeof(StateHead) + 14 * sizeof(UnitRec), "packed state test packet");
    packet.head = guest_header(PEER_PAUSE_SYNC); packet.head.h.count = 14;
    expect(deliver(packet.head, sizeof packet), "complete 14-unit datagram accepts its header");
    packet.head.ready = 0;
    expect(!deliver(packet.head, sizeof packet) && g_peer_ready == 2,
           "same-snapshot chunk cannot roll back a previously applied header");
    advance_phase(&manager, 4000);
    expect(start_calls == 2, "ready peers with pause capability may enter battle");

    g_phase = 2; g_pause_ever_connected = true;
    ui.bytes[0xed9] = 1;
    expect(start_action_hook(&ui, 0, 0, 0) == 77 && !g_pause.my_vote(),
           "alternate/modal button action remains with the game");
    input_action_hook(&ui, 0, event);
    expect(input_calls == 1 && !g_pause.my_vote(), "alternate/modal key action remains with the game");
    ui.bytes[0xed9] = 0; ui.bytes[0xeda] = 1;
    input_action_hook(&ui, 0, event);
    expect(input_calls == 2 && !g_pause.my_vote(), "disabled UI action remains with the game");
    ui.bytes[0xeda] = 0;
    memcpy(event + 8, &press, sizeof press);
    input_action_hook(&ui, 0, event);
    expect(input_calls == 3 && !g_pause.my_vote(), "non-release key event is not a vote");
    memcpy(event + 8, &release, sizeof release);

    expect(start_action_hook(&ui, 0, 0, 0) == 1 && g_pause.my_vote(), "battle button proposes a pause");
    timer_check(0, "one pause vote must not stop the timer");
    expect(guest.press() && to_host() && g_pause.stage() == 1, "both votes prepare pause");
    timer_check(0, "prepare alone must not stop the timer");
    expect(to_guest() && to_host() && g_pause.paused() && !guest.paused(), "guest acknowledgement commits on host");
    timer_check(1, "committed pause reaches the real timer hook");
    set_pause_hook(timer, 0);
    expect(pause_set_calls == 1 && timer[0x15c] == 1, "ordinary timer setter cannot cancel the agreed pause");
    expect(to_guest() && to_host() && guest.paused() && !g_pause.stage(), "guest applies and acknowledges pause");

    input_action_hook(&ui, 0, event);
    expect(!g_pause.my_vote(), "battle keyboard can vote to resume");
    timer_check(1, "one resume vote does not run the timer");
    expect(guest.press() && to_host() && to_guest() && to_host() && to_guest() && to_host(),
           "two resume votes complete the protocol");
    timer_check(0, "agreed resume runs the timer");
    set_pause_hook(timer, 1);
    expect(timer[0x15c] == 0, "ordinary timer setter cannot introduce a unilateral pause");

    expect(to_host(0), "a newer state reports missing pause capability");
    expect(g_pause.paused() && g_pause.recovering() && !g_pause.active(), "lost peer capability enters recovery");
    timer_check(1, "missing peer capability holds the timer");
    expect(to_host() && g_pause.paused() && g_pause.recovering(), "capability return cannot silently resume");
    expect(to_guest() && guest.paused() && to_host(), "the peer observes the recovery hold");
    expect(guest.press(), "guest votes to resume after recovery");
    expect(start_action_hook(&ui, 0, 0, 0) == 1, "host votes to resume after recovery");
    expect(to_host() && to_guest() && to_host() && to_guest() && to_host(), "recovery requires a new agreement");
    timer_check(0, "timer runs only after recovery agreement");

    g_pause.reset(true); guest.reset(false); host_sequence = guest_sequence = 0; g_state_headers.reset();
    g_pause_ever_connected = false; g_connected = true;
    pause_connection_lost(); g_connected = false;
    expect(g_pause_ever_connected && g_pause.paused() && g_pause.recovering(),
           "loss before the first control packet still enters recovery");
    timer_check(1, "loss before the first control packet keeps the timer held");
    g_connected = true;
    expect(to_host() && g_pause.paused(), "reconnection without votes does not release the first-packet hold");
    expect(to_guest() && guest.paused(), "the reconnecting peer learns the hold");
    expect(guest.press() && g_pause.press() && to_host() && to_guest() && to_host() && to_guest() && to_host(),
           "both peers may explicitly recover from an early loss");
    timer_check(0, "early-loss recovery resumes the timer after both votes");

    printf("pause session source-slice selftest: %d checks, %d failed (%d timer calls)\n", checks, failures, timer_calls);
    return failures ? 1 : 0;
}

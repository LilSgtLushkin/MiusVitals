// Detailed, automatic logging for our own network module.
//
// The published module logs into out\basic_network_<program>_<pid>.log, one line per event, and it
// is written by whoever remembers to call mp_log().  That is fine while a feature is being built
// and useless when a player reports "the battle went out of sync at minute ten": by then the file
// has rolled over with the process, the pid changes every run, and nothing in it says which build
// produced it.
//
// This layer fixes the three things that make a log useless for a bug report:
//
//   1. identity - the first lines say which build this is, the file's size and hash, the flags the
//      process was started with, the command line and the OS.  A log without that cannot be
//      matched to a binary, and a report about "the new dll" is worthless if nobody can tell which
//      new dll it was;
//   2. a stable name - out\net_v21_<program>.log, truncated per run, the previous run kept as
//      .prev.  The player is told "send out\net_v21_opgame.log", not "find the newest file whose
//      name has a pid in it";
//   3. traffic and battle accounting - every datagram is counted by type and direction, a line is
//      written every five seconds with the totals, and the battle's own milestones (first
//      snapshot, hits, craters, ceasefire, end) go in with the same timestamp as everything else.
//
// Everything here is best-effort: if the log cannot be opened the module keeps running, because a
// mod that refuses to play when it cannot write a file is worse than a mod with no log.
#pragma once

#include <windows.h>

// How many packet types the counters keep.  The protocol has 11 today; the table is deliberately
// larger so a new type does not silently fall off the end.
enum { NETLOG_TYPES = 24 };

// Opens the log for this process.  `out_dir` is the module's out\ folder (the caller already knows
// how to derive it), `program` is opgame/tacgame.  Safe to call twice; the second call is a no-op.
void netlog_open(HMODULE self, const char *program, const char *out_dir);

// Flushes and closes.  Called from DllMain detach; also safe to call when nothing was opened.
void netlog_shutdown();

// One line from the module's own log, mirrored here so a single file has everything in order.
void netlog_mirror(const char *text);

// Free-form line that is always written (no rate limiting).  Used for identity and settings.
void netlog_note(const char *fmt, ...);

// A datagram went out or came in.  `family` is the socket address family of the peer: AF_INET for
// UDP, 0x5354 for a Steam peer, 0 when unknown.  Rate limited per type, so a battle at 60 packets a
// second cannot fill the disk - the five-second summary carries the true totals.
void netlog_packet(const char *dir, const unsigned char *data, int size,
                   unsigned int peer_ip, unsigned short peer_port, unsigned short family);

// Called once per frame from session_frame(): advances the frame counter and, every five seconds,
// writes the summary line.
void netlog_frame();

// A named counter (shots, hits, craters, resyncs, ...).  Its total appears in every summary.
void netlog_count(const char *name, long long delta = 1);

// A battle milestone.  Written as "battle: ..." with the usual prefix; never rate limited.
void netlog_mark(const char *fmt, ...);

// The battle's current state, written into the summary: phase, sim time, players, sides.
void netlog_state(int phase, unsigned sim_ms, int peers, unsigned sides);

const char *netlog_path();

// Test hook: write the five-second summary now instead of waiting for the timer.  The module never
// calls it; the self-test does, because a test cannot sit still for five seconds.
void netlog_test_summary();

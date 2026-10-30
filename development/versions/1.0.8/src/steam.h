// Steam transport for the network module - our own implementation of the layer v20 has and the
// published v12 source does not (docs/V20_STEAM_LAYER.md describes what the v20 binary does).
//
// The point of the design is that nothing above this file knows Steam exists.  A peer is a
// sockaddr_in whose sa_family is STEAM_FAMILY and whose sa_data carries the Steam ID, exactly the
// way the v20 module does it, so the packet code, the HELLO handshake and the lobby keep working
// unchanged whether the bytes go over UDP or through Steam's P2P relay.
//
// Three modes, checked in this order:
//   MP_STEAM=0            off            - plain UDP, no steam_api64.dll needed
//   MP_STEAM_FAKE=<n>     stand-in       - identity "steam:<n>", datagrams over loopback UDP
//   otherwise             real Steam     - steam_api64.dll of the game, flat API through GetProcAddress
//
// No Steamworks SDK is required: the module resolves 21 entry points at run time, so it does not
// link against steam_api64.lib and cannot break when the game updates - a missing entry point is
// reported in the log instead of crashing at load.
#pragma once

// winsock2 must come before windows.h, or the old winsock1 headers win the race
#include <winsock2.h>
#include <windows.h>
#include <stdint.h>
#include <string.h>
#include "steam_friends.h"

// A peer that lives inside Steam: sa_family is STEAM_FAMILY, the Steam ID sits at +8, the name
// at +32.  Same encoding as v20, so a packet can be addressed the same way for both transports.
enum { STEAM_FAMILY = 0x5354, STEAM_NAME_MAX = 64 };
enum { STEAM_CHANNEL = 0 };

struct steam_peer {
    unsigned short family;      // STEAM_FAMILY
    unsigned short pad;
    uint64_t id;                // CSteamID / the stand-in's number
    char name[STEAM_NAME_MAX];  // persona name, sanitised
};

// Logging is supplied by whoever links this file: the module forwards to mp_log so Steam lines land
// in the same log as everything else, the self-test prints to stdout.
void steam_log(const char *fmt, ...);

// Where the five-second traffic totals go.  The module feeds them to the run log; a console program
// (self-test, join launcher) links the empty implementation in steam_logger.cpp.  One of the two is
// always linked, so steam.cpp never needs to know which world it is in.
void steam_traffic_note(unsigned long long sent, unsigned long long received);

// Lifecycle.  steam_init() is idempotent and safe to call every frame; it returns true when the
// transport is usable.  It never throws and never aborts the game: every failure is a log line.
bool steam_init();
bool steam_ready();
void steam_shutdown();

// Identity.
bool steam_is_fake();
uint64_t steam_user_id();
const char *steam_user_name();   // "" until ready
uint32_t steam_app_id();
int steam_friend_count();
// Friends playing this application (including those whose hosting presence has not arrived yet).
// Returns the total matching friends; writes at most capacity records. Never connects by itself.
size_t steam_friends_playing(lobby_friends::Friend *out, size_t capacity, bool request_presence);

// The lobby Joiner and the game session share Steam's one P2P receive queue. The Joiner reserves it
// while connecting/downloading, so session_frame cannot consume and discard the lobby's replies.
bool steam_lobby_claim();
void steam_lobby_release();
bool steam_lobby_receiving();

// Pump: delivers callbacks and drains P2P packets.  Call once per frame.
void steam_frame();

// A peer on the Steam transport is an ordinary sockaddr_in whose family is STEAM_FAMILY and whose
// Steam ID sits at +8, where sin_zero starts - the encoding v20 uses, so one address describes a peer
// whichever way its bytes travel.  These two turn it around; they are inline so the session code and
// the joiner can use them without linking anything extra.
inline bool steam_peer_make(uint64_t id, sockaddr_in *out)
{
    if (!out || !id)
        return false;
    *out = sockaddr_in();
    out->sin_family = STEAM_FAMILY;
    memcpy(out->sin_zero, &id, sizeof id);
    return true;
}

// Strict decimal parsing: reject zero, suffixes, signs and uint64 overflow rather than joining a
// different peer than the user selected. The stand-in's small numeric IDs remain valid in tests.
inline bool steam_parse_id(const char *text, uint64_t *id)
{
    if (!text || !*text || !id) return false;
    uint64_t value = 0;
    for (const char *p = text; *p; ++p) {
        if (*p < '0' || *p > '9') return false;
        unsigned digit = (unsigned)(*p - '0');
        if (value > (UINT64_MAX - digit) / 10) return false;
        value = value * 10 + digit;
    }
    if (!value) return false;
    *id = value;
    return true;
}

inline uint64_t steam_peer_id(const sockaddr_in *address)
{
    if (!address || address->sin_family != STEAM_FAMILY)
        return 0;
    uint64_t id = 0;
    memcpy(&id, address->sin_zero, sizeof id);
    return id;
}

// Totals since the layer came up: what the transport has actually moved.  The self-test uses them;
// the five-second line is built from the same counters.
void steam_traffic_totals(unsigned long long *sent, unsigned long long *received);

// Datagrams.  Returns false when the peer is unreachable or the transport is off.
bool steam_send(uint64_t peer, const void *data, int len);
// >0 bytes received, 0 nothing pending, -1 transport off.  Fills *peer when something arrives.
int steam_recv(uint64_t *peer, void *data, int capacity);

// Rich presence: the note friends see, which is how they know what to join.
bool steam_presence(const char *value);
const char *steam_presence_value();          // last value we asked Steam to hold
bool steam_presence_check(char *out, size_t size);  // read it back and compare, like v20 does

// Tell friends that this game can be joined, or that it can no longer be joined.
//
// v20 does this in one place (snapshot_operation_saves) and with two presence keys: "gtmf" gets
// "<operation number> game", and "connect" gets the text Steam puts behind the Join Game entry -
// "root\programs\opgame.progpack, +gtmf_join=steam:<our id>".  Clicking that entry starts the
// game with "+gtmf_join=steam:<our id>" on the command line, which steam_note_command_line()
// picks up.  With hosting false both keys are emptied, which hides the game from friends.
bool steam_broadcast(bool hosting, unsigned operation);
const char *steam_connect_text();            // the last "connect" text, for the log and tests

// Join requests: "+gtmf_join=steam:<id>" on the command line, or a friend pressing Join Game.
void steam_note_command_line(const char *command_line);
bool steam_join_take(uint64_t *id);          // true once per request
const char *steam_join_source();             // "the command line" / "a friend's game"

// Glue supplied by steam_module.cpp: read the command line for a join request and bring the layer
// up; deliver callbacks and P2P packets once per frame.  Both are cheap to call repeatedly.
void steam_install();
void steam_tick();

// Test hooks (self-test only; the module never calls them).
void steam_test_set_command_line(const char *text);
void steam_test_join_callback(const char *text, size_t size);
bool steam_test_callback_abi();
void steam_test_reset();
#ifdef NETV21_TEST_HOOKS
bool steam_test_wait_packet(unsigned timeout_ms); // fake transport only; does not consume the packet
#endif

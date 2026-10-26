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

// Pump: delivers callbacks and drains P2P packets.  Call once per frame.
void steam_frame();

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

// Test hooks (self-test only; the module never calls them).
void steam_test_set_command_line(const char *text);
void steam_test_reset();

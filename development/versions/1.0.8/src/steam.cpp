// Implementation of the Steam layer described in docs/V20_STEAM_LAYER.md.
//
// Everything here was reconstructed from the v20 module's machine code, so the names of the
// environment variables, the log lines, the callback ids and the 21 resolved entry points are not
// invented - they are what the released module does.  What is new is the arrangement: this is a
// plain C++ file with no SDK and no link-time dependency on Steam, so it can be built, reviewed and
// tested like any other file in the tree.
#include "steam.h"

#include <atomic>
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --------------------------------------------------------------------------- Steam flat API
// Only the pieces this file touches.  A CSteamID is a uint64 by value; every function returns bool
// unless noted, which is how the flat wrappers in steam_api64.dll behave.
typedef uint64_t csteamid;

typedef void *(*fn_steamfriends_v017)();
typedef void *(*fn_steamuser_v021)();
typedef void *(*fn_steamnetworking_v006)();
typedef void *(*fn_steamutils_v010)();

typedef uint64_t (*fn_user_get_id)(void *user);
typedef bool (*fn_user_logged_on)(void *user);
typedef uint32_t (*fn_utils_app_id)(void *utils);
typedef const char *(*fn_friends_name)(void *friends);
typedef bool (*fn_friends_set_presence)(void *friends, const char *key, const char *value);
typedef const char *(*fn_friends_get_presence)(void *friends, csteamid friend_id, const char *key);
typedef void (*fn_friends_request_presence)(void *friends, csteamid friend_id);
typedef int (*fn_friends_count)(void *friends, int flags);
typedef csteamid (*fn_friends_by_index)(void *friends, int index, int flags);
typedef const char *(*fn_friends_persona_name)(void *friends, csteamid friend_id);
// FriendGameInfo_t from Steamworks is 24 bytes with the lobby ID at +16 on Windows x64.
struct FriendGameInfo {
    uint64_t game_id;
    uint32_t ip;
    uint16_t game_port, query_port;
    uint64_t lobby_id;
};
static_assert(sizeof(FriendGameInfo) == 24 && offsetof(FriendGameInfo, lobby_id) == 16, "Steam FriendGameInfo ABI");
typedef bool (*fn_friends_game_played)(void *friends, csteamid friend_id, FriendGameInfo *info);

typedef bool (*fn_net_send)(void *net, csteamid peer, const void *data, uint32_t len,
                            int send_type, int channel);
typedef bool (*fn_net_available)(void *net, uint32_t *size, int channel);
typedef bool (*fn_net_read)(void *net, void *dest, uint32_t capacity, uint32_t *size,
                            csteamid *peer, int channel);
typedef bool (*fn_net_accept)(void *net, csteamid peer);
typedef bool (*fn_net_relay)(void *net, bool allow);
typedef void (*fn_register_callback)(void *callback, int id);

struct steam_api {
    // interfaces
    fn_steamfriends_v017 friends_iface;
    fn_steamuser_v021 user_iface;
    fn_steamnetworking_v006 net_iface;
    fn_steamutils_v010 utils_iface;
    // functions
    fn_user_get_id user_get_id;
    fn_user_logged_on user_logged_on;
    fn_utils_app_id utils_app_id;
    fn_friends_name friends_name;
    fn_friends_set_presence set_presence;
    fn_friends_get_presence get_presence;
    fn_friends_request_presence request_presence;
    fn_friends_count friends_count;
    fn_friends_by_index friend_by_index;
    fn_friends_persona_name friend_persona;
    fn_friends_game_played friend_game_played;
    fn_net_send net_send;
    fn_net_available net_available;
    fn_net_read net_read;
    fn_net_accept net_accept;
    fn_net_relay net_relay;
    fn_register_callback register_callback;
};

// The 21 names, in the order the v20 module resolves them.
static const char *const kEntryPoints[] = {
    "SteamAPI_SteamFriends_v017", "SteamAPI_SteamUser_v021",
    "SteamAPI_SteamNetworking_v006", "SteamAPI_SteamUtils_v010",
    "SteamAPI_ISteamUser_GetSteamID", "SteamAPI_ISteamUser_BLoggedOn",
    "SteamAPI_ISteamUtils_GetAppID", "SteamAPI_ISteamFriends_GetPersonaName",
    "SteamAPI_ISteamFriends_SetRichPresence", "SteamAPI_ISteamFriends_GetFriendRichPresence",
    "SteamAPI_ISteamFriends_RequestFriendRichPresence", "SteamAPI_ISteamFriends_GetFriendCount",
    "SteamAPI_ISteamFriends_GetFriendByIndex", "SteamAPI_ISteamFriends_GetFriendPersonaName",
    "SteamAPI_ISteamFriends_GetFriendGamePlayed", "SteamAPI_ISteamNetworking_SendP2PPacket",
    "SteamAPI_ISteamNetworking_IsP2PPacketAvailable", "SteamAPI_ISteamNetworking_ReadP2PPacket",
    "SteamAPI_ISteamNetworking_AcceptP2PSessionWithUser",
    "SteamAPI_ISteamNetworking_AllowP2PPacketRelay", "SteamAPI_RegisterCallback",
};

// Steamworks callback bases are 1200 (networking) and 300 (friends): presence update is
// +36, while Join Game is +37. Treating 336 as the 264-byte join payload reads past a
// 16-byte presence update and never handles the real join event (337).
enum { CALLBACK_SESSION_REQUEST = 1202, CALLBACK_SESSION_FAIL = 1203,
       CALLBACK_PRESENCE_UPDATED = 336, CALLBACK_JOIN_REQUESTED = 337 };

// EP2PSend.  The game's protocol is written for UDP (it does its own pacing), so unreliable is the
// faithful default; k_EP2PSendReliable is one constant away if a test wants ordering instead.
enum { SEND_UNRELIABLE = 0, SEND_RELIABLE = 2 };
static const int kSendType = SEND_UNRELIABLE;

// --------------------------------------------------------------------------- callback plumbing
// Layout required by the flat API: vtable, one byte of flags, the callback id, then the payload.
// This is the SDK's CCallbackBase, restated so the module does not need the SDK headers.
class callback_base {
public:
    callback_base() : flags(0), id(0) {}
    // Steam's CCallbackBase ABI explicitly has NO virtual destructor. Adding one moves every
    // vtable slot, so Steam would call the wrong function for Run/GetCallbackSizeBytes.
    virtual void run(void *param) = 0;
    virtual void run(void *param, bool io_failure, uint64_t call) = 0;
    virtual int size_of() = 0;

    unsigned char flags;
    int id;
};

struct session_request_cb : callback_base {
    uint64_t remote;
    void run(void *param) override;
    void run(void *param, bool, uint64_t) override { run(param); }
    int size_of() override { return (int)sizeof(uint64_t); }
};

struct session_fail_cb : callback_base {
    uint64_t remote;
    unsigned char error;
    void run(void *param) override;
    void run(void *param, bool, uint64_t) override { run(param); }
    int size_of() override { return 16; }
};

struct join_requested_cb : callback_base {
    uint64_t friend_id;
    char connect[256];
    void run(void *param) override;
    void run(void *param, bool, uint64_t) override { run(param); }
    int size_of() override { return (int)(sizeof(uint64_t) + 256); }
};

struct presence_updated_cb : callback_base {
    void run(void *param) override;
    void run(void *param, bool, uint64_t) override { run(param); }
    int size_of() override { return 16; }     // FriendRichPresenceUpdate_t, Windows x64 packing
};

// --------------------------------------------------------------------------- state
static bool g_initialised;      // steam_init() has run to completion at least once
static bool g_ready;            // a transport is usable
static bool g_fake;             // MP_STEAM_FAKE stand-in instead of real Steam
static bool g_fake_requested;   // do not silently fall back to real Steam after a stand-in error
static bool g_off;              // MP_STEAM=0: explicitly no Steam
static std::atomic<bool> g_lobby_inbox(false); // one owner for the menu's Steam receive queue
static u64   g_retry_at;         // the game may load/initialise Steam after the network module
static steam_api g_api;
static void *g_friends;
static void *g_user;
static void *g_net;
static void *g_utils;
static uint64_t g_user_id;
static uint32_t g_app_id;
static int g_friend_count;
static char g_persona[STEAM_NAME_MAX] = {0};
static char g_presence[128] = {0};
static char g_connect[256] = {0};
static bool g_broadcasting;      // last "can be joined" state we told Steam about
static bool g_broadcast_valid;   // both Steam writes actually succeeded
static unsigned g_operation;     // and the operation it was for

static SOCKET g_fake_socket = INVALID_SOCKET;
static uint16_t g_fake_port;
static char g_fake_file[MAX_PATH] = {0};
static char g_fake_persona[STEAM_NAME_MAX] = {0};
static unsigned long long g_sent_packets, g_received_packets;   // since the last five-second line
static unsigned long long g_sent_total, g_received_total;
static u64   g_stats_at;

static session_request_cb g_cb_request;
static session_fail_cb g_cb_fail;
static join_requested_cb g_cb_join;
static presence_updated_cb g_cb_presence;

static uint64_t g_join_id;
static bool g_join_pending;
static const char *g_join_source = "";
static char g_command_line[2048] = {0};

// --------------------------------------------------------------------------- small helpers
static bool env_number(const char *name, uint64_t *out)
{
    char text[64] = {0};
    DWORD got = GetEnvironmentVariableA(name, text, (DWORD)sizeof(text));
    if (!got || got >= sizeof(text))
        return false;
    return sscanf_s(text, "%llu", out) == 1;
}

static void sanitise(char *text, size_t size)
{
    // Persona names come from outside; everything but letters, digits, '_' and '-' becomes '_',
    // so a name can never inject a path or a command (v20 does the same).
    for (size_t i = 0; i < size && text[i]; ++i) {
        unsigned char c = (unsigned char)text[i];
        bool ok = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')
                  || c == '_' || c == '-';
        if (!ok)
            text[i] = '_';
    }
}

static void copy_text(char *dest, size_t size, const char *source)
{
    if (!source)
        source = "";
    size_t length = strlen(source);
    if (length >= size)
        length = size - 1;
    memcpy(dest, source, length);
    dest[length] = 0;
}

// --------------------------------------------------------------------------- stand-in mode
static bool fake_open(uint16_t port)
{
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return false;
    g_fake_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (g_fake_socket == INVALID_SOCKET) {
        WSACleanup();
        return false;
    }
    sockaddr_in address = {};
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (bind(g_fake_socket, (sockaddr *)&address, sizeof(address)) != 0) {
        closesocket(g_fake_socket);
        g_fake_socket = INVALID_SOCKET;
        WSACleanup();
        return false;
    }
    u_long nonblocking = 1;
    ioctlsocket(g_fake_socket, FIONBIO, &nonblocking);

    char temp[MAX_PATH] = {0};
    DWORD got = GetTempPathA((DWORD)sizeof(temp), temp);
    if (got && got < sizeof(temp))
        sprintf_s(g_fake_file, sizeof(g_fake_file), "%sgtmf_fake_steam_%u.txt", temp, port);
    return true;
}

static void fake_flush()
{
    // The stand-in keeps its notes in a file the way v20's does: a second process can read where the
    // first one says it is, without any Steam involvement.  Two keys, like real rich presence.
    if (!g_fake_file[0])
        return;
    FILE *file = nullptr;
    if (fopen_s(&file, g_fake_file, "w") == 0 && file) {
        fprintf(file, "persona=%s\ngtmf=%s\nconnect=%s\n", g_fake_persona, g_presence, g_connect);
        fclose(file);
    }
}

// --------------------------------------------------------------------------- callbacks
void session_request_cb::run(void *param)
{
    if (!param)
        return;
    uint64_t from = *(uint64_t *)param;
    if (g_api.net_accept && g_net) {
        g_api.net_accept(g_net, from);
        steam_log("steam: session asked for by %llu - accepted", from);
        return;
    }
    steam_log("steam: session asked for by %llu - dropped (no networking interface)", from);
}

void session_fail_cb::run(void *param)
{
    if (!param)
        return;
    uint64_t from = *(uint64_t *)param;
    unsigned char error = *((unsigned char *)param + 8);
    steam_log("steam: no connection to %llu could be made (error %u)", from, error);
}

void join_requested_cb::run(void *param)
{
    if (!param) return;
    // Steam supplies a fixed 256-byte field, which need not end in NUL. Its contents can be either
    // steam:<id> or the full connect string published by steam_broadcast().
    char connect[257];
    memcpy(connect, (const char *)param + 8, 256);
    connect[256] = 0;
    steam_log("steam: Steam's Join Game asks to join %.256s (a friend's game, while this game runs)", connect);
    if (_strnicmp(connect, "steam:", 6) == 0) {
        g_join_id = _strtoui64(connect + 6, nullptr, 10);
        g_join_pending = g_join_id != 0;
        if (g_join_pending) g_join_source = "a friend's game";
    } else if (const char *marker = strstr(connect, "+gtmf_join=steam:")) {
        uint64_t id = _strtoui64(marker + strlen("+gtmf_join=steam:"), nullptr, 10);
        if (id) {
            g_join_id = id;
            g_join_pending = true;
            g_join_source = "a friend's game";
        }
    }
}

void presence_updated_cb::run(void *param)
{
    (void)param; // presence update 336 is not a Join Game request
}

// --------------------------------------------------------------------------- interface binding
static bool bind_interfaces(HMODULE library)
{
    if (!library) {
        steam_log("steam: the game has not loaded Steam's library - no Steam joining");
        return false;
    }
    void *slots[21] = {0};
    for (int i = 0; i < 21; ++i) {
        slots[i] = (void *)GetProcAddress(library, kEntryPoints[i]);
        if (!slots[i]) {
            // Name the missing entry point: after a game update this is the line that explains it.
            steam_log("steam: steam_api64.dll has no %s - no Steam joining", kEntryPoints[i]);
            return false;
        }
    }
    g_api.friends_iface = (fn_steamfriends_v017)slots[0];
    g_api.user_iface = (fn_steamuser_v021)slots[1];
    g_api.net_iface = (fn_steamnetworking_v006)slots[2];
    g_api.utils_iface = (fn_steamutils_v010)slots[3];
    g_api.user_get_id = (fn_user_get_id)slots[4];
    g_api.user_logged_on = (fn_user_logged_on)slots[5];
    g_api.utils_app_id = (fn_utils_app_id)slots[6];
    g_api.friends_name = (fn_friends_name)slots[7];
    g_api.set_presence = (fn_friends_set_presence)slots[8];
    g_api.get_presence = (fn_friends_get_presence)slots[9];
    g_api.request_presence = (fn_friends_request_presence)slots[10];
    g_api.friends_count = (fn_friends_count)slots[11];
    g_api.friend_by_index = (fn_friends_by_index)slots[12];
    g_api.friend_persona = (fn_friends_persona_name)slots[13];
    g_api.friend_game_played = (fn_friends_game_played)slots[14];
    g_api.net_send = (fn_net_send)slots[15];
    g_api.net_available = (fn_net_available)slots[16];
    g_api.net_read = (fn_net_read)slots[17];
    g_api.net_accept = (fn_net_accept)slots[18];
    g_api.net_relay = (fn_net_relay)slots[19];
    g_api.register_callback = (fn_register_callback)slots[20];
    return true;
}

// --------------------------------------------------------------------------- lifecycle
bool steam_init()
{
    if (g_initialised) {
        if (g_ready || g_off || g_fake_requested) return g_ready;
        // basic_steam.codelib imports steam_api64.dll, but it can be loaded after this module.
        // Never load or initialise a foreign DLL ourselves; retry the game's own library once it
        // exists, at most once per five seconds while its interfaces are still being brought up.
        u64 now = GetTickCount64();
        if (!GetModuleHandleA("steam_api64.dll") || now - g_retry_at < 5000) return false;
    }
    g_initialised = true;
    g_retry_at = GetTickCount64();

    uint64_t value = 0;
    if (env_number("MP_STEAM", &value) && value == 0) {
        g_off = true;
        steam_log("steam: switched off (MP_STEAM=0)");
        return false;
    }

    if (env_number("MP_STEAM_FAKE", &value) && value != 0) {
        g_fake_requested = true;
        if (value > 65535) {
            steam_log("steam: test stand-in port %llu is outside 1..65535", value);
            return false;
        }
        g_fake = true;
        g_fake_port = (uint16_t)value;
        if (!fake_open(g_fake_port)) {
            steam_log("steam: test stand-in: UDP port %llu is taken", value);
            g_fake = false;
            return false;
        }
        g_user_id = value;
        g_app_id = 0;
        sprintf_s(g_persona, sizeof(g_persona), "steam:%llu", value);
        g_ready = true;
        copy_text(g_fake_persona, sizeof(g_fake_persona), g_persona);
        steam_log("steam: TEST STAND-IN for Steam: this game is 'steam:%llu',"
                  " its datagrams travel over UDP on this PC", value);
        fake_flush();
        return true;
    }

#if defined(__MINGW32__) && !defined(_MSC_VER)
    // The GNU C++ ABI orders overloaded virtuals differently from the MSVC Steam DLL. This
    // toolchain is used for cross-compilation checks only; never register incompatible callbacks.
    g_off = true;
    steam_log("steam: incompatible GNU callback ABI; build with the Windows MSVC toolchain%s", "");
    return false;
#endif
    HMODULE library = GetModuleHandleA("steam_api64.dll");
    if (!bind_interfaces(library))
        return false;

    g_friends = g_api.friends_iface ? g_api.friends_iface() : nullptr;
    g_user = g_api.user_iface ? g_api.user_iface() : nullptr;
    g_net = g_api.net_iface ? g_api.net_iface() : nullptr;
    g_utils = g_api.utils_iface ? g_api.utils_iface() : nullptr;
    if (!g_friends || !g_user || !g_net || !g_utils) {
        steam_log("steam: Steam is not running for this game - no Steam joining");
        return false;
    }

    g_user_id = g_api.user_get_id(g_user);
    g_app_id = g_api.utils_app_id(g_utils);
    const char *persona = g_api.friends_name(g_friends);
    copy_text(g_persona, sizeof(g_persona), persona ? persona : "");
    sanitise(g_persona, sizeof(g_persona));
    if (!g_user_id) {
        steam_log("steam: no Steam user - no Steam joining");
        return false;
    }

    g_api.net_relay(g_net, true);
    g_cb_request.id = CALLBACK_SESSION_REQUEST;
    g_cb_fail.id = CALLBACK_SESSION_FAIL;
    g_cb_join.id = CALLBACK_JOIN_REQUESTED;
    g_cb_presence.id = CALLBACK_PRESENCE_UPDATED;
    g_api.register_callback(&g_cb_request, CALLBACK_SESSION_REQUEST);
    g_api.register_callback(&g_cb_fail, CALLBACK_SESSION_FAIL);
    g_api.register_callback(&g_cb_presence, CALLBACK_PRESENCE_UPDATED);
    g_api.register_callback(&g_cb_join, CALLBACK_JOIN_REQUESTED);
    g_api.request_presence(g_friends, g_user_id);

    g_friend_count = g_api.friends_count ? g_api.friends_count(g_friends, 4) : 0;
    g_ready = true;
    steam_log("steam: ready - user %llu (%s), application %u, %d friends, logged on: %s",
              g_user_id, g_persona, g_app_id, g_friend_count,
              g_api.user_logged_on(g_user) ? "yes" : "no");
    steam_log("steam: Steam's callbacks arrive in this program");
    return true;
}

bool steam_ready() { return g_ready; }
bool steam_is_fake() { return g_fake; }
uint64_t steam_user_id() { return g_user_id; }
const char *steam_user_name() { return g_ready ? g_persona : ""; }
uint32_t steam_app_id() { return g_app_id; }
int steam_friend_count() { return g_friend_count; }

namespace {
struct SteamFriendsSource {
    int count() { return g_api.friends_count(g_friends, 4); } // k_EFriendFlagImmediate
    uint64_t id(int index) { return g_api.friend_by_index(g_friends, index, 4); }
    bool game(uint64_t id, uint64_t *game_id) {
        FriendGameInfo info = {};
        if (!g_api.friend_game_played(g_friends, id, &info)) return false;
        *game_id = info.game_id;
        return true;
    }
    const char *name(uint64_t id) { return g_api.friend_persona(g_friends, id); }
    const char *note(uint64_t id) { return g_api.get_presence(g_friends, id, "gtmf"); }
    void request(uint64_t id) { g_api.request_presence(g_friends, id); }
};
}
size_t steam_friends_playing(lobby_friends::Friend *out, size_t capacity, bool request_presence)
{
    if (!g_ready || g_fake || !g_friends || !g_api.friends_count || !g_api.friend_by_index ||
        !g_api.friend_game_played || !g_api.friend_persona || !g_api.get_presence || !g_api.request_presence)
        return 0;
    SteamFriendsSource source;
    return lobby_friends::collect(source, g_app_id, g_user_id, out, capacity, request_presence);
}

bool steam_lobby_claim()
{
    bool expected = false;
    return g_lobby_inbox.compare_exchange_strong(expected, true, std::memory_order_acq_rel);
}
void steam_lobby_release() { g_lobby_inbox.store(false, std::memory_order_release); }
bool steam_lobby_receiving() { return g_lobby_inbox.load(std::memory_order_acquire); }

void steam_shutdown()
{
    steam_lobby_release();
    if (g_fake_socket != INVALID_SOCKET) {
        closesocket(g_fake_socket);
        g_fake_socket = INVALID_SOCKET;
        WSACleanup();
    }
    g_ready = false;
}

void steam_traffic_totals(unsigned long long *sent, unsigned long long *received)
{
    if (sent)
        *sent = g_sent_total;
    if (received)
        *received = g_received_total;
}

// v20 counts what actually crosses the transport and says so every five seconds: in a battle that
// is the only line that tells "Steam is being used" from "Steam was initialised and nothing goes
// through it".  The stand-in carries the same counter, so the number is meaningful without Steam.
static void report_traffic()
{
    u64 now = GetTickCount64();
    if (now - g_stats_at < 5000)
        return;
    g_stats_at = now;
    if (!g_sent_packets && !g_received_packets)
        return;
    steam_log("steam: 5 s: %llu datagrams sent through Steam, %llu received%s",
              g_sent_packets, g_received_packets, g_fake ? " (test stand-in)" : "");
    steam_traffic_note(g_sent_packets, g_received_packets);
    g_sent_packets = g_received_packets = 0;
}

void steam_frame()
{
    if (!g_ready)
        return;
    report_traffic();
    if (g_fake)
        return;
    // RunCallbacks is what makes the registered callbacks fire.  It lives in the game's
    // steam_api64.dll as a flat export; we resolve it lazily rather than add a 22nd slot for it.
    static void (*run_callbacks)() = nullptr;
    if (!run_callbacks) {
        HMODULE library = GetModuleHandleA("steam_api64.dll");
        if (library)
            run_callbacks = (void (*)())GetProcAddress(library, "SteamAPI_RunCallbacks");
    }
    if (run_callbacks)
        run_callbacks();
}

// --------------------------------------------------------------------------- datagrams
bool steam_send(uint64_t peer, const void *data, int len)
{
    if (!g_ready || !data || len <= 0)
        return false;
    if (g_fake) {
        sockaddr_in address = {};
        address.sin_family = AF_INET;
        address.sin_port = htons((uint16_t)peer);
        address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        int sent = sendto(g_fake_socket, (const char *)data, len, 0, (sockaddr *)&address,
                          sizeof(address));
        if (sent == len) {
            ++g_sent_packets;
            ++g_sent_total;
        }
        return sent == len;
    }
    bool ok = g_api.net_send(g_net, peer, data, (uint32_t)len, kSendType, STEAM_CHANNEL);
    if (ok) {
        ++g_sent_packets;
        ++g_sent_total;
    }
    return ok;
}

int steam_recv(uint64_t *peer, void *data, int capacity)
{
    if (!g_ready || !data || capacity <= 0)
        return -1;
    if (g_fake) {
        sockaddr_in address = {};
        int size = (int)sizeof(address);
        int got = recvfrom(g_fake_socket, (char *)data, capacity, 0, (sockaddr *)&address, &size);
        if (got <= 0)
            return 0;
        if (peer)
            *peer = ntohs(address.sin_port);
        ++g_received_packets;
        ++g_received_total;
        return got;
    }
    uint32_t size = 0;
    if (g_api.net_available(g_net, &size, STEAM_CHANNEL) && size > 0) {
        uint32_t read = 0;
        csteamid from = 0;
        if (g_api.net_read(g_net, data, (uint32_t)capacity, &read, &from, STEAM_CHANNEL)) {
            if (peer)
                *peer = from;
            ++g_received_packets;
            ++g_received_total;
            return (int)read;
        }
    }
    return 0;
}

// --------------------------------------------------------------------------- presence and join
bool steam_presence(const char *value)
{
    if (!g_ready || !value)
        return false;
    copy_text(g_presence, sizeof(g_presence), value);
    if (g_fake) {
        fake_flush();
        return true;
    }
    return g_api.set_presence && g_api.set_presence(g_friends, "gtmf", g_presence);
}

const char *steam_presence_value() { return g_presence; }

const char *steam_connect_text() { return g_connect; }

bool steam_broadcast(bool hosting, unsigned operation)
{
    if (!g_ready)
        return false;

    char note[32] = {0};
    char connect[256] = {0};
    if (hosting) {
        // The same words v20 puts there, character for character: a friend's Steam client shows
        // "Join Game" from the second key, and the first one is what the lobby list is keyed on.
        sprintf_s(note, sizeof(note), "%u game", operation);
        sprintf_s(connect, sizeof(connect), "root\\programs\\opgame.progpack, +gtmf_join=steam:%llu",
                  (unsigned long long)g_user_id);
    }

    if (g_broadcast_valid && g_broadcasting == hosting && g_operation == operation &&
        strcmp(g_presence, note) == 0 && strcmp(g_connect, connect) == 0) {
        if (g_fake) return true;
        // The menu and tactical programs can overlap. One may clear the account's presence after
        // this process advertised it. Verify both keys before treating the cached result as valid.
        const char *held_note = g_api.get_presence ? g_api.get_presence(g_friends, g_user_id, "gtmf") : nullptr;
        bool same_note = strcmp(held_note ? held_note : "", note) == 0;
        const char *held_connect = g_api.get_presence ? g_api.get_presence(g_friends, g_user_id, "connect") : nullptr;
        if (same_note && strcmp(held_connect ? held_connect : "", connect) == 0) return true;
    }

    copy_text(g_presence, sizeof(g_presence), note);
    copy_text(g_connect, sizeof(g_connect), connect);
    g_broadcasting = hosting;
    g_operation = operation;
    g_broadcast_valid = false;

    if (g_fake) {
        fake_flush();
        g_broadcast_valid = true;
        if (hosting)
            steam_log("steam: friends are told that this game can be joined (%s)", g_presence);
        else
            steam_log("steam: no longer shown to friends as a game to join%s", "");
        return true;
    }

    bool noted = g_api.set_presence && g_api.set_presence(g_friends, "gtmf", g_presence);
    const char *held = g_api.get_presence ? g_api.get_presence(g_friends, g_user_id, "gtmf") : nullptr;
    if (!noted || strcmp(held ? held : "", g_presence) != 0)
        steam_log("steam: Steam did not take the note for friends (asked for '%s', it holds '%s')",
                  g_presence, held ? held : "");
    bool connected = g_api.set_presence && g_api.set_presence(g_friends, "connect", g_connect);
    if (!connected) steam_log("steam: Steam did not take the \"connect\" text for its Join Game");
    // A failed attempt must not be cached as a success: a later frame can retry it.
    g_broadcast_valid = noted && connected;
    if (!g_broadcast_valid) return false;
    if (hosting)
        steam_log("steam: friends are told that this game can be joined (%s)", g_presence);
    else
        steam_log("steam: no longer shown to friends as a game to join%s", "");
    return true;
}

bool steam_presence_check(char *out, size_t size)
{
    if (out && size)
        out[0] = 0;
    if (!g_ready || g_fake)
        return false;
    const char *held = g_api.get_presence ? g_api.get_presence(g_friends, g_user_id, "gtmf") : nullptr;
    if (out && size)
        copy_text(out, size, held ? held : "");
    if (!held || strcmp(held, g_presence) != 0) {
        steam_log("steam: Steam did not take the note for friends (asked for '%s', it holds '%s')",
                  g_presence, held ? held : "");
        return false;
    }
    return true;
}

void steam_note_command_line(const char *command_line)
{
    copy_text(g_command_line, sizeof(g_command_line), command_line ? command_line : "");
    const char *marker = strstr(g_command_line, "+gtmf_join=");
    if (!marker)
        return;
    marker += strlen("+gtmf_join=");
    if (_strnicmp(marker, "steam:", 6) != 0) {
        steam_log("steam: a join request that is not a multiplayer game's ('%.80s') - ignored", marker);
        return;
    }
    uint64_t id = _strtoui64(marker + 6, nullptr, 10);
    if (!id)
        return;
    g_join_id = id;
    g_join_pending = true;
    g_join_source = "the command line";
    steam_log("steam: Steam's Join Game asks to join steam:%llu (on the command line)", id);
}

bool steam_join_take(uint64_t *id)
{
    if (!g_join_pending)
        return false;
    if (id)
        *id = g_join_id;
    g_join_pending = false;
    return true;
}

const char *steam_join_source() { return g_join_source; }

#ifdef NETV21_TEST_HOOKS
bool steam_test_wait_packet(unsigned timeout_ms)
{
    if (!g_fake || g_fake_socket == INVALID_SOCKET) return false;
    fd_set readable;
    FD_ZERO(&readable); FD_SET(g_fake_socket, &readable);
    timeval timeout = { (long)(timeout_ms / 1000), (long)((timeout_ms % 1000) * 1000) };
    return select(0, &readable, nullptr, nullptr, &timeout) > 0;
}
#endif

void steam_test_set_command_line(const char *text) { steam_note_command_line(text); }

void steam_test_join_callback(const char *text, size_t size)
{
    unsigned char param[8 + 256] = {};
    if (text) memcpy(param + 8, text, size < 256 ? size : 256);
    g_cb_join.run(param);
}

bool steam_test_callback_abi()
{
    // Invoke the actual compiler-generated slots as SteamAPI does, not a direct C++ call.
    // x64 CCallbackBase is vptr + flag byte + padded callback id. MSVC reverses the two
    // overloaded Run slots; the Itanium ABI used by the cross-compiler keeps declaration order.
    callback_base *callbacks[] = { &g_cb_request, &g_cb_fail, &g_cb_presence, &g_cb_join };
    const int sizes[] = { 8, 16, 16, 264 };
    for (int i = 0; i < 4; i++) {
        callback_base *cb = callbacks[i];
        ptrdiff_t flags_at = (char *)&cb->flags - (char *)cb, id_at = (char *)&cb->id - (char *)cb;
        if (flags_at != 8 || id_at != 12) {
            steam_log("steam ABI selftest: callback %d fields at %td/%td instead of 8/12", i, flags_at, id_at);
            return false;
        }
        void **vt = *(void ***)cb;
        int actual = ((int (*)(callback_base *))vt[2])(cb);
        if (actual != sizes[i]) {
            steam_log("steam ABI selftest: callback %d slot 2 returned %d instead of %d", i, actual, sizes[i]);
            return false;
        }
    }
    if (CALLBACK_PRESENCE_UPDATED != 336 || CALLBACK_JOIN_REQUESTED != 337) return false;
    unsigned char payload[264] = {};
    memcpy(payload + 8, "steam:42", 9);
    void **vt = *(void ***)&g_cb_join;
#if defined(_MSC_VER)
    const int single = 1, multi = 0;
#else
    const int single = 0, multi = 1; // e.g. the GNU/Itanium ABI used by the syntax-only build
#endif
    ((void (*)(callback_base *, void *))vt[single])(&g_cb_join, payload);
    if (!g_join_pending || g_join_id != 42) {
        steam_log("steam ABI selftest: single-argument Run dispatch failed%s", "");
        return false;
    }
    g_join_pending = false;
    g_join_id = 0;
    ((void (*)(callback_base *, void *, bool, uint64_t))vt[multi])(&g_cb_join, payload, false, 0);
    return g_join_pending && g_join_id == 42;
}

void steam_test_reset()
{
    steam_lobby_release();
    g_initialised = false;
    g_ready = false;
    g_fake = false;
    g_fake_requested = false;
    g_off = false;
    g_retry_at = 0;
    g_join_pending = false;
    g_join_id = 0;
    g_presence[0] = 0;
    g_connect[0] = 0;
    g_broadcasting = false;
    g_broadcast_valid = false;
    g_operation = 0;
    g_sent_packets = g_received_packets = g_sent_total = g_received_total = 0;
    g_stats_at = 0;
    if (g_fake_socket != INVALID_SOCKET) {
        closesocket(g_fake_socket);
        g_fake_socket = INVALID_SOCKET;
        WSACleanup();
    }
}

// Self-test for the Steam layer: runs on a machine with no Steam at all.
//
// It exercises what can be exercised without a second player - the stand-in transport, the
// datagram path end to end over loopback, the join-request parser and the off switch - and prints
// one line per check.  steam_log comes from steam_logger.cpp, the same one mp_join.exe links.  The parts that need real Steam (rich presence, callbacks, P2P relay) are
// logged but cannot be asserted here; they are what the two-PC run is for.
#include "steam.h"

#include <stdio.h>
#include <string.h>

static int g_failures;
static int g_checks;

static void check(bool condition, const char *what)
{
    ++g_checks;
    if (!condition)
        ++g_failures;
    printf("%s %s\n", condition ? "OK  " : "FAIL", what);
}

static void set_env(const char *name, const char *value)
{
    if (value)
        SetEnvironmentVariableA(name, value);
    else
        SetEnvironmentVariableA(name, nullptr);
}

int main()
{
    printf("steam layer self-test\n");

    // ---- 1. the off switch must win over everything else
    set_env("MP_STEAM", "0");
    set_env("MP_STEAM_FAKE", nullptr);
    steam_test_reset();
    check(!steam_init(), "MP_STEAM=0 turns the layer off");
    check(!steam_ready(), "and reports itself not ready");

    set_env("MP_STEAM", nullptr);
    set_env("MP_STEAM_FAKE", "65536");
    steam_test_reset();
    check(!steam_init() && !steam_ready(), "an out-of-range stand-in port is refused");
    check(!steam_init(), "a failed stand-in does not silently switch to real Steam");

    // ---- 2. stand-in mode
    const unsigned short port = 39917;
    char text[32];
    sprintf_s(text, sizeof(text), "%u", port);
    set_env("MP_STEAM", nullptr);
    set_env("MP_STEAM_FAKE", text);
    steam_test_reset();
    check(steam_init(), "MP_STEAM_FAKE starts the stand-in");
    check(steam_is_fake(), "the stand-in reports itself as such");
    check(steam_user_id() == port, "its identity is the port it was given");
    sprintf_s(text, sizeof(text), "steam:%u", port);
    check(strcmp(steam_user_name(), text) == 0, "and its persona name is steam:<port>");

    // ---- 3. a datagram in both directions over loopback
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
    SOCKET helper = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    sockaddr_in local = {};
    local.sin_family = AF_INET;
    local.sin_port = 0;
    local.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    bind(helper, (sockaddr *)&local, sizeof(local));
    int length = (int)sizeof(local);
    getsockname(helper, (sockaddr *)&local, &length);
    unsigned short helper_port = ntohs(local.sin_port);

    check(steam_send(helper_port, "ping", 4), "a datagram is accepted for a peer");
    char buffer[64] = {0};
    sockaddr_in from = {};
    int from_size = (int)sizeof(from);
    u_long nonblocking = 1;
    ioctlsocket(helper, FIONBIO, &nonblocking);
    int got = recvfrom(helper, buffer, sizeof(buffer), 0, (sockaddr *)&from, &from_size);
    check(got == 4 && memcmp(buffer, "ping", 4) == 0, "it arrives at the peer unchanged");

    sockaddr_in steam_addr = {};
    steam_addr.sin_family = AF_INET;
    steam_addr.sin_port = htons(port);
    steam_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    sendto(helper, "pong", 4, 0, (sockaddr *)&steam_addr, sizeof(steam_addr));
    uint64_t peer = 0;
    got = steam_recv(&peer, buffer, sizeof(buffer));
    check(got == 4 && memcmp(buffer, "pong", 4) == 0, "a datagram from the peer is delivered");
    check(peer == helper_port, "with the peer's number attached");
    check(steam_recv(&peer, buffer, sizeof(buffer)) == 0, "and an empty queue says so");

    // ---- 4a. the address of a Steam peer: same struct as a UDP peer, another family
    sockaddr_in address;
    check(steam_peer_make(76561198000000042ULL, &address), "a Steam ID makes a peer address");
    check(address.sin_family == 0x5354, "with the family v20 marks Steam peers with");
    check(steam_peer_id(&address) == 76561198000000042ULL, "and the ID reads back unchanged");
    sockaddr_in plain = {};
    plain.sin_family = AF_INET;
    check(!steam_peer_make(0, &plain) && !steam_peer_make(1, nullptr),
          "an empty ID or no address is refused");
    check(steam_peer_id(&plain) == 0, "and a UDP peer has no Steam ID");

    // ---- 4. presence in stand-in mode is a note, never a failure
    check(steam_presence("17 cloud"), "presence is recorded in stand-in mode");
    check(strcmp(steam_presence_value(), "17 cloud") == 0, "and reads back");

    // ---- 4b. the totals behind the five-second line v20 prints
    unsigned long long sent_total = 0, received_total = 0;
    steam_traffic_totals(&sent_total, &received_total);
    check(sent_total >= 1 && received_total >= 1, "the transport counted the datagrams it moved");

    // ---- 5. what friends are shown: both presence keys v20 uses, word for word
    check(steam_broadcast(true, 17), "hosting is advertised to friends");
    check(strcmp(steam_presence_value(), "17 game") == 0,
          "the note says which operation is being played");
    char expected[256];
    sprintf_s(expected, sizeof(expected), "root\\programs\\opgame.progpack, +gtmf_join=steam:%llu",
              (unsigned long long)steam_user_id());
    check(strcmp(steam_connect_text(), expected) == 0,
          "and Join Game carries the progpack plus the steam: address");
    check(steam_broadcast(true, 17), "the same successful broadcast is idempotent");
    check(steam_presence("different"), "a changed presence can be detected");
    check(steam_broadcast(true, 17) && strcmp(steam_presence_value(), "17 game") == 0,
          "advertising restores a note changed after the first broadcast");
    check(steam_broadcast(false, 0), "leaving the game is advertised too");
    check(steam_presence_value()[0] == 0 && steam_connect_text()[0] == 0,
          "with both notes cleared, so friends see nothing to join");

    // ---- 6. join requests off the command line
    steam_note_command_line("\"gtmp.exe\" +gtmf_join=steam:76561198000000042");
    uint64_t join = 0;
    check(steam_join_take(&join) && join == 76561198000000042ULL, "a steam: join request is parsed");
    check(!steam_join_take(&join), "and is taken only once");
    steam_note_command_line("\"gtmp.exe\" +gtmf_join=not-a-game");
    check(!steam_join_take(&join), "a foreign join request is ignored");
    const char *full = "root\\programs\\opgame.progpack, +gtmf_join=steam:76561198000000042";
    steam_test_join_callback(full, strlen(full));
    check(steam_join_take(&join) && join == 76561198000000042ULL &&
          strcmp(steam_join_source(), "a friend's game") == 0,
          "the full advertised connect string works in a friend's callback");
    char unterminated[256];
    memset(unterminated, 'x', sizeof unterminated);
    const char *short_request = "steam:42";
    memcpy(unterminated, short_request, strlen(short_request));
    steam_test_join_callback(unterminated, sizeof unterminated);
    check(steam_join_take(&join) && join == 42, "a non-NUL-terminated callback stays within its field");

    steam_shutdown();
    closesocket(helper);
    WSACleanup();

    printf("%d checks, %d failures\n", g_checks, g_failures);
    return g_failures ? 1 : 0;
}

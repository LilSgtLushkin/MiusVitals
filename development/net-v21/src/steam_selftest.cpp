// Self-test for the Steam layer: runs on a machine with no Steam at all.
//
// It exercises what can be exercised without a second player - the stand-in transport, the
// datagram path end to end over loopback, the join-request parser and the off switch - and prints
// one line per check.  The parts that need real Steam (rich presence, callbacks, P2P relay) are
// logged but cannot be asserted here; they are what the two-PC run is for.
#include "steam.h"

#include <stdio.h>
#include <string.h>

static int g_failures;
static int g_checks;

void steam_log(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    printf("    log: ");
    vprintf(fmt, args);
    printf("\n");
    va_end(args);
}

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

    sendto(helper, "pong", 4, 0, (sockaddr *)&local, sizeof(local));
    uint64_t peer = 0;
    got = steam_recv(&peer, buffer, sizeof(buffer));
    check(got == 4 && memcmp(buffer, "pong", 4) == 0, "a datagram from the peer is delivered");
    check(peer == helper_port, "with the peer's number attached");
    check(steam_recv(&peer, buffer, sizeof(buffer)) == 0, "and an empty queue says so");

    // ---- 4. presence in stand-in mode is a note, never a failure
    check(steam_presence("17 cloud"), "presence is recorded in stand-in mode");
    check(strcmp(steam_presence_value(), "17 cloud") == 0, "and reads back");

    // ---- 5. join requests off the command line
    steam_note_command_line("\"gtmp.exe\" +gtmf_join=steam:76561198000000042");
    uint64_t join = 0;
    check(steam_join_take(&join) && join == 76561198000000042ULL, "a steam: join request is parsed");
    check(!steam_join_take(&join), "and is taken only once");
    steam_note_command_line("\"gtmp.exe\" +gtmf_join=not-a-game");
    check(!steam_join_take(&join), "a foreign join request is ignored");

    steam_shutdown();
    closesocket(helper);
    WSACleanup();

    printf("%d checks, %d failures\n", g_checks, g_failures);
    return g_failures ? 1 : 0;
}

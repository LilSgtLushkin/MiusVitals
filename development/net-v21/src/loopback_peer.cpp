// Two independent processes using the real packet structs and the production reliable inbox.
// Deliberately loses, duplicates and reverses datagrams. This is a protocol/queue test, not the game.
#include "protocol.h"
#include "reliable_inbox.h"
#include <chrono>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>
#include <vector>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
using Socket = SOCKET;
static const Socket bad_socket = INVALID_SOCKET;
static void close_socket(Socket s) { closesocket(s); }
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
using Socket = int;
static const Socket bad_socket = -1;
static void close_socket(Socket s) { close(s); }
#endif

using Clock = std::chrono::steady_clock;
static unsigned long long millis() {
    return (unsigned long long)std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now().time_since_epoch()).count();
}

int main(int argc, char **argv)
{
    if (argc != 4) { fprintf(stderr, "usage: peer <1|2> <local-port> <peer-port>\n"); return 2; }
    const int id = atoi(argv[1]);
    const int port = atoi(argv[2]), peer_port = atoi(argv[3]);
    if ((id != 1 && id != 2) || port <= 0 || port > 65535 || peer_port <= 0 || peer_port > 65535) return 2;
#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa)) return 2;
#endif
    Socket sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == bad_socket) return 2;
    sockaddr_in local = {}, peer = {};
    local.sin_family = peer.sin_family = AF_INET;
    local.sin_addr.s_addr = peer.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    local.sin_port = htons((unsigned short)port);
    peer.sin_port = htons((unsigned short)peer_port);
    if (bind(sock, (sockaddr *)&local, sizeof local) != 0) { close_socket(sock); return 2; }
#ifdef _WIN32
    u_long nonblocking = 1;
    ioctlsocket(sock, FIONBIO, &nonblocking);
#else
    fcntl(sock, F_SETFL, fcntl(sock, F_GETFL, 0) | O_NONBLOCK);
#endif
    unsigned attempted = 0, dropped = 0, duplicates = 0, gaps = 0, data_sent = 0;
    auto send_packet = [&](const void *data, int size, bool may_drop) {
        if (may_drop && (++attempted % 7) == 0) { ++dropped; return; }
        if (sendto(sock, (const char *)data, size, 0, (const sockaddr *)&peer, sizeof peer) == size)
            ++data_sent;
    };
    const unsigned total = 64;
    const unsigned epoch = 0x6a000000u + (unsigned)id;
    reliable::Inbox<ShotRec> inbox;
    std::vector<unsigned> applied(total + 1, 0);
    unsigned ack = 0;
    bool established = false, got_end = false, bad = false;
    unsigned long long start = millis(), last_hello = 0, last_events = 0, last_end = 0;
    unsigned long long first_event = 0, finished_at = 0;
    while (millis() - start < 15000) {
        unsigned long long now = millis();
        // The joining peer announces its protocol, then confirms receipt of the host's welcome.
        if (id == 2 && now - last_hello >= 40) {
            Hello h = {};
            h.h = make_head(PKT_HELLO);
            memcpy(h.proto, k_protocol, sizeof k_protocol);
            h.version = k_version;
            h.ready = established ? 1 : 0;
            send_packet(&h, sizeof h, false);
            last_hello = now;
        }
        if (established && now - last_events >= 25 && ack < total) {
            unsigned last = ack + 24 < total ? ack + 24 : total;
            // Reverse order creates gaps. Retransmission fills them and tests duplicate suppression.
            for (unsigned seq = last; seq > ack; --seq) {
                struct Datagram { ShotHead head; ShotRec record; } d = {};
                d.head.h = make_head(PKT_SHOT, 1);
                d.head.epoch = epoch;
                d.record.seq = seq;
                d.record.kind = (unsigned char)(seq % 4);
                d.record.ammo = seq ^ (0x5a00u + (unsigned)id);
                d.record.launch[0] = (unsigned char)(seq ^ (unsigned)id);
                send_packet(&d, sizeof d, true);
                if ((seq % 11) == 0) send_packet(&d, sizeof d, true);
            }
            last_events = now;
        }
        if (established && now - last_end >= 100) {
            EndState end = { make_head(PKT_END), 12345, 0 };
            end.h.flags = 1;
            send_packet(&end, sizeof end, false);
            last_end = now;
        }
        for (int count = 0; count < 512; ++count) {
            unsigned char buffer[2048];
            sockaddr_in from = {};
#ifdef _WIN32
            int from_size = sizeof from;
#else
            socklen_t from_size = sizeof from;
#endif
            int n = recvfrom(sock, (char *)buffer, sizeof buffer, 0, (sockaddr *)&from, &from_size);
            if (n <= 0) break;
            if (from.sin_family != AF_INET || from.sin_addr.s_addr != peer.sin_addr.s_addr || from.sin_port != peer.sin_port || n < (int)sizeof(Head)) continue;
            const Head *h = (const Head *)buffer;
            if (h->magic != k_magic) continue;
            if (id == 1 && h->type == PKT_HELLO && n == (int)sizeof(Hello)) {
                const Hello *hello = (const Hello *)buffer;
                if (hello->version != k_version || memcmp(hello->proto, k_protocol, sizeof k_protocol)) { bad = true; continue; }
                Welcome w = {};
                w.h = make_head(PKT_WELCOME);
                memcpy(w.proto, k_protocol, sizeof k_protocol);
                w.version = k_version;
                w.in_battle = 1;
                send_packet(&w, sizeof w, false);
                if (hello->ready) established = true;
            } else if (id == 2 && h->type == PKT_WELCOME) {
                if (!valid_welcome(buffer, (size_t)n)) { bad = true; continue; }
                established = true;
            } else if (h->type == PKT_END && n == (int)sizeof(EndState)) {
                if (h->flags & 1) got_end = true;
            } else if (established && h->type == PKT_SHOT && h->count == 1 && n == (int)(sizeof(ShotHead) + sizeof(ShotRec))) {
                const ShotHead *sh = (const ShotHead *)buffer;
                const ShotRec *rec = (const ShotRec *)(sh + 1);
                if (!inbox.epoch(sh->epoch, rec->seq)) continue;
                auto result = inbox.receive(*rec);
                if (result == reliable::Inbox<ShotRec>::duplicate) ++duplicates;
                if (result == reliable::Inbox<ShotRec>::gap) ++gaps;
                if (result == reliable::Inbox<ShotRec>::accepted && !first_event) first_event = now;
                ShotAck reply = { make_head(PKT_SHOT_ACK), inbox.current_epoch(), inbox.committed() };
                send_packet(&reply, sizeof reply, true);
            } else if (established && h->type == PKT_SHOT_ACK && n == (int)sizeof(ShotAck)) {
                const ShotAck *reply = (const ShotAck *)buffer;
                if (reply->epoch == epoch && reply->seq <= total && reply->seq > ack) ack = reply->seq;
            }
        }
        // Delay application deliberately: ACK must remain zero even though seq 1 was received.
        if (first_event && now - first_event >= 120) {
            for (auto &item : inbox.items()) {
                if (item.complete) continue;
                unsigned seq = item.record.seq;
                unsigned other_id = id == 1 ? 2u : 1u;
                if (seq > total || item.record.ammo != (seq ^ (0x5a00u + other_id)) ||
                    item.record.launch[0] != (unsigned char)(seq ^ other_id)) { bad = true; item.complete = true; continue; }
                ++applied[seq];
                item.complete = true;
            }
            unsigned before = inbox.committed();
            if (inbox.commit() != before) {
                ShotAck reply = { make_head(PKT_SHOT_ACK), inbox.current_epoch(), inbox.committed() };
                send_packet(&reply, sizeof reply, true);
            }
        }
        if (ack == total && inbox.committed() == total && got_end) {
            if (!finished_at) finished_at = now;
            if (now - finished_at >= 700) break; // keep answering the other process's retries
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    for (unsigned seq = 1; seq <= total; ++seq) if (applied[seq] != 1) bad = true;
    bool ok = !bad && established && got_end && ack == total && inbox.committed() == total && dropped && duplicates && gaps;
    printf("peer %d: %s, sent=%u ack=%u applied=%u, dropped=%u duplicates=%u gaps=%u end=%d\n",
           id, ok ? "OK" : "FAIL", data_sent, ack, inbox.committed(), dropped, duplicates, gaps, (int)got_end);
    close_socket(sock);
#ifdef _WIN32
    WSACleanup();
#endif
    return ok ? 0 : 1;
}

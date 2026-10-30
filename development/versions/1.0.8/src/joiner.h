// Joining side of the connection up to the moment the battle starts: find the host, wait until it is in a
// battle, download the battle description, start the game into that battle.
// Non-blocking; poll() is called every frame by the in-game menu and in a loop by mp_join.exe.
#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include "steam.h"
#include <stdlib.h>
#include <string.h>
#include <vector>
#include "protocol.h"
#include "image_chunks.h"
#include "operation_turn.h"
#include "image.h"
#include "battle_identity.h"
#include "opsave.h"
#include "opsave_checked.h"
#include "map_file_windows.h"

struct Joiner {
    enum State { IDLE, CONNECTING, WAITING_FOR_BATTLE, RECEIVING, DONE, FAILED };
    State state = IDLE;
    char error[160] = "";
    char address[128] = "";
    unsigned char host_mode = 0;
    unsigned prefer_side = 0;          // side code the player wants to command; set before start()
    unsigned host_side = 0, other_side = 0;
    bool versus = false;               // decided by the host from prefer_side
    char my_name[24] = "Player", host_name[24] = "";
    bool lobby_ready = false;          // this player pressed READY; the battle is only entered when set
    unsigned char swap_req = 0;        // number of SWAP SIDES presses
    bool swap = false, host_in_battle = false;
    u64 last_welcome = 0;
    bool want_map = false;             // what is being received: the operational map instead of a battle
    char map_file[96] = "";
    unsigned map_turns = 0, map_epoch = 0, map_revision = 0;
    std::string map_backup; // retained when the profile already had a save
    SOCKET sock = INVALID_SOCKET;
    bool winsock_started = false, owns_steam_inbox = false;
    sockaddr_in host = {};
    std::vector<unsigned char> image, have;
    std::vector<u64> asked;
    unsigned image_hash = 0, got = 0;
    u64 last_hello = 0, started = 0, last_ask = 0;

    bool fail(const char *why)
    {
        strcpy_s(error, why);
        close();
        state = FAILED;
        return false;
    }

    void close()
    {
        if (owns_steam_inbox) steam_lobby_release();
        owns_steam_inbox = false;
        if (sock != INVALID_SOCKET) closesocket(sock);
        sock = INVALID_SOCKET;
        if (winsock_started) WSACleanup();
        winsock_started = false;
    }

    void cancel()
    {
        close();
        state = IDLE;
    }

    bool start(const char *addr, bool reserve_steam_inbox = true)
    {
        cancel();
        error[0] = 0;
        strcpy_s(address, addr);
        char name[128];
        strcpy_s(name, addr);
        WSADATA wsa;
        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return fail("Windows networking could not start.");
        winsock_started = true;
        host = {};
        if (!name[0]) return fail("Enter the host's address first.");
        if (!_strnicmp(name, "steam:", 6)) {
            // A friend's game: the Steam ID is the whole address, and the bytes go through Steam's P2P
            // relay instead of the socket.  Checked before the host:port split, which would otherwise
            // read the ID as a port number.
            uint64_t id = 0;
            if (!steam_parse_id(name + 6, &id) || !steam_peer_make(id, &host))
                return fail("That Steam ID is not a valid number.");
            if (!steam_ready() && !steam_init())
                return fail("Steam is not available. Start the game through Steam, or use the IP address.");
        } else {
            unsigned short port = k_default_port;
            if (char *colon = strchr(name, ':')) { *colon = 0; port = (unsigned short)atoi(colon + 1); }
            host.sin_family = AF_INET;
            host.sin_port = htons(port);
            if (inet_pton(AF_INET, name, &host.sin_addr) != 1) {
                addrinfo hints = {}, *res = 0;
                hints.ai_family = AF_INET;
                if (getaddrinfo(name, 0, &hints, &res) || !res) return fail("That address could not be found.");
                host.sin_addr = ((sockaddr_in *)res->ai_addr)->sin_addr;
                freeaddrinfo(res);
            }
        }
        sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (sock != INVALID_SOCKET) {
            u_long nonblocking = 1;
            if (ioctlsocket(sock, FIONBIO, &nonblocking)) {
                closesocket(sock);
                sock = INVALID_SOCKET;
            } else {
                DWORD off = 0, ret = 0;
                WSAIoctl(sock, _WSAIOW(IOC_VENDOR, 12) /* SIO_UDP_CONNRESET */, &off, sizeof off, 0, 0, &ret, 0, 0);
            }
        }
        if (host.sin_family != STEAM_FAMILY && sock == INVALID_SOCKET)
            return fail("Could not open a nonblocking UDP socket.");
        if (host.sin_family == STEAM_FAMILY && reserve_steam_inbox) {
            if (!steam_lobby_claim()) return fail("Another Steam lobby connection is already active.");
            owns_steam_inbox = true;
        }
        image.clear();
        have.clear();
        got = 0;
        last_hello = 0;
        started = GetTickCount64();
        state = CONNECTING;
        return true;
    }

    float progress() const { return have.empty() ? 0 : (float)got / have.size(); }
    unsigned waited_seconds() const { return (unsigned)((GetTickCount64() - started) / 1000); }

    // Same seam as the module: the address decides the transport.
    void send_host(const void *data, int size)
    {
        if (host.sin_family == STEAM_FAMILY) steam_send(steam_peer_id(&host), data, size);
        else sendto(sock, (const char *)data, size, 0, (sockaddr *)&host, sizeof host);
    }

    void poll()
    {
        if (state != CONNECTING && state != WAITING_FOR_BATTLE && state != RECEIVING) return;
        u64 now = GetTickCount64();
        if (now - last_hello >= 500) {
            Hello hello = { make_head(PKT_HELLO) };
            memcpy(hello.proto, k_protocol, sizeof k_protocol);
            hello.version = k_version;
            hello.prefer_side = prefer_side;
            hello.ready = 0;
            hello.phase = 0xff;         // not in a battle yet
            hello.lobby_ready = lobby_ready;
            hello.swap_req = swap_req;
            strncpy_s(hello.name, my_name, sizeof hello.name - 1);
            send_host(&hello, sizeof hello);
            last_hello = now;
        }
        // The pieces are asked for in small bursts, not one burst per frame: a menu that wants a whole
        // description used to put a thousand requests on the wire in half a second and get nothing back,
        // because the host was already in the battle.
        if (state == RECEIVING && now - last_ask >= 60) {
            last_ask = now;
            unsigned sent = 0;
            for (unsigned i = 0; i < have.size() && sent < 24; i++) {
                if (have[i] || now - asked[i] < 300) continue;
                ImageRequest r = { make_head(PKT_IMAGE_REQUEST), i };
                r.h.flags = want_map ? 1 : 0;
                if (want_map) { r.map_epoch = map_epoch; r.map_revision = map_revision; }
                send_host(&r, sizeof r);
                asked[i] = now;
                sent++;
            }
        }
        for (int guard = 0; guard < 256; guard++) {
            unsigned char buf[2048];
            sockaddr_in from = {};
            int n;
            if (host.sin_family == STEAM_FAMILY) {
                uint64_t id = 0;
                n = steam_recv(&id, buf, sizeof buf);
                if (n <= 0) break;
                if (!steam_peer_make(id, &from) || id != steam_peer_id(&host)) continue;
            } else {
                int flen = sizeof from;
                n = recvfrom(sock, (char *)buf, sizeof buf, 0, (sockaddr *)&from, &flen);
                if (n <= 0) break;
                if (from.sin_family != AF_INET || from.sin_addr.s_addr != host.sin_addr.s_addr ||
                    from.sin_port != host.sin_port) continue;
            }
            if (n < (int)sizeof(Head)) continue;
            const Head *h = (const Head *)buf;
            if (h->magic != k_magic) continue;
            if (h->type == PKT_WELCOME && n >= 24) {
                if (!valid_welcome(buf, (size_t)n)) { fail("The host runs a different version of the multiplayer files."); return; }
                const Welcome *w = (const Welcome *)buf;
                last_welcome = now;
                swap = w->swap != 0; host_in_battle = w->in_battle != 0;
                memcpy(host_name, w->name, sizeof host_name); host_name[sizeof host_name - 1] = 0;
                host_mode = w->host_mode; host_side = w->user_side; other_side = w->enemy_side;
                versus = w->versus != 0;
                bool map = !w->in_battle && w->map_size != 0;
                unsigned size = map ? w->map_size : w->image_size;
                unsigned offered_epoch = 0;
                operation_turn::Phase phase;
                bool identity_ok = operation_turn::decode(w->map_flags, &offered_epoch, &phase) && w->map_revision &&
                                   phase == operation_turn::Phase::host;
                if (state == RECEIVING && want_map && identity_ok) {
                    bool same = offered_epoch == map_epoch && w->map_revision == map_revision;
                    bool newer = (offered_epoch == map_epoch && operation_turn::newer_revision(w->map_revision, map_revision)) ||
                                 operation_turn::newer_epoch(offered_epoch, map_epoch);
                    if (!same && !newer) continue; // delayed old welcome
                    if (same && map && size == image.size() && w->map_hash == image_hash) continue;
                    if (same && map && (size != image.size() || w->map_hash != image_hash)) continue; // identity collision
                } else if (state == RECEIVING && !want_map && !w->in_battle) {
                    continue; // an old map welcome must not replace an in-flight battle
                } else if (state == RECEIVING && !want_map && w->in_battle &&
                           size == image.size() && w->image_hash == image_hash) {
                    continue;
                }
                if ((!map && !w->in_battle) || !size || !lobby_ready) {
                    image.clear(); have.clear(); asked.clear(); got = 0;
                    state = WAITING_FOR_BATTLE; continue;
                }
                if (size > (64u << 20)) { fail("The host sent an invalid battle or map."); return; }
                if (map) {
                    if (!identity_ok) { state = WAITING_FOR_BATTLE; continue; }
                    char offered_file[sizeof map_file];
                    memcpy(offered_file, w->map_file, sizeof offered_file);
                    if (offered_file[sizeof offered_file - 1] != 0 || !opsave::safe_file_name(offered_file)) {
                        fail("The host sent an invalid operation save."); return;
                    }
                    strcpy_s(map_file, offered_file);
                    map_turns = w->map_turns; map_epoch = offered_epoch; map_revision = w->map_revision;
                }
                // A newer offer supersedes an incomplete download; old fragments carry the old
                // identity and image_chunks::valid rejects them. Continue HELLOs while receiving.
                want_map = map; image.assign(size, 0); image_hash = map ? w->map_hash : w->image_hash;
                have.assign((size + k_chunk - 1) / k_chunk, 0); asked.assign(have.size(), 0);
                got = 0; last_ask = 0; state = RECEIVING;
            } else if (h->type == PKT_IMAGE_DATA && state == RECEIVING && n >= (int)(sizeof(ImageData) - k_chunk)) {
                const ImageData *d = (const ImageData *)buf;
                if (!image_chunks::valid(d, (size_t)n, image.size(), want_map ? 1 : 0, map_epoch, map_revision) ||
                    have[d->index]) continue;
                memcpy(image.data() + (size_t)d->index * k_chunk, d->data, d->size);
                have[d->index] = 1;
                got++;
            }
        }
        // A dead host must not leave an obsolete incomplete download alive forever.
        if (state == RECEIVING && now - last_welcome > 20000) {
            image.clear(); have.clear(); asked.clear(); got = 0;
            state = CONNECTING; started = now; host_name[0] = 0;
        }
        // a host that stops answering is looked for again
        if (state == WAITING_FOR_BATTLE && now - last_welcome > 6000) { state = CONNECTING; started = now; host_name[0] = 0; }
        if (state == RECEIVING && got == have.size()) {
            close();
            if (fnv(image.data(), image.size()) != image_hash) fail("The battle arrived damaged. Try again.");
            else state = DONE;
        }
    }

    // Is there an operation of this name in the game's data? Operations are files
    // <pack>\shared\camps\<campaign>\<name>.campack2, where <pack> is a folder one or two levels below
    // data\ (data\oprum, data\addons\rage_bridge, data\common\ma_croat_leg, ...).
    static bool operation_in(const char *dir, const char *campaign, const char *name, int depth)
    {
        char find[MAX_PATH], path[MAX_PATH];
        sprintf_s(path, "%s\\shared\\camps\\%s\\%s.campack2", dir, campaign, name);
        if (GetFileAttributesA(path) != INVALID_FILE_ATTRIBUTES) return true;
        if (depth <= 0) return false;
        WIN32_FIND_DATAA fd;
        sprintf_s(find, "%s\\*", dir);
        HANDLE h = FindFirstFileA(find, &fd);
        if (h == INVALID_HANDLE_VALUE) return false;
        bool found = false;
        do {
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) || fd.cFileName[0] == '.') continue;
            if (sprintf_s(path, "%s\\%s", dir, fd.cFileName) > 0) found = operation_in(path, campaign, name, depth - 1);
        } while (!found && FindNextFileA(h, &fd));
        FindClose(h);
        return found;
    }

    static bool operation_exists(const char *root, const char *campaign, const char *name)
    {
        char data[MAX_PATH];
        sprintf_s(data, "%s\\data", root);
        return operation_in(data, campaign, name, 2);
    }

    // Load the exact host operation as a spectator copy, retaining a transactional backup.
    // Do not mirror sides or switch to a counterpart: the host is the only operational authority.
    bool launch_map(const char *root)
    {
        char path[MAX_PATH], id[64] = "cloud", text[512] = "", cmd[1024], local_file[96];
        std::string why;
        if (!root || !*root || !opsave::safe_file_name(map_file) || !operation_turn::valid_epoch(map_epoch) ||
            !map_revision || !opsave_checked::validate(image, &why))
            return fail("The operational map or its snapshot identity is invalid.");
        sprintf_s(path, "%s\\users\\profiles\\cur_profile.engcfg", root);
        FILE *f = 0;
        if (!fopen_s(&f, path, "rb") && f) {
            text[fread(text, 1, sizeof text - 1, f)] = 0;
            fclose(f);
            const char *p = strstr(text, "cur_id");
            if (p && (p = strchr(p, '='))) {
                for (p++; *p == ' ' || *p == '\t'; p++) {}
                size_t k = strcspn(p, ";\r\n");
                if (k && k < sizeof id) { memcpy(id, p, k); id[k] = 0; }
            }
        }
        if (!opsave_checked::safe_profile_id(id)) return fail("The profile id is not a safe folder name.");
        std::string name = opsave::operation_name(image);
        std::string op = opsave::operation_id(map_file, name);
        if (op.empty()) return fail("The host's operation save could not be recognised.");
        std::vector<unsigned char> prepared = image;
        strcpy_s(local_file, map_file);
        if (!opsave_checked::validate(prepared, &why)) return fail("The prepared operational map is invalid.");
        if (sprintf_s(path, "%s\\users\\profiles\\%s\\%s", root, id, local_file) < 0)
            return fail("The operational save path is too long.");
        if (sprintf_s(cmd, "\"%s\\starter_x64.exe\" root\\programs\\opgame.progpack, %s, %s, 0, %u", root, id,
                      op.c_str(), map_turns ? map_turns : opsave::turns(prepared)) < 0)
            return fail("The operational launch command is too long.");
        char epoch[16], revision[16];
        sprintf_s(epoch, "%u", map_epoch); sprintf_s(revision, "%u", map_revision);
        map_file_windows::Environment env;
        if (!env.set("MP_ROLE", "join") || !env.set("MP_PEER", address) || !env.set("MP_MAP", "1") ||
            !env.set("MP_HOST_MODE", "operation") || !env.set("MP_MAP_EPOCH", epoch) || !env.set("MP_MAP_REV", revision) ||
            !env.set(battle_identity::k_environment, nullptr) ||
            !env.set("MP_AUTOJOIN", nullptr) || !env.set("MP_ENDTURN", nullptr) || !env.set("MP_TURNDONE", nullptr))
            return fail("Could not prepare the child game's environment; no save was replaced.");
        DWORD launch_error = 0;
        map_file::Result result = map_file_windows::replace_and_launch(root, cmd, path, prepared, &map_backup, &launch_error);
        if (!map_file::started(result)) {
            char message[160];
            sprintf_s(message, "Could not open the operation: %.100s (Windows %lu).", map_file::reason(result), launch_error);
            return fail(message);
        }
        if (!env.restore()) printf("Warning: could not fully restore the parent process environment.\n");
        if (result == map_file::Result::ok_marker_retained)
            printf("Operation started, but its recovery marker remains beside the save.\n");
        return true;
    }

    // Writes the received battle where the game expects it and starts the game straight into it, with the
    // multiplayer module told to follow the host. `root` is the game folder.
    bool launch(const char *root)
    {
        char path[MAX_PATH], id[64] = "cloud", text[512] = "", cmd[1024];
        sprintf_s(path, "%s\\users\\profiles\\cur_profile.engcfg", root);
        FILE *f = 0;
        if (!fopen_s(&f, path, "rb") && f) {
            text[fread(text, 1, sizeof text - 1, f)] = 0;
            fclose(f);
            const char *p = strstr(text, "cur_id");
            if (p && (p = strchr(p, '='))) {
                for (p++; *p == ' ' || *p == '\t'; p++) {}
                size_t k = strcspn(p, ";\r\n");
                if (k && k < sizeof id) { memcpy(id, p, k); id[k] = 0; }
            }
        }
        if (!opsave_checked::safe_profile_id(id)) return fail("The profile id is not a safe folder name.");
        std::string why;
        if (!opsave_checked::validate(image, &why)) return fail("The received battle description is invalid.");
        // poll() verified this checksum on receipt. Check again before passing the source
        // identity to the child in case the buffered image changed after receipt.
        if (fnv(image.data(), image.size()) != image_hash) return fail("The received battle description changed after transfer.");
        std::vector<unsigned char> prepared = image;
        // commanding the host's opponent: same battle, with the command of the two forces exchanged
        if (versus && !image::mirror(prepared)) return fail("The host's battle could not be prepared for the other side.");
        if (!opsave_checked::validate(prepared, &why)) return fail("The prepared battle description is invalid.");
        if (sprintf_s(path, "%s\\%s", root, k_image_path) < 0 ||
            sprintf_s(cmd, "\"%s\\starter_x64.exe\" root\\programs\\tacgame.progpack, %s, users\\profiles, %s, *:*, 0, 1",
                      root, k_image_path, id) < 0) return fail("The battle path or command is too long.");
        char code[8] = { 0 }; memcpy(code, &prefer_side, 4);
        char identity[battle_identity::k_text_size];
        battle_identity::format(image_hash, battle_identity::fingerprint(prepared.data(), prepared.size()), identity);
        map_file_windows::Environment env;
        if (!env.set(battle_identity::k_environment, identity) ||
            !env.set("MP_ROLE", "join") || !env.set("MP_PEER", address) ||
            !env.set("MP_PREFER", prefer_side ? code : nullptr) || !env.set("MP_VERSUS", versus ? "1" : "0") ||
            !env.set("MP_HOST_MODE", host_mode == HOST_MODE_OPERATION ? "operation" : host_mode == HOST_MODE_MISSION ? "mission" : "skirmish") ||
            !env.set("MP_MAP", nullptr) || !env.set("MP_AUTOJOIN", host_mode == HOST_MODE_OPERATION ? address : nullptr))
            return fail("Could not prepare the child game's environment; no battle file was replaced.");
        DWORD launch_error = 0;
        map_file::Result result = map_file_windows::replace_and_launch(root, cmd, path, prepared, &map_backup, &launch_error);
        if (!map_file::started(result)) {
            char message[160];
            sprintf_s(message, "Could not start the battle: %.100s (Windows %lu).", map_file::reason(result), launch_error);
            return fail(message);
        }
        if (!env.restore()) printf("Warning: could not fully restore the parent process environment.\n");
        return true;
    }
};

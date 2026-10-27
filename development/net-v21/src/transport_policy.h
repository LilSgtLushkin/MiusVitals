// Tactical Steam P2P must remain usable when the independent IP listener cannot bind. Keep the
// pre-existing operation-map behaviour untouched; IP still requires its own socket.
#pragma once
namespace transport_policy {
inline bool drive_session(bool tactical, bool udp_open, bool steam_ready)
{
    return udp_open || (tactical && steam_ready);
}
inline bool send_peer(bool tactical, bool udp_open, bool have_peer, bool steam_address)
{
    return have_peer && (udp_open || (tactical && steam_address));
}
}

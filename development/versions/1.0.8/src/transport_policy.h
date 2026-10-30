// A live Steam P2P path is independent of the IP socket in both tactical and operation sessions.
// An IP peer still requires its own UDP socket; the caller retries Steam initialization separately.
#pragma once
namespace transport_policy {
inline bool drive_session(bool tactical, bool udp_open, bool steam_ready)
{
    (void)tactical;
    return udp_open || steam_ready;
}
inline bool send_peer(bool tactical, bool udp_open, bool have_peer, bool steam_address)
{
    (void)tactical;
    return have_peer && (udp_open || steam_address);
}
}

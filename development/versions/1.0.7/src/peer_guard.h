// Tactical host: don't let an unrelated HELLO reset a recently active opponent's battle.
// Keep the historic three-second reconnect window; operational-map behaviour is unchanged.
#pragma once
#include <stdint.h>
namespace peer_guard {
inline bool reject_hello(bool tactical, bool connected, bool from_peer, uint32_t now, uint32_t last_rx) {
    return tactical && connected && !from_peer && (uint32_t)(now - last_rx) < 3000u;
}
}

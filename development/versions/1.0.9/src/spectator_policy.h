// Separate operation observation from tactical traffic. Only locally inherited spectator
// processes are covered; real engine process/environment behaviour still needs game testing.
#pragma once
#include "protocol.h"
namespace spectator_policy {
inline bool local_battle_offline(bool tactical, bool joiner, bool spectator_marker) {
    return tactical && joiner && spectator_marker;
}
inline bool incoming(bool operation_host, bool spectator, unsigned type, unsigned flags) {
    if (!operation_host && !spectator) return true; // leave tactical and other modes unchanged
    if (type == PKT_BYE || type == PKT_KEEPALIVE) return true;
    if (operation_host && type == PKT_IMAGE_REQUEST && flags == 1) return true;
    if (spectator && type == PKT_WELCOME) return true;
    if (spectator && type == PKT_IMAGE_DATA && flags == 1) return true;
    return false;
}
inline bool accept_hello(bool operation_host, unsigned phase) {
    // A tactical process on the guest's local copy is not an authoritative map peer.
    return !operation_host || phase >= 0xfe;
}
}

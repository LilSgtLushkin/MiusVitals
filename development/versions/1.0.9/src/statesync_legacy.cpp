// Frozen non-tactical state handler. M5 is out of scope, so keep the exact implementation
// from 30405d1 separately from the hardened tactical implementation in statesync.cpp.
// Load headers outside the namespace; the included copy's own includes are then guarded.
// The namespace isolates not only functions but also its different Track/Incoming layouts
// and their template instantiations from the newer tactical code (avoiding ODR corruption).
#include "common.h"
#include "env_value.h"
#include "netlog.h"
#include "protocol.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unordered_map>
#include <vector>

namespace frozen_non_tactical {
#include "statesync_legacy_30405d1.inc"
}

void statesync_legacy_on_packet(const unsigned char *data, int size, void (*send)(const void *, int), bool versus)
{
    frozen_non_tactical::statesync_on_packet(data, size, send, versus);
}
void statesync_legacy_reset(void)
{
    frozen_non_tactical::statesync_reset();
}

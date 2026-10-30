#include "transport_policy.h"
#include <stdio.h>
static int checks, failed;
static void check(bool ok, const char *name) { ++checks; if (!ok) { ++failed; printf("FAIL: %s\n", name); } }
int main()
{
    using namespace transport_policy;
    check(drive_session(true, false, true), "tactical Steam runs without UDP");
    check(!drive_session(true, false, false), "no usable transport does not run");
    check(drive_session(true, true, false), "tactical IP fallback runs");
    check(drive_session(false, false, true), "operation-map Steam runs without UDP");
    check(send_peer(true, false, true, true), "tactical Steam peer can send without UDP");
    check(!send_peer(true, false, true, false), "IP peer cannot send without UDP");
    check(!send_peer(true, true, false, true), "no peer means no send");
    check(send_peer(false, false, true, true), "operation-map Steam peer can send without UDP");
    check(!send_peer(false, false, true, false), "operation-map IP peer still needs UDP");
    printf("transport policy selftest: %d checks, %d failed\n", checks, failed);
    return failed ? 1 : 0;
}

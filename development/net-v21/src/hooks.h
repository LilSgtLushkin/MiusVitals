// Finding an engine function in a game build we have not seen before.
//
// The module detours a handful of engine functions: the Start button and the box it opens, the battle
// clock, the operation map's "next turn" and the step that carries the turn out.  Each is known by the
// bytes at its start.  A game update moves them, and a moved function is not a reason to lose the feature:
// the bytes are still somewhere in the module's executable section.
//
// So: the offset a build is known to use is checked first (no scan, no cost, and it is what a bug report
// shows); if those bytes are not there, the section is searched for the signature and the function is only
// used when the signature occurs exactly once.  Two candidates is not a guess worth making - the hook is
// left out and the log says so. The checked tactical resolver verifies all fixed offsets first, and
// fallback succeeds only if all matching signatures point to the same unique address.
#pragma once

struct HookSig
{
    unsigned rva;           // the offset this build is known to use, 0 to only scan
    const void *sig;
    unsigned size;
};

// Existing operation-map resolver: kept unchanged for M5.
char *hook_resolve(const char *module, const HookSig *list, unsigned count, const char *what);
// Tactical resolver: validates mapped PE bounds and rejects ambiguous cross-version matches.
char *hook_resolve_checked(const char *module, const HookSig *list, unsigned count, const char *what);

// The offset of an address inside a loaded module, for the log ("adv_interf+0x3b320"); -1 when the
// address is not in that module.
long hook_offset(const char *module, const void *address);

#ifdef NETV21_TEST_HOOKS
// Tests only: inspect an in-memory PE without finding or patching a real game module.
char *hook_test_resolve(const void *base, const HookSig *list, unsigned count, int *reason);
#endif

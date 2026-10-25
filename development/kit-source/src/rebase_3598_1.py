#!/usr/bin/env python3
"""Moves the code addresses that the Steam release (v20) of basic_network.codelib expects in the game's modules to where
the same code stands in the game build 6.00 3598/1 (files of 2026-10-19). The game's code is the same (it was only linked
anew, so functions moved by a few hundred bytes); v20 compares every address with the bytes it expects before it hooks
anything, so a wrong address would leave a feature off, never break it.

    python rebase_3598_1.py  v20/basic_network.codelib  rebased.codelib          [--no-start: leave the Start group (3 functions + 16 bytes of data) alone]
Only 32-bit constants inside the module's own code change (listed below; one of them is the state stream's speed limit). Refuses anything but the v20 release.
"""
import hashlib, struct, sys

V20_SHA256 = "8108bd1e9af8080ef8b1050512b8311848f5a336acb49a7479388733b4ae5723"

# (what, offset of the constant in v20's code (RVA), old value, new value)
EDITS = [
    # adv_interf.codelib: "the other player's units can be selected" (a function that now stands 0x160 lower)
    ("adv_interf unit select check", 0x124a7, 0x57470, 0x57310), ("adv_interf unit select hook", 0x124c0, 0x57470, 0x57310),
    # adv_physic.codelib: render handles of loaded states stay local; crews blowing up a vehicle
    ("adv_physic render handles check", 0x2efc7, 0x82090, 0x82020), ("adv_physic render handles hook", 0x2efe5, 0x82090, 0x82020),
    ("adv_physic crew explosion check", 0x349b5, 0x9b520, 0x9b4b0), ("adv_physic crew explosion hook", 0x349ce, 0x9b520, 0x9b4b0),
    # adv_physic.codelib: hits. v-table slots (i_techn, i_human) and the functions they must hold (i_human's moved by -0x30)
    ("hit: i_techn v-table slot", 0x3cfbe, 0xc7a18, 0xc79d8), ("hit: i_human v-table slot", 0x3cfcd, 0xc67a0, 0xc6760),
    ("hit: i_techn function", 0x3cff6, 0x43230, 0x43230), ("hit: i_human function", 0x3d003, 0x66cc0, 0x66c90),
    # not an address: the cap of the state stream (MP_STATE_KBPS default) 600 -> 1500 KB/s; the logs show ~180 KB/s on average, the first seconds
    # of a battle (all units' full states) are what was held at 600
    ("state stream cap KB/s", 0x320a6, 600, 1500),
    # adv_opgame.codelib: operation map (end of turn, scouting, saving, menu screens).
    # On Steam 3598/1 the click handler starts ten bytes earlier; using the former +36b40
    # enters it after its prologue, so the strict signature check deliberately left cooperative
    # operation turns off. The target handler at +36b30 is verified below by its own full prologue.
    ("opgame next-turn handler", 0x1a327, 0x36b40, 0x36b30),
    ("opgame end turn check", 0x101a9, 0x76810, 0x76510), ("opgame end turn hook", 0x101f3, 0x76810, 0x76510), ("opgame turn flow", 0x1a377, 0x76810, 0x76510),
    ("opgame scouting", 0x12b1e, 0x35540, 0x353e0),
    ("opgame save", 0x1983a, 0xd6660, 0xd6080),
    ("opgame screen 1", 0x24604, 0x627f0, 0x624f0), ("opgame screen 2", 0x2843f, 0x627f0, 0x624f0), ("opgame screen 3", 0x2a067, 0x627f0, 0x624f0),
    ("opgame screen 4", 0x29cf7, 0x66650, 0x66350),
]

# The "Start" group (v20 makes the 'Start battle' button wait for the other player, so both games leave the deployment together): three functions of
# adv_interf.codelib, each hooked only after v20 has compared the function's first bytes with what it expects. The functions moved, and the second one's
# first 16 bytes are the same code with two registers swapped (the compiler's choice). Any mismatch would leave the whole group off (never break anything).
START_EDITS = [
    ("adv_interf start 1 (deploy ends)", 0x123dd, 0x3b3b0, 0x3b320),
    ("adv_interf start 2 (end-deployment event)", 0x123fb, 0x3b4f0, 0x3b460),
    ("adv_interf start 3 (box result)", 0x12405, 0x41700, 0x41630),
]
# Signatures embedded in basic_network.codelib and checked before a detour is
# installed.  They must move together with the target functions; otherwise the
# checked build correctly leaves the feature disabled.
TARGET_DATA = [  # (what, RVA in v20's .rdata, bytes v20 expects, bytes of Steam 3598/1)
    ("expected prologue of opgame next turn", 0x48da0,
     bytes.fromhex("48895c2408574883ec300f57c0498bd8"),
     bytes.fromhex("40534883ec404c8b81d8010000488bd9")),
]
START_DATA = [   # (what, RVA in v20's .rdata, bytes v20 expects, bytes of the build 3598/1)
    ("expected prologue of start 2", 0x497a8, bytes.fromhex("48895c2408574883ec50498bd8488bf9"), bytes.fromhex("48895c2408574883ec40498bf8488bd9")),
]

def section_of(d, rva):
    pe = struct.unpack_from("<I", d, 0x3c)[0]
    nsec, = struct.unpack_from("<H", d, pe + 6); optsz, = struct.unpack_from("<H", d, pe + 20)
    for i in range(nsec):
        o = pe + 24 + optsz + 40 * i
        vsz, va, rsz, rptr = struct.unpack_from("<IIII", d, o + 8)
        if va <= rva < va + min(vsz, rsz): return rptr + rva - va
    raise SystemExit("RVA %x is in no section" % rva)

def rebase(src, start=True):
    d = bytearray(src)
    pe = struct.unpack_from("<I", d, 0x3c)[0]
    nsec, = struct.unpack_from("<H", d, pe + 6)
    optsz, = struct.unpack_from("<H", d, pe + 20)
    sects = pe + 24 + optsz
    text = None
    for i in range(nsec):
        o = sects + 40 * i
        if bytes(d[o:o+8]).rstrip(b"\0") == b".text":
            vsz, va, rsz, rptr = struct.unpack_from("<IIII", d, o + 8); text = (va, vsz, rptr)
    va, vsz, rptr = text
    for what, rva, old, new in EDITS + (START_EDITS if start else []):
        if not (va <= rva < va + vsz): raise SystemExit("%s: outside .text" % what)
        off = rptr + rva - va
        cur, = struct.unpack_from("<I", d, off)
        if cur != old: raise SystemExit("%s: found %x at %x, expected %x" % (what, cur, rva, old))
        struct.pack_into("<I", d, off, new)
    for what, rva, old, new in TARGET_DATA + (START_DATA if start else []):
        off = section_of(d, rva)
        if bytes(d[off:off + len(old)]) != old: raise SystemExit("%s: bytes at %x are not the expected ones" % (what, rva))
        d[off:off + len(new)] = new
    return bytes(d)

if __name__ == "__main__":
    start = "--no-start" not in sys.argv
    sys.argv = [a for a in sys.argv if a != "--no-start"]
    if len(sys.argv) != 3: raise SystemExit(__doc__)
    src = open(sys.argv[1], "rb").read()
    if hashlib.sha256(src).hexdigest() != V20_SHA256: raise SystemExit("SHA-256 differs from the v20 release")
    out = rebase(src, start)
    open(sys.argv[2], "wb").write(out)
    print("written", sys.argv[2], len(out), "bytes, sha256", hashlib.sha256(out).hexdigest(), "-", sum(1 for e in EDITS + (START_EDITS if start else []) if e[2] != e[3]), "constants moved", "(with the Start group)" if start else "(without the Start group)")

#!/usr/bin/env python3
"""Find a hook signature in a game codelib and print its RVA.

The module detours engine functions by their first bytes.  When a game update moves them, the offsets
in session.cpp are stale; this tool says where the bytes actually are, which is what the next build's
candidate list needs.  RVAs are what the module is given (it adds the module base).

    python tools/pe_scan.py adv_interf.codelib --sig "40 53 48 83 EC 40 48 8B D9"
    python tools/pe_scan.py adv_interf.codelib --sig "49 8B D8 48 8B F9" --context 24

Text after `|` in --sig is ignored, so a signature can be pasted as it reads in session.cpp.
"""
from __future__ import annotations

import argparse
import struct
import sys
from pathlib import Path


class PE:
    def __init__(self, path: Path):
        self.path = path
        self.data = path.read_bytes()
        d = self.data
        if d[:2] != b"MZ":
            raise SystemExit(f"{path}: not an MZ image")
        e = struct.unpack_from("<I", d, 0x3C)[0]
        if d[e:e + 4] != b"PE\0\0":
            raise SystemExit(f"{path}: not a PE image")
        coff = e + 4
        sections, = struct.unpack_from("<H", d, coff + 2)
        opt_size, = struct.unpack_from("<H", d, coff + 16)
        opt = coff + 20
        magic, = struct.unpack_from("<H", d, opt)
        self.image_base, = struct.unpack_from("<Q" if magic == 0x20B else "<I", d, opt + 24)
        self.sections = []
        s = opt + opt_size
        for _ in range(sections):
            name = d[s:s + 8].rstrip(b"\0").decode("latin-1")
            vsize, vaddr, rawsize, rawptr = struct.unpack_from("<IIII", d, s + 8)
            self.sections.append((name, vaddr, vsize, rawptr, rawsize))
            s += 40

    def rva(self, file_offset: int) -> int | None:
        for _name, va, _vs, rp, rs in self.sections:
            if rp <= file_offset < rp + rs:
                return va + (file_offset - rp)
        return None

    def executable(self):
        for name, va, _vs, rp, rs in self.sections:
            yield name, va, rp, rs


def signature(text: str) -> bytes:
    text = text.split("|")[0]
    parts = [p for p in text.replace(",", " ").split() if p]
    try:
        return bytes(int(p, 16) for p in parts)
    except ValueError as error:
        raise SystemExit(f"bad signature {text!r}: {error}")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("codelib", type=Path)
    parser.add_argument("--sig", required=True, help="bytes, e.g. \"40 53 48 83 EC 40\"")
    parser.add_argument("--context", type=int, default=0, help="bytes of context to print after each hit")
    parser.add_argument("--prefix", type=int, default=0, help="match only the first N bytes of --sig")
    args = parser.parse_args()

    sig = signature(args.sig)
    if args.prefix:
        sig = sig[:args.prefix]
    pe = PE(args.codelib)
    print(f"{args.codelib.name}: image base 0x{pe.image_base:x}, {len(pe.sections)} sections")
    hits = 0
    for name, va, rp, rs in pe.executable():
        blob = pe.data[rp:rp + rs]
        i = blob.find(sig)
        while i >= 0:
            rva = va + i
            line = f"  {name}+0x{rva:06x}"
            if args.context:
                line += "  " + " ".join(f"{b:02X}" for b in blob[i:i + len(sig) + args.context])
            print(line)
            hits += 1
            i = blob.find(sig, i + 1)
    print(f"{hits} hit(s)")
    return 0 if hits else 1


if __name__ == "__main__":
    sys.exit(main())

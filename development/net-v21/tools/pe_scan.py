#!/usr/bin/env python3
"""Find a hook signature in a game codelib and print its RVA.

The module detours engine functions by their first bytes.  When a game update moves them, the offsets
in session.cpp are stale; this tool says where the bytes actually are, which is what the next build's
candidate list needs.  RVAs are what the module is given (it adds the module base).

    python tools/pe_scan.py adv_interf.codelib --sig "40 53 48 83 EC 40 48 8B D9"
    python tools/pe_scan.py adv_interf.codelib --sig "49 8B D8 48 8B F9" --context 24

Text after `|` in --sig is ignored, so a signature can be pasted as it reads in session.cpp.

An address instead of a signature -- what a user's own notes or a debugger give:

    python tools/pe_scan.py adv_physic.codelib --at 0xC79D8        # an RVA: what stands there
    python tools/pe_scan.py adv_physic.codelib --va 0x7FF8A1233230 # an address as a debugger shows it

For an address it prints the file's passport (size, sha256, PE timestamp, image base), the section, the
function bound from .pdata and its first 32 bytes (the bytes a detour is checked with), and -- when a
v-table slot was given -- the function the slot holds.  Either end is enough to find the other: a slot
leads to its function, and a function is found again by scanning .rdata for a pointer to it.
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
        self.timestamp, = struct.unpack_from("<I", d, coff + 4)
        self.sections = []
        s = opt + opt_size
        for _ in range(sections):
            name = d[s:s + 8].rstrip(b"\0").decode("latin-1")
            vsize, vaddr, rawsize, rawptr = struct.unpack_from("<IIII", d, s + 8)
            self.sections.append((name, vaddr, vsize, rawptr, rawsize))
            s += 40

    def offset(self, rva: int) -> int | None:
        for _name, va, vsize, rp, rs in self.sections:
            if va <= rva < va + max(vsize, rs):
                delta = rva - va
                return rp + delta if delta < rs else None
        return None

    def section(self, rva: int) -> str:
        for name, va, vsize, _rp, rs in self.sections:
            if va <= rva < va + max(vsize, rs):
                return f"{name}+0x{rva - va:x}"
        return "outside every section"

    def executable_rva(self, rva: int) -> bool:
        for name, va, vsize, _rp, rs in self.sections:
            if va <= rva < va + max(vsize, rs):
                return name.startswith(".text") or name.startswith("CODE")
        return False

    def function_of(self, rva: int):
        """The .pdata record that contains this RVA: (begin, end, unwind), or None."""
        for name, va, vsize, rp, rs in self.sections:
            if name not in (".pdata", "PAGE"):
                continue
            for i in range(0, rs - 11, 12):
                begin, end, unwind = struct.unpack_from("<III", self.data, rp + i)
                if begin <= rva < end and end > begin:
                    return begin, end, unwind
        return None

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


def number(text: str) -> int:
    return int(text.replace("_", ""), 16) if text.lower().startswith(("0x", "0X")) else int(text, 0)


def describe(pe: PE, rva: int, base: int) -> None:
    """Everything the module needs about one address: where it is, which function holds it, what is there."""
    print(f"  rva      0x{rva:x}   ({pe.section(rva)})")
    if base:
        print(f"  address  0x{rva + base:x}   (with the image base {base:#x} an address is subtracted by it)")
    bounds = pe.function_of(rva)
    if bounds:
        begin, end, _unwind = bounds
        print(f"  function +0x{begin:x}..+0x{end:x}  ({end - begin} bytes), this address is +0x{rva - begin:x} into it")
        start = pe.offset(begin)
        if start is not None:
            head = pe.data[start:start + 32]
            print(f"  prologue {' '.join(f'{b:02X}' for b in head)}   <- the bytes a detour is checked with")
    else:
        print("  function no .pdata record here (data, or a leaf function)")
    at = pe.offset(rva)
    if at is None:
        print("  bytes    (not in the file: uninitialised data)")
        return
    blob = pe.data[at:at + 48]
    print(f"  bytes    {' '.join(f'{b:02X}' for b in blob)}")
    # a v-table slot or any other pointer: follow it, because the function is usually one indirection away
    for width in (8, 4):
        if len(blob) < width:
            continue
        value, = struct.unpack_from("<Q" if width == 8 else "<I", blob, 0)
        if value and pe.executable_rva(value):
            print(f"  points to +0x{value:x}  ({pe.section(value)})  - the function this slot holds:")
            inner = pe.function_of(value)
            if inner:
                print(f"            function +0x{inner[0]:x}..+0x{inner[1]:x} ({inner[1] - inner[0]} bytes)")
                start = pe.offset(inner[0])
                if start is not None:
                    print(f"            prologue {' '.join(f'{b:02X}' for b in pe.data[start:start + 32])}")
            target = pe.offset(value)
            if target is not None:
                print(f"            bytes    {' '.join(f'{b:02X}' for b in pe.data[target:target + 32])}")
            break


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("codelib", type=Path)
    parser.add_argument("--sig", help="bytes, e.g. \"40 53 48 83 EC 40\"")
    parser.add_argument("--at", nargs="+", metavar="RVA", help="one or more RVAs to describe (0x43230)")
    parser.add_argument("--va", nargs="+", metavar="ADDRESS", help="absolute addresses as a debugger shows them")
    parser.add_argument("--base", help="image base of --va, when it is not the file's own")
    parser.add_argument("--context", type=int, default=0, help="bytes of context to print after each hit")
    parser.add_argument("--prefix", type=int, default=0, help="match only the first N bytes of --sig")
    args = parser.parse_args()
    if bool(args.sig) == bool(args.at or args.va):
        parser.error("use exactly one of --sig or --at/--va")

    if args.at or args.va:
        pe = PE(args.codelib)
        import datetime
        stamp = datetime.datetime.utcfromtimestamp(pe.timestamp).strftime("%Y-%m-%d %H:%M:%S")
        print(f"file     {args.codelib.name}  {len(pe.data)} bytes")
        print(f"sha256   {__import__('hashlib').sha256(pe.data).hexdigest()}")
        print(f"pe       timestamp 0x{pe.timestamp:08x} ({stamp} UTC), image base {pe.image_base:#x}")
        print(f"code     " + ", ".join(f"{n} rva 0x{v:x} ({rs} bytes)" for n, v, _vs, _rp, rs in pe.sections if n.startswith(".text")))
        base = number(args.base) if args.base else pe.image_base
        for text in (args.at or []):
            print(f"\n{args.codelib.name} --at {text}")
            describe(pe, number(text), 0)
        for text in (args.va or []):
            print(f"\n{args.codelib.name} --va {text}")
            value = number(text)
            rva = value - base if value > base else value
            descum = ""
            if value > base and rva > (1 << 30):
                print(f"  {text} is below the image base {base:#x} - is --base right?")
                continue
            describe(pe, rva, base)
        return 0

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

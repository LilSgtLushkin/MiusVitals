#!/usr/bin/env python3
"""Make the decompiler dump readable: put the actual strings where the addresses were.

angr prints an unknown global as `g_4f198`, which is correct and useless.  Almost all of those
names are string literals in `.rdata`; this pass substitutes them back, so a function reads

    mp_log("steam: the game has not loaded Steam's library - no Steam joining");

instead of

    ::0x27dc::mp_log(&g_4f290);

It also drops the wall of `extern char g_...` declarations, which exist only because the addresses
had no names, and names call targets that are known functions.

    python3 development/v20-recovery/tools/polish.py            # rewrites work/decomp in place
    python3 development/v20-recovery/tools/polish.py --dir reports/decomp
"""
from __future__ import annotations

import argparse
import json
import re
import struct
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parent
WORK = ROOT / "work"

EXTERN = re.compile(r"^extern .*;\n", re.M)
CALL = re.compile(r"::0x([0-9a-f]{4,})::(\w+)?")
GLOBAL = re.compile(r"(?<![\w.])&?g_([0-9a-f]{1,})\b")
# angr prints a bare immediate as an address, so `x & 0x2000` comes out as `x & &g_2000` and
# `p[3]` as `p[&g_3]`.  Everything below the first section (0x1000) can only be a constant.
SMALL = re.compile(r"&?g_([0-9a-f]{1,3})\b")
FLAGS = re.compile(r"\[D\] amd64g_calculate_rflags_c")


class Image:
    """RVA -> file offset, plus the little text reader the substitution needs."""

    def __init__(self, path: Path) -> None:
        self.data = path.read_bytes()
        pe = struct.unpack_from("<I", self.data, 0x3C)[0]
        count = struct.unpack_from("<H", self.data, pe + 6)[0]
        optsize = struct.unpack_from("<H", self.data, pe + 20)[0]
        at = pe + 24 + optsize
        self.sections = []
        for _ in range(count):
            vsize, va, rsize, raw = struct.unpack_from("<IIII", self.data, at + 8)
            self.sections.append((va, max(vsize, rsize), raw, rsize))
            at += 40
        self.size = max(va + size for va, size, _, _ in self.sections)

    def read(self, rva: int, count: int) -> bytes | None:
        for va, size, raw, rawsize in self.sections:
            if va <= rva < va + size and rva - va < rawsize:
                start = raw + rva - va
                return self.data[start:start + count]
        return None

    def rva_of(self, offset: int) -> int | None:
        for va, size, raw, rawsize in self.sections:
            if raw <= offset < raw + rawsize:
                return va + offset - raw
        return None

    def cstring(self, rva: int, limit: int = 400) -> str | None:
        """The literal at `rva`, if there is a sane one there (printable, not a lone byte)."""
        blob = self.read(rva, limit)
        if not blob:
            return None
        end = blob.find(b"\0")
        if end < 3:
            return None
        text = blob[:end]
        if any(b < 0x20 and b not in b"\t\r\n" for b in text) or any(b > 0x7E for b in text):
            return None
        try:
            return text.decode("ascii")
        except UnicodeDecodeError:
            return None


def quote(text: str) -> str:
    return '"' + text.replace("\\", "\\\\").replace('"', '\\"') + '"'


def polish(text: str, image: Image, names: dict[int, str]) -> tuple[str, int, int]:
    substitutions = unresolved = 0

    def do_global(match: re.Match) -> str:
        nonlocal substitutions, unresolved
        rva = int(match.group(1), 16)
        literal = image.cstring(rva)
        if literal is not None:
            substitutions += 1
            return quote(literal)
        name = names.get(rva)
        if name:
            substitutions += 1
            return name
        if rva in function_starts:
            substitutions += 1
            return f"sub_{rva:x}"
        unresolved += 1
        return match.group(0)

    text = EXTERN.sub("", text)
    text = SMALL.sub(lambda m: str(int(m.group(1), 16)), text)
    text = FLAGS.sub("flags_of", text)
    # Short string literals can start mid-object; try the whole aligned blob first, then the exact
    # address, then the nearest 4-byte-aligned start (MSVC aligns string data at least to 4).
    text = GLOBAL.sub(do_global, text)
    text = CALL.sub(lambda m: names.get(int(m.group(1), 16)) or m.group(2) or f"sub_{m.group(1)}", text)
    return text, substitutions, unresolved


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--dir", type=Path, default=WORK / "decomp")
    parser.add_argument("--image", type=Path, default=WORK / "v20.codelib")
    parser.add_argument("--quiet", action="store_true")
    args = parser.parse_args()

    global function_starts
    image = Image(args.image)
    names: dict[int, str] = {}
    classified = WORK / "v20-classified.json"
    if classified.is_file():
        for row in json.loads(classified.read_text(encoding="utf-8")):
            label = row.get("name") or row.get("ref")
            if label:
                names[row["rva"]] = label.split("@@")[0].lstrip("?")
    function_starts = {row["rva"] for row in
                       json.loads(classified.read_text(encoding="utf-8"))} if classified.is_file() else set()

    files = sorted(args.dir.glob("*.c"))
    total_sub = total_unres = 0
    for path in files:
        original = path.read_text(encoding="utf-8")
        body, sub, unres = polish(original, image, names)
        if body != original:
            path.write_text(body, encoding="utf-8")
        total_sub += sub
        total_unres += unres
    if not args.quiet:
        print(f"polished {len(files)} files: {total_sub} addresses resolved to text or names, "
              f"{total_unres} left as raw addresses")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

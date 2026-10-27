#!/usr/bin/env python3
"""Fingerprint a game build: every codelib's size, sha256, PE timestamp and image base.

The module hooks the game by fixed offsets and checks the bytes there before it detours anything.  That
makes "which build of the game is this?" a fact the repository has to keep, not a guess: a log line says
`unknown game build` when the bytes moved, and this file says what the build in `x64/` actually is.

    python development/game-build-3598-1/manifest.py            # print the manifest
    python development/game-build-3598-1/manifest.py --write     # rewrite MANIFEST.txt
    python development/game-build-3598-1/manifest.py --verify    # check MANIFEST.txt against the files
"""
from __future__ import annotations

import argparse
import datetime
import hashlib
import struct
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
GAME = HERE / "x64"
MANIFEST = HERE / "MANIFEST.txt"


def pe_info(data: bytes) -> tuple[int, int, list[str]]:
    """(timestamp, image base, sections) of a PE image, or raise."""
    if data[:2] != b"MZ":
        raise ValueError("not an MZ image")
    e = struct.unpack_from("<I", data, 0x3C)[0]
    if data[e:e + 4] != b"PE\0\0":
        raise ValueError("not a PE image")
    coff = e + 4
    sections, = struct.unpack_from("<H", data, coff + 2)
    opt_size, = struct.unpack_from("<H", data, coff + 16)
    timestamp, = struct.unpack_from("<I", data, coff + 4)
    opt = coff + 20
    magic, = struct.unpack_from("<H", data, opt)
    if magic != 0x20B:
        raise ValueError("not a PE32+ image")
    base, = struct.unpack_from("<Q", data, opt + 24)
    names = []
    s = opt + opt_size
    for _ in range(sections):
        names.append(data[s:s + 8].rstrip(b"\0").decode("latin-1"))
        s += 40
    return timestamp, base, names


def line(path: Path) -> str:
    data = path.read_bytes()
    digest = hashlib.sha256(data).hexdigest()
    if path.suffix.lower() in (".codelib", ".reslib"):
        try:
            timestamp, base, _names = pe_info(data)
            stamp = datetime.datetime.utcfromtimestamp(timestamp).strftime("%Y-%m-%d %H:%M")
            return f"{digest}  {len(data):>9d}  pe 0x{timestamp:08x} {stamp}  base 0x{base:x}  {path.name}"
        except (ValueError, struct.error) as error:
            return f"{digest}  {len(data):>9d}  (invalid PE: {error})  {path.name}"
    return f"{digest}  {len(data):>9d}  {'-' * 31}  {'-' * 15}  {path.name}"


def build() -> str:
    files = sorted(p for p in GAME.iterdir() if p.is_file())
    head = [
        "# x64 files supplied as a reference for the game build; not an installer.",
        "#",
        "# The module detours by offset and checks the bytes there first, so the build has to be pinned:",
        "# every line is sha256, size, PE timestamp, image base, name.  A game update changes these.",
        "# Regenerate with `python development/game-build-3598-1/manifest.py --write`; verify with --verify.",
        "#",
        f"# {len(files)} files.",
        "#",
    ]
    return "\n".join(head + [line(p) for p in files]) + "\n"


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    action = parser.add_mutually_exclusive_group()
    action.add_argument("--write", action="store_true", help="rewrite MANIFEST.txt")
    action.add_argument("--verify", action="store_true", help="check MANIFEST.txt against the files")
    args = parser.parse_args()

    current = build()
    if args.write:
        MANIFEST.write_text(current, encoding="utf-8")
        print(f"wrote {MANIFEST.relative_to(HERE.parent.parent)}")
        return 0
    if args.verify:
        if not MANIFEST.is_file():
            print("no MANIFEST.txt")
            return 1
        committed = MANIFEST.read_text(encoding="utf-8")
        if committed == current:
            print(f"OK {sum(p.is_file() for p in GAME.iterdir())} files match the manifest")
            return 0
        old, new = set(committed.splitlines()), set(current.splitlines())
        for gone in sorted(old - new):
            print(f"changed or gone: {gone}")
        for fresh in sorted(new - old):
            print(f"new or changed:  {fresh}")
        return 1
    print(current, end="")
    return 0


if __name__ == "__main__":
    sys.exit(main())

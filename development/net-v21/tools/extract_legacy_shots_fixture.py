#!/usr/bin/env python3
"""Verify and copy pinned 1.0.7 sources for the protocol-17 no-game compatibility test.

The small checked-in fixtures are never compiled into the game. Verify their actual bytes and
refuse to silently compare a different protocol; generated copies live under build/.
"""
from __future__ import annotations

import argparse
import hashlib
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
FIXTURE = ROOT / "development" / "net-v21" / "fixtures" / "protocol17"
CURRENT_PROTOCOL = ROOT / "development" / "net-v21" / "src" / "protocol.h"
EXPECTED = {
    "shots.cpp": "7345597d535340f3e27664749dcc659a0376ddc043d8738a424364f1c4b18f08",
    "xlate.cpp": "a21315034a5fd9773af6757db1c64d2b5abb8ae1668c22b8c3c296c66b758e3e",
    "protocol.h": "b07f632c28651cf0cd6c5ebe6f56e54f39217c52c6c227f09b1ff2ebf34dddd5",
}


def verify_fixture(fixture: Path, current_protocol: Path) -> dict[str, bytes]:
    files = {}
    for name, expected in EXPECTED.items():
        # Git may check text out with CRLF on Windows; the pinned hashes are for LF bytes.
        data = (fixture / name).read_bytes().replace(b"\r\n", b"\n")
        actual = hashlib.sha256(data).hexdigest()
        if actual != expected:
            raise ValueError(f"1.0.7 {name} did not match its pinned SHA-256: {actual}")
        files[name] = data
    if current_protocol.read_bytes().replace(b"\r\n", b"\n") != files["protocol.h"]:
        raise ValueError("the current wire header differs from 1.0.7; review the compatibility fixture")
    return files


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("out", type=Path, help="ignored build directory for generated fixtures")
    args = parser.parse_args()
    files = verify_fixture(FIXTURE, CURRENT_PROTOCOL)
    args.out.mkdir(parents=True, exist_ok=True)
    for name, data in files.items():
        (args.out / name).write_bytes(data)
    print("protocol-17 compatibility fixture: pinned 1.0.7 shots/xlate/header verified")


if __name__ == "__main__":
    main()

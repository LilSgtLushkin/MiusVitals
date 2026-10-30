#!/usr/bin/env python3
"""Verify and copy pinned 1.0.7 sources for the protocol-17 no-game compatibility test.

The small checked-in fixtures are never compiled into the game. Verify their actual bytes and
refuse to silently use a different set; generated copies live under build/.

The wire format of the module moved on (the current header carries protocol 18) while these three
files stay exactly as 1.0.7 shipped them. That is the point of a pinned fixture, so a difference
between the two is reported, not treated as damage: inter-version compatibility is not a goal, and
these files are a fixed reference for the old event stream, not a contract the current source has
to keep. What must never pass quietly is a change to the pinned bytes themselves.
"""
from __future__ import annotations

import argparse
import hashlib
import re
from pathlib import Path

TREE = Path(__file__).resolve().parents[1]  # development/versions/<version>: исходник этой версии
FIXTURE = TREE / "fixtures" / "protocol17"
CURRENT_PROTOCOL = TREE / "src" / "protocol.h"
EXPECTED = {
    "shots.cpp": "7345597d535340f3e27664749dcc659a0376ddc043d8738a424364f1c4b18f08",
    "xlate.cpp": "a21315034a5fd9773af6757db1c64d2b5abb8ae1668c22b8c3c296c66b758e3e",
    "protocol.h": "b07f632c28651cf0cd6c5ebe6f56e54f39217c52c6c227f09b1ff2ebf34dddd5",
}
# The pinned bytes fix the wire tag as well; this is only how the log names it.
_WIRE = re.compile(rb'k_wire_tag\[\]\s*=\s*"([^"]+)"')


def wire_tag(data: bytes) -> str:
    match = _WIRE.search(data)
    return match.group(1).decode("ascii") if match else ""


def verify_fixture(fixture: Path, current_protocol: Path) -> dict[str, bytes]:
    """Check the pinned bytes and return them. The current header is not required to match."""
    files = {}
    for name, expected in EXPECTED.items():
        # Git may check text out with CRLF on Windows; the pinned hashes are for LF bytes.
        data = (fixture / name).read_bytes().replace(b"\r\n", b"\n")
        actual = hashlib.sha256(data).hexdigest()
        if actual != expected:
            raise ValueError(f"1.0.7 {name} did not match its pinned SHA-256: {actual}")
        files[name] = data
    return files


def header_note(files: dict[str, bytes], current_protocol: Path) -> str:
    """How the current wire header relates to the pinned one, for the log."""
    current = wire_tag(current_protocol.read_bytes().replace(b"\r\n", b"\n"))
    pinned = wire_tag(files["protocol.h"])
    if not current:
        return f"current wire tag not found; the pinned fixture is {pinned}"
    if current == pinned:
        return f"the current wire tag is still {current}"
    return (f"the current wire tag {current} differs from the pinned {pinned}; "
            f"the fixture is a fixed 1.0.7 reference and is not rebuilt from the current source")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("out", type=Path, help="ignored build directory for generated fixtures")
    args = parser.parse_args()
    files = verify_fixture(FIXTURE, CURRENT_PROTOCOL)
    args.out.mkdir(parents=True, exist_ok=True)
    for name, data in files.items():
        (args.out / name).write_bytes(data)
    print(f"protocol-17 compatibility fixture: pinned 1.0.7 shots/xlate/header verified; "
          f"{header_note(files, CURRENT_PROTOCOL)}")


if __name__ == "__main__":
    main()

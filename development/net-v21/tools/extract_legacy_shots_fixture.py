#!/usr/bin/env python3
"""Extract the immutable 1.0.7 sources for a no-game protocol-17 compatibility test.

The review export is already checked in. Verify the actual bytes, not just its labels, and
refuse to silently compare against a different protocol. Generated files live under build/.
"""
from __future__ import annotations

import argparse
import hashlib
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[3]
EXPORT = ROOT / "review-export" / "MiusVitals_net_v21_1.0.7_production_code.txt"
EXPECTED = {
    "shots.cpp": "7345597d535340f3e27664749dcc659a0376ddc043d8738a424364f1c4b18f08",
    "xlate.cpp": "a21315034a5fd9773af6757db1c64d2b5abb8ae1668c22b8c3c296c66b758e3e",
    "protocol.h": "b07f632c28651cf0cd6c5ebe6f56e54f39217c52c6c227f09b1ff2ebf34dddd5",
}


def extract(bundle: bytes, name: str) -> bytes:
    path = f"development/net-v21/src/{name}".encode("ascii")
    pattern = rb"==================== BEGIN " + re.escape(path) + rb" \(sha256 ([0-9a-f]{64})\) ====================\n"
    matches = list(re.finditer(pattern, bundle))
    if len(matches) != 1:
        raise ValueError(f"expected exactly one 1.0.7 section for {name}")
    match = matches[0]
    end = b"==================== END " + path + b" ===================="
    end_at = bundle.find(end, match.end())
    if end_at < 0:
        raise ValueError(f"missing end marker for {name}")
    data = bundle[match.end():end_at]
    actual = hashlib.sha256(data).hexdigest()
    if match.group(1).decode("ascii") != EXPECTED[name] or actual != EXPECTED[name]:
        raise ValueError(f"1.0.7 {name} did not match its pinned SHA-256: {actual}")
    return data


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("out", type=Path, help="ignored build directory for generated fixtures")
    args = parser.parse_args()
    # Git may check text out with CRLF on Windows; the pinned export records Git's LF bytes.
    bundle = EXPORT.read_bytes().replace(b"\r\n", b"\n")
    files = {name: extract(bundle, name) for name in EXPECTED}
    current_protocol = ROOT / "development" / "net-v21" / "src" / "protocol.h"
    if current_protocol.read_bytes().replace(b"\r\n", b"\n") != files["protocol.h"]:
        raise ValueError("the current wire header differs from 1.0.7; review the compatibility fixture")
    args.out.mkdir(parents=True, exist_ok=True)
    for name, data in files.items():
        (args.out / name).write_bytes(data)
    print("protocol-17 compatibility fixture: pinned 1.0.7 shots/xlate/header verified")


if __name__ == "__main__":
    main()

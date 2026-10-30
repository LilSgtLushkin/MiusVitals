#!/usr/bin/env python3
"""Guard the M5/non-tactical state source against changes during the tactical work.

The embedded baseline is the exact statesync.cpp from commit 30405d1, before the
1c87118 state hardening. The wrapper isolates all original code and types in a
namespace; the runtime integration test separately exercises both dispatch paths.
"""
from hashlib import sha256
from pathlib import Path
import sys

SOURCE = Path(__file__).resolve().parents[1] / "src" / "statesync_legacy_30405d1.inc"
EXPECTED = "d932d7f46423621f1628c480fe01f874a0c2703d1c088236a444b6cf8c348671"


def main() -> int:
    actual = sha256(SOURCE.read_bytes()).hexdigest()
    if actual != EXPECTED:
        print(f"FAIL: frozen non-tactical state source differs: {actual}", file=sys.stderr)
        return 1
    print(f"non-tactical state source matches 30405d1: {actual}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

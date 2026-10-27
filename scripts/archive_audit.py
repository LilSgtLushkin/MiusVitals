#!/usr/bin/env python3
"""Find duplicate root ZIP payloads without confusing distinct builds with duplicate installers.

All file names and uncompressed bytes are compared; ZIP compression, timestamps and the build
passport alone do not make a new payload. A protected _known_good rollback may intentionally be
retained beside an identical build. Use --check in CI to fail on any other duplicate or bad ZIP.
"""
from __future__ import annotations

import argparse
import hashlib
from pathlib import Path
import sys
import zipfile

ROOT = Path(__file__).resolve().parents[1]
PASSPORT = "MiusMultiplayer/TEST_BUILD.txt"
ROLLBACK = "MiusMultiplayer_v1.0.0_TEST_INSTALL_known_good.zip"


def fingerprint(path: Path) -> tuple[tuple[str, str], ...]:
    """Return names and SHA-256 for every file except the non-runtime build passport."""
    with zipfile.ZipFile(path) as archive:
        if bad := archive.testzip():
            raise ValueError(f"CRC check failed for {bad}")
        files = [entry for entry in archive.infolist() if not entry.is_dir()]
        names = [entry.filename for entry in files]
        if len(names) != len(set(names)):
            raise ValueError("duplicate file names in ZIP")
        entries = []
        for entry in files:
            if entry.filename == PASSPORT:
                continue
            digest = hashlib.sha256()
            with archive.open(entry) as stream:
                for block in iter(lambda: stream.read(1 << 20), b""):
                    digest.update(block)
            entries.append((entry.filename, digest.hexdigest()))
        return tuple(sorted(entries))


def duplicate_groups(paths: list[Path]) -> list[tuple[Path, ...]]:
    groups: dict[tuple[tuple[str, str], ...], list[Path]] = {}
    for path in sorted(paths):
        groups.setdefault(fingerprint(path), []).append(path)
    return [tuple(group) for group in groups.values() if len(group) > 1]


def is_redundant(group: tuple[Path, ...]) -> bool:
    # The designated recovery copy is always retained, even if another archive matches it.
    return sum(path.name != ROLLBACK for path in group) > 1


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--check", action="store_true", help="fail on corrupt or redundant archives")
    args = parser.parse_args()
    paths = sorted(ROOT.glob("*.zip"))
    try:
        groups = duplicate_groups(paths)
    except (OSError, ValueError, zipfile.BadZipFile, RuntimeError) as error:
        print(f"archive audit failed: {error}", file=sys.stderr)
        return 1
    if not groups:
        print(f"OK {len(paths)} archives: no duplicate payloads (excluding build passports)")
        return 0
    redundant = False
    for group in groups:
        flagged = is_redundant(group)
        redundant |= flagged
        note = "redundant" if flagged else "protected rollback retained"
        print(f"{note}: {', '.join(path.name for path in group)}")
    return 1 if args.check and redundant else 0


if __name__ == "__main__":
    sys.exit(main())

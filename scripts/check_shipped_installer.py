#!/usr/bin/env python3
"""Verify the shipped installer against the SHA-256 recorded beside it.

The installer is the only 1.0.7 archive in the tree and publish-installer.yml rebuilds it from the
gated source, so its bytes change with the source. No fixed hash can therefore be pinned in
scripts/check_artifacts.py: that pin would be written by the same job that builds the file and
would prove nothing.

What can still be checked, and is checked here:

1. the packager's own verification of the archive - the kit inside is complete and consistent, the
   module and launcher carry the matching wire-protocol tag, the rollback copy is the module the
   installer writes;
2. the archive still matches the SHA-256 recorded when it was built, so a truncated, rewritten or
   silently replaced copy cannot pass.

This is the single implementation: scripts/verify_release.sh and the release gate both call it,
instead of each carrying its own copy of the check.
"""
from __future__ import annotations

import argparse
import hashlib
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_NAME = "MiusMultiplayer_v1.0.7_EXPERIMENTAL_NET+FPS_INSTALL.zip"


def recorded_digest(archive: Path) -> tuple[str, str]:
    """Return the SHA-256 and the source commit recorded beside the archive."""
    record = archive.with_name(archive.name + ".sha256.txt")
    lines = record.read_text().split("\n")
    fields = lines[0].split()
    if len(fields) != 2:
        raise SystemExit(f"unexpected record format in {record.name}: {lines[0]!r}")
    if fields[1] != archive.name:
        raise SystemExit(f"{record.name} describes {fields[1]}, not {archive.name}")
    if len(lines) < 2 or not lines[1].startswith("source commit: "):
        raise SystemExit(f"{record.name} does not name the source commit")
    return fields[0], lines[1][len("source commit: "):].strip()


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("archive", nargs="?", default=str(ROOT / DEFAULT_NAME),
                        help=f"installer to check (default: {DEFAULT_NAME} in the repository root)")
    args = parser.parse_args()
    archive = Path(args.archive)
    if not archive.is_file():
        raise SystemExit(f"the shipped installer is absent: {archive}")

    sys.path.insert(0, str(ROOT / "scripts"))
    import package_net_v21_test as packager  # noqa: E402  (the path is set above)

    packager.verify(archive)
    expected, commit = recorded_digest(archive)
    digest = hashlib.sha256(archive.read_bytes()).hexdigest()
    if digest != expected:
        raise SystemExit(f"{archive.name} SHA-256 {digest} does not match the recorded {expected}")
    print(f"OK {archive.name}: {digest}, matches the record for source commit {commit}")
    return 0


if __name__ == "__main__":
    sys.exit(main())

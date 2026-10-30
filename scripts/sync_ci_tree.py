#!/usr/bin/env python3
"""Copy a version folder onto development/net-v21, which is the tree the CI jobs build.

Version folders under development/versions/ are the working copies; the workflows still
point at development\\net-v21, and that does not change until the checks themselves are
reworked. So before a release the fixed tree is mirrored there, and this script proves
the mirror is byte-identical afterwards.

    python3 scripts/sync_ci_tree.py 1.0.8

Only files git tracks in development/net-v21 are managed, so build output left in the
working tree is left alone.
"""
from __future__ import annotations

import subprocess
import sys
from pathlib import Path

SRC_ROOT = Path("development/versions")
DST = Path("development/net-v21")


def tracked_under(path: Path) -> set[str]:
    out = subprocess.run(
        ["git", "ls-files", str(path)], capture_output=True, text=True, check=True
    ).stdout.split()
    prefix = str(path) + "/"
    return {p[len(prefix):] for p in out if p.startswith(prefix)}


def walk(path: Path) -> dict[str, Path]:
    return {
        str(p.relative_to(path)): p for p in path.rglob("*") if p.is_file()
    }


def main() -> int:
    if len(sys.argv) != 2:
        print("usage: sync_ci_tree.py <version>   e.g. sync_ci_tree.py 1.0.8")
        return 2
    src = SRC_ROOT / sys.argv[1]
    if not src.is_dir():
        print(f"FAIL {src} does not exist")
        return 1
    if not DST.is_dir():
        print(f"FAIL {DST} does not exist")
        return 1

    source = walk(src)
    known = tracked_under(DST)

    copied = updated = 0
    for rel, p in source.items():
        target = DST / rel
        data = p.read_bytes()
        if not target.is_file():
            copied += 1
        elif target.read_bytes() != data:
            updated += 1
        else:
            continue
        target.parent.mkdir(parents=True, exist_ok=True)
        target.write_bytes(data)

    removed = 0
    for rel in sorted(known - set(source)):
        (DST / rel).unlink()
        removed += 1

    print(f"  {src} -> {DST}: {copied} new, {updated} changed, {removed} removed")

    left = walk(src)
    right = walk(DST)
    problems = []
    for rel, p in left.items():
        q = right.get(rel)
        if q is None:
            problems.append(f"missing in {DST}: {rel}")
        elif q.read_bytes() != p.read_bytes():
            problems.append(f"differs: {rel}")
    extra = sorted(set(tracked_under(DST)) - set(left))
    for rel in extra:
        problems.append(f"stale file left in {DST}: {rel}")

    if problems:
        for p in problems[:20]:
            print(f"FAIL {p}")
        print(f"FAIL {len(problems)} mismatch(es). The CI tree does not match {src}.")
        return 1
    print(f"OK {DST} is byte-identical to {src} ({len(left)} files)")
    return 0


if __name__ == "__main__":
    sys.exit(main())

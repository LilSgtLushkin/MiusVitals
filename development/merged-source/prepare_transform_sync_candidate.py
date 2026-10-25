#!/usr/bin/env python3
"""Create a disposable source-only basic_network candidate with the FPS patch applied.

The output is for Windows source-build analysis only.  It is never copied to
MiusMultiplayer and cannot reuse the released Vitals RVA profile.
"""
from __future__ import annotations

import argparse
from pathlib import Path
import shutil
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
SOURCE = ROOT / "development/upstream/miusplayer-public-43b2736/basic_network"
PATCH = ROOT / "development/merged-source/patches/0001-elide-idempotent-remote-transform-writes.patch"
DEFAULT_OUTPUT = Path("development/merged-source/candidates/transform-sync")
PREFIX = "development/upstream/miusplayer-public-43b2736/"


def root_relative(path_text: str) -> Path:
    path = Path(path_text)
    absolute = path.resolve() if path.is_absolute() else (ROOT / path).resolve()
    try:
        return absolute.relative_to(ROOT)
    except ValueError as exc:
        raise SystemExit("candidate output must remain inside the repository") from exc


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--output", default=str(DEFAULT_OUTPUT), help="repository-relative disposable output directory")
    args = parser.parse_args()
    output = root_relative(args.output)
    if output == SOURCE.relative_to(ROOT) or SOURCE.relative_to(ROOT) in output.parents:
        raise SystemExit("candidate output must not overwrite the immutable pinned public source")
    target = ROOT / output
    if target.exists():
        raise SystemExit(f"candidate directory already exists: {output} (remove it explicitly first)")

    # First prove the patch still describes the immutable upstream baseline.
    subprocess.run(["git", "apply", "--check", str(PATCH)], cwd=ROOT, check=True)
    shutil.copytree(SOURCE, target / "basic_network")
    rewritten = (target / ".transform-sync.patch")
    patch = PATCH.read_text(encoding="utf-8")
    if PREFIX not in patch:
        shutil.rmtree(target)
        raise SystemExit("candidate patch does not target the pinned source prefix")
    rewritten.write_text(patch.replace(PREFIX, ""), encoding="utf-8")
    try:
        subprocess.run(["git", "apply", f"--directory={output}", "--check", str(rewritten)], cwd=ROOT, check=True)
        subprocess.run(["git", "apply", f"--directory={output}", str(rewritten)], cwd=ROOT, check=True)
    except subprocess.CalledProcessError:
        shutil.rmtree(target)
        raise

    print(f"Prepared source-only transform candidate: {output / 'basic_network'}")
    print("Do not install its output. Build/PE/Vitals-profile/two-peer validation are still required.")


if __name__ == "__main__":
    main()

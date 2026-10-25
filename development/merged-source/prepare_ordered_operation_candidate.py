#!/usr/bin/env python3
"""Prepare a disposable C++ candidate with transform and ordered-operation overlays.

The candidate is source-only.  It must not be copied into an install kit: rebuilding
also requires a matching launcher, PE/profile validation, and two-peer Windows tests.
"""
from __future__ import annotations

import argparse
from pathlib import Path
import shutil
import subprocess

ROOT = Path(__file__).resolve().parents[2]
SOURCE = ROOT / "development/upstream/miusplayer-public-43b2736/basic_network"
PATCHES = (
    ROOT / "development/merged-source/patches/0001-elide-idempotent-remote-transform-writes.patch",
    ROOT / "development/merged-source/patches/0002-ordered-operation-turns-and-network-hygiene.patch",
)
PREFIX = "development/upstream/miusplayer-public-43b2736/"
DEFAULT_OUTPUT = Path("development/merged-source/candidates/ordered-operation-network")


def repo_relative(value: str) -> Path:
    path = Path(value)
    absolute = path.resolve() if path.is_absolute() else (ROOT / path).resolve()
    try:
        return absolute.relative_to(ROOT)
    except ValueError as exc:
        raise SystemExit("candidate output must remain inside the repository") from exc


def apply_overlay(output: Path, patch_path: Path) -> None:
    patch = patch_path.read_text(encoding="utf-8")
    if PREFIX not in patch:
        raise SystemExit(f"{patch_path.name} does not target the pinned source prefix")
    rewritten = ROOT / output / f".{patch_path.stem}.patch"
    rewritten.write_text(patch.replace(PREFIX, ""), encoding="utf-8")
    try:
        subprocess.run(["git", "apply", f"--directory={output}", "--check", str(rewritten)], cwd=ROOT, check=True)
        subprocess.run(["git", "apply", f"--directory={output}", str(rewritten)], cwd=ROOT, check=True)
    finally:
        rewritten.unlink(missing_ok=True)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--output", default=str(DEFAULT_OUTPUT), help="repository-relative disposable output directory")
    args = parser.parse_args()
    output = repo_relative(args.output)
    source_relative = SOURCE.relative_to(ROOT)
    if output == source_relative or source_relative in output.parents:
        raise SystemExit("candidate output must not overwrite the immutable pinned public source")
    target = ROOT / output
    if target.exists():
        raise SystemExit(f"candidate directory already exists: {output} (remove it explicitly first)")

    # The first overlay is always defined directly against the immutable source tree.
    subprocess.run(["git", "apply", "--check", str(PATCHES[0])], cwd=ROOT, check=True)
    shutil.copytree(SOURCE, target / "basic_network")
    try:
        for patch in PATCHES:
            apply_overlay(output, patch)
    except Exception:
        shutil.rmtree(target)
        raise

    print(f"Prepared source-only ordered-operation candidate: {output / 'basic_network'}")
    print("Do not install its output. Matching launcher/PE/Vitals-profile/two-peer validation are still required.")


if __name__ == "__main__":
    main()

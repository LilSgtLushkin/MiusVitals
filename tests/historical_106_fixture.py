"""Read the retired 1.0.6 installer as a pinned Git blob, without restoring it to the tree."""
from __future__ import annotations

import hashlib
from pathlib import Path
import subprocess

ROOT = Path(__file__).resolve().parents[1]
REVISION = "047195f1b16f034cf06f4a204402e6e800ca05ea"
NAME = "MiusMultiplayer_v1.0.6_TEST_INSTALL.zip"
SHA256 = "ce4f80223b4a876b5d65b0c7ddc90b818c9fb7bf63de1bb827af1d0dccf284c5"


def read_verified_blob(repo: Path, revision: str, name: str, expected: str) -> bytes:
    try:
        result = subprocess.run(["git", "show", f"{revision}:{name}"], cwd=repo,
                                stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                                check=False, timeout=60)
    except (OSError, subprocess.TimeoutExpired) as error:
        raise RuntimeError(f"cannot read the historical installer from Git: {error}") from error
    if result.returncode:
        raise RuntimeError(f"historical installer is unavailable; fetch its pinned commit: "
                           f"git fetch --no-tags --depth=1 origin {revision}")
    actual = hashlib.sha256(result.stdout).hexdigest()
    if actual != expected:
        raise ValueError(f"historical installer SHA-256 mismatch: {actual}, expected {expected}")
    return result.stdout


def read_106() -> bytes:
    return read_verified_blob(ROOT, REVISION, NAME, SHA256)

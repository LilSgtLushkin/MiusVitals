#!/usr/bin/env python3
"""Fetch the two reference modules the comparisons need.

  v12  the module built from the published source - what our baseline build should reproduce
  v20  the module players actually run   - what the Steam layer is measured against

Both are binaries from the CC0 MiusPlayer repository, pinned to one commit and checked against the
SHA-256 that the rest of this repository already treats as authoritative
(docs/BUILD_AND_VALIDATION.md).  Nothing is committed here: `development/versions/<версия>/references/` is
git-ignored.

    python development/versions/<версия>/tools/fetch_references.py
"""
from __future__ import annotations

import hashlib
import io
import sys
import urllib.request
import zipfile
from pathlib import Path

COMMIT = "43b2736d3b4a1e208e6c80f45f6ebbdd476de3aa"
RAW = f"https://raw.githubusercontent.com/VampyrCat/MiusPlayer/{COMMIT}/"
V12_PATH = "_mp_research/basic_network/build/basic_network.codelib"
V20_ZIP = "GTMPRelease1.zip"
V20_PATH = "mpmod1/files/bin/rel/x64/basic_network.codelib"

EXPECTED = {
    "v12.codelib": "eff7ae12c5f440dc91d0fccc9d9578022a383b0485da58793bffd217922f7af5",
    "v20.codelib": "8108bd1e9af8080ef8b1050512b8311848f5a336acb49a7479388733b4ae5723",
}

OUT = Path(__file__).resolve().parent.parent / "references"


def download(url: str) -> bytes:
    with urllib.request.urlopen(url, timeout=120) as response:
        return response.read()


def main() -> int:
    OUT.mkdir(parents=True, exist_ok=True)
    targets = {
        "v12.codelib": lambda: download(RAW + V12_PATH),
        "v20.codelib": lambda: zipfile.ZipFile(io.BytesIO(download(RAW + V20_ZIP))).read(V20_PATH),
    }
    failures = 0
    for name, produce in targets.items():
        path = OUT / name
        if path.is_file() and hashlib.sha256(path.read_bytes()).hexdigest() == EXPECTED[name]:
            print(f"{name}: already present and correct")
            continue
        try:
            data = produce()
        except Exception as error:  # noqa: BLE001 - the network is the only thing that can fail here
            print(f"{name}: download failed: {error}", file=sys.stderr)
            failures += 1
            continue
        digest = hashlib.sha256(data).hexdigest()
        if digest != EXPECTED[name]:
            print(f"{name}: sha256 {digest}, expected {EXPECTED[name]}", file=sys.stderr)
            failures += 1
            continue
        path.write_bytes(data)
        print(f"{name}: {len(data)} bytes, sha256 ok")
    return 1 if failures else 0


if __name__ == "__main__":
    raise SystemExit(main())

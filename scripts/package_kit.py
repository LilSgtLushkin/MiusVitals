#!/usr/bin/env python3
"""Create or validate the exact downloadable MiusVitals v7.2.1 kit.

The repository deliberately contains development files as well as the kit.  This
script is the allow-list between the two: no manifest, verifier, workflow, cache,
or release helper can silently enter the player download.
"""
from __future__ import annotations

import argparse
import hashlib
import os
from pathlib import Path
import sys
import zipfile

VERSION = "7.2.1"
# `development/kit-source` contains the editable source and verification helpers.
# It is deliberately not named `MiusVitals`, so a repository checkout is not mistaken
# for the player install. The generated archive still has the required game root.
SOURCE_KIT = Path("development/kit-source")
ARCHIVE_ROOT = "MiusVitals"
# This is the complete, ordered, player-visible tree requested for this release.
DIRS = tuple(f"{ARCHIVE_ROOT}{suffix}" for suffix in (
    "/",
    "/files/",
    "/files/alt/",
    "/src/",
    "/src/test/",
    "/src/test/stub/",
))
FILES = (
    "Check_Build.ps1",
    "Uninstall_Vitals.bat",
    "known_3598.sha256",
    "Check_Build.bat",
    "Install_Vitals.bat",
    "README_VITALS.txt",
    "src/mp_vitals.c",
    "src/patch_import.py",
    "src/build.sh",
    "src/rebase_3598_1.py",
    "src/test/t.c",
    "src/test/stub/windows.h",
    "files/mp_vitals.dll",
    "files/basic_network.codelib",
    "files/alt/basic_network_nostart.codelib",
    "files/alt/basic_network_v5_norebase.codelib",
)
# ZIP timestamps are intentionally fixed: this produces byte-identical output
# from identical kit files on every supported host.
ZIP_TIME = (2026, 10, 25, 0, 0, 0)


def sha256(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def expected_names() -> tuple[str, ...]:
    return DIRS + tuple(f"{ARCHIVE_ROOT}/{relative}" for relative in FILES)


def kit_path(repo: Path) -> Path:
    return repo / SOURCE_KIT


def make_info(name: str, directory: bool = False) -> zipfile.ZipInfo:
    info = zipfile.ZipInfo(name, date_time=ZIP_TIME)
    info.create_system = 3
    # Keep the POSIX permission deterministic too. build.sh remains directly
    # executable after extracting on POSIX; player-facing Windows files need no
    # executable bit there.
    mode = 0o40755 if directory else (0o100755 if name.endswith("/build.sh") else 0o100644)
    info.external_attr = mode << 16
    info.compress_type = zipfile.ZIP_DEFLATED
    return info


def require_source(repo: Path) -> None:
    missing = [relative for relative in FILES if not (kit_path(repo) / relative).is_file()]
    if missing:
        raise SystemExit("kit source is incomplete: " + ", ".join(missing))


def packed_source_bytes(path: Path) -> bytes:
    """Make the player-facing Windows launchers invariant to Git checkout EOLs.

    .gitattributes deliberately checks *.bat and *.ps1 out as CRLF. The archive
    is committed and must verify byte-for-byte on Linux CI as well as on a
    Windows checkout, so package those two formats canonically as CRLF instead
    of depending on the host's working-tree conversion.
    """
    data = path.read_bytes()
    if path.suffix.lower() in {".bat", ".ps1"}:
        return data.replace(b"\r\n", b"\n").replace(b"\r", b"\n").replace(b"\n", b"\r\n")
    return data


def build(repo: Path, output: Path) -> None:
    require_source(repo)
    output.parent.mkdir(parents=True, exist_ok=True)
    temporary = output.with_name(output.name + ".tmp")
    try:
        with zipfile.ZipFile(temporary, "w", compression=zipfile.ZIP_DEFLATED, compresslevel=9, strict_timestamps=True) as archive:
            for name in DIRS:
                archive.writestr(make_info(name, directory=True), b"")
            for relative in FILES:
                archive.writestr(make_info(f"{ARCHIVE_ROOT}/{relative}"), packed_source_bytes(kit_path(repo) / relative))
        os.replace(temporary, output)
    finally:
        temporary.unlink(missing_ok=True)
    verify(repo, output)


def verify(repo: Path, archive_path: Path) -> None:
    require_source(repo)
    if not archive_path.is_file():
        raise SystemExit(f"downloadable kit is absent: {archive_path}")
    wanted = expected_names()
    with zipfile.ZipFile(archive_path) as archive:
        bad = archive.testzip()
        if bad:
            raise SystemExit(f"ZIP CRC failed: {bad}")
        names = tuple(archive.namelist())
        if names != wanted:
            raise SystemExit("ZIP tree differs from v7.2.1 allow-list:\n"
                             f"  expected: {list(wanted)}\n  got: {list(names)}")
        for info in archive.infolist():
            if info.date_time != ZIP_TIME:
                raise SystemExit(f"ZIP timestamp is not reproducible: {info.filename}")
        for relative in FILES:
            source = packed_source_bytes(kit_path(repo) / relative)
            packed = archive.read(f"{ARCHIVE_ROOT}/{relative}")
            if packed != source:
                raise SystemExit(f"ZIP content differs from kit source: {relative}")
    print(f"OK {archive_path}: {len(FILES)} files, exact v{VERSION} kit tree, sha256 {sha256(archive_path.read_bytes())}")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--output", type=Path, help="write the deterministic kit archive")
    parser.add_argument("--verify", type=Path, help="verify an existing archive")
    args = parser.parse_args()
    if bool(args.output) == bool(args.verify):
        parser.error("use exactly one of --output or --verify")
    repo = Path(__file__).resolve().parents[1]
    target = (args.output or args.verify).resolve()
    if args.output:
        build(repo, target)
    else:
        verify(repo, target)


if __name__ == "__main__":
    main()

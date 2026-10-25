#!/usr/bin/env python3
"""Create or verify the v1.0.1 Steam + FPS + Vitals one-archive installer.

The old v1.0.0 archive deliberately keeps the original Steam-capable module.
This release is a separately versioned, complete player installer whose module
is the deterministic Steam-preserving FPS candidate.  It is not the removed
experimental codelib-only installer wrapped under a different name.
"""
from __future__ import annotations

import argparse
import hashlib
import os
from pathlib import Path
import zipfile

from build_steam_compatible_fps_candidate import verify_candidate

VERSION = "1.0.1"
SOURCE = Path("development/unified-fps-kit-source")
ROOT = "MiusMultiplayer"
FPS_HASH = "7b42a69abe58deb1583d2f5320eebbecd9718ffa27ea08933613e67df1901f67"
HASHES = {
    "files/bin/rel/x64/basic_network.codelib": FPS_HASH,
    "files/mp_vitals.dll": "71a1bf3a37923bbe6c8ee84448d0b91705c2bff7382b647f990b9d671ee224df",
    "files/mp_join.exe": "6aa417cf6e1f01480c997446f2691a58ab55b90fa36e1ff511945989aadfeeaa",
    "files/root/programs/opgame.progpack": "eca75b740f7b11d9e13f2e2001f1b2b133427d0860fb3770220b89b0b8909535",
    "files/root/programs/tacgame.progpack": "01923eabdf6a9d5fa37dc70a3d76740242dbc9a7f26c0298ca3faa8cca87600d",
    "files/steam_appid.txt": "1a43840ca36f1401c85ff5f6e6981088927cfe09b23d155a9c4301dd13ca9610",
    "game_build_3598_1.sha256": "2e2be1939cab6b9871fabb1a33396ed01a88ffc02eb9b974786e73afa2ebc8b8",
}
FILES = (
    "Enable_Multiplayer.bat",
    "Disable_Multiplayer.bat",
    "Check_Install.bat",
    "Check_Install.ps1",
    "README.txt",
    "LICENSE_CC0.txt",
    *HASHES.keys(),
)
DIRS = tuple(f"{ROOT}{suffix}" for suffix in (
    "/", "/files/", "/files/bin/", "/files/bin/rel/", "/files/bin/rel/x64/",
    "/files/root/", "/files/root/programs/",
))
ZIP_TIME = (2026, 10, 25, 0, 0, 0)


def sha256(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def source_path(repo: Path) -> Path:
    return repo / SOURCE


def packed_bytes(path: Path) -> bytes:
    data = path.read_bytes()
    if path.suffix.lower() in {".bat", ".ps1", ".txt", ".sha256"}:
        return data.replace(b"\r\n", b"\n").replace(b"\r", b"\n").replace(b"\n", b"\r\n")
    return data


def expected_names() -> tuple[str, ...]:
    return DIRS + tuple(f"{ROOT}/{relative}" for relative in FILES)


def require_sources(repo: Path) -> None:
    source = source_path(repo)
    missing = [relative for relative in FILES if not (source / relative).is_file()]
    if missing:
        raise SystemExit("unified FPS kit source is incomplete: " + ", ".join(missing))
    for relative, expected in HASHES.items():
        found = sha256((source / relative).read_bytes())
        if found != expected:
            raise SystemExit(f"unexpected unified FPS runtime artifact {relative}: {found}, expected {expected}")

    candidate = (source / "files/bin/rel/x64/basic_network.codelib").read_bytes()
    beta_candidate = (repo / "development/fps-beta-build/basic_network.codelib").read_bytes()
    stable = (repo / "development/kit-source/files/basic_network.codelib").read_bytes()
    try:
        verify_candidate(candidate, stable)
    except ValueError as exc:
        raise SystemExit(f"unified FPS module is not the exact Steam-preserving candidate: {exc}") from exc
    if candidate != beta_candidate:
        raise SystemExit("unified FPS module differs from the checked FPS candidate build")

    for relative in ("files/root/programs/opgame.progpack", "files/root/programs/tacgame.progpack"):
        if b"basic_network.codelib" not in (source / relative).read_bytes():
            raise SystemExit(f"program pack no longer loads basic_network.codelib: {relative}")

    enable = (source / "Enable_Multiplayer.bat").read_text(encoding="utf-8")
    disable = (source / "Disable_Multiplayer.bat").read_text(encoding="utf-8")
    check = (source / "Check_Install.ps1").read_text(encoding="utf-8")
    readme = (source / "README.txt").read_text(encoding="utf-8")
    # The installer must replace every runtime file unconditionally. The markers below are the
    # unconditional write path; the forbidden markers are the old gates that could abort the
    # install (or leave it half done) and thereby leave the main menu without MULTIPLAYER.
    for marker in (
        "v1.0.1", f"NETWORK={FPS_HASH.upper()}", "Steam 3598/1", "force-replaced",
        "call :keep", "call :put", ":overwrite", "state\\installed.txt", "call :unblock",
        "files\\mp_vitals.dll", "MpVitalsInit", "NOT REPLACED:",
    ):
        if marker not in enable:
            raise SystemExit(f"unified FPS installer lost required marker: {marker}")
    for gate in (
        "certutil", "known_network", "copy_checked", "foreign_network", "ODD", "goto :wrong_game",
        "not as expected:", "SHA-256 verification failed", "game_build_3598_1.sha256", 'if /i "%H%"',
    ):
        if gate in enable:
            raise SystemExit(f"unified FPS installer must not contain the replacement gate: {gate}")
    for marker in ("v1.0.1", FPS_HASH.upper(), "call :drop", "call :restore", "installed.txt", "left changed file"):
        if marker not in disable:
            raise SystemExit(f"unified FPS uninstaller lost required marker: {marker}")
    for marker in ("v1.0.1", FPS_HASH.upper()):
        if marker not in check:
            raise SystemExit(f"unified FPS checker lost required marker: {marker}")
    for marker in (
        "v1.0.1", FPS_HASH, ".fpsfix", "Steam Friends", "SteamNetworking", "mp_vitals.dll", "zero-ms",
        "MULTIPLAYER", "mp_vitals.dll!MpVitalsInit", "basic_network_opgame",
    ):
        if marker not in readme:
            raise SystemExit(f"unified FPS README lost required provenance or behavior: {marker}")


def info(name: str, directory: bool = False) -> zipfile.ZipInfo:
    result = zipfile.ZipInfo(name, date_time=ZIP_TIME)
    result.create_system = 3
    result.external_attr = (0o40755 if directory else 0o100644) << 16
    result.compress_type = zipfile.ZIP_DEFLATED
    return result


def verify(repo: Path, archive_path: Path) -> None:
    require_sources(repo)
    if not archive_path.is_file():
        raise SystemExit(f"unified FPS install archive is absent: {archive_path}")
    with zipfile.ZipFile(archive_path) as archive:
        if archive.testzip():
            raise SystemExit(f"ZIP CRC failed: {archive.testzip()}")
        if tuple(archive.namelist()) != expected_names():
            raise SystemExit("ZIP tree differs from the unified FPS MiusMultiplayer allow-list")
        for entry in archive.infolist():
            if entry.date_time != ZIP_TIME:
                raise SystemExit(f"ZIP timestamp is not reproducible: {entry.filename}")
        for relative in FILES:
            packed = archive.read(f"{ROOT}/{relative}")
            if packed != packed_bytes(source_path(repo) / relative):
                raise SystemExit(f"ZIP content differs from unified FPS kit source: {relative}")
    print(f"OK {archive_path}: {len(FILES)} files, exact MiusMultiplayer v{VERSION} tree, sha256 {sha256(archive_path.read_bytes())}")


def build(repo: Path, archive_path: Path) -> None:
    require_sources(repo)
    archive_path.parent.mkdir(parents=True, exist_ok=True)
    temporary = archive_path.with_name(archive_path.name + ".tmp")
    try:
        with zipfile.ZipFile(temporary, "w", compression=zipfile.ZIP_DEFLATED, compresslevel=9, strict_timestamps=True) as archive:
            for name in DIRS:
                archive.writestr(info(name, directory=True), b"")
            for relative in FILES:
                archive.writestr(info(f"{ROOT}/{relative}"), packed_bytes(source_path(repo) / relative))
        os.replace(temporary, archive_path)
    finally:
        temporary.unlink(missing_ok=True)
    verify(repo, archive_path)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--output", type=Path)
    parser.add_argument("--verify", type=Path)
    args = parser.parse_args()
    if bool(args.output) == bool(args.verify):
        parser.error("use exactly one of --output or --verify")
    repo = Path(__file__).resolve().parents[1]
    if args.output:
        build(repo, args.output)
    else:
        verify(repo, args.verify)


if __name__ == "__main__":
    main()

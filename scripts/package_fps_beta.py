#!/usr/bin/env python3
"""Create or validate the separate experimental MiusMultiplayer beta_FPS_fix installer."""
from __future__ import annotations

import argparse
import hashlib
import os
from pathlib import Path
import struct
import zipfile

VERSION = "beta_FPS_fix"
SOURCE = Path("development/fps-beta-kit-source")
ROOT = "MiusMultiplayer_beta_FPS_fix"
FILES = (
    "Enable_Beta_FPS_Fix.bat",
    "Disable_Beta_FPS_Fix.bat",
    "Check_Beta_FPS_Fix.bat",
    "README.txt",
    "BUILD_INFO.txt",
    "LICENSE_CC0.txt",
    "candidate.sha256",
    "files/bin/rel/x64/basic_network.codelib",
)
DIRS = tuple(f"{ROOT}{suffix}" for suffix in (
    "/", "/files/", "/files/bin/", "/files/bin/rel/", "/files/bin/rel/x64/",
))
ZIP_TIME = (2026, 10, 25, 0, 0, 0)


def sha256(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def source_path(repo: Path) -> Path:
    return repo / SOURCE


def packed_bytes(path: Path) -> bytes:
    data = path.read_bytes()
    if path.suffix.lower() in {".bat", ".txt", ".sha256"}:
        return data.replace(b"\r\n", b"\n").replace(b"\r", b"\n").replace(b"\n", b"\r\n")
    return data


def expected_names() -> tuple[str, ...]:
    return DIRS + tuple(f"{ROOT}/{relative}" for relative in FILES)


def require_pe_candidate(repo: Path, path: Path) -> None:
    """Reject any beta payload not rebuilt by the narrow Steam-preserving transform."""
    data = path.read_bytes()
    if data[:2] != b"MZ" or len(data) < 0x100:
        raise SystemExit("FPS beta codelib is not a PE file")
    pe, = struct.unpack_from("<I", data, 0x3C)
    if data[pe:pe + 4] != b"PE\0\0":
        raise SystemExit("FPS beta codelib has no PE signature")
    machine, = struct.unpack_from("<H", data, pe + 4)
    magic, = struct.unpack_from("<H", data, pe + 24)
    if machine != 0x8664 or magic != 0x20B:
        raise SystemExit("FPS beta codelib must be x64 PE32+")
    try:
        from build_steam_compatible_fps_candidate import verify_candidate
        stable = (repo / "development/kit-source/files/basic_network.codelib").read_bytes()
        verify_candidate(data, stable)
    except (OSError, ValueError) as exc:
        raise SystemExit(f"FPS beta candidate is not the verified Steam-preserving transformation: {exc}") from exc


def require_sources(repo: Path) -> None:
    source = source_path(repo)
    missing = [relative for relative in FILES if not (source / relative).is_file()]
    if missing:
        raise SystemExit("FPS beta source is incomplete: " + ", ".join(missing))
    candidate = source / "files/bin/rel/x64/basic_network.codelib"
    built = repo / "development/fps-beta-build/basic_network.codelib"
    build_record = repo / "development/fps-beta-build/candidate.sha256"
    candidate_hash = sha256(candidate.read_bytes())
    if not built.is_file() or not build_record.is_file() or candidate.read_bytes() != built.read_bytes():
        raise SystemExit("FPS beta source codelib is not the deterministic build candidate")
    if build_record.read_text(encoding="ascii").strip().split() != [candidate_hash, "basic_network.codelib"]:
        raise SystemExit("deterministic FPS beta build hash record is inconsistent")
    record = (source / "candidate.sha256").read_text(encoding="ascii").strip().split()
    if record != [candidate_hash, "basic_network.codelib"]:
        raise SystemExit("FPS beta candidate.sha256 does not identify the packaged codelib")
    require_pe_candidate(repo, candidate)
    enable = (source / "Enable_Beta_FPS_Fix.bat").read_text(encoding="utf-8")
    disable = (source / "Disable_Beta_FPS_Fix.bat").read_text(encoding="utf-8")
    check = (source / "Check_Beta_FPS_Fix.bat").read_text(encoding="utf-8")
    readme = (source / "README.txt").read_text(encoding="utf-8")
    for marker in ("%CANDIDATE%", "%LEGACY_CANDIDATE%", "LEGACY_CANDIDATE=78E271", "call :backup_previous", "call :restore_backup_after_failed_install", "%BACKUP_HASH%", "including an unrecognised one", "Backup verification failed", "stale beta backup", "if /i \"%ACTIVE_HASH%\"==\"%EXPECTED_BACKUP_HASH%\" exit /b 0", "call :copy_checked"):
        if marker not in enable:
            raise SystemExit(f"FPS beta installer lost required backup/restore marker: {marker}")
    if "call :known_network" in enable or "will not overwrite an unknown module" in enable:
        raise SystemExit("FPS beta installer must safely back up and replace unknown codelibs")
    if "if not exist \"%BACKUP%\" goto :create_backup" not in enable or "if exist \"%BACKUP%\" (" in enable:
        raise SystemExit("FPS beta installer must compare a saved hash after subroutines, outside a cmd block")
    for marker in ("%CANDIDATE%", "%LEGACY_CANDIDATE%", "LEGACY_CANDIDATE=78E271", "%BACKUP_HASH%", "if not exist \"%BACKUP%\" goto :remove_candidate", "call :read_backup_hash", "Restored the exact codelib", "Nothing was changed"):
        if marker not in disable:
            raise SystemExit(f"FPS beta remover lost required verified-restore marker: {marker}")
    for marker in ("LEGACY_CANDIDATE=78E271", "WITHDRAWN public-source beta candidate", "lacks Steam friends/P2P support"):
        if marker not in check:
            raise SystemExit(f"FPS beta checker lost required legacy Steam-regression repair marker: {marker}")
    for marker in ("EXPERIMENTAL", "Vitals", "compatibility", "including an unknown module", "SHA-256", candidate_hash, "Steam friends", "JoinRequested", ".fpsfix", "zero-millisecond", "recognizes and restores"):
        if marker not in readme:
            raise SystemExit(f"FPS beta README lost required Steam/FPS warning or provenance: {marker}")


def info(name: str, directory: bool = False) -> zipfile.ZipInfo:
    result = zipfile.ZipInfo(name, date_time=ZIP_TIME)
    result.create_system = 3
    result.external_attr = (0o40755 if directory else 0o100644) << 16
    result.compress_type = zipfile.ZIP_DEFLATED
    return result


def verify(repo: Path, archive_path: Path) -> None:
    require_sources(repo)
    if not archive_path.is_file():
        raise SystemExit(f"FPS beta archive is absent: {archive_path}")
    with zipfile.ZipFile(archive_path) as archive:
        if archive.testzip():
            raise SystemExit(f"FPS beta ZIP CRC failed: {archive.testzip()}")
        if tuple(archive.namelist()) != expected_names():
            raise SystemExit("FPS beta ZIP tree differs from the allow-list")
        for entry in archive.infolist():
            if entry.date_time != ZIP_TIME:
                raise SystemExit(f"FPS beta ZIP timestamp is not reproducible: {entry.filename}")
        for relative in FILES:
            packed = archive.read(f"{ROOT}/{relative}")
            if packed != packed_bytes(source_path(repo) / relative):
                raise SystemExit(f"FPS beta ZIP content differs from beta source: {relative}")
    print(f"OK {archive_path}: {len(FILES)} files, exact MiusMultiplayer {VERSION} tree, sha256 {sha256(archive_path.read_bytes())}")


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

#!/usr/bin/env python3
"""Create or verify the GTMP_alpha installer.

GTMP_alpha is *not* a new mod: it is the reviewed v1.0.1 install tree with exactly one runtime
file replaced — a `mp_vitals.dll` rebuilt from the current source — plus the same installer
scripts relabelled so a tester can tell the two kits apart and uninstall the alpha DLL.

Everything else (basic_network.codelib, mp_join.exe, both program packs, steam_appid.txt) is
byte-identical to v1.0.1 and is taken from `development/unified-fps-kit-source`, which
`package_unified_fps_mod.py` already pins.  That keeps the alpha honest: it cannot silently
ship a different module.
"""
from __future__ import annotations

import argparse
import hashlib
import os
from pathlib import Path
import sys
import zipfile

sys.path.insert(0, str(Path(__file__).resolve().parent))
import package_unified_fps_mod as stable  # noqa: E402  (same directory, reviewed kit definition)

VERSION = "GTMP_alpha"
ALPHA = Path("development/gtmp-alpha")
ROOT = "MiusMultiplayer"
STABLE_DLL = "71a1bf3a37923bbe6c8ee84448d0b91705c2bff7382b647f990b9d671ee224df"
ALPHA_DLL = "808e1d5f6cb301ac971700e0aba6e78d1df1b13b02e748b8d01dd4bfc8a9881c"
# Files that exist only in the alpha kit: the player notes and the one-file log collector.
ALPHA_EXTRA = ("ALPHA.txt", "Collect_Logs.bat", "Collect_Logs.ps1")
FILES = stable.FILES + ALPHA_EXTRA
DIRS = stable.DIRS
DLL_PATH = "files/mp_vitals.dll"
TEXT_SUFFIXES = {".bat", ".ps1", ".txt", ".sha256"}
# Alpha ships one day later than the stable tree and keeps its own reproducible timestamp.
ZIP_TIME = (2026, 10, 26, 0, 0, 0)


def sha256(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def crlf(data: bytes) -> bytes:
    return data.replace(b"\r\n", b"\n").replace(b"\r", b"\n").replace(b"\n", b"\r\n")


def alpha_text(relative: str, data: bytes) -> bytes:
    """Relabel a stable installer text and point it at the alpha DLL."""
    data = crlf(data)
    data = data.replace(STABLE_DLL.upper().encode(), ALPHA_DLL.upper().encode())
    data = data.replace(STABLE_DLL.encode(), ALPHA_DLL.encode())
    data = data.replace(b"v1.0.1", VERSION.encode())
    if relative == "README.txt":
        if not data.endswith(b"\r\n"):
            data += b"\r\n"
        appendix = (Path(__file__).resolve().parents[1] / ALPHA / "extra" / "README_APPENDIX.txt").read_bytes().strip()
        data += crlf(appendix) + b"\r\n"
    return data


def packed_bytes(repo: Path, relative: str) -> bytes:
    if relative in ALPHA_EXTRA:
        return crlf((repo / ALPHA / ("extra" if relative != "ALPHA.txt" else "") / relative).read_bytes())
    if relative == DLL_PATH:
        return (repo / ALPHA / DLL_PATH).read_bytes()
    data = (repo / stable.SOURCE / relative).read_bytes()
    if Path(relative).suffix.lower() in TEXT_SUFFIXES:
        return alpha_text(relative, data)
    return data


def expected_names() -> tuple[str, ...]:
    return DIRS + tuple(f"{ROOT}/{relative}" for relative in FILES)


def require_sources(repo: Path) -> None:
    # The whole reviewed v1.0.1 chain must still be intact: hashes, exact FPS candidate, installer markers.
    stable.require_sources(repo)
    dll = repo / ALPHA / DLL_PATH
    if not dll.is_file():
        raise SystemExit(f"alpha DLL is absent: {ALPHA / DLL_PATH}")
    found = sha256(dll.read_bytes())
    if found != ALPHA_DLL:
        raise SystemExit(f"unexpected alpha mp_vitals.dll: {found}, expected {ALPHA_DLL}")
    for extra in ALPHA_EXTRA:
        path = repo / ALPHA / ("extra" if extra != "ALPHA.txt" else "") / extra
        if not path.is_file():
            raise SystemExit(f"alpha file is absent: {path}")
    if ALPHA_DLL.upper().encode() not in (repo / ALPHA / "ALPHA.txt").read_bytes():
        raise SystemExit("ALPHA.txt does not name the alpha DLL hash")
    if b"GTMP_alpha_log.txt" not in packed_bytes(repo, "README.txt"):
        raise SystemExit("alpha README does not tell the tester how to send one report file")
    if b"GTMP_alpha_log.txt" not in packed_bytes(repo, "ALPHA.txt"):
        raise SystemExit("ALPHA.txt does not name the report file")
    # The relabelled installer must not keep a v1.0.1 marker, and must name the alpha DLL.
    for relative in stable.FILES:
        if Path(relative).suffix.lower() not in TEXT_SUFFIXES:
            continue
        packed = packed_bytes(repo, relative)
        if b"v1.0.1" in packed:
            raise SystemExit(f"alpha installer text still says v1.0.1: {relative}")
    enable = packed_bytes(repo, "Enable_Multiplayer.bat").decode("utf-8")
    disable = packed_bytes(repo, "Disable_Multiplayer.bat").decode("utf-8")
    check = packed_bytes(repo, "Check_Install.ps1").decode("utf-8")
    for name, text, marker in (
        ("Enable_Multiplayer.bat", enable, f"VITALS={ALPHA_DLL.upper()}"),
        ("Enable_Multiplayer.bat", enable, "call :keep"),
        ("Disable_Multiplayer.bat", disable, f"VITALS={ALPHA_DLL.upper()}"),
        ("Disable_Multiplayer.bat", disable, "call :drop"),
        ("Check_Install.ps1", check, ALPHA_DLL.upper()),
        ("Check_Install.ps1", check, VERSION),
    ):
        if marker not in text:
            raise SystemExit(f"{name} lost required alpha marker: {marker}")


def info(name: str, directory: bool = False) -> zipfile.ZipInfo:
    result = zipfile.ZipInfo(name, date_time=ZIP_TIME)
    result.create_system = 3
    result.external_attr = (0o40755 if directory else 0o100644) << 16
    result.compress_type = zipfile.ZIP_DEFLATED
    return result


def verify(repo: Path, archive_path: Path) -> None:
    require_sources(repo)
    if not archive_path.is_file():
        raise SystemExit(f"alpha install archive is absent: {archive_path}")
    with zipfile.ZipFile(archive_path) as archive:
        if archive.testzip():
            raise SystemExit(f"ZIP CRC failed: {archive.testzip()}")
        if tuple(archive.namelist()) != expected_names():
            raise SystemExit("ZIP tree differs from the GTMP_alpha allow-list")
        for entry in archive.infolist():
            if entry.date_time != ZIP_TIME:
                raise SystemExit(f"ZIP timestamp is not reproducible: {entry.filename}")
        for relative in FILES:
            packed = archive.read(f"{ROOT}/{relative}")
            if packed != packed_bytes(repo, relative):
                raise SystemExit(f"ZIP content differs from its source: {relative}")
    print(f"OK {archive_path}: {len(FILES)} files, GTMP_alpha tree over MiusMultiplayer v1.0.1, "
          f"sha256 {sha256(archive_path.read_bytes())}")


def build(repo: Path, archive_path: Path) -> None:
    require_sources(repo)
    archive_path.parent.mkdir(parents=True, exist_ok=True)
    temporary = archive_path.with_name(archive_path.name + ".tmp")
    try:
        with zipfile.ZipFile(temporary, "w", compression=zipfile.ZIP_DEFLATED, compresslevel=9, strict_timestamps=True) as archive:
            for name in DIRS:
                archive.writestr(info(name, directory=True), b"")
            for relative in FILES:
                archive.writestr(info(f"{ROOT}/{relative}"), packed_bytes(repo, relative))
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

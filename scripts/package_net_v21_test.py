#!/usr/bin/env python3
"""Package the net_v21 test installer: the proven v1.0.1 install plus our own module beside it.

Why a separate archive and not a new release
--------------------------------------------
The module built in `development/net-v21/` is our own code, but it is not v20: the battle delta of
M3-M5 is still being written, and it deliberately does not import `mp_vitals.dll`.  Shipping it as
*the* module would make every player's first v1.0.0 experience worse than v1.0.1, which is exactly
what this project must not do.

So this archive installs exactly what v1.0.1 installs - the same bytes, checked against the same
hashes - and carries our module in `net_v21/` beside it, with a switch script and a one-click way
back.  Installing this archive changes nothing about how the game runs until somebody runs
`Switch_To_NetV21.bat`, and even then the proven module travels in the same archive.

The one thing that is new is that our module logs: identity, traffic and battle counters, into
`out/net_v21_<program>.log`.  That log is the point of the test.

    python scripts/package_net_v21_test.py --codelib build/module/basic_network.codelib \
        --output MiusMultiplayer_v1.0.0_TEST_INSTALL.zip
    python scripts/package_net_v21_test.py --verify MiusMultiplayer_v1.0.0_TEST_INSTALL.zip
"""
from __future__ import annotations

import argparse
import hashlib
import os
from pathlib import Path
import zipfile

VERSION = "1.0.0"
ROOT = "MiusMultiplayer"
PROVEN = Path("development/unified-fps-kit-source")
KIT = Path("development/net-v21-kit")

# Everything taken from the proven v1.0.1 tree.  Binaries are pinned by hash: if one of them moves,
# this archive is no longer "v1.0.1 plus a switch" and packaging must stop instead of shipping a
# mixture nobody validated.
PROVEN_BINARIES = {
    "files/bin/rel/x64/basic_network.codelib": "7b42a69abe58deb1583d2f5320eebbecd9718ffa27ea08933613e67df1901f67",
    "files/mp_vitals.dll": "71a1bf3a37923bbe6c8ee84448d0b91705c2bff7382b647f990b9d671ee224df",
    "files/mp_join.exe": "6aa417cf6e1f01480c997446f2691a58ab55b90fa36e1ff511945989aadfeeaa",
    "files/root/programs/opgame.progpack": "eca75b740f7b11d9e13f2e2001f1b2b133427d0860fb3770220b89b0b8909535",
    "files/root/programs/tacgame.progpack": "01923eabdf6a9d5fa37dc70a3d76740242dbc9a7f26c0298ca3faa8cca87600d",
    "files/steam_appid.txt": "1a43840ca36f1401c85ff5f6e6981088927cfe09b23d155a9c4301dd13ca9610",
}
PROVEN_TEXT = ("Enable_Multiplayer.bat", "Disable_Multiplayer.bat", "Check_Install.bat",
               "Check_Install.ps1", "README.txt", "LICENSE_CC0.txt", "game_build_3598_1.sha256")
KIT_TEXT = ("net_v21/Switch_To_NetV21.bat", "net_v21/Switch_Back_To_Proven.bat",
            "net_v21/Collect_Logs.bat", "net_v21/README_TEST.txt")
PASSPORT = "TEST_BUILD.txt"

BINARY_SUFFIXES = {".codelib", ".dll", ".exe", ".progpack"}
ZIP_TIME = (2026, 10, 26, 0, 0, 0)
DIRS = tuple(f"{ROOT}{suffix}" for suffix in (
    "/", "/files/", "/files/bin/", "/files/bin/rel/", "/files/bin/rel/x64/",
    "/files/root/", "/files/root/programs/", "/net_v21/",
))


def sha256(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def with_crlf(name: str, data: bytes) -> bytes:
    """Text files go into the archive with CRLF (a .bat with LF does not run on Windows)."""
    if Path(name).suffix.lower() in BINARY_SUFFIXES:
        return data
    return data.replace(b"\r\n", b"\n").replace(b"\r", b"\n").replace(b"\n", b"\r\n")


def collect(repo: Path, codelib: Path) -> dict[str, bytes]:
    """The archive's whole tree, before CRLF handling: name -> raw bytes."""
    tree: dict[str, bytes] = {}
    for relative, expected in PROVEN_BINARIES.items():
        source = repo / PROVEN / relative
        if not source.is_file():
            raise SystemExit(f"the proven v1.0.1 tree is incomplete: {relative}")
        data = source.read_bytes()
        if sha256(data) != expected:
            raise SystemExit(f"{relative}: {sha256(data)} is not the proven v1.0.1 file {expected}")
        tree[relative] = data
    for relative in PROVEN_TEXT:
        source = repo / PROVEN / relative
        if not source.is_file():
            raise SystemExit(f"the proven v1.0.1 tree is incomplete: {relative}")
        tree[relative] = source.read_bytes()
    for relative in KIT_TEXT:
        source = repo / KIT / relative
        if not source.is_file():
            raise SystemExit(f"the net_v21 kit is incomplete: {relative}")
        tree[relative] = source.read_bytes()

    tree["net_v21/basic_network.netv21.codelib"] = codelib.read_bytes()
    tree["net_v21/basic_network.proven.codelib"] = tree["files/bin/rel/x64/basic_network.codelib"]

    module_hash = sha256(tree["net_v21/basic_network.netv21.codelib"])
    passport = [
        f"MiusMultiplayer {VERSION} - test archive",
        "",
        f"net_v21 module:         net_v21/basic_network.netv21.codelib",
        f"net_v21 sha256:         {module_hash}",
        f"net_v21 size:           {len(tree['net_v21/basic_network.netv21.codelib'])} bytes",
        "",
        "the archive installs these proven v1.0.1 files:",
    ]
    for relative in sorted(PROVEN_BINARIES):
        passport.append(f"  {sha256(tree[relative])}  {relative}")
    for relative in sorted(PROVEN_TEXT):
        passport.append(f"  {sha256(tree[relative])}  {relative}")
    passport += [
        "",
        "Enable_Multiplayer.bat writes the files above, exactly as v1.0.1 does.",
        "The net_v21 module is used only after net_v21\\Switch_To_NetV21.bat;",
        "net_v21\\Switch_Back_To_Proven.bat restores the proven module from this same archive.",
    ]
    tree[PASSPORT] = ("\n".join(passport) + "\n").encode("utf-8")
    return tree


def info(name: str, directory: bool = False) -> zipfile.ZipInfo:
    result = zipfile.ZipInfo(name, date_time=ZIP_TIME)
    result.create_system = 3
    result.external_attr = (0o40755 if directory else 0o100644) << 16
    result.compress_type = zipfile.ZIP_DEFLATED
    return result


def verify(archive_path: Path) -> None:
    if not archive_path.is_file():
        raise SystemExit(f"archive is absent: {archive_path}")
    with zipfile.ZipFile(archive_path) as archive:
        if archive.testzip():
            raise SystemExit(f"ZIP CRC failed: {archive.testzip()}")
        names = tuple(archive.namelist())
        if names[:len(DIRS)] != DIRS:
            raise SystemExit("ZIP does not start with the expected directories")
        for entry in archive.infolist():
            if entry.date_time != ZIP_TIME:
                raise SystemExit(f"ZIP timestamp is not reproducible: {entry.filename}")
        stamp = archive.read(f"{ROOT}/{PASSPORT}").decode()
        module = archive.read(f"{ROOT}/net_v21/basic_network.netv21.codelib")
        if not module:
            raise SystemExit("the net_v21 module in the archive is empty")
        if sha256(module) not in stamp:
            raise SystemExit("the passport does not describe the module in the archive")
        proven = archive.read(f"{ROOT}/files/bin/rel/x64/basic_network.codelib")
        if proven != archive.read(f"{ROOT}/net_v21/basic_network.proven.codelib"):
            raise SystemExit("the rollback copy is not the module the installer writes")
        if sha256(proven) != PROVEN_BINARIES["files/bin/rel/x64/basic_network.codelib"]:
            raise SystemExit("the installed module is not the proven v1.0.1 module")
        for relative, expected in PROVEN_BINARIES.items():
            if sha256(archive.read(f"{ROOT}/{relative}")) != expected:
                raise SystemExit(f"{relative} is not the proven v1.0.1 file")
        if b"basic_network.codelib" not in archive.read(f"{ROOT}/files/root/programs/opgame.progpack"):
            raise SystemExit("opgame.progpack no longer loads basic_network.codelib")
        for relative in KIT_TEXT:
            if not archive.read(f"{ROOT}/{relative}"):
                raise SystemExit(f"kit script is empty: {relative}")
    print(f"OK {archive_path}: {len(names)} entries, net_v21 module {sha256(module)[:16]}, "
          f"archive sha256 {sha256(archive_path.read_bytes())}")


def build(repo: Path, codelib: Path, archive_path: Path) -> None:
    if not codelib.is_file():
        raise SystemExit(f"no module to package: {codelib}")
    tree = collect(repo, codelib)
    archive_path.parent.mkdir(parents=True, exist_ok=True)
    temporary = archive_path.with_name(archive_path.name + ".tmp")
    try:
        with zipfile.ZipFile(temporary, "w", compression=zipfile.ZIP_DEFLATED, compresslevel=9,
                             strict_timestamps=True) as archive:
            for name in DIRS:
                archive.writestr(info(name, directory=True), b"")
            for name in sorted(tree):
                archive.writestr(info(f"{ROOT}/{name}"), with_crlf(name, tree[name]))
        os.replace(temporary, archive_path)
    finally:
        temporary.unlink(missing_ok=True)
    verify(archive_path)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--output", type=Path)
    parser.add_argument("--verify", type=Path)
    parser.add_argument("--codelib", type=Path,
                        default=Path("development/net-v21/build/module/basic_network.codelib"))
    args = parser.parse_args()
    if bool(args.output) == bool(args.verify):
        parser.error("use exactly one of --output or --verify")
    repo = Path(__file__).resolve().parents[1]
    if args.output:
        codelib = args.codelib if args.codelib.is_absolute() else repo / args.codelib
        build(repo, codelib, args.output)
    else:
        verify(args.verify)


if __name__ == "__main__":
    main()

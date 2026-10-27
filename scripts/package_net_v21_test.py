#!/usr/bin/env python3
"""Package the net_v21 test installer: the proven v1.0.1 install plus our own module beside it.

Why a separate archive and not a new release
--------------------------------------------
The module built in `development/net-v21/` is our own candidate, but hit/animation replication and
its `mp_vitals.dll` integration are not complete. Shipping it as the default would replace working
functionality from the proven v1.0.1 module. This is a test archive, not a full 1.0.4 release.

So this archive installs exactly what v1.0.1 installs - the same bytes, checked against the same
hashes - and carries our module in `net_v21/` beside it, with a switch script and a one-click way
back.  Installing this archive changes nothing about how the game runs until somebody runs
`Switch_To_NetV21.bat`, and even then the proven module travels in the same archive.

The candidate logs identity, traffic and battle counters into `out/net_v21_<program>.log`.
The archive includes a log collector and preserves the known-good rollback separately.

    python scripts/package_net_v21_test.py --codelib build/module/basic_network.codelib \
        --output MiusMultiplayer_v1.0.4_TEST_INSTALL.zip
    python scripts/package_net_v21_test.py --verify MiusMultiplayer_v1.0.4_TEST_INSTALL.zip
"""
from __future__ import annotations

import argparse
import hashlib
import os
from pathlib import Path
import re
import struct
import zipfile

VERSION = "1.0.4"
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
            "net_v21/Collect_Logs.bat", "net_v21/Collect_Logs.ps1", "net_v21/README_TEST.txt")
PASSPORT = "TEST_BUILD.txt"

BINARY_SUFFIXES = {".codelib", ".dll", ".exe", ".progpack"}
ZIP_TIME = (2026, 10, 26, 0, 0, 0)
DIRS = tuple(f"{ROOT}{suffix}" for suffix in (
    "/", "/files/", "/files/bin/", "/files/bin/rel/", "/files/bin/rel/x64/",
    "/files/root/", "/files/root/programs/", "/net_v21/",
))


def sha256(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def validate_module(data: bytes) -> None:
    """Reject a truncated/non-x64/non-DLL payload or one without the engine and Steam exports."""
    def fail(reason: str) -> None:
        raise SystemExit(f"invalid net_v21 module: {reason}")

    def read(fmt: str, at: int):
        size = struct.calcsize(fmt)
        if at < 0 or at + size > len(data):
            fail("truncated PE data")
        return struct.unpack_from(fmt, data, at)

    if len(data) < 0x40 or data[:2] != b"MZ":
        fail("missing DOS header")
    (pe,) = read("<I", 0x3c)
    if pe < 0x40 or pe + 24 > len(data) or data[pe:pe + 4] != b"PE\0\0":
        fail("missing PE header")
    machine, nsections, _, _, _, opt_size, flags = read("<HHIIIHH", pe + 4)
    opt = pe + 24
    if machine != 0x8664 or not flags & 0x2000 or not 1 <= nsections <= 96:
        fail("expected an x86-64 DLL with sections")
    if opt_size < 120 or opt + opt_size > len(data) or read("<H", opt)[0] != 0x20b:
        fail("expected a PE32+ optional header")
    section_table = opt + opt_size
    if section_table + 40 * nsections > len(data):
        fail("truncated section table")
    sections = []
    for i in range(nsections):
        at = section_table + 40 * i
        virtual_size, rva, raw_size, raw = read("<IIII", at + 8)
        if raw + raw_size > len(data):
            fail("truncated section")
        sections.append((rva, virtual_size, raw, raw_size))

    def offset(rva: int, size: int) -> int:
        for start, virtual_size, raw, raw_size in sections:
            relative = rva - start
            if 0 <= relative and relative + size <= raw_size and relative + size <= max(virtual_size, raw_size):
                return raw + relative
        fail("export address is outside file-backed sections")
        return 0

    if read("<I", opt + 108)[0] < 1:
        fail("missing data directories")
    export_rva, export_size = read("<II", opt + 112)
    if not export_rva or export_size < 40:
        fail("missing exports")
    directory = offset(export_rva, 40)
    n_names = read("<I", directory + 24)[0]
    names_rva = read("<I", directory + 32)[0]
    if not 1 <= n_names <= 4096:
        fail("invalid export count")
    names_offset = offset(names_rva, n_names * 4)
    names = set()
    for i in range(n_names):
        name_rva = read("<I", names_offset + i * 4)[0]
        at = offset(name_rva, 1)
        end = data.find(b"\0", at, min(len(data), at + 256))
        if end < 0:
            fail("unterminated export name")
        offset(name_rva, end - at + 1)
        names.add(data[at:end])
    if not {b"GetFactory3", b"gtmf_steam_layer"} <= names:
        fail("GetFactory3 or gtmf_steam_layer export is missing")


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

    candidate = codelib.read_bytes()
    validate_module(candidate)
    if candidate == tree["files/bin/rel/x64/basic_network.codelib"]:
        raise SystemExit("candidate is the proven module, not the net_v21 build")
    tree["net_v21/basic_network.netv21.codelib"] = candidate
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
        # The archive stores CRLF even when the source checkout has LF. The passport must
        # describe the bytes in the ZIP, not the platform-dependent checkout bytes.
        passport.append(f"  {sha256(with_crlf(relative, tree[relative]))}  {relative}")
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
        validate_module(module)
        module_hash_lines = [line.partition(":")[2].strip() for line in stamp.splitlines()
                             if line.startswith("net_v21 sha256:")]
        if module_hash_lines != [sha256(module)]:
            raise SystemExit("the passport does not describe the module in the archive")
        passport_files = {}
        for line in stamp.splitlines():
            match = re.fullmatch(r"\s*([0-9a-f]{64})\s{2}(.+?)\s*", line)
            if not match:
                continue
            digest, relative = match.groups()
            if relative in passport_files:
                raise SystemExit(f"duplicate passport entry: {relative}")
            passport_files[relative] = digest
        expected_files = set(PROVEN_BINARIES) | set(PROVEN_TEXT)
        if set(passport_files) != expected_files:
            raise SystemExit("the passport file list is incomplete or unexpected")
        for relative, digest in passport_files.items():
            if sha256(archive.read(f"{ROOT}/{relative}")) != digest:
                raise SystemExit(f"passport hash mismatch: {relative}")
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
            name = f"{ROOT}/{relative}"
            # Older rollback archives used a self-contained .bat.  A newer .bat that calls the
            # PowerShell collector must never be accepted without that companion file.
            if relative == "net_v21/Collect_Logs.ps1" and name not in names:
                bat = archive.read(f"{ROOT}/net_v21/Collect_Logs.bat")
                if b"Collect_Logs.ps1" in bat:
                    raise SystemExit("collector calls a missing Collect_Logs.ps1")
                continue
            if not archive.read(name):
                raise SystemExit(f"kit script is empty: {relative}")
    print(f"OK {archive_path}: {len(names)} entries, net_v21 module {sha256(module)[:16]}, "
          f"archive sha256 {sha256(archive_path.read_bytes())}")


def build(repo: Path, codelib: Path, archive_path: Path) -> None:
    # New builds must never overwrite any preserved release/test or the known-good rollback.
    protected = ("MiusMultiplayer_v1.0.0_TEST_INSTALL_known_good.zip",
                 "MiusMultiplayer_v1.0.0_TEST_INSTALL.zip", "MiusMultiplayer_v1.0.0_M3M4_INSTALL.zip",
                 "MiusMultiplayer_v1.0.1_INSTALL.zip", "MiusMultiplayer_v1.0.2_TEST_INSTALL.zip")
    if archive_path.resolve() in {(repo / name).resolve() for name in protected}:
        raise SystemExit("refusing to overwrite a preserved archive")
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

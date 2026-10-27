#!/usr/bin/env python3
"""Build the single MiusMultiplayer 1.0.5 TEST archive.

The matched net_v21 module and launcher are the actual files installed by Enable_Multiplayer.bat;
there is no separate patch switch. The archive is experimental and does not claim real-game or
Steam validation. Historical archives are verified using their original layout and are protected
from overwrite.
"""
from __future__ import annotations

import argparse
import hashlib
import os
from pathlib import Path
import re
import struct
import zipfile

VERSION = "1.0.5"
WIRE_TAG = b"NETV21_WIRE_PROTOCOL_14"
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
KIT_TEXT = ("net_v21/Switch_To_NetV21.bat", "net_v21/Switch_Back_To_Proven.bat", "net_v21/Switch_Pair.bat",
            "net_v21/Collect_Logs.bat", "net_v21/Collect_Logs.ps1", "net_v21/README_TEST.txt")
PASSPORT = "TEST_BUILD.txt"
UNIFIED = KIT / "unified"
UNIFIED_TEXT = ("Enable_Multiplayer.bat", "Disable_Multiplayer.bat", "Apply_Multiplayer.ps1",
                "Check_Install.bat", "Check_Install.ps1", "README.txt")
COLLECTOR_TEXT = ("Collect_Logs.bat", "Collect_Logs.ps1")
COMMON_TEXT = ("LICENSE_CC0.txt", "game_build_3598_1.sha256")
MODULE_PATH = "files/bin/rel/x64/basic_network.codelib"
LAUNCHER_PATH = "files/mp_join.exe"
SHARED_BINARIES = {name: digest for name, digest in PROVEN_BINARIES.items()
                   if name not in (MODULE_PATH, LAUNCHER_PATH)}
FILES_MANIFEST = "FILES_SHA256.txt"

BINARY_SUFFIXES = {".codelib", ".dll", ".exe", ".progpack"}
ZIP_TIME = (2026, 10, 26, 0, 0, 0)
DIRS = tuple(f"{ROOT}{suffix}" for suffix in (
    "/", "/files/", "/files/bin/", "/files/bin/rel/", "/files/bin/rel/x64/",
    "/files/root/", "/files/root/programs/", "/net_v21/",
))
UNIFIED_DIRS = DIRS[:-1]


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


def validate_launcher(data: bytes) -> None:
    """Reject truncated or non-x64 executable payloads, without running them."""
    def fail(reason: str) -> None:
        raise SystemExit(f"invalid net_v21 launcher: {reason}")

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
    if machine != 0x8664 or not flags & 0x0002 or flags & 0x2000 or not 1 <= nsections <= 96:
        fail("expected an x86-64 executable with sections")
    if opt_size < 120 or opt + opt_size > len(data) or read("<H", opt)[0] != 0x20b:
        fail("expected a PE32+ optional header")
    section_table = opt + opt_size
    if section_table + nsections * 40 > len(data):
        fail("truncated section table")
    for i in range(nsections):
        section = section_table + i * 40
        raw_size, raw_at = read("<II", section + 16)
        if raw_size and (raw_at > len(data) or raw_size > len(data) - raw_at):
            fail("section outside file")


def with_crlf(name: str, data: bytes) -> bytes:
    """Text files go into the archive with CRLF (a .bat with LF does not run on Windows)."""
    if Path(name).suffix.lower() in BINARY_SUFFIXES:
        return data
    return data.replace(b"\r\n", b"\n").replace(b"\r", b"\n").replace(b"\n", b"\r\n")


def collect(repo: Path, codelib: Path, launcher: Path) -> dict[str, bytes]:
    """Assemble a one-step installer; all runtime files are under files/."""
    tree: dict[str, bytes] = {}
    for relative, expected in SHARED_BINARIES.items():
        source = repo / PROVEN / relative
        if not source.is_file() or sha256(source.read_bytes()) != expected:
            raise SystemExit(f"shared runtime payload is missing or changed: {relative}")
        tree[relative] = source.read_bytes()
    for relative in COMMON_TEXT:
        source = repo / PROVEN / relative
        if not source.is_file():
            raise SystemExit(f"common kit file is missing: {relative}")
        tree[relative] = source.read_bytes()
    for relative in UNIFIED_TEXT:
        source = repo / UNIFIED / relative
        if not source.is_file():
            raise SystemExit(f"unified kit file is missing: {relative}")
        tree[relative] = source.read_bytes()
    for relative in COLLECTOR_TEXT:
        source = repo / KIT / "net_v21" / relative
        if not source.is_file():
            raise SystemExit(f"collector file is missing: {relative}")
        tree[relative] = source.read_bytes()
    candidate, candidate_launcher = codelib.read_bytes(), launcher.read_bytes()
    validate_module(candidate)
    validate_launcher(candidate_launcher)
    if WIRE_TAG not in candidate or WIRE_TAG not in candidate_launcher:
        raise SystemExit("module and launcher must contain the matching net_v21 wire-protocol tag")
    if sha256(candidate) == PROVEN_BINARIES[MODULE_PATH] or sha256(candidate_launcher) == PROVEN_BINARIES[LAUNCHER_PATH]:
        raise SystemExit("candidate module and launcher must not be the old proven binaries")
    tree[MODULE_PATH] = candidate
    tree[LAUNCHER_PATH] = candidate_launcher
    runtime = sorted((*SHARED_BINARIES, MODULE_PATH, LAUNCHER_PATH))
    tree[FILES_MANIFEST] = ("\n".join(f"{sha256(tree[name])}  {name}" for name in runtime) + "\n").encode()
    passport = [f"MiusMultiplayer {VERSION} - test archive", "layout: unified", "",
                f"wire protocol: {WIRE_TAG.decode('ascii')}",
                f"net_v21 sha256: {sha256(candidate)}", f"launcher sha256: {sha256(candidate_launcher)}", "",
                "Enable_Multiplayer.bat installs the entire test kit, including net_v21; no separate patch switch.",
                "Files in this archive (SHA-256 of actual bytes):"]
    for relative in sorted(tree):
        passport.append(f"  {sha256(with_crlf(relative, tree[relative]))}  {relative}")
    tree[PASSPORT] = ("\n".join(passport) + "\n").encode("utf-8")
    return tree


def info(name: str, directory: bool = False) -> zipfile.ZipInfo:
    result = zipfile.ZipInfo(name, date_time=ZIP_TIME)
    result.create_system = 3
    result.external_attr = (0o40755 if directory else 0o100644) << 16
    result.compress_type = zipfile.ZIP_DEFLATED
    return result


def _verify_legacy(archive_path: Path) -> None:
    if not archive_path.is_file():
        raise SystemExit(f"archive is absent: {archive_path}")
    with zipfile.ZipFile(archive_path) as archive:
        if archive.testzip():
            raise SystemExit(f"ZIP CRC failed: {archive.testzip()}")
        names = tuple(archive.namelist())
        if len(names) != len(set(names)):
            raise SystemExit("ZIP contains duplicate entry names")
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
        # Historical archives predate paired launchers. Version 1.0.5 cannot fall back to that layout.
        version_match = re.search(r"^MiusMultiplayer (\d+)\.(\d+)\.(\d+) - test archive", stamp, re.M)
        version = tuple(map(int, version_match.groups())) if version_match else (0, 0, 0)
        paired = version >= (1, 0, 5) or "net_v21 launcher sha256:" in stamp
        if paired:
            launcher_name = f"{ROOT}/net_v21/mp_join.netv21.exe"
            if launcher_name not in names:
                raise SystemExit("missing candidate launcher")
            launcher = archive.read(launcher_name)
            validate_launcher(launcher)
            if WIRE_TAG not in module or WIRE_TAG not in launcher or f"wire protocol:          {WIRE_TAG.decode('ascii')}" not in stamp:
                raise SystemExit("module/launcher wire protocol tag mismatch")
            if f"net_v21 launcher sha256: {sha256(launcher)}" not in stamp:
                raise SystemExit("the passport does not describe the candidate launcher")
            if f"proven launcher sha256: {PROVEN_BINARIES['files/mp_join.exe']}" not in stamp:
                raise SystemExit("the passport does not describe the proven launcher")
            kit_lines = [line for line in stamp.splitlines() if line.startswith("kit sha256: ")]
            if len(kit_lines) != len(KIT_TEXT):
                raise SystemExit("the passport kit list is incomplete")
            for relative in KIT_TEXT:
                name = f"{ROOT}/{relative}"
                if name not in names:
                    raise SystemExit(f"missing kit file: {relative}")
                if f"kit sha256: {sha256(archive.read(name))}  {relative}" not in kit_lines:
                    raise SystemExit(f"kit passport hash mismatch: {relative}")
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
            if relative == "net_v21/Switch_Pair.bat" and name not in names:
                if any(b"Switch_Pair.bat" in archive.read(f"{ROOT}/net_v21/{wrapper}")
                       for wrapper in ("Switch_To_NetV21.bat", "Switch_Back_To_Proven.bat")):
                    raise SystemExit("switch calls a missing Switch_Pair.bat")
                continue
            if relative == "net_v21/Collect_Logs.ps1" and name not in names:
                bat = archive.read(f"{ROOT}/net_v21/Collect_Logs.bat")
                if b"Collect_Logs.ps1" in bat:
                    raise SystemExit("collector calls a missing Collect_Logs.ps1")
                continue
            if not archive.read(name):
                raise SystemExit(f"kit script is empty: {relative}")
    print(f"OK {archive_path}: {len(names)} entries, net_v21 module {sha256(module)[:16]}, "
          f"archive sha256 {sha256(archive_path.read_bytes())}")


def verify(archive_path: Path) -> None:
    if not archive_path.is_file():
        raise SystemExit(f"archive is absent: {archive_path}")
    with zipfile.ZipFile(archive_path) as archive:
        names = tuple(archive.namelist())
        if len(names) != len(set(names)):
            raise SystemExit("ZIP contains duplicate entry names")
        if f"{ROOT}/{PASSPORT}" not in names:
            raise SystemExit("missing build passport")
        stamp = archive.read(f"{ROOT}/{PASSPORT}").decode("utf-8")
        unified = "layout: unified" in stamp
    if not unified:
        version_match = re.search(r"^MiusMultiplayer (\d+)\.(\d+)\.(\d+) - test archive", stamp, re.M)
        if version_match and tuple(map(int, version_match.groups())) >= (1, 0, 5):
            raise SystemExit("version 1.0.5 requires the unified layout")
        _verify_legacy(archive_path)
        return
    expected_files = set(SHARED_BINARIES) | {MODULE_PATH, LAUNCHER_PATH, FILES_MANIFEST, PASSPORT} | \
                     set(UNIFIED_TEXT) | set(COLLECTOR_TEXT) | set(COMMON_TEXT)
    with zipfile.ZipFile(archive_path) as archive:
        if archive.testzip():
            raise SystemExit("ZIP CRC failed")
        names = tuple(archive.namelist())
        if names[:len(UNIFIED_DIRS)] != UNIFIED_DIRS:
            raise SystemExit("ZIP does not start with the unified directories")
        if set(names[len(UNIFIED_DIRS):]) != {f"{ROOT}/{name}" for name in expected_files}:
            missing = {f"{ROOT}/{name}" for name in expected_files} - set(names)
            raise SystemExit(f"unified archive file list differs; missing: {sorted(missing)}")
        if any(entry.date_time != ZIP_TIME for entry in archive.infolist()):
            raise SystemExit("ZIP timestamp is not reproducible")
        stamp = archive.read(f"{ROOT}/{PASSPORT}").decode("utf-8")
        if f"MiusMultiplayer {VERSION} - test archive" not in stamp or f"wire protocol: {WIRE_TAG.decode('ascii')}" not in stamp:
            raise SystemExit("build passport version or wire protocol mismatch")
        module, launcher = archive.read(f"{ROOT}/{MODULE_PATH}"), archive.read(f"{ROOT}/{LAUNCHER_PATH}")
        validate_module(module); validate_launcher(launcher)
        if WIRE_TAG not in module or WIRE_TAG not in launcher:
            raise SystemExit("module/launcher wire protocol tag mismatch")
        if f"net_v21 sha256: {sha256(module)}" not in stamp or f"launcher sha256: {sha256(launcher)}" not in stamp:
            raise SystemExit("build passport does not identify the installed module and launcher")
        if sha256(module) == PROVEN_BINARIES[MODULE_PATH] or sha256(launcher) == PROVEN_BINARIES[LAUNCHER_PATH]:
            raise SystemExit("old module/launcher present instead of net_v21")
        for relative, digest in SHARED_BINARIES.items():
            if sha256(archive.read(f"{ROOT}/{relative}")) != digest:
                raise SystemExit(f"shared runtime hash mismatch: {relative}")
        passport_files = {}
        for line in stamp.splitlines():
            match = re.fullmatch(r"\s*([0-9a-f]{64})\s{2}(.+?)\s*", line)
            if match:
                digest, relative = match.groups()
                if relative in passport_files:
                    raise SystemExit(f"duplicate passport entry: {relative}")
                passport_files[relative] = digest
        if set(passport_files) != expected_files - {PASSPORT}:
            raise SystemExit("unified passport file list is incomplete or unexpected")
        for relative, digest in passport_files.items():
            if sha256(archive.read(f"{ROOT}/{relative}")) != digest:
                raise SystemExit(f"passport hash mismatch: {relative}")
        lines = archive.read(f"{ROOT}/{FILES_MANIFEST}").decode().splitlines()
        expected_runtime = sorted((*SHARED_BINARIES, MODULE_PATH, LAUNCHER_PATH))
        expected_lines = [f"{sha256(archive.read(f'{ROOT}/{name}'))}  {name}" for name in expected_runtime]
        if lines != expected_lines:
            raise SystemExit("runtime manifest does not describe the six installed files")
        enable = archive.read(f"{ROOT}/Enable_Multiplayer.bat")
        if b"Apply_Multiplayer.ps1" not in enable or b"-Action Install" not in enable:
            raise SystemExit("unified installer does not install the full kit")
        if b"basic_network.codelib" not in archive.read(f"{ROOT}/files/root/programs/opgame.progpack"):
            raise SystemExit("opgame.progpack no longer loads basic_network.codelib")
    print(f"OK {archive_path}: {len(names)} entries, unified net_v21 module {sha256(module)[:16]}, "
          f"archive sha256 {sha256(archive_path.read_bytes())}")


def build(repo: Path, codelib: Path, archive_path: Path, launcher: Path) -> None:
    # New builds must never overwrite any preserved release/test or the known-good rollback.
    protected = ("MiusMultiplayer_v1.0.0_TEST_INSTALL_known_good.zip",
                 "MiusMultiplayer_v1.0.0_TEST_INSTALL.zip", "MiusMultiplayer_v1.0.0_M3M4_INSTALL.zip",
                 "MiusMultiplayer_v1.0.1_INSTALL.zip", "MiusMultiplayer_v1.0.2_TEST_INSTALL.zip",
                 "MiusMultiplayer_v1.0.3_TEST_INSTALL.zip", "MiusMultiplayer_v1.0.4_TEST_INSTALL.zip")
    if archive_path.resolve() in {(repo / name).resolve() for name in protected}:
        raise SystemExit("refusing to overwrite a preserved archive")
    if not codelib.is_file():
        raise SystemExit(f"no module to package: {codelib}")
    if not launcher.is_file():
        raise SystemExit(f"no launcher to package: {launcher}")
    tree = collect(repo, codelib, launcher)
    archive_path.parent.mkdir(parents=True, exist_ok=True)
    temporary = archive_path.with_name(archive_path.name + ".tmp")
    try:
        with zipfile.ZipFile(temporary, "w", compression=zipfile.ZIP_DEFLATED, compresslevel=9,
                             strict_timestamps=True) as archive:
            for name in UNIFIED_DIRS:
                archive.writestr(info(name, directory=True), b"")
            for name in sorted(tree):
                archive.writestr(info(f"{ROOT}/{name}"), with_crlf(name, tree[name]))
        verify(temporary)  # do not replace an earlier archive with an unverified one
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
    parser.add_argument("--launcher", type=Path,
                        default=Path("development/net-v21/build/join/mp_join.exe"))
    args = parser.parse_args()
    if bool(args.output) == bool(args.verify):
        parser.error("use exactly one of --output or --verify")
    repo = Path(__file__).resolve().parents[1]
    if args.output:
        codelib = args.codelib if args.codelib.is_absolute() else repo / args.codelib
        launcher = args.launcher if args.launcher.is_absolute() else repo / args.launcher
        build(repo, codelib, args.output, launcher)
    else:
        verify(args.verify)


if __name__ == "__main__":
    main()

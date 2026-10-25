#!/usr/bin/env python3
"""Build and verify the Steam-preserving FPS candidate.

The public MiusPlayer C++ snapshot does not contain the Steam friends/P2P layer
that is present in the released `basic_network.codelib`.  Rebuilding that public
snapshot therefore removes that layer.  This tool instead starts with the
verified released Steam-capable module and makes one narrowly scoped, reviewed
machine-code change:

    on_ctrl_tick: do not call session_sim_step when the engine reports 0 ms.

The old unconditional call is the first part of the reviewed source FPS fix. A
small executable PE section holds the conditional trampoline; no released Steam
code, imports, rich-presence strings, or P2P callbacks are replaced.  The
transformation is deliberately deterministic and fully reconstructible from the
checked-in stable module.

This is a binary compatibility candidate, not a claim that unavailable Steam
source code was rebuilt.  It still requires a real Steam two-peer regression
before it can be called gameplay-certified.
"""
from __future__ import annotations

import argparse
import hashlib
from dataclasses import dataclass
from pathlib import Path
import struct
import sys

ROOT = Path(__file__).resolve().parents[1]
STABLE = ROOT / "development/kit-source/files/basic_network.codelib"
DEFAULT_OUTPUT = ROOT / "development/fps-beta-build/basic_network.codelib"
EXPECTED_STABLE_SHA256 = "485b655662bd062e16278f9d699a3097b49a08de1948d2500dd589703a3102a3"

# Verified against the 6.00 build 3598/1 stable module.  The original nine-byte
# sequence is `mov rcx,[rdi+60h] ; call session_sim_step` inside on_ctrl_tick.
CTRL_TICK_CALL_RVA = 0x2C36
CTRL_TICK_CALL = bytes.fromhex("48 8b 4f 60 e8 05 99 01 00")
CTRL_TICK_CONTINUE_RVA = CTRL_TICK_CALL_RVA + len(CTRL_TICK_CALL)
SESSION_SIM_STEP_RVA = 0x1C544
FPS_SECTION_NAME = b".fpsfix\0"
FPS_SECTION_RVA = 0x64000
FPS_CODE_SIZE = 23
FPS_SECTION_RAW_SIZE = 0x200
FPS_SECTION_CHARACTERISTICS = 0x60000020  # code, execute, read
EXPECTED_SECTION_COUNT = 6
EXPECTED_OUTPUT_SECTION_COUNT = 7
EXPECTED_SIZE_OF_IMAGE = 0x65000

# These are runtime-facing capabilities, not merely installer companions.  All
# must survive in the generated PE; their absence is exactly the regression this
# guard is designed to prevent.
STEAM_CAPABILITY_MARKERS = (
    b"MP_STEAM_PEER",
    b"+gtmf_join=",
    b"SteamAPI_SteamFriends_v017",
    b"SteamAPI_SteamUser_v021",
    b"SteamAPI_SteamNetworking_v006",
    b"SendP2PPacket",
    b"ReadP2PPacket",
    b"JoinRequested",
)


@dataclass(frozen=True)
class PeLayout:
    pe_offset: int
    sections_offset: int
    section_alignment: int
    file_alignment: int
    section_count: int
    size_of_image_offset: int
    size_of_headers: int
    last_section_virtual_end: int


def sha256(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def align(value: int, multiple: int) -> int:
    return (value + multiple - 1) & -multiple


def parse_pe(data: bytes) -> PeLayout:
    if len(data) < 0x100 or data[:2] != b"MZ":
        raise ValueError("not an MZ executable")
    pe_offset, = struct.unpack_from("<I", data, 0x3C)
    if pe_offset + 24 > len(data) or data[pe_offset:pe_offset + 4] != b"PE\0\0":
        raise ValueError("missing PE signature")
    machine, sections = struct.unpack_from("<HH", data, pe_offset + 4)
    optional_size, = struct.unpack_from("<H", data, pe_offset + 20)
    optional = pe_offset + 24
    if machine != 0x8664 or optional + optional_size > len(data):
        raise ValueError("not an x64 PE")
    magic, = struct.unpack_from("<H", data, optional)
    if magic != 0x20B:
        raise ValueError("not a PE32+ image")
    section_alignment, file_alignment = struct.unpack_from("<II", data, optional + 32)
    size_of_image, size_of_headers = struct.unpack_from("<II", data, optional + 56)
    sections_offset = optional + optional_size
    if sections_offset + sections * 40 > len(data):
        raise ValueError("truncated section table")
    last_end = 0
    for index in range(sections):
        at = sections_offset + index * 40
        virtual_size, virtual_address, raw_size, raw_offset = struct.unpack_from("<IIII", data, at + 8)
        if raw_offset + raw_size > len(data):
            raise ValueError(f"truncated section {index}")
        last_end = max(last_end, virtual_address + max(virtual_size, raw_size))
    if size_of_image < align(last_end, section_alignment):
        raise ValueError("invalid SizeOfImage")
    return PeLayout(
        pe_offset=pe_offset,
        sections_offset=sections_offset,
        section_alignment=section_alignment,
        file_alignment=file_alignment,
        section_count=sections,
        size_of_image_offset=optional + 56,
        size_of_headers=size_of_headers,
        last_section_virtual_end=last_end,
    )


def rva_file_offset(data: bytes, layout: PeLayout, rva: int) -> int:
    for index in range(layout.section_count):
        at = layout.sections_offset + index * 40
        virtual_size, virtual_address, raw_size, raw_offset = struct.unpack_from("<IIII", data, at + 8)
        span = max(virtual_size, raw_size)
        if virtual_address <= rva < virtual_address + span:
            offset = raw_offset + rva - virtual_address
            if offset >= len(data):
                break
            return offset
    raise ValueError(f"RVA 0x{rva:x} is not backed by a file section")


def rel32(target_rva: int, next_rva: int) -> bytes:
    displacement = target_rva - next_rva
    if not -(1 << 31) <= displacement < (1 << 31):
        raise ValueError("relative branch is out of range")
    return struct.pack("<i", displacement)


def fps_code(section_rva: int) -> bytes:
    """Return the trampoline machine code at *section_rva*.

    test r12d,r12d
    je   on_ctrl_tick+9
    mov  rcx,[rdi+60h]
    call session_sim_step
    jmp  on_ctrl_tick+9
    """
    code = bytearray()
    code += b"\x45\x85\xe4"  # test r12d, r12d; r12d holds msg[+18] (step ms)
    code += b"\x0f\x84" + rel32(CTRL_TICK_CONTINUE_RVA, section_rva + 9)
    code += b"\x48\x8b\x4f\x60"
    code += b"\xe8" + rel32(SESSION_SIM_STEP_RVA, section_rva + len(code) + 5)
    code += b"\xe9" + rel32(CTRL_TICK_CONTINUE_RVA, section_rva + len(code) + 5)
    if len(code) != FPS_CODE_SIZE:
        raise AssertionError(f"unexpected trampoline size {len(code)}")
    return bytes(code)


def require_stable_input(data: bytes) -> PeLayout:
    if sha256(data) != EXPECTED_STABLE_SHA256:
        raise ValueError("stable input SHA-256 is not the verified Steam-capable 3598/1 module")
    layout = parse_pe(data)
    if layout.section_count != EXPECTED_SECTION_COUNT:
        raise ValueError("stable input PE section count changed")
    if FPS_SECTION_RVA != align(layout.last_section_virtual_end, layout.section_alignment):
        raise ValueError("stable input no longer has the reviewed FPS section RVA")
    if layout.sections_offset + (layout.section_count + 1) * 40 > layout.size_of_headers:
        raise ValueError("stable input PE headers have no room for the FPS section")
    call_at = rva_file_offset(data, layout, CTRL_TICK_CALL_RVA)
    if data[call_at:call_at + len(CTRL_TICK_CALL)] != CTRL_TICK_CALL:
        raise ValueError("stable input ctrl_tick call signature differs")
    missing = [marker.decode("ascii") for marker in STEAM_CAPABILITY_MARKERS if marker not in data]
    if missing:
        raise ValueError("stable input lacks Steam capability markers: " + ", ".join(missing))
    if b"mp_vitals.dll" not in data.lower():
        raise ValueError("stable input lost its existing Vitals sidecar import")
    return layout


def transform(stable: bytes) -> bytes:
    layout = require_stable_input(stable)
    output = bytearray(stable)
    call_at = rva_file_offset(output, layout, CTRL_TICK_CALL_RVA)
    output[call_at:call_at + len(CTRL_TICK_CALL)] = b"\xe9" + rel32(FPS_SECTION_RVA, CTRL_TICK_CALL_RVA + 5) + b"\x90" * 4

    new_section = layout.sections_offset + layout.section_count * 40
    output[new_section:new_section + 40] = b"\0" * 40
    output[new_section:new_section + 8] = FPS_SECTION_NAME
    struct.pack_into("<IIIIIIHHI", output, new_section + 8,
                     FPS_CODE_SIZE, FPS_SECTION_RVA, FPS_SECTION_RAW_SIZE, align(len(output), layout.file_alignment),
                     0, 0, 0, 0, FPS_SECTION_CHARACTERISTICS)
    struct.pack_into("<H", output, layout.pe_offset + 6, EXPECTED_OUTPUT_SECTION_COUNT)
    struct.pack_into("<I", output, layout.size_of_image_offset, EXPECTED_SIZE_OF_IMAGE)

    raw_offset = align(len(output), layout.file_alignment)
    output.extend(b"\0" * (raw_offset - len(output)))
    output.extend(fps_code(FPS_SECTION_RVA))
    output.extend(b"\0" * (FPS_SECTION_RAW_SIZE - FPS_CODE_SIZE))
    return bytes(output)


def verify_candidate(candidate: bytes, stable: bytes) -> None:
    expected = transform(stable)
    if candidate != expected:
        raise ValueError("candidate is not the exact deterministic Steam-preserving FPS transformation")
    layout = parse_pe(candidate)
    if layout.section_count != EXPECTED_OUTPUT_SECTION_COUNT:
        raise ValueError("candidate has the wrong PE section count")
    if struct.unpack_from("<I", candidate, layout.size_of_image_offset)[0] != EXPECTED_SIZE_OF_IMAGE:
        raise ValueError("candidate SizeOfImage differs")
    last = layout.sections_offset + (layout.section_count - 1) * 40
    name = candidate[last:last + 8]
    virtual_size, virtual_address, raw_size, raw_offset = struct.unpack_from("<IIII", candidate, last + 8)
    characteristics, = struct.unpack_from("<I", candidate, last + 36)
    if (name, virtual_size, virtual_address, raw_size, characteristics) != (
        FPS_SECTION_NAME, FPS_CODE_SIZE, FPS_SECTION_RVA, FPS_SECTION_RAW_SIZE, FPS_SECTION_CHARACTERISTICS
    ):
        raise ValueError("candidate FPS PE section layout differs")
    if candidate[raw_offset:raw_offset + FPS_CODE_SIZE] != fps_code(FPS_SECTION_RVA):
        raise ValueError("candidate FPS trampoline differs")
    call_at = rva_file_offset(candidate, layout, CTRL_TICK_CALL_RVA)
    wanted_call = b"\xe9" + rel32(FPS_SECTION_RVA, CTRL_TICK_CALL_RVA + 5) + b"\x90" * 4
    if candidate[call_at:call_at + len(wanted_call)] != wanted_call:
        raise ValueError("candidate ctrl_tick redirection differs")
    missing = [marker.decode("ascii") for marker in STEAM_CAPABILITY_MARKERS if marker not in candidate]
    if missing:
        raise ValueError("candidate lost Steam capability markers: " + ", ".join(missing))
    if b"mp_vitals.dll" not in candidate.lower():
        raise ValueError("candidate lost the stable Vitals sidecar import")


def read_stable(path: Path) -> bytes:
    try:
        return path.read_bytes()
    except OSError as exc:
        raise SystemExit(f"cannot read stable input {path}: {exc}") from exc


def display_path(path: Path) -> str:
    try:
        return str(path.relative_to(ROOT))
    except ValueError:
        return str(path)


def cmd_build(output: Path) -> None:
    stable = read_stable(STABLE)
    try:
        candidate = transform(stable)
        verify_candidate(candidate, stable)
    except ValueError as exc:
        raise SystemExit(f"cannot build Steam-preserving FPS candidate: {exc}") from exc
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_bytes(candidate)
    print(f"built {display_path(output)}")
    print(f"sha256 {sha256(candidate)}")
    print("Steam friends/P2P markers and the released Vitals import remain present.")


def cmd_verify(candidate_path: Path) -> None:
    stable = read_stable(STABLE)
    try:
        verify_candidate(candidate_path.read_bytes(), stable)
    except OSError as exc:
        raise SystemExit(f"cannot read candidate {candidate_path}: {exc}") from exc
    except ValueError as exc:
        raise SystemExit(f"Steam-preserving FPS candidate verification failed: {exc}") from exc
    print(f"OK {display_path(candidate_path)}: Steam-preserving ctrl_tick zero-ms guard, sha256 {sha256(candidate_path.read_bytes())}")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest="command", required=True)
    build = commands.add_parser("build", help="build the deterministic candidate from the verified stable module")
    build.add_argument("--output", type=Path, default=DEFAULT_OUTPUT)
    verify = commands.add_parser("verify", help="verify a candidate against the deterministic transformation")
    verify.add_argument("--candidate", type=Path, default=DEFAULT_OUTPUT)
    args = parser.parse_args()
    path = args.output if args.command == "build" else args.candidate
    path = path if path.is_absolute() else ROOT / path
    if args.command == "build":
        cmd_build(path)
    else:
        cmd_verify(path)


if __name__ == "__main__":
    main()

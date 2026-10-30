#!/usr/bin/env python3
"""Offline, fail-closed audit of hit and animation hook candidates. Never patches a process.

Fixed RVA is checked first; on a miss a code signature must match exactly once in executable
sections. Vtable candidates must likewise be unique in .rdata. This proves byte/pointer identity,
not calling convention, game-thread safety, replication semantics, or real-game behaviour.
"""
from __future__ import annotations
import argparse
from dataclasses import dataclass
import hashlib
import struct
from pathlib import Path
from pe_scan import PE


class AuditError(Exception):
    pass


@dataclass(frozen=True)
class Spec:
    name: str
    rva: int
    signature: bytes


def spec(name: str, rva: int, signature: str) -> Spec:
    return Spec(name, rva, bytes.fromhex(signature))


HIT = (
    (0xC79D8, spec("vehicle hit", 0x43230,
        "48 8B C4 48 89 58 20 44 89 40 18 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 A8 FE FF FF 48 81 EC")),
    (0xC6760, spec("human hit", 0x66C90,
        "48 8B C4 48 89 58 20 55 56 41 57 48 83 EC 60 80 BC 24 A8 00 00 00 00 4D 8B F9 41 8B E8 48 8B F2")),
)
ANIM_BASE = 0x12F128
ANIM = (
    (4, spec("anim slot 4", 0xA4040, "8B C2 48 8D 14 80 48 8B 81 40 01 00 00 48 C1 E2 06 44 89 84 02 A8 00 00")),
    (8, spec("anim slot 8", 0xA1350, "8B C2 48 8D 14 80 48 8B 81 40 01 00 00 48 C1 E2 06 F3 0F 11 94 02 D4 00")),
    (10, spec("anim slot 10", 0xA13B0, "8B C2 48 8D 14 80 48 8B 81 40 01 00 00 48 C1 E2 06 44 89 84 02 00 01 00")),
    (11, spec("anim slot 11", 0xA4EC0, "48 89 5C 24 08 57 48 83 EC 60 8B C2 4D 8B C8 48 8D 3C 80 48 C1 E7 06 48")),
    (12, spec("anim slot 12", 0xA4FF0, "8B C2 48 8D 14 80 48 C1 E2 06 48 03 91 40 01 00 00 48 8B 42 70 48 85 C0")),
    (13, spec("anim slot 13", 0xA13D0, "8B C2 48 8D 14 80 48 C1 E2 06 48 03 91 40 01 00 00 C7 82 04 01 00 00 00")),
    (14, spec("anim slot 14", 0xA5030, "F3 0F 10 44 24 28 F3 0F 10 4C 24 30 8B C2 48 8D 14 80 48 C1 E2 06 48 03")),
    (16, spec("anim slot 16", 0xA1420, "8B C2 48 8D 14 80 48 8B 81 40 01 00 00 48 C1 E2 06 F3 0F 11 94 02 28 01")),
    (20, spec("anim slot 20", 0xA1440, "8B C2 48 8D 14 80 48 8B 81 40 01 00 00 48 C1 E2 06 F3 0F 11 94 02 2C 01")),
    (21, spec("anim slot 21", 0xA1460, "8B C2 48 8D 14 80 48 8B 81 40 01 00 00 48 C1 E2 06 44 89 84 02 14 01 00")),
    (26, spec("anim slot 26", 0xA14C0, "8B C2 48 8D 14 80 48 8B 81 40 01 00 00 48 C1 E2 06 44 88 84 02 39 01 00")),
)


def span(pe: PE, rva: int, size: int, sections) -> int | None:
    for _name, start, _vsize, raw, raw_size in sections:
        delta = rva - start
        if 0 <= delta and delta + size <= raw_size and raw + delta + size <= len(pe.data):
            return raw + delta
    return None


def resolve_code(pe: PE, item: Spec) -> tuple[int, str]:
    at = span(pe, item.rva, len(item.signature), pe.exec_sections)
    if at is not None and pe.data[at:at + len(item.signature)] == item.signature:
        return item.rva, "fixed"
    matches = []
    for name, rva, raw, size in pe.executable():
        section = pe.data[raw:raw + size]
        at = section.find(item.signature)
        while at != -1:
            matches.append(rva + at)
            at = section.find(item.signature, at + 1)
    if len(matches) != 1:
        raise AuditError(f"{item.name}: signature occurs {len(matches)} times in executable sections")
    return matches[0], "unique code scan"


def rdata_sections(pe: PE):
    return [section for section in pe.sections if section[0] == ".rdata" and section not in pe.exec_sections]


def vtable_matches(pe: PE, base: int, expected: dict[int, int]) -> bool:
    sections = rdata_sections(pe)
    for index, target in expected.items():
        at = span(pe, base + index * 8, 8, sections)
        if at is None or struct.unpack_from("<Q", pe.data, at)[0] != pe.image_base + target:
            return False
    return True


def resolve_vtable(pe: PE, fixed: int, expected: dict[int, int], name: str) -> tuple[int, str]:
    if vtable_matches(pe, fixed, expected):
        return fixed, "fixed"
    matches = []
    max_index = max(expected)
    for _name, rva, _vsize, raw, size in rdata_sections(pe):
        # Limit candidates to complete, aligned tables within this section.
        for delta in range((-rva) % 8, max(0, size - max_index * 8 - 7), 8):
            candidate = rva + delta
            if vtable_matches(pe, candidate, expected):
                matches.append(candidate)
    if len(matches) != 1:
        raise AuditError(f"{name}: vtable occurs {len(matches)} times in .rdata")
    return matches[0], "unique .rdata scan"


def audit(phys: PE, game: PE) -> list[str]:
    lines = []
    for slot, item in HIT:
        target, how_code = resolve_code(phys, item)
        found, how_slot = resolve_vtable(phys, slot, {0: target}, item.name)
        lines.append(f"{item.name}: code +0x{target:x} ({how_code}), slot +0x{found:x} ({how_slot})")
    targets = {}
    for index, item in ANIM:
        target, how = resolve_code(game, item)
        targets[index] = target
        lines.append(f"{item.name}: code +0x{target:x} ({how})")
    base, how = resolve_vtable(game, ANIM_BASE, targets, "i_animch")
    lines.append(f"i_animch: vtable +0x{base:x} ({how}), {len(targets)} slots verified")
    return lines


def main() -> int:
    root = Path(__file__).resolve().parents[2] / "game-build-3598-1/x64"
    parser = argparse.ArgumentParser()
    parser.add_argument("--physic", type=Path, default=root / "adv_physic.codelib")
    parser.add_argument("--game", type=Path, default=root / "adv_game.codelib")
    args = parser.parse_args()
    try:
        phys, game = PE(args.physic), PE(args.game)
        print("OFFLINE AUDIT ONLY: no hook is installed and no game memory is changed")
        for pe in (phys, game):
            print(f"{pe.path.name}: SHA-256 {hashlib.sha256(pe.data).hexdigest()}")
        for line in audit(phys, game):
            print("OK " + line)
        print("Byte identity is not proof of runtime ABI, synchronization or behaviour.")
        return 0
    except (AuditError, OSError, ValueError, struct.error) as error:
        print(f"FAIL {error}")
        return 1


if __name__ == "__main__":
    raise SystemExit(main())

#!/usr/bin/env python3
"""Offline Vitals layout/signature audit against the user-supplied 3598/1 PE images.

This checks the exact signatures compiled into vitals.cpp, the member vtable slot and the
statistics RNG byte pattern. It cannot prove ABI, thread safety or in-game behaviour.
"""
from __future__ import annotations
import argparse
import re
import struct
from pathlib import Path
from hook_audit import AuditError, Spec, resolve_code, span
from pe_scan import PE

ROOT = Path(__file__).resolve().parents[3]
SOURCE = ROOT / "development/net-v21/src/vitals.cpp"
PHYS = ROOT / "development/game-build-3598-1/x64/adv_physic.codelib"
GAME = ROOT / "development/game-build-3598-1/x64/adv_game.codelib"


def signature(source: str, name: str) -> bytes:
    match = re.search(r"const unsigned char\s+" + re.escape(name) + r"\[\]\s*=\s*\{([^}]*)\}", source)
    if not match:
        raise AuditError(f"missing {name} in vitals.cpp")
    try:
        return bytes(int(item.strip(), 0) for item in match.group(1).split(",") if item.strip())
    except ValueError as exc:
        raise AuditError(f"invalid {name}") from exc


def stamp(pe: PE) -> int:
    nt = struct.unpack_from("<I", pe.data, 0x3c)[0]
    return struct.unpack_from("<I", pe.data, nt + 8)[0]


def image_size(pe: PE) -> int:
    nt = struct.unpack_from("<I", pe.data, 0x3c)[0]
    return struct.unpack_from("<I", pe.data, nt + 24 + 56)[0]


def audit(phys_path: Path, game_path: Path, source: str) -> dict[str, int]:
    phys, game = PE(phys_path), PE(game_path)
    if (stamp(phys), image_size(phys)) != (0x66f2a91e, 0xf2000):
        raise AuditError("unexpected physics image timestamp or size")
    if (stamp(game), image_size(game)) != (0x66f2d3a9, 0x178000):
        raise AuditError("unexpected game image timestamp or size")
    offsets: dict[str, int] = {}
    for name, rva, binary in (("member_sig", 0x9c410, phys), ("human_sig", 0x66c90, phys),
                              ("techn_sig", 0x43230, phys), ("stat_sig", 0xf80c0, game)):
        resolved, _how = resolve_code(binary, Spec(name, rva, signature(source, name)))
        offsets[name] = resolved
    slot = span(phys, 0xc7888, 8, phys.sections)
    if slot is None or struct.unpack_from("<Q", phys.data, slot)[0] != phys.image_base + offsets["member_sig"]:
        raise AuditError("i_techn get_member vtable slot does not point at the checked function")
    body = span(game, offsets["stat_sig"], 11000, game.exec_sections)
    if body is None or game.data[body:body + 11000].count(signature(source, "stat_rng")) != 3:
        raise AuditError("statistics function does not contain exactly three RNG patterns")
    if signature(source, "stat_sig")[:15] != bytes.fromhex("48 8b c4 44 88 40 18 48 89 50 10 48 89 48 08"):
        raise AuditError("statistics trampoline's first 15 bytes changed")
    return offsets


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--physics", type=Path, default=PHYS)
    parser.add_argument("--game", type=Path, default=GAME)
    args = parser.parse_args()
    try:
        offsets = audit(args.physics, args.game, SOURCE.read_text(encoding="utf-8"))
    except (AuditError, ValueError, OSError, struct.error) as exc:
        print(f"Vitals PE audit: FAIL: {exc}")
        return 1
    print("Vitals PE audit: PASS " + " ".join(f"{name}=0x{value:x}" for name, value in offsets.items()))
    print("Only byte/layout identity was checked; this is not an ABI or game-thread test.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

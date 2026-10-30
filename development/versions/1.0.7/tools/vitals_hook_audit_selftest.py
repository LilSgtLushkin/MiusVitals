#!/usr/bin/env python3
"""Fail-closed regression checks for the Vitals PE audit; modifies disposable copies only."""
from __future__ import annotations
import struct
import tempfile
from pathlib import Path
from hook_audit import AuditError, span
from pe_scan import PE
from vitals_hook_audit import PHYS, GAME, SOURCE, audit, signature


def main() -> int:
    source = SOURCE.read_text(encoding="utf-8")
    phys, game = PE(PHYS), PE(GAME)
    audit(PHYS, GAME, source)
    checks = 1
    with tempfile.TemporaryDirectory() as td:
        root = Path(td)
        def rejected(label: str, pb: bytearray, gb: bytearray) -> None:
            nonlocal checks
            p, g = root / "phys.codelib", root / "game.codelib"
            p.write_bytes(pb); g.write_bytes(gb)
            try:
                audit(p, g, source)
            except (AuditError, ValueError, OSError, struct.error):
                checks += 1
                return
            raise AssertionError(f"{label} accepted")

        member = span(phys, 0x9c410, len(signature(source, "member_sig")), phys.exec_sections)
        table = span(phys, 0xc7888, 8, phys.sections)
        stat = span(game, 0xf80c0, 11000, game.exec_sections)
        assert member is not None and table is not None and stat is not None
        pb, gb = bytearray(phys.data), bytearray(game.data)
        pb[member] ^= 1
        rejected("missing member signature", pb, gb)
        pb, gb = bytearray(phys.data), bytearray(game.data)
        pb[table] ^= 1
        rejected("mismatched vtable slot", pb, gb)
        pb, gb = bytearray(phys.data), bytearray(game.data)
        rng = signature(source, "stat_rng")
        loc = gb.find(rng, stat, stat + 11000)
        assert loc >= 0
        gb[loc] ^= 1
        rejected("changed statistics RNG body", pb, gb)
        pb, gb = bytearray(phys.data), bytearray(game.data)
        nt = struct.unpack_from("<I", pb, 0x3c)[0]
        pb[nt + 8] ^= 1
        rejected("wrong physics image", pb, gb)
    print(f"vitals hook audit selftest: {checks} checks, 0 failed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

#!/usr/bin/env python3
"""Offline tests: exact offsets, moved signatures, ambiguity and mutated vtables."""
from pathlib import Path
import struct
import unittest
from pe_scan import PE
import hook_audit as audit

ROOT = Path(__file__).resolve().parents[2] / "game-build-3598-1/x64"


def load(name):
    pe = PE(ROOT / name)
    pe.data = bytearray(pe.data)  # mutate only the in-memory test copy
    return pe


def write_at(pe, rva, value):
    at = pe.offset(rva)
    assert at is not None
    pe.data[at:at + len(value)] = value


class HookAudit(unittest.TestCase):
    def test_supplied_game_files(self):
        lines = audit.audit(load("adv_physic.codelib"), load("adv_game.codelib"))
        self.assertEqual(len(lines), 14)
        self.assertIn("11 slots verified", lines[-1])

    def test_fixed_and_unique_code_fallback(self):
        pe = load("adv_physic.codelib")
        slot, item = audit.HIT[0]
        self.assertEqual(audit.resolve_code(pe, item), (item.rva, "fixed"))
        moved = pe.exec_sections[0][1] + 0x100
        write_at(pe, item.rva, b"\x90")
        write_at(pe, moved, item.signature)
        write_at(pe, slot, struct.pack("<Q", pe.image_base + moved))
        self.assertEqual(audit.resolve_code(pe, item), (moved, "unique code scan"))
        self.assertEqual(audit.resolve_vtable(pe, slot, {0: moved}, item.name), (slot, "fixed"))
        write_at(pe, moved + 0x80, item.signature)
        with self.assertRaisesRegex(audit.AuditError, "occurs 2 times"):
            audit.resolve_code(pe, item)

    def test_vtable_fallback_and_ambiguous_refusal(self):
        pe = load("adv_physic.codelib")
        slot, item = audit.HIT[0]
        rdata = audit.rdata_sections(pe)[0][1]
        a, b = (rdata + 0x100 + 7) & ~7, (rdata + 0x180 + 7) & ~7
        pointer = struct.pack("<Q", pe.image_base + item.rva)
        write_at(pe, slot, b"\0" * 8)
        write_at(pe, a, pointer)
        self.assertEqual(audit.resolve_vtable(pe, slot, {0: item.rva}, item.name), (a, "unique .rdata scan"))
        write_at(pe, b, pointer)
        with self.assertRaisesRegex(audit.AuditError, "occurs 2 times"):
            audit.resolve_vtable(pe, slot, {0: item.rva}, item.name)

    def test_animation_pointer_or_prologue_mismatch_is_not_accepted(self):
        phys, game = load("adv_physic.codelib"), load("adv_game.codelib")
        write_at(game, audit.ANIM[3][1].rva, b"\x90")
        with self.assertRaises(audit.AuditError):
            audit.audit(phys, game)
        game = load("adv_game.codelib")
        write_at(game, audit.ANIM_BASE + audit.ANIM[0][0] * 8, b"\0" * 8)
        with self.assertRaises(audit.AuditError):
            audit.audit(phys, game)


if __name__ == "__main__":
    unittest.main(verbosity=2)

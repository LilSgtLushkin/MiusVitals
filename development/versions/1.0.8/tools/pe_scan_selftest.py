#!/usr/bin/env python3
"""Regression tests for PE addresses, using the checked-in reference images."""
from __future__ import annotations

import io
import subprocess
import sys
import unittest
from contextlib import redirect_stdout
from pathlib import Path

import pe_scan

REPO = Path(__file__).resolve().parents[4]
PHYSIC = REPO / "development/game-build-3598-1/x64/adv_physic.codelib"
FPS = REPO / "development/fps-candidate/basic_network.codelib"
SCRIPT = Path(pe_scan.__file__).resolve()


class Addresses(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.physic = pe_scan.PE(PHYSIC)
        cls.fps = pe_scan.PE(FPS)

    def describe(self, pe, rva):
        with io.StringIO() as output, redirect_stdout(output):
            pe_scan.describe(pe, rva, 0)
            return output.getvalue()

    def cli(self, *args):
        return subprocess.run([sys.executable, str(SCRIPT), str(PHYSIC), *args],
                              capture_output=True, text=True, check=False)

    def test_vehicle_and_human_vtable_use_preferred_va(self):
        for slot, fn in ((0xC79D8, 0x43230), (0xC6760, 0x66C90)):
            with self.subTest(slot=slot):
                raw = self.physic.offset(slot)
                self.assertIsNotNone(raw)
                value = int.from_bytes(self.physic.data[raw:raw + 8], "little")
                self.assertEqual(value, self.physic.image_base + fn)
                self.assertEqual(pe_scan.pointer_rva(self.physic, value), fn)
                self.assertEqual(self.physic.function_of(fn)[0], fn)
                text = self.describe(self.physic, slot)
                self.assertIn(f"possible pointer to +0x{fn:x}", text)
                self.assertIn(f"function +0x{fn:x}..", text)
                self.assertIn("prologue", text)

    def test_rva_and_non_code_values(self):
        self.assertEqual(pe_scan.pointer_rva(self.physic, 0x43230), 0x43230)
        self.assertIsNone(pe_scan.pointer_rva(self.physic, 0))
        self.assertIsNone(pe_scan.pointer_rva(self.physic, self.physic.image_base + 0xC79D8))
        self.assertIn("possible pointer to +0x43378", self.describe(self.fps, 0x580A0))

    def test_relocated_va_converts_address_but_not_file_contents(self):
        base = 0x7FF800000000
        result = self.cli("--va", hex(base + 0xC79D8), "--base", hex(base))
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn("rva      0xc79d8", result.stdout)
        self.assertIn("possible pointer to +0x43230", result.stdout)
        self.assertIn("64-bit value 0x180043230", result.stdout)

    def test_absolute_address_below_base_is_rejected(self):
        result = self.cli("--va", "0x43230")
        self.assertEqual(result.returncode, 1)
        self.assertIn("below the image base", result.stdout)
        self.assertNotIn("rva      0x43230", result.stdout)

    def test_plain_rva(self):
        result = self.cli("--at", "0xC6760")
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn("possible pointer to +0x66c90", result.stdout)

    def test_signature_search_only_counts_executable_sections(self):
        code = self.physic.offset(0x43230)
        sig = self.physic.data[code:code + 32]
        result = self.cli("--sig", sig.hex(" "))
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn("rva 0x043230  (.text+0x42230)", result.stdout)
        self.assertIn("1 hit(s)", result.stdout)

        slot = self.physic.offset(0xC79D8)
        data_sig = self.physic.data[slot:slot + 32]
        result = self.cli("--sig", data_sig.hex(" "))
        self.assertEqual(result.returncode, 1)
        self.assertIn("0 hit(s)", result.stdout)


if __name__ == "__main__":
    unittest.main(verbosity=2)

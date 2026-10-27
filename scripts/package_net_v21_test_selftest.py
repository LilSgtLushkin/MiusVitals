#!/usr/bin/env python3
"""No-game tests for the 1.0.3 test archive builder and rollback protection."""
from __future__ import annotations
import hashlib
import importlib.util
from pathlib import Path
import struct
import tempfile
import unittest
import zipfile

ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location("package_net_v21_test", ROOT / "scripts/package_net_v21_test.py")
pkg = importlib.util.module_from_spec(spec)
assert spec and spec.loader
spec.loader.exec_module(pkg)


class Packaging(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.known_good = ROOT / "MiusMultiplayer_v1.0.0_TEST_INSTALL_known_good.zip"
        with zipfile.ZipFile(cls.known_good) as z:
            cls.module = z.read("MiusMultiplayer/net_v21/basic_network.netv21.codelib")

    def test_pe_validation(self):
        pkg.validate_module(self.module)
        for invalid in (b"", self.module[:100], b"NO" + self.module[2:]):
            with self.assertRaises(SystemExit):
                pkg.validate_module(invalid)
        bad_machine = bytearray(self.module)
        pe = struct.unpack_from("<I", bad_machine, 0x3c)[0]
        struct.pack_into("<H", bad_machine, pe + 4, 0x14c)
        with self.assertRaises(SystemExit):
            pkg.validate_module(bytes(bad_machine))
        bad_export = self.module.replace(b"GetFactory3\0", b"NoFactory_3\0")
        self.assertNotEqual(bad_export, self.module)
        with self.assertRaises(SystemExit):
            pkg.validate_module(bad_export)

    def test_protected_rollback(self):
        before = hashlib.sha256(self.known_good.read_bytes()).digest()
        with self.assertRaisesRegex(SystemExit, "refusing to overwrite"):
            pkg.build(ROOT, Path("nonexistent.codelib"), self.known_good)
        self.assertEqual(before, hashlib.sha256(self.known_good.read_bytes()).digest())

    def test_determinism_and_missing_collector(self):
        with tempfile.TemporaryDirectory() as tmp:
            temp = Path(tmp)
            binary, first, second, broken = (temp / x for x in ("module.codelib", "a.zip", "b.zip", "broken.zip"))
            binary.write_bytes(self.module)
            pkg.build(ROOT, binary, first)
            pkg.build(ROOT, binary, second)
            self.assertEqual(first.read_bytes(), second.read_bytes())
            with zipfile.ZipFile(first) as src, zipfile.ZipFile(broken, "w") as dst:
                for item in src.infolist():
                    if item.filename != "MiusMultiplayer/net_v21/Collect_Logs.ps1":
                        dst.writestr(item, src.read(item.filename))
            with self.assertRaisesRegex(SystemExit, "missing Collect_Logs.ps1"):
                pkg.verify(broken)
            pkg.verify(self.known_good)  # a historical standalone .bat remains verifiable


if __name__ == "__main__":
    unittest.main(verbosity=2)

#!/usr/bin/env python3
"""No-game tests for the 1.0.3 test archive builder and rollback protection."""
from __future__ import annotations
import copy
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

    def test_passports_of_preserved_test_archives(self):
        for name in ("MiusMultiplayer_v1.0.0_TEST_INSTALL.zip",
                     "MiusMultiplayer_v1.0.0_TEST_INSTALL_known_good.zip",
                     "MiusMultiplayer_v1.0.2_TEST_INSTALL.zip",
                     "MiusMultiplayer_v1.0.3_TEST_INSTALL.zip"):
            with self.subTest(archive=name):
                pkg.verify(ROOT / name)

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

    def test_passport_hashes_actual_crlf_and_detects_tampering(self):
        with tempfile.TemporaryDirectory() as tmp:
            temp = Path(tmp)
            binary, good, broken_passport, broken_text = (
                temp / name for name in ("module.codelib", "good.zip", "passport.zip", "text.zip"))
            binary.write_bytes(self.module)
            # Give the builder an LF-only checkout even on Windows, where Git may use CRLF.
            repo = temp / "repo"
            for base, paths in ((pkg.PROVEN, (*pkg.PROVEN_BINARIES, *pkg.PROVEN_TEXT)),
                                (pkg.KIT, pkg.KIT_TEXT)):
                for relative in paths:
                    source, target = ROOT / base / relative, repo / base / relative
                    target.parent.mkdir(parents=True, exist_ok=True)
                    data = source.read_bytes()
                    if base == pkg.PROVEN and relative == "README.txt":
                        data = data.replace(b"\r\n", b"\n")
                        self.assertIn(b"\n", data)
                        self.assertNotIn(b"\r\n", data)
                    target.write_bytes(data)
            pkg.build(repo, binary, good)
            with zipfile.ZipFile(good) as src:
                self.assertIn(b"\r\n", src.read("MiusMultiplayer/README.txt"))
                actual_hash = pkg.sha256(src.read("MiusMultiplayer/README.txt")).encode()
                for output, altered in ((broken_passport, "passport"), (broken_text, "text")):
                    with zipfile.ZipFile(output, "w") as dst:
                        for item in src.infolist():
                            data = src.read(item.filename)
                            if altered == "passport" and item.filename == "MiusMultiplayer/TEST_BUILD.txt":
                                self.assertIn(actual_hash, data)
                                data = data.replace(actual_hash, b"0" * 64, 1)
                            elif altered == "text" and item.filename == "MiusMultiplayer/README.txt":
                                data += b"\r\naltered\r\n"
                            # writestr mutates ZipInfo; do not change the source archive's CRC.
                            dst.writestr(copy.copy(item), data)
                    with self.subTest(altered=altered), self.assertRaisesRegex(SystemExit, "passport hash mismatch: README.txt"):
                        pkg.verify(output)


if __name__ == "__main__":
    unittest.main(verbosity=2)

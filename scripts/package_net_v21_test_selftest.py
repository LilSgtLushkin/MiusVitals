#!/usr/bin/env python3
"""No-game packaging checks for the unified 1.0.7 installer and preserved archives."""
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
        with zipfile.ZipFile(cls.known_good) as archive:
            cls.module = archive.read("MiusMultiplayer/net_v21/basic_network.netv21.codelib")
            cls.old_launcher = archive.read("MiusMultiplayer/files/mp_join.exe")
        # PE overlays are packaging fixtures only; they do not make compatible binaries.
        cls.tagged_module = cls.module + pkg.WIRE_TAG
        cls.tagged_launcher = cls.old_launcher + pkg.WIRE_TAG

    def build_fixture(self, directory: Path, name="test.zip") -> Path:
        module, launcher, archive = directory / "module.codelib", directory / "launcher.exe", directory / name
        module.write_bytes(self.tagged_module)
        launcher.write_bytes(self.tagged_launcher)
        pkg.build(ROOT, module, archive, launcher)
        return archive

    def test_pe_validation(self):
        pkg.validate_module(self.module)
        pkg.validate_launcher(self.old_launcher)
        for invalid in (b"", self.module[:100], b"NO" + self.module[2:]):
            with self.assertRaises(SystemExit):
                pkg.validate_module(invalid)
        for invalid in (b"", self.old_launcher[:100], b"NO" + self.old_launcher[2:], self.module):
            with self.assertRaises(SystemExit):
                pkg.validate_launcher(invalid)
        bad_machine = bytearray(self.module)
        pe = struct.unpack_from("<I", bad_machine, 0x3c)[0]
        struct.pack_into("<H", bad_machine, pe + 4, 0x14c)
        with self.assertRaises(SystemExit):
            pkg.validate_module(bytes(bad_machine))
        bad_export = self.module.replace(b"GetFactory3\0", b"NoFactory_3\0")
        self.assertNotEqual(bad_export, self.module)
        with self.assertRaises(SystemExit):
            pkg.validate_module(bad_export)

    def test_preserved_archives_are_protected_and_verifiable(self):
        for name in ("MiusMultiplayer_v1.0.0_TEST_INSTALL.zip",
                     "MiusMultiplayer_v1.0.0_TEST_INSTALL_known_good.zip",
                     "MiusMultiplayer_v1.0.2_TEST_INSTALL.zip",
                     "MiusMultiplayer_v1.0.3_TEST_INSTALL.zip",
                     "MiusMultiplayer_v1.0.4_TEST_INSTALL.zip",
                     "MiusMultiplayer_v1.0.5_TEST_INSTALL.zip",
                     "MiusMultiplayer_v1.0.6_TEST_INSTALL.zip",
                     "MiusMultiplayer_v1.0.7_TEST_INSTALL.zip"):
            archive = ROOT / name
            with self.subTest(name=name):
                before = hashlib.sha256(archive.read_bytes()).digest()
                pkg.verify(archive)
                with self.assertRaisesRegex(SystemExit, "refusing to overwrite"):
                    pkg.build(ROOT, Path("nonexistent.codelib"), archive, Path("nonexistent.exe"))
                self.assertEqual(before, hashlib.sha256(archive.read_bytes()).digest())

    def test_unified_layout_and_reproducibility(self):
        with tempfile.TemporaryDirectory() as tmp:
            temp = Path(tmp)
            first = self.build_fixture(temp, "a.zip")
            second = self.build_fixture(temp, "b.zip")
            self.assertEqual(first.read_bytes(), second.read_bytes())
            with zipfile.ZipFile(first) as archive:
                names = archive.namelist()
                self.assertNotIn("MiusMultiplayer/net_v21/", names)
                self.assertFalse(any("Switch_" in name for name in names))
                self.assertEqual(self.tagged_module, archive.read(f"MiusMultiplayer/{pkg.MODULE_PATH}"))
                self.assertEqual(self.tagged_launcher, archive.read(f"MiusMultiplayer/{pkg.LAUNCHER_PATH}"))
                self.assertIn(b"-Action Install", archive.read("MiusMultiplayer/Enable_Multiplayer.bat"))
                runtime = archive.read("MiusMultiplayer/FILES_SHA256.txt").decode().splitlines()
                self.assertEqual(len(runtime), 6)
                self.assertTrue(any(pkg.MODULE_PATH in line for line in runtime))
                self.assertTrue(any(pkg.LAUNCHER_PATH in line for line in runtime))

    def test_tagged_pair_is_required(self):
        with tempfile.TemporaryDirectory() as tmp:
            temp = Path(tmp)
            module, launcher, archive = temp / "module", temp / "launcher", temp / "test.zip"
            module.write_bytes(self.tagged_module)
            launcher.write_bytes(self.old_launcher)
            with self.assertRaisesRegex(SystemExit, "wire-protocol tag"):
                pkg.build(ROOT, module, archive, launcher)
            module.write_bytes(self.module)
            launcher.write_bytes(self.tagged_launcher)
            with self.assertRaisesRegex(SystemExit, "wire-protocol tag"):
                pkg.build(ROOT, module, archive, launcher)

    def test_missing_or_tampered_files_are_rejected(self):
        with tempfile.TemporaryDirectory() as tmp:
            temp = Path(tmp)
            archive = self.build_fixture(temp)
            for path, how in ((pkg.MODULE_PATH, "missing"), (pkg.MODULE_PATH, "altered"),
                              (pkg.LAUNCHER_PATH, "missing"), (pkg.LAUNCHER_PATH, "altered"),
                              ("Apply_Multiplayer.ps1", "missing"), ("Apply_Multiplayer.ps1", "altered"),
                              ("Collect_Logs.ps1", "missing"), ("FILES_SHA256.txt", "altered"),
                              ("README.txt", "altered"), ("TEST_BUILD.txt", "layout")):
                output = temp / (how + "-" + Path(path).name + ".zip")
                with zipfile.ZipFile(archive) as src, zipfile.ZipFile(output, "w") as dst:
                    for item in src.infolist():
                        target = item.filename == "MiusMultiplayer/" + path
                        if target and how == "missing":
                            continue
                        data = src.read(item.filename)
                        if target and how == "altered":
                            data += b"changed"
                        if target and how == "layout":
                            data = data.replace(b"layout: unified", b"layout: removed")
                        dst.writestr(copy.copy(item), data)
                with self.subTest(path=path, how=how), self.assertRaises(SystemExit):
                    pkg.verify(output)

    def test_passport_hashes_actual_crlf(self):
        with tempfile.TemporaryDirectory() as tmp:
            temp = Path(tmp)
            repo = temp / "repo"
            groups = ((pkg.PROVEN, (*pkg.SHARED_BINARIES, *pkg.COMMON_TEXT)),
                      (pkg.UNIFIED, pkg.UNIFIED_TEXT),
                      (pkg.KIT / "net_v21", pkg.COLLECTOR_TEXT))
            for base, paths in groups:
                for relative in paths:
                    source, target = ROOT / base / relative, repo / base / relative
                    target.parent.mkdir(parents=True, exist_ok=True)
                    data = source.read_bytes()
                    if base == pkg.UNIFIED and relative == "README.txt":
                        data = data.replace(b"\r\n", b"\n")
                        self.assertIn(b"\n", data)
                        self.assertNotIn(b"\r\n", data)
                    target.write_bytes(data)
            module, launcher, archive = temp / "module", temp / "launcher", temp / "test.zip"
            module.write_bytes(self.tagged_module); launcher.write_bytes(self.tagged_launcher)
            pkg.build(repo, module, archive, launcher)
            pkg.verify_source(repo, archive)
            with zipfile.ZipFile(archive) as src:
                data = src.read("MiusMultiplayer/README.txt")
                self.assertIn(b"\r\n", data)
                self.assertIn(pkg.sha256(data).encode(), src.read("MiusMultiplayer/TEST_BUILD.txt"))
            readme = repo / pkg.UNIFIED / "README.txt"
            readme.write_bytes(readme.read_bytes() + b"later source change\n")
            with self.assertRaisesRegex(SystemExit, "committed archive differs from source"):
                pkg.verify_source(repo, archive)


if __name__ == "__main__":
    unittest.main(verbosity=2)

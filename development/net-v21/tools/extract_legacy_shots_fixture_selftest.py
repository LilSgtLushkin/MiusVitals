#!/usr/bin/env python3
"""Pin, tamper, missing-file and wire-compatibility checks for the 1.0.7 fixture."""
from pathlib import Path
import shutil
import tempfile
import unittest

import extract_legacy_shots_fixture as fixture


class FixtureTest(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.root = Path(self.temp.name)
        self.copy = self.root / "fixture"
        shutil.copytree(fixture.FIXTURE, self.copy)
        self.current = self.root / "protocol.h"
        self.current.write_bytes(fixture.CURRENT_PROTOCOL.read_bytes())
        self.addCleanup(self.temp.cleanup)

    def test_pins_and_windows_line_endings(self):
        expected = fixture.verify_fixture(self.copy, self.current)
        self.assertEqual(set(expected), set(fixture.EXPECTED))
        for name in fixture.EXPECTED:
            path = self.copy / name
            path.write_bytes(path.read_bytes().replace(b"\r\n", b"\n").replace(b"\n", b"\r\n"))
        self.current.write_bytes(self.current.read_bytes().replace(b"\r\n", b"\n").replace(b"\n", b"\r\n"))
        self.assertEqual(fixture.verify_fixture(self.copy, self.current), expected)

    def test_each_modified_fixture_is_refused(self):
        for name in fixture.EXPECTED:
            with self.subTest(name=name):
                path = self.copy / name
                original = path.read_bytes()
                path.write_bytes(original + b"changed\n")
                with self.assertRaisesRegex(ValueError, "pinned SHA-256"):
                    fixture.verify_fixture(self.copy, self.current)
                path.write_bytes(original)

    def test_missing_file_is_refused(self):
        (self.copy / "shots.cpp").unlink()
        with self.assertRaises(FileNotFoundError):
            fixture.verify_fixture(self.copy, self.current)

    def test_changed_current_wire_header_is_refused(self):
        self.current.write_bytes(self.current.read_bytes() + b"// changed\n")
        with self.assertRaisesRegex(ValueError, "current wire header differs"):
            fixture.verify_fixture(self.copy, self.current)


if __name__ == "__main__":
    unittest.main()

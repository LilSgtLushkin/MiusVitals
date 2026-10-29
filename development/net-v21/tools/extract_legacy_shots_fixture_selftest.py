#!/usr/bin/env python3
"""Pin, tamper, missing-file and wire-tag checks for the 1.0.7 fixture.

The pinned files are a fixed reference for the old event stream. The current source has moved to a
later wire tag, so the test proves the pins are intact and that the difference is reported - it
must not pass a changed pinned byte, and it must not silently pretend the two protocols are one.
"""
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

    def test_wire_tag_is_read_from_a_header(self):
        self.assertEqual(fixture.wire_tag(b'static const char k_wire_tag[] = "NETV21_WIRE_PROTOCOL_17";\n'),
                         "NETV21_WIRE_PROTOCOL_17")
        self.assertEqual(fixture.wire_tag(b"nothing here"), "")

    def test_divergence_from_the_current_header_is_reported_not_refused(self):
        files = fixture.verify_fixture(self.copy, self.current)
        pinned = fixture.wire_tag(files["protocol.h"])
        self.assertEqual(pinned, "NETV21_WIRE_PROTOCOL_17")
        note = fixture.header_note(files, self.current)
        self.assertIn(pinned, note)
        if fixture.wire_tag(self.current.read_bytes()) != pinned:
            self.assertIn("differs", note)
        # A header with no wire tag must be reported as unreadable, not as matching.
        self.current.write_bytes(b"// no tag\n")
        self.assertIn("not found", fixture.header_note(files, self.current))


if __name__ == "__main__":
    unittest.main()

#!/usr/bin/env python3
"""No-game tests for duplicate detection and preservation of rollback archives."""
from __future__ import annotations

import tempfile
import unittest
import zipfile
from pathlib import Path

import archive_audit as audit


def make(path: Path, payload: bytes, passport: bytes = b"", *, compress: int = zipfile.ZIP_STORED) -> Path:
    with zipfile.ZipFile(path, "w", compression=compress) as archive:
        archive.writestr("MiusMultiplayer/files/basic_network.codelib", payload)
        archive.writestr(audit.PASSPORT, passport)
    return path


class ArchiveAudit(unittest.TestCase):
    def test_payload_duplicates_and_distinct_builds(self):
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            first = make(root / "first.zip", b"same module", b"one")
            duplicate = make(root / "duplicate.zip", b"same module", b"two", compress=zipfile.ZIP_DEFLATED)
            other = make(root / "other.zip", b"different module", b"one")
            groups = audit.duplicate_groups([first, duplicate, other])
            self.assertEqual(len(groups), 1)
            self.assertEqual(set(groups[0]), {first, duplicate})
            self.assertTrue(audit.is_redundant(groups[0]))

    def test_known_good_is_protected_but_other_copies_are_not(self):
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            backup = make(root / audit.ROLLBACK, b"same")
            one = make(root / "one.zip", b"same")
            group = audit.duplicate_groups([backup, one])[0]
            self.assertFalse(audit.is_redundant(group))
            two = make(root / "two.zip", b"same")
            group = audit.duplicate_groups([backup, one, two])[0]
            self.assertTrue(audit.is_redundant(group))

    def test_corrupt_and_duplicate_names_are_rejected(self):
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            invalid = root / "bad.zip"
            invalid.write_bytes(b"not a zip")
            with self.assertRaises(zipfile.BadZipFile):
                audit.fingerprint(invalid)
            corrupt = make(root / "crc.zip", b"unique-payload-for-crc")
            content = corrupt.read_bytes()
            self.assertEqual(content.count(b"unique-payload-for-crc"), 1)
            corrupt.write_bytes(content.replace(b"unique-payload-for-crc", b"Unique-payload-for-crc", 1))
            with self.assertRaisesRegex(ValueError, "CRC check failed"):
                audit.fingerprint(corrupt)
            duplicate = root / "duplicate_names.zip"
            with zipfile.ZipFile(duplicate, "w") as archive:
                archive.writestr("same.txt", b"a")
                # A duplicate central-directory name must not be silently overwritten.
                import warnings
                with warnings.catch_warnings():
                    warnings.simplefilter("ignore", UserWarning)
                    archive.writestr("same.txt", b"b")
            with self.assertRaisesRegex(ValueError, "duplicate file names"):
                audit.fingerprint(duplicate)


if __name__ == "__main__":
    unittest.main(verbosity=2)

#!/usr/bin/env python3
"""Offline tests for the pinned, history-backed installer fixture loader."""
from __future__ import annotations

import hashlib
from pathlib import Path
import subprocess
from types import SimpleNamespace
import unittest
from unittest.mock import patch

import historical_106_fixture as fixture
import historical_107_fixture as fixture_107


class HistoricalBlob(unittest.TestCase):
    def setUp(self):
        self.data = b"synthetic historical ZIP bytes"
        self.digest = hashlib.sha256(self.data).hexdigest()
        self.repo = Path("fixture-repository")

    @patch("historical_106_fixture.subprocess.run")
    def test_reads_exact_revision_and_verifies_the_result(self, run):
        run.return_value = SimpleNamespace(returncode=0, stdout=self.data, stderr=b"")
        self.assertEqual(fixture.read_verified_blob(self.repo, "abc", "old.zip", self.digest), self.data)
        args, kwargs = run.call_args
        self.assertEqual(args[0], ["git", "show", "abc:old.zip"])
        self.assertEqual(kwargs["cwd"], self.repo)
        self.assertFalse(kwargs["check"])

    @patch("historical_106_fixture.subprocess.run")
    def test_modified_blob_is_rejected(self, run):
        run.return_value = SimpleNamespace(returncode=0, stdout=self.data + b"!", stderr=b"")
        with self.assertRaisesRegex(ValueError, "SHA-256 mismatch"):
            fixture.read_verified_blob(self.repo, "abc", "old.zip", self.digest)

    @patch("historical_106_fixture.subprocess.run")
    def test_missing_commit_fails_with_fetch_instruction(self, run):
        run.return_value = SimpleNamespace(returncode=128, stdout=b"", stderr=b"missing object")
        with self.assertRaisesRegex(RuntimeError, "git fetch --no-tags --depth=1 origin abc"):
            fixture.read_verified_blob(self.repo, "abc", "old.zip", self.digest)

    @patch("historical_106_fixture.subprocess.run")
    def test_missing_git_and_timeout_are_not_silently_skipped(self, run):
        for error in (FileNotFoundError("git"), subprocess.TimeoutExpired("git", 60)):
            run.side_effect = error
            with self.subTest(error=error), self.assertRaisesRegex(RuntimeError, "cannot read"):
                fixture.read_verified_blob(self.repo, "abc", "old.zip", self.digest)

class Retired107Fixture(unittest.TestCase):
    """The retired 1.0.7 installers are read from one pinned commit, not from the tree."""

    def test_both_readers_use_their_pinned_commit_name_and_hash(self):
        data = b"retired 1.0.7 installer bytes"
        with patch("historical_107_fixture.read_verified_blob", return_value=data) as read:
            self.assertEqual(fixture_107.read_107(), data)
            self.assertEqual(fixture_107.read_107_experimental(), data)
        self.assertEqual([call.args[1:] for call in read.call_args_list],
                         [(fixture_107.REVISION, fixture_107.NAME_107, fixture_107.SHA256_107),
                          (fixture_107.REVISION, fixture_107.NAME_107_EXPERIMENTAL,
                           fixture_107.SHA256_107_EXPERIMENTAL)])

    def test_pinned_coordinates_describe_the_retired_archives(self):
        self.assertEqual(len(fixture_107.REVISION), 40, "the commit must be pinned, not a branch")
        self.assertEqual(fixture_107.NAME_107, "MiusMultiplayer_v1.0.7_TEST_INSTALL.zip")
        self.assertEqual(fixture_107.NAME_107_EXPERIMENTAL,
                         "MiusMultiplayer_v1.0.7_EXPERIMENTAL_INSTALL.zip")
        for digest in (fixture_107.SHA256_107, fixture_107.SHA256_107_EXPERIMENTAL):
            with self.subTest(digest=digest):
                self.assertEqual(len(digest), 64)
                int(digest, 16)


if __name__ == "__main__":
    unittest.main(verbosity=2)

#!/usr/bin/env python3
"""Regression tests for integrating the user-supplied reference with the artifact guard."""
from __future__ import annotations

import contextlib
import importlib.util
import io
from pathlib import Path
import shutil
import tempfile
import unittest

HERE = Path(__file__).resolve().parent
spec = importlib.util.spec_from_file_location("check_artifacts", HERE / "check_artifacts.py")
assert spec and spec.loader
checker = importlib.util.module_from_spec(spec)
spec.loader.exec_module(checker)
REFERENCE = HERE.parent / "development/game-build-3598-1"


class GameReference(unittest.TestCase):
    def test_manifest_and_unknown_binary_guard(self):
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            copy = root / "development/game-build-3598-1"
            copy.mkdir(parents=True)
            shutil.copy(REFERENCE / "manifest.py", copy)
            shutil.copy(REFERENCE / "MANIFEST.txt", copy)
            shutil.copytree(REFERENCE / "x64", copy / "x64")
            old_root, old_artifacts = checker.ROOT, checker.ARTIFACTS
            checker.ROOT, checker.ARTIFACTS = root, {}
            try:
                def scan():
                    errors = []
                    with contextlib.redirect_stdout(io.StringIO()):
                        checker.check_artifacts(errors)
                    return errors

                self.assertEqual(scan(), [], "a complete verified reference is recognised")
                path = copy / "x64/adv_physic.codelib"
                data = path.read_bytes()
                path.write_bytes(b"X" + data[1:])
                self.assertTrue(any("game reference manifest verification failed" in e for e in scan()),
                                "a modified reference still fails")
                path.write_bytes(data)
                path.unlink()
                self.assertTrue(any("game reference manifest verification failed" in e for e in scan()),
                                "a missing reference still fails")
                path.write_bytes(data)
                extra = copy / "x64/extra.codelib"
                extra.write_bytes(b"extra")
                self.assertTrue(any("game reference manifest verification failed" in e for e in scan()),
                                "an unlisted top-level file still fails")
                extra.unlink()
                nested = copy / "x64/nested/extra.codelib"
                nested.parent.mkdir()
                nested.write_bytes(b"extra")
                self.assertTrue(any("binary not listed" in e for e in scan()),
                                "the whole reference directory is not blindly exempted")
                nested.unlink()
                unknown = root / "development/unexpected.codelib"
                unknown.write_bytes(b"unlisted")
                self.assertTrue(any("binary not listed" in e for e in scan()),
                                "unlisted binaries outside the reference still fail")
                unknown.unlink()
                (copy / "MANIFEST.txt").unlink()
                self.assertTrue(any("verifier, manifest or x64 directory is missing" in e for e in scan()),
                                "a missing manifest is not ignored")
            finally:
                checker.ROOT, checker.ARTIFACTS = old_root, old_artifacts


if __name__ == "__main__":
    unittest.main(verbosity=2)

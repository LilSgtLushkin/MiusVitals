#!/usr/bin/env python3
"""Check that the saved manifest detects modified, missing and extra reference files."""
from __future__ import annotations

import shutil
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

HERE = Path(__file__).resolve().parent


class Manifest(unittest.TestCase):
    def test_verify_and_changes(self):
        with tempfile.TemporaryDirectory() as temp:
            copy = Path(temp) / "reference"
            copy.mkdir()
            shutil.copy(HERE / "manifest.py", copy)
            shutil.copy(HERE / "MANIFEST.txt", copy)
            shutil.copytree(HERE / "x64", copy / "x64")

            def verify():
                return subprocess.run([sys.executable, str(copy / "manifest.py"), "--verify"],
                                      capture_output=True, text=True, check=False)

            result = verify()
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            self.assertIn("OK 19 files", result.stdout)

            path = copy / "x64/adv_physic.codelib"
            data = path.read_bytes()
            path.write_bytes(b"X" + data[1:])
            result = verify()
            self.assertEqual(result.returncode, 1)
            self.assertIn("adv_physic.codelib", result.stdout)

            path.write_bytes(data[:16])  # A truncated PE should be reported as changed, not crash.
            result = verify()
            self.assertEqual(result.returncode, 1)
            self.assertIn("adv_physic.codelib", result.stdout)
            self.assertNotIn("Traceback", result.stderr)

            path.unlink()
            result = verify()
            self.assertEqual(result.returncode, 1)
            self.assertIn("adv_physic.codelib", result.stdout)

            path.write_bytes(data)
            (copy / "x64/extra.codelib").write_bytes(b"extra")
            result = verify()
            self.assertEqual(result.returncode, 1)
            self.assertIn("extra.codelib", result.stdout)


if __name__ == "__main__":
    unittest.main(verbosity=2)

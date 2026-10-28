#!/usr/bin/env python3
"""Check that the offline legacy-kit fixture preserves source and tamper detection."""
from __future__ import annotations

from pathlib import Path
import shutil
import tempfile
import unittest

import verify_legacy_fps_fixture as fixture


class HistoricalFpsFixture(unittest.TestCase):
    def test_intact_source_matches_all_released_files(self):
        fixture.verify()

    def test_text_change_is_rejected_even_if_its_required_markers_remain(self):
        with tempfile.TemporaryDirectory(prefix="mius-legacy-test-") as directory:
            repo = Path(directory)
            source = fixture.package.SOURCE
            shutil.copytree(fixture.ROOT / source, repo / source)
            for name in ("development/fps-candidate/basic_network.codelib",
                         "development/kit-source/files/basic_network.codelib"):
                target = repo / name
                target.parent.mkdir(parents=True, exist_ok=True)
                shutil.copy2(fixture.ROOT / name, target)
            readme = repo / source / "README.txt"
            readme.write_bytes(readme.read_bytes() + b"\r\nchanged after release\r\n")
            with self.assertRaisesRegex(SystemExit, "historical FPS fixture differs.*README.txt"):
                fixture.verify(repo)


if __name__ == "__main__":
    unittest.main(verbosity=2)

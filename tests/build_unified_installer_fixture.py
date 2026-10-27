#!/usr/bin/env python3
"""Create a disposable packaging-only archive for fast Windows installer tests.

The PE overlays are deliberately NOT real compatible net_v21 binaries. Never publish this archive;
production CI builds its own module and launcher with MSVC before packaging.
"""
import importlib.util
from pathlib import Path
import sys
import tempfile
import zipfile

ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location("package_net_v21_test", ROOT / "scripts/package_net_v21_test.py")
pkg = importlib.util.module_from_spec(spec)
assert spec and spec.loader
spec.loader.exec_module(pkg)
if len(sys.argv) != 2:
    raise SystemExit("usage: build_unified_installer_fixture.py <temporary zip>")
output = Path(sys.argv[1]).resolve()
if ROOT == output.parent or ROOT in output.parents:
    raise SystemExit("the disposable fixture must be outside the repository")
with tempfile.TemporaryDirectory() as tmp, zipfile.ZipFile(ROOT / "MiusMultiplayer_v1.0.0_TEST_INSTALL_known_good.zip") as old:
    module, launcher = Path(tmp) / "module.codelib", Path(tmp) / "launcher.exe"
    module.write_bytes(old.read("MiusMultiplayer/net_v21/basic_network.netv21.codelib") + pkg.WIRE_TAG)
    launcher.write_bytes(old.read("MiusMultiplayer/files/mp_join.exe") + pkg.WIRE_TAG)
    pkg.build(ROOT, module, output, launcher)

#!/usr/bin/env python3
"""Exercise the actual switch scripts on Windows with disposable files; no game/Steam."""
import os
from pathlib import Path
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
KIT = ROOT / "development/net-v21-kit/net_v21"


@unittest.skipUnless(os.name == "nt", "requires the real Windows cmd.exe, copy, move and fc")
class SwitchPair(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory(prefix="m5 switch ")
        self.game = Path(self.tmp.name) / "Game with spaces"
        self.mod = self.game / "MiusMultiplayer"
        self.kit = self.mod / "net_v21"
        self.kit.mkdir(parents=True)
        (self.game / "starter_x64.exe").write_bytes(b"fixture only")
        self.target_mod = self.game / "bin/rel/x64/basic_network.codelib"
        self.target_mod.parent.mkdir(parents=True)
        self.target_exe = self.game / "mp_join.exe"
        self.previous_mod = b"previous installed module"
        self.previous_exe = b"previous installed launcher"
        self.candidate_mod = b"new matching module"
        self.candidate_exe = b"new matching launcher"
        self.proven_mod = b"proven module"
        self.proven_exe = b"proven launcher"
        self.target_mod.write_bytes(self.previous_mod)
        self.target_exe.write_bytes(self.previous_exe)
        (self.kit / "basic_network.netv21.codelib").write_bytes(self.candidate_mod)
        (self.kit / "mp_join.netv21.exe").write_bytes(self.candidate_exe)
        (self.kit / "basic_network.proven.codelib").write_bytes(self.proven_mod)
        (self.mod / "files").mkdir()
        (self.mod / "files/mp_join.exe").write_bytes(self.proven_exe)
        for name in ("Switch_Pair.bat", "Switch_To_NetV21.bat", "Switch_Back_To_Proven.bat"):
            text = (KIT / name).read_text(encoding="utf-8")
            (self.kit / name).write_bytes(text.replace("\r\n", "\n").replace("\n", "\r\n").encode())
        self.state = self.mod / "state/net_v21_switch"
        self.marker = self.state / "pending.txt"

    def tearDown(self):
        self.tmp.cleanup()

    def run_switch(self, name):
        cmd = f'call "{self.kit / name}" /quiet'
        return subprocess.run(["cmd.exe", "/D", "/C", cmd], cwd=str(self.kit),
                              capture_output=True, text=True, timeout=25)

    def assert_pair(self, module, launcher):
        self.assertEqual(self.target_mod.read_bytes(), module)
        self.assertEqual(self.target_exe.read_bytes(), launcher)

    def inject(self, anchor, addition):
        # Fault injection changes only the disposable test copy, never the shipped script.
        path = self.kit / "Switch_Pair.bat"
        script = path.read_text(encoding="utf-8")
        self.assertEqual(script.count(anchor), 1)
        script = script.replace(anchor, anchor + "\n" + addition)
        path.write_bytes(script.replace("\r\n", "\n").replace("\n", "\r\n").encode())

    def test_switch_and_back_replaces_both_and_keeps_backups(self):
        result = self.run_switch("Switch_To_NetV21.bat")
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assert_pair(self.candidate_mod, self.candidate_exe)
        self.assertFalse(self.marker.exists())
        self.assertEqual((self.state / "previous.basic_network.codelib").read_bytes(), self.previous_mod)
        self.assertEqual((self.state / "previous.mp_join.exe").read_bytes(), self.previous_exe)
        result = self.run_switch("Switch_Back_To_Proven.bat")
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assert_pair(self.proven_mod, self.proven_exe)
        self.assertFalse(self.marker.exists())

    def test_missing_launcher_changes_nothing(self):
        (self.kit / "mp_join.netv21.exe").unlink()
        result = self.run_switch("Switch_To_NetV21.bat")
        self.assertNotEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assert_pair(self.previous_mod, self.previous_exe)
        self.assertFalse(self.marker.exists())

    def test_failure_between_moves_restores_both(self):
        self.inject('move /y "%DSTMOD%.mp_v21.put" "%DSTMOD%" >nul || goto :rollback', 'goto :rollback')
        result = self.run_switch("Switch_To_NetV21.bat")
        self.assertNotEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assert_pair(self.previous_mod, self.previous_exe)
        self.assertFalse(self.marker.exists())

    def test_failed_rollback_leaves_marker_and_prevents_retry(self):
        self.inject('move /y "%DSTMOD%.mp_v21.put" "%DSTMOD%" >nul || goto :rollback', 'goto :rollback')
        self.inject('\n:restore', 'exit /b 1')
        result = self.run_switch("Switch_To_NetV21.bat")
        self.assertNotEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertTrue(self.marker.exists())
        self.assertEqual((self.state / "previous.basic_network.codelib").read_bytes(), self.previous_mod)
        self.assertEqual((self.state / "previous.mp_join.exe").read_bytes(), self.previous_exe)
        pair_before = (self.target_mod.read_bytes(), self.target_exe.read_bytes())
        retry = self.run_switch("Switch_Back_To_Proven.bat")
        self.assertNotEqual(retry.returncode, 0, retry.stdout + retry.stderr)
        self.assert_pair(*pair_before)
        self.assertTrue(self.marker.exists())


if __name__ == "__main__":
    unittest.main(verbosity=2)

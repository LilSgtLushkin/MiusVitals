#!/usr/bin/env python3
"""Test the actual packaged BAT/PowerShell installer on disposable Windows game folders.

The archive is produced by the current MSVC build. No game or Steam is run. Error injection is
inserted ONLY into disposable extracted script copies; the archive and its installer remain intact.
"""
from __future__ import annotations
import ctypes
from ctypes import wintypes
import hashlib
import os
from pathlib import Path
import stat
import subprocess
import tempfile
import unittest
import zipfile

ROOT = Path(__file__).resolve().parents[1]
ARCHIVE = Path(os.environ.get("MIUS_TEST_ARCHIVE", ROOT / "MiusMultiplayer_v1.0.7_TEST_INSTALL.zip"))
FILES = ("mp_vitals.dll", "bin/rel/x64/basic_network.codelib", "mp_join.exe",
         "root/programs/opgame.progpack", "root/programs/tacgame.progpack", "steam_appid.txt")


@unittest.skipUnless(os.name == "nt", "requires Windows cmd.exe and Windows PowerShell")
class UnifiedInstaller(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(prefix="mius installer ")
        self.game = Path(self.temp.name) / "Game with spaces"
        self.game.mkdir()
        with zipfile.ZipFile(ARCHIVE) as archive:
            archive.extractall(self.game)
        self.kit = self.game / "MiusMultiplayer"
        (self.game / "starter_x64.exe").write_bytes(b"fixture, not an executable")
        self.original = {}
        for index, relative in enumerate(FILES):
            target = self.game / relative
            # Cover both replacement of existing files and deletion of newly introduced files.
            data = None if index in (2, 5) else ("previous file " + relative).encode()
            self.original[relative] = data
            if data is not None:
                target.parent.mkdir(parents=True, exist_ok=True)
                target.write_bytes(data)
        self.readonly = self.game / FILES[1]
        os.chmod(self.readonly, stat.S_IREAD)
        self.sentinel = self.game / "untouched.txt"
        self.sentinel.write_bytes(b"unrelated user data")
        self.state = self.kit / "state" / os.environ.get("MIUS_TEST_STATE", "unified_test_v107")
        self.baseline = self.state / "baseline"
        self.pending = self.state / "pending"

    def tearDown(self):
        for path in Path(self.temp.name).rglob("*"):
            if path.is_file():
                os.chmod(path, stat.S_IREAD | stat.S_IWRITE)
        self.temp.cleanup()

    def bat(self, name, extra=None):
        env = os.environ.copy()
        for env_name in ("MIUS_CI_FAIL_AT", "MIUS_CI_FAIL_ROLLBACK", "MIUS_CI_CRASH_AT",
                         "MIUS_CI_CRASH_BACKUP", "MIUS_CI_CRASH_RESTORE_AT"):
            env.pop(env_name, None)
        if extra:
            env.update(extra)
        # cmd.exe parses quoted batch paths differently from list2cmdline's C-runtime escaping.
        command = f'cmd.exe /D /C call "{self.kit / name}" /quiet'
        return subprocess.run(command, cwd=str(self.kit), env=env, capture_output=True,
                              text=True, encoding="utf-8", errors="replace", timeout=90)

    def succeed(self, result):
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    def failed(self, result):
        self.assertNotEqual(result.returncode, 0, result.stdout + result.stderr)

    def check_original(self):
        for relative, data in self.original.items():
            target = self.game / relative
            with self.subTest(path=relative):
                if data is None:
                    self.assertFalse(target.exists())
                else:
                    self.assertEqual(target.read_bytes(), data)
        self.assertFalse(os.stat(self.readonly).st_mode & stat.S_IWRITE, "read-only attribute lost")
        self.assertEqual(self.sentinel.read_bytes(), b"unrelated user data")

    def check_installed(self):
        for relative in FILES:
            self.assertEqual((self.game / relative).read_bytes(), (self.kit / "files" / relative).read_bytes(), relative)
        self.assertEqual(self.sentinel.read_bytes(), b"unrelated user data")

    def inject(self):
        path = self.kit / "Apply_Multiplayer.ps1"
        script = path.read_text(encoding="utf-8")
        for anchor, addition in (("# TEST-ANCHOR: installed-file",
                                  "if ($env:MIUS_CI_CRASH_AT -eq [string]$index) { [Environment]::Exit(91) }\n"
                                  "if ($env:MIUS_CI_FAIL_AT -eq [string]$index) { throw 'injected install failure' }"),
                                 ("# TEST-ANCHOR: restore-snapshot",
                                  "if ($env:MIUS_CI_FAIL_ROLLBACK -eq '1') { throw 'injected rollback failure' }"),
                                 ("# TEST-ANCHOR: snapshot-copied-file",
                                  "if ($env:MIUS_CI_CRASH_BACKUP -eq '1' -and $folder -like '*pending_*.tmp') { [Environment]::Exit(92) }"),
                                 ("# TEST-ANCHOR: restored-file",
                                  "if ($folder -eq $baseline -and $env:MIUS_CI_CRASH_RESTORE_AT -eq [string]$restoreIndex) { [Environment]::Exit(93) }")):
            self.assertEqual(script.count(anchor), 1)
            script = script.replace(anchor, anchor + "\n    " + addition)
        path.write_bytes(script.replace("\r\n", "\n").replace("\n", "\r\n").encode("utf-8"))

    def snapshot(self):
        return {p: (self.game / p).read_bytes() if (self.game / p).exists() else None for p in FILES}

    def test_one_step_repeat_check_and_uninstall(self):
        self.failed(self.bat("Check_Install.bat"))
        self.succeed(self.bat("Enable_Multiplayer.bat"))
        self.check_installed()
        self.succeed(self.bat("Check_Install.bat"))
        log = self.game / "out/net_v21_opgame.log"
        log.write_bytes(b"synthetic log, no game run")
        collector = subprocess.run(["powershell.exe", "-NoProfile", "-ExecutionPolicy", "Bypass", "-File",
                                    str(self.kit / "Collect_Logs.ps1"), "-NoOpen"], cwd=str(self.kit),
                                   capture_output=True, text=True, encoding="utf-8", errors="replace", timeout=60)
        self.succeed(collector)
        reports = list(self.kit.glob("logs_*"))
        self.assertEqual(len(reports), 1)
        self.assertEqual((reports[0] / log.name).read_bytes(), log.read_bytes())
        report = (reports[0] / "COLLECTION.txt").read_text(encoding="utf-8-sig")
        digest = hashlib.sha256((self.game / FILES[1]).read_bytes()).hexdigest().upper()
        self.assertIn(f"archive network module: {digest}", report)
        self.assertIn(f"active network module: {digest}", report)
        self.assertFalse(any(p.suffix in (".dll", ".exe", ".codelib", ".opsave") for p in reports[0].iterdir()))
        original_baseline = (self.baseline / "manifest.json").read_bytes()
        self.succeed(self.bat("Enable_Multiplayer.bat"))
        self.check_installed()
        self.assertEqual((self.baseline / "manifest.json").read_bytes(), original_baseline)
        self.assertFalse(self.pending.exists())
        self.succeed(self.bat("Disable_Multiplayer.bat"))
        self.check_original()
        self.assertFalse(self.baseline.exists())
        self.succeed(self.bat("Disable_Multiplayer.bat"))
        self.check_original()

    def test_upgrade_from_106_restores_its_runtime_and_preserves_its_backup(self):
        old = ROOT / "MiusMultiplayer_v1.0.6_TEST_INSTALL.zip"
        with zipfile.ZipFile(old) as archive:
            os.chmod(self.readonly, stat.S_IREAD | stat.S_IWRITE)
            for relative in FILES:
                path = self.game / relative
                path.parent.mkdir(parents=True, exist_ok=True)
                path.write_bytes(archive.read("MiusMultiplayer/files/" + relative))
        previous = self.snapshot()
        old_state = self.kit / "state/unified_test_v106/baseline"
        old_state.mkdir(parents=True)
        marker = old_state / "old-backup-preserved.txt"
        marker.write_bytes(b"do not overwrite the previous installer's backup")
        self.succeed(self.bat("Enable_Multiplayer.bat"))
        self.check_installed()
        self.assertEqual(marker.read_bytes(), b"do not overwrite the previous installer's backup")
        self.succeed(self.bat("Disable_Multiplayer.bat"))
        self.assertEqual(self.snapshot(), previous)
        self.assertEqual(marker.read_bytes(), b"do not overwrite the previous installer's backup")

    def test_corrupt_or_missing_payload_and_manifest_do_not_change_anything(self):
        payload = self.kit / "files" / FILES[1]
        content = payload.read_bytes()
        payload.write_bytes(content + b"corrupted")
        self.failed(self.bat("Enable_Multiplayer.bat"))
        self.check_original()
        self.assertFalse(self.baseline.exists())
        payload.unlink()
        self.failed(self.bat("Enable_Multiplayer.bat"))
        self.check_original()
        payload.write_bytes(content)
        manifest = self.kit / "FILES_SHA256.txt"
        manifest.write_bytes(manifest.read_bytes() + b"invalid line\r\n")
        self.failed(self.bat("Enable_Multiplayer.bat"))
        self.check_original()
        self.assertFalse(self.baseline.exists())

    def test_fault_after_each_replacement_restores_everything(self):
        self.inject()
        for index in range(1, 7):
            with self.subTest(failure_after=index):
                result = self.bat("Enable_Multiplayer.bat", {"MIUS_CI_FAIL_AT": str(index)})
                self.failed(result)
                self.check_original()
                self.assertFalse(self.pending.exists())
                self.assertFalse(self.baseline.exists())
        self.succeed(self.bat("Enable_Multiplayer.bat"))
        self.check_installed()
        self.succeed(self.bat("Disable_Multiplayer.bat"))
        self.check_original()

    def test_reinstall_failure_restores_pre_attempt_state_then_uninstall_restores_original(self):
        self.inject()
        self.succeed(self.bat("Enable_Multiplayer.bat"))
        modified = self.game / FILES[0]
        modified.write_bytes(b"user changed a previously installed file")
        before = self.snapshot()
        baseline = (self.baseline / "manifest.json").read_bytes()
        for index in range(1, 7):
            with self.subTest(failure_after=index):
                self.failed(self.bat("Enable_Multiplayer.bat", {"MIUS_CI_FAIL_AT": str(index)}))
                self.assertEqual(self.snapshot(), before)
                self.assertEqual((self.baseline / "manifest.json").read_bytes(), baseline)
                self.assertFalse(self.pending.exists())
        self.succeed(self.bat("Disable_Multiplayer.bat"))
        self.check_original()

    def test_failed_rollback_blocks_install_and_disable_recovers(self):
        self.inject()
        result = self.bat("Enable_Multiplayer.bat", {"MIUS_CI_FAIL_AT": "3", "MIUS_CI_FAIL_ROLLBACK": "1"})
        self.failed(result)
        self.assertIn("RECOVERY REQUIRED", result.stdout)
        self.assertTrue((self.pending / "manifest.json").exists())
        before = self.snapshot()
        self.failed(self.bat("Enable_Multiplayer.bat"))
        self.assertEqual(self.snapshot(), before)
        self.succeed(self.bat("Disable_Multiplayer.bat"))
        self.check_original()
        self.assertFalse(self.pending.exists())
        self.assertFalse(self.baseline.exists())
        self.succeed(self.bat("Enable_Multiplayer.bat"))
        self.succeed(self.bat("Disable_Multiplayer.bat"))
        self.check_original()

    def test_crash_while_building_snapshot_never_publishes_an_incomplete_pending(self):
        self.inject()
        result = self.bat("Enable_Multiplayer.bat", {"MIUS_CI_CRASH_BACKUP": "1"})
        self.assertEqual(result.returncode, 92, result.stdout + result.stderr)
        self.check_original()
        self.assertFalse(self.pending.exists())
        self.assertTrue(self.baseline.exists())
        self.assertTrue(list(self.state.glob("pending_*.tmp")))
        self.failed(self.bat("Enable_Multiplayer.bat"))
        self.succeed(self.bat("Disable_Multiplayer.bat"))
        self.assertFalse(self.baseline.exists())
        self.check_original()
        self.succeed(self.bat("Enable_Multiplayer.bat"))
        self.succeed(self.bat("Disable_Multiplayer.bat"))
        self.check_original()

    def test_process_crash_after_each_replacement_recovers_on_disable(self):
        self.inject()
        for index in range(1, 7):
            with self.subTest(crash_after=index):
                result = self.bat("Enable_Multiplayer.bat", {"MIUS_CI_CRASH_AT": str(index)})
                self.assertEqual(result.returncode, 91, result.stdout + result.stderr)
                self.assertTrue((self.pending / "manifest.json").exists())
                self.failed(self.bat("Enable_Multiplayer.bat"))
                self.succeed(self.bat("Disable_Multiplayer.bat"))
                self.check_original()
                self.assertFalse(self.pending.exists())
                self.assertFalse(self.baseline.exists())

    def test_process_crash_during_uninstall_is_recoverable(self):
        self.inject()
        self.succeed(self.bat("Enable_Multiplayer.bat"))
        result = self.bat("Disable_Multiplayer.bat", {"MIUS_CI_CRASH_RESTORE_AT": "3"})
        self.assertEqual(result.returncode, 93, result.stdout + result.stderr)
        self.assertTrue((self.pending / "manifest.json").exists())
        self.succeed(self.bat("Disable_Multiplayer.bat"))
        self.check_original()
        self.assertFalse(self.pending.exists())
        self.assertFalse(self.baseline.exists())

    def test_exclusive_lock_refuses_concurrent_modification(self):
        self.state.mkdir(parents=True)
        kernel = ctypes.WinDLL("kernel32", use_last_error=True)
        kernel.CreateFileW.argtypes = (wintypes.LPCWSTR, wintypes.DWORD, wintypes.DWORD,
                                       wintypes.LPVOID, wintypes.DWORD, wintypes.DWORD, wintypes.HANDLE)
        kernel.CreateFileW.restype = wintypes.HANDLE
        kernel.CloseHandle.argtypes = (wintypes.HANDLE,)
        handle = kernel.CreateFileW(str(self.state / "lock"), 0xC0000000, 0, None, 4, 0x80, None)
        self.assertNotEqual(handle, ctypes.c_void_p(-1).value, ctypes.get_last_error())
        try:
            self.failed(self.bat("Enable_Multiplayer.bat"))
            self.check_original()
            self.assertFalse(self.baseline.exists())
        finally:
            kernel.CloseHandle(handle)
        self.succeed(self.bat("Enable_Multiplayer.bat"))
        self.succeed(self.bat("Disable_Multiplayer.bat"))
        self.check_original()

    def test_linked_runtime_file_is_rejected_before_any_changes(self):
        target = self.game / FILES[0]
        target.unlink()
        referent = self.game / "linked-original.bin"
        referent.write_bytes(b"data behind a link")
        try:
            os.symlink(str(referent), str(target))
        except OSError as error:
            self.skipTest(f"creating symlinks is not permitted on this Windows runner: {error}")
        before = self.snapshot()
        result = self.bat("Enable_Multiplayer.bat")
        self.failed(result)
        self.assertIn("linked runtime file is not supported", result.stdout)
        self.assertTrue(target.is_symlink())
        self.assertEqual(self.snapshot(), before)
        self.assertEqual(referent.read_bytes(), b"data behind a link")
        self.assertFalse(self.baseline.exists())

    @unittest.skipUnless(os.environ.get("MIUS_TEST_UPGRADE") == "1", "experimental upgrade fixture only")
    def test_old_107_then_experimental_and_two_step_rollback(self):
        old = ROOT / "MiusMultiplayer_v1.0.7_TEST_INSTALL.zip"
        if not old.exists():
            self.skipTest("the preserved 1.0.7 archive is not available")
        with zipfile.ZipFile(old) as archive:
            archive.extractall(self.game)
        self.succeed(self.bat("Enable_Multiplayer.bat"))
        installed_old = {relative: (self.game / relative).read_bytes() for relative in FILES}
        old_state = self.kit / "state/unified_test_v107"
        self.assertTrue((old_state / "active.txt").exists())
        with zipfile.ZipFile(ARCHIVE) as archive:
            archive.extractall(self.game)
        self.succeed(self.bat("Enable_Multiplayer.bat"))
        self.assertTrue((self.state / "active.txt").exists())
        self.assertTrue((old_state / "active.txt").exists())
        self.check_installed()
        self.succeed(self.bat("Disable_Multiplayer.bat"))
        for relative in FILES:
            self.assertEqual((self.game / relative).read_bytes(), installed_old[relative], relative)
        with zipfile.ZipFile(old) as archive:
            archive.extractall(self.game)
        self.succeed(self.bat("Disable_Multiplayer.bat"))
        self.check_original()

    def test_damaged_backup_prevents_uninstall_without_any_change(self):
        self.succeed(self.bat("Enable_Multiplayer.bat"))
        backup = self.baseline / "files" / FILES[1]
        saved = backup.read_bytes()
        os.chmod(backup, stat.S_IREAD | stat.S_IWRITE)
        backup.write_bytes(b"damaged backup")
        before = self.snapshot()
        result = self.bat("Disable_Multiplayer.bat")
        self.failed(result)
        self.assertEqual(self.snapshot(), before)
        self.assertTrue((self.state / "active.txt").exists())
        self.assertFalse(self.pending.exists())
        backup.write_bytes(saved)
        self.succeed(self.bat("Disable_Multiplayer.bat"))
        self.check_original()


if __name__ == "__main__":
    unittest.main(verbosity=2)

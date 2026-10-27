#!/usr/bin/env python3
"""No-game end-to-end smoke test of the actual mp_join.exe, IP and fake Steam only.

The real session peer serves a synthetic map; a harmless starter fixture checks the child
command/environment. Never runs a game or Steam. Tests both successful launch and rollback when
CreateProcess fails, with paths containing spaces and an existing save to protect.
"""
from __future__ import annotations
import argparse
import ctypes
from ctypes import wintypes
import os
from pathlib import Path
import shutil
import socket
import subprocess
import tempfile
import time


def free_port() -> int:
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as s:
        s.bind(("127.0.0.1", 0))
        return int(s.getsockname()[1])


def run_case(peer: Path, launcher: Path, starter: Path, steam: bool, start_ok: bool) -> None:
    host_port = free_port()
    client_port = free_port()
    while client_port == host_port:
        client_port = free_port()
    transport = "steam" if steam else "ip"
    with tempfile.TemporaryDirectory(prefix="spectator launcher ") as temporary:
        root = Path(temporary) / "Game with spaces"
        profile = root / "users" / "profiles" / "cloud"
        profile.mkdir(parents=True)
        shutil.copyfile(launcher, root / "mp_join.exe")
        if start_ok:
            shutil.copyfile(starter, root / "starter_x64.exe")
        original = b"an existing save that must be protected"
        saved = profile / "camp_test_rus.opsave"
        saved.write_bytes(original)
        env = os.environ.copy()
        for name in ("MP_STEAM", "MP_STEAM_FAKE", "MP_ROLE", "MP_MAP", "MP_PEER", "MP_PORT", "MP_ENDTURN",
                     "MP_TURNDONE", "MP_AUTOJOIN", "MP_HOST_MODE", "MP_MAP_EPOCH", "MP_MAP_REV"):
            env.pop(name, None)
        env["MP_STEAM_FAKE" if steam else "MP_STEAM"] = str(client_port) if steam else "0"
        host = subprocess.Popen([str(peer), f"spectator-{transport}-host", str(host_port), str(client_port)],
                                stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
        try:
            address = f"steam:{host_port}" if steam else f"127.0.0.1:{host_port}"
            try:
                result = subprocess.run([str(root / "mp_join.exe"), address], env=env, cwd=str(root),
                                        capture_output=True, text=True, timeout=18)
            except subprocess.TimeoutExpired as error:
                raise AssertionError(f"{transport}: standalone launcher timed out") from error
            output = result.stdout + result.stderr
            backup = list(profile.glob("camp_test_rus.opsave.before_mp_*"))
            if len(backup) != 1 or backup[0].read_bytes() != original:
                raise AssertionError(f"{transport}: missing or incorrect verified backup: {output}")
            if (profile / "camp_test_rus.opsave.mp_transaction").exists():
                raise AssertionError(f"{transport}: marker unexpectedly retained: {output}")
            if not start_ok:
                if result.returncode == 0 or saved.read_bytes() != original or "Could not open the operation" not in output:
                    raise AssertionError(f"{transport}: failed launch did not restore original: {output}")
            else:
                if result.returncode != 0 or not saved.read_bytes().startswith(b"AZCF"):
                    raise AssertionError(f"{transport}: launch or map replacement failed: {output}")
                marker = root / "starter_fixture_result.txt"
                deadline = time.monotonic() + 5
                while not marker.exists() and time.monotonic() < deadline:
                    time.sleep(0.05)
                content = marker.read_text() if marker.exists() else ""
                if not content.startswith("ok:") or not content[3:].isdigit():
                    raise AssertionError(f"{transport}: child did not inherit correct map environment: {output}")
                # CreateProcess's production wrapper closes its handle. Wait for the fixture to exit
                # before deleting its temporary executable/directory on Windows.
                kernel = ctypes.WinDLL("kernel32", use_last_error=True)
                kernel.OpenProcess.argtypes = (wintypes.DWORD, wintypes.BOOL, wintypes.DWORD)
                kernel.OpenProcess.restype = wintypes.HANDLE
                kernel.WaitForSingleObject.argtypes = (wintypes.HANDLE, wintypes.DWORD)
                kernel.CloseHandle.argtypes = (wintypes.HANDLE,)
                child = kernel.OpenProcess(0x00100000, False, int(content[3:]))
                if child:
                    if kernel.WaitForSingleObject(child, 5000) != 0:
                        kernel.CloseHandle(child)
                        raise AssertionError(f"{transport}: child fixture failed to exit")
                    kernel.CloseHandle(child)
            print(f"STANDALONE_{transport.upper()} {'start' if start_ok else 'rollback'} ok=1")
        finally:
            if host.poll() is None:
                host.terminate()
            try:
                host_output, _ = host.communicate(timeout=5)
            except subprocess.TimeoutExpired:
                host.kill()
                host_output, _ = host.communicate()
            if host.returncode not in (0, 1, 15, -15) and host.returncode is not None:
                # A Windows terminated process commonly returns 1; preserve the host tail on unexpected exits.
                print(f"host exit {host.returncode}: {host_output[-3000:]}")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--peer", required=True, type=Path)
    parser.add_argument("--launcher", required=True, type=Path)
    parser.add_argument("--starter", required=True, type=Path)
    args = parser.parse_args()
    if os.name != "nt":
        print("standalone loopback requires Windows")
        return 1
    peer, launcher, starter = (p.resolve(strict=True) for p in (args.peer, args.launcher, args.starter))
    for steam in (False, True):
        for start_ok in (True, False):
            run_case(peer, launcher, starter, steam, start_ok)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

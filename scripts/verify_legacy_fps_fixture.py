#!/usr/bin/env python3
"""Verify the retired 1.0.1 kit without retaining its released ZIP in the tree.

The hashes below describe the normalized, packed bytes of all 13 files in the
historical archive at main commit 047195f1. A temporary ZIP exercises the same
packager and is removed after verification. Git history is not needed.
"""
from __future__ import annotations

import hashlib
from pathlib import Path
import tempfile
import zipfile

import package_unified_fps_mod as package

ROOT = Path(__file__).resolve().parents[1]
PACKED_HASHES = {
    "Enable_Multiplayer.bat": "50b5d88d80c7780c81e333d7384b79508427cccc779ba12e0dcb28a9345bd7af",
    "Disable_Multiplayer.bat": "5c331bd06aafa94b11738f9a89c59c7e73f893608f96f16fb10febea06d20326",
    "Check_Install.bat": "aef286db0ee24e6f10b850fd89c085ee3189a4803fe08cd721283fa22a9ccad3",
    "Check_Install.ps1": "2b68b021c0fba5257026482de3ad3e8e5f3654e8e7e87d06c4af54270c1a96d3",
    "README.txt": "7ffec876db41f433b37901bfde8afe197a603543f88e7354a8ac10a6a28cc6d4",
    "LICENSE_CC0.txt": "f4e7f373b9b996950337e8d41a4a2939c2d90b7725e9baf3d5084a22717ad328",
    "files/bin/rel/x64/basic_network.codelib": "7b42a69abe58deb1583d2f5320eebbecd9718ffa27ea08933613e67df1901f67",
    "files/mp_vitals.dll": "71a1bf3a37923bbe6c8ee84448d0b91705c2bff7382b647f990b9d671ee224df",
    "files/mp_join.exe": "6aa417cf6e1f01480c997446f2691a58ab55b90fa36e1ff511945989aadfeeaa",
    "files/root/programs/opgame.progpack": "eca75b740f7b11d9e13f2e2001f1b2b133427d0860fb3770220b89b0b8909535",
    "files/root/programs/tacgame.progpack": "01923eabdf6a9d5fa37dc70a3d76740242dbc9a7f26c0298ca3faa8cca87600d",
    "files/steam_appid.txt": "1a43840ca36f1401c85ff5f6e6981088927cfe09b23d155a9c4301dd13ca9610",
    "game_build_3598_1.sha256": "a1f1e17524904ded9850d8f12551636393dcdb8c361f83e161590ef994a99069",
}


def verify(repo: Path = ROOT) -> None:
    if set(PACKED_HASHES) != set(package.FILES):
        raise SystemExit("historical FPS fixture manifest does not match the package file list")
    with tempfile.TemporaryDirectory(prefix="mius-legacy-fps-") as temporary:
        archive_path = Path(temporary) / "fixture.zip"
        package.build(repo, archive_path)  # also verifies CRC, tree, timestamps and source equality
        with zipfile.ZipFile(archive_path) as archive:
            for name, expected in PACKED_HASHES.items():
                actual = hashlib.sha256(archive.read(f"{package.ROOT}/{name}")).hexdigest()
                if actual != expected:
                    raise SystemExit(f"historical FPS fixture differs from the released bytes: {name}")
    print(f"OK retired 1.0.1 FPS fixture: {len(PACKED_HASHES)} historical payload hashes match")


if __name__ == "__main__":
    verify()

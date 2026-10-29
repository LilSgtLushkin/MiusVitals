"""Read the retired 1.0.7 installers as pinned Git blobs, without restoring them to the tree.

The 1.0.7 releases were consolidated onto one shipped installer, so the older archives no longer
exist in the working tree. The upgrade and two-step rollback paths they exercise are still worth
testing: installing over an older 1.0.7 and then rolling back is exactly what a player does when a
new build replaces a previous one. The repository already proved this pattern with the retired
1.0.6, so it is reused here: read the bytes from a pinned commit and check their SHA-256 before
use, which keeps the fixture honest without adding files back to the tree.

The shared loader lives in historical_106_fixture; it is the one verified reader for historical
installers and this module only supplies the pinned coordinates.
"""
from __future__ import annotations

from pathlib import Path

from historical_106_fixture import read_verified_blob

ROOT = Path(__file__).resolve().parents[1]

# The last commit that carried both installers in the tree, before the consolidation. Callers must
# make it available first: git fetch --no-tags --depth=1 origin <revision>
REVISION = "14adad9432ca01991c5ff91d437cab4fa017d598"

NAME_107 = "MiusMultiplayer_v1.0.7_TEST_INSTALL.zip"
SHA256_107 = "65f7a325095577cec98b2bab590f4674b81ad240bc5191adce346ebf8bec5ec6"

NAME_107_EXPERIMENTAL = "MiusMultiplayer_v1.0.7_EXPERIMENTAL_INSTALL.zip"
SHA256_107_EXPERIMENTAL = "60e7b7a2f2914ae75cf203f48aacc89e9469f71b15b654f70805d50e50736f78"


def read_107() -> bytes:
    """The released 1.0.7 installer, before the experimental line branched from it."""
    return read_verified_blob(ROOT, REVISION, NAME_107, SHA256_107)


def read_107_experimental() -> bytes:
    """The 1.0.7 EXPERIMENTAL installer that the net+fps build replaced."""
    return read_verified_blob(ROOT, REVISION, NAME_107_EXPERIMENTAL, SHA256_107_EXPERIMENTAL)

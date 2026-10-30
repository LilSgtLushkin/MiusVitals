#!/usr/bin/env python3
"""Cross-tree artifact check for MiusVitals.

`development/kit-source/src/verify_artifacts.py` owns the kit itself: it proves that every
codelib is the exact rebased v20 module with the Vitals import and that the DLL keeps its
KERNEL32-only import surface.  What no single package check can see is that the *same*
binary is copied into several kit sources, that the retained release archives are still
the reviewed ones, and that the paths named in `.gitattributes`/`.gitignore` exist.

This script checks exactly those invariants:

1. every shipped binary has a pinned SHA-256 and every copy of one artifact is byte-identical;
2. the user-supplied game reference is verified against its own tracked SHA-256/PE manifest,
   without treating those reference files as shipped installer artifacts;
3. no binary appears elsewhere under development without being listed (a new artifact cannot
   slip into an archive unverified);
4. release archives still match their pinned hashes;
5. every literal path referenced by `.gitattributes` exists, and the two `.gitignore`
   exceptions still point at tracked launchers.

It reads files only, needs no network and no game installation.
"""
from __future__ import annotations

import hashlib
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]

# role -> (expected sha256, [paths relative to the repository root])
ARTIFACTS: dict[str, tuple[str, tuple[str, ...]]] = {
    "main codelib (Steam v20 rebased for 3598/1 + Vitals import)": (
        "485b655662bd062e16278f9d699a3097b49a08de1948d2500dd589703a3102a3",
        (
            "development/kit-source/files/basic_network.codelib",
            "development/unified-kit-source/files/bin/rel/x64/basic_network.codelib",
        ),
    ),
    "alternative codelib (rebased without the Start group)": (
        "e4317e630a21198a55e0e86be4aa29c71fcada6065824c858acd0e2bdef8adf9",
        ("development/kit-source/files/alt/basic_network_nostart.codelib",),
    ),
    "alternative codelib (v20 + import only, not rebased)": (
        "d2e34581b42d0573547e221873d7538b64a413267e8ed388abeb46dd42b630d2",
        ("development/kit-source/files/alt/basic_network_v5_norebase.codelib",),
    ),
    "FPS candidate codelib (Steam-preserving, zero-ms gated)": (
        "7b42a69abe58deb1583d2f5320eebbecd9718ffa27ea08933613e67df1901f67",
        (
            "development/fps-candidate/basic_network.codelib",
            "development/unified-fps-kit-source/files/bin/rel/x64/basic_network.codelib",
        ),
    ),
    "mp_vitals.dll (Zig, KERNEL32 only, MpVitalsInit export)": (
        "71a1bf3a37923bbe6c8ee84448d0b91705c2bff7382b647f990b9d671ee224df",
        (
            "development/kit-source/files/mp_vitals.dll",
            "development/unified-kit-source/files/mp_vitals.dll",
            "development/unified-fps-kit-source/files/mp_vitals.dll",
            # Parked copies in development/forbidden: vitals work is on hold until the crashes
            # are understood, and these must stay byte-identical to the shipped file.
            "development/forbidden/development/kit-source/files/mp_vitals.dll",
            "development/forbidden/development/unified-kit-source/files/mp_vitals.dll",
            "development/forbidden/development/unified-fps-kit-source/files/mp_vitals.dll",
        ),
    ),
    "experimental mp_vitals.dll for GTMP_alpha (same surface, only the reviewed bugfixes)": (
        "808e1d5f6cb301ac971700e0aba6e78d1df1b13b02e748b8d01dd4bfc8a9881c",
        (
            "development/gtmp-alpha/files/mp_vitals.dll",
            "development/forbidden/development/gtmp-alpha/files/mp_vitals.dll",
        ),
    ),
    "mp_join.exe (public MiusPlayer launcher)": (
        "6aa417cf6e1f01480c997446f2691a58ab55b90fa36e1ff511945989aadfeeaa",
        (
            "development/unified-kit-source/files/mp_join.exe",
            "development/unified-fps-kit-source/files/mp_join.exe",
        ),
    ),
    "opgame.progpack (public MiusPlayer program pack)": (
        "eca75b740f7b11d9e13f2e2001f1b2b133427d0860fb3770220b89b0b8909535",
        (
            "development/unified-kit-source/files/root/programs/opgame.progpack",
            "development/unified-fps-kit-source/files/root/programs/opgame.progpack",
        ),
    ),
    "tacgame.progpack (public MiusPlayer program pack)": (
        "01923eabdf6a9d5fa37dc70a3d76740242dbc9a7f26c0298ca3faa8cca87600d",
        (
            "development/unified-kit-source/files/root/programs/tacgame.progpack",
            "development/unified-fps-kit-source/files/root/programs/tacgame.progpack",
        ),
    ),
}

# Immutable archives only. The shipped installer is rebuilt every release, so it is checked by
# is deliberately absent: it is the single current 1.0.7 archive and publish-installer.yml rebuilds
# and commits it from the gated source, so a pin here would be written by the same job that produces
# the file and would prove nothing. verify_release.sh checks that archive structurally and against
# the SHA-256 recorded beside it instead. The older 1.0.7 archives were removed from the tree when
# the releases were consolidated; their names stay reserved in scripts/package_net_v21_test.py.
ARCHIVES: dict[str, str] = {
}

BINARY_SUFFIXES = {".codelib", ".dll", ".exe", ".progpack"}
# Local build output, ignored by Git and by verify_workspace.py as well.
# "work" holds scratch downloaded by tools (development/v20-recovery/work/): not shipped.
SKIP_PARTS = {"build", "Multiplayer_DropIn", "__pycache__", "state", "out", "candidates", "work"}


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for block in iter(lambda: handle.read(1 << 20), b""):
            digest.update(block)
    return digest.hexdigest()


def check_game_reference(root: Path, errors: list[str]) -> set[Path]:
    """Verify the separately tracked game reference before recognising its top-level binaries.

    The manifest covers all 19 files, including the two .reslib files, sizes and PE metadata.
    Do not exempt an entire directory: an unexpected nested binary must still be reported by the
    development-tree scan, and an added or changed top-level file must fail the manifest check.
    """
    reference = root / "development/game-build-3598-1"
    verifier = reference / "manifest.py"
    if not verifier.is_file() or not (reference / "MANIFEST.txt").is_file() or not (reference / "x64").is_dir():
        errors.append("game reference verifier, manifest or x64 directory is missing")
        return set()
    try:
        result = subprocess.run([sys.executable, str(verifier), "--verify"], capture_output=True,
                                text=True, check=False, timeout=120)
    except (OSError, subprocess.TimeoutExpired) as error:
        errors.append(f"game reference verification failed to run: {error}")
        return set()
    if result.returncode:
        detail = (result.stdout + result.stderr).strip()
        errors.append(f"game reference manifest verification failed: {detail[:2000]}")
        return set()
    print(result.stdout.strip())
    return {path for path in (reference / "x64").iterdir()
            if path.is_file() and path.suffix.lower() in BINARY_SUFFIXES}


def check_artifacts(errors: list[str]) -> None:
    listed: set[Path] = check_game_reference(ROOT, errors)
    for role, (expected, relatives) in ARTIFACTS.items():
        for relative in relatives:
            path = ROOT / relative
            listed.add(path)
            if not path.is_file():
                errors.append(f"missing artifact: {relative} ({role})")
                continue
            actual = sha256(path)
            if actual != expected:
                errors.append(f"{relative}: sha256 {actual}, expected {expected} ({role})")
    for path in sorted((ROOT / "development").rglob("*")):
        if not path.is_file() or path.suffix not in BINARY_SUFFIXES:
            continue
        if SKIP_PARTS & set(path.relative_to(ROOT).parts):
            continue
        if path not in listed:
            errors.append(f"binary not listed in scripts/check_artifacts.py: {path.relative_to(ROOT).as_posix()}")
    print(f"OK {sum(len(v[1]) for v in ARTIFACTS.values())} shipped binaries, "
          f"{len(ARTIFACTS)} roles, every copy byte-identical")


def check_archives(errors: list[str]) -> None:
    for name, expected in ARCHIVES.items():
        path = ROOT / name
        if not path.is_file():
            errors.append(f"missing release archive: {name}")
            continue
        actual = sha256(path)
        if actual != expected:
            errors.append(f"{name}: sha256 {actual}, expected {expected}")
    print(f"OK {len(ARCHIVES)} release archives")


def check_fps_candidate_guard(errors: list[str]) -> None:
    """The FPS candidate must come from the reviewed transformation only.

    `package_unified_fps_mod.py --verify` already compares bytes, but nothing proved that the
    guard itself can fail: a plain copy of the stable module used to be indistinguishable from
    a real candidate to a reader of the logs.  Here the builder must accept the checked-in
    candidate and reject the stable module handed in as if it were the candidate.
    """
    sys.path.insert(0, str(Path(__file__).resolve().parent))
    try:
        import build_steam_compatible_fps_candidate as builder
    except Exception as error:  # noqa: BLE001 - any import failure is a broken workbench
        errors.append(f"cannot import the FPS candidate builder: {error}")
        return
    stable = (ROOT / "development/kit-source/files/basic_network.codelib").read_bytes()
    candidate = (ROOT / "development/fps-candidate/basic_network.codelib").read_bytes()
    try:
        builder.verify_candidate(candidate, stable)
    except Exception as error:  # noqa: BLE001
        errors.append(f"FPS candidate is not the reviewed transformation: {error}")
        return
    try:
        builder.verify_candidate(stable, stable)
    except ValueError:
        pass
    else:
        errors.append("FPS candidate builder accepts a plain binary substitution")
    print("OK FPS candidate: reproducible from the stable module, plain substitution rejected")


def check_config_paths(errors: list[str]) -> None:
    """Every literal path in .gitattributes must exist; a rule for a renamed file is dead."""
    attributes = ROOT / ".gitattributes"
    if attributes.is_file():
        for number, raw in enumerate(attributes.read_text(encoding="utf-8").splitlines(), 1):
            line = raw.split("#", 1)[0].strip()
            if not line or line.startswith("!"):
                continue
            pattern = line.split()[0].lstrip("/")
            if pattern.startswith("[attr]") or any(ch in pattern for ch in "*?["):
                continue
            if not (ROOT / pattern).exists():
                errors.append(f".gitattributes:{number}: rule names a missing path: {pattern}")
    ignored_exceptions = (
        "development/unified-kit-source/files/mp_join.exe",
        "development/unified-fps-kit-source/files/mp_join.exe",
    )
    for relative in ignored_exceptions:
        if not (ROOT / relative).is_file():
            errors.append(f".gitignore exception names a missing file: {relative}")
    print("OK .gitattributes/.gitignore paths resolve")


def main() -> int:
    errors: list[str] = []
    check_artifacts(errors)
    check_archives(errors)
    check_fps_candidate_guard(errors)
    check_config_paths(errors)
    if errors:
        print("\nartifact check failed:", file=sys.stderr)
        for error in errors:
            print(f"  {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

#!/usr/bin/env python3
"""Verify the pinned public MiusPlayer source and its deliberate MiusVitals boundary.

This is a source-workbench check.  It does not claim that compiling the public
C++ tree reproduces the player-installable binary; UPSTREAM.json records why.
"""
from __future__ import annotations

import hashlib
import json
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[2]
UPSTREAM = ROOT / "development/upstream/miusplayer-public-43b2736"
MANIFEST = UPSTREAM / "SOURCE_MANIFEST.sha256"
VITALS = ROOT / "development/kit-source/src/mp_vitals.c"
SESSION = UPSTREAM / "basic_network/session.cpp"
TRANSFORM_PATCH = ROOT / "development/merged-source/patches/0001-elide-idempotent-remote-transform-writes.patch"
OPERATION_PATCH = ROOT / "development/merged-source/patches/0002-ordered-operation-turns-and-network-hygiene.patch"


def sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def source_files() -> set[str]:
    generated = {"basic_network/build", "Multiplayer_DropIn"}
    out: set[str] = set()
    for path in UPSTREAM.rglob("*"):
        relative = path.relative_to(UPSTREAM).as_posix()
        if not path.is_file() or relative == MANIFEST.name:
            continue
        if any(relative == prefix or relative.startswith(prefix + "/") for prefix in generated):
            continue
        out.add(relative)
    return out


def verify_source_manifest() -> None:
    errors: list[str] = []
    expected: dict[str, str] = {}
    for raw in MANIFEST.read_text(encoding="utf-8").splitlines():
        if not raw or raw.startswith("#"):
            continue
        digest, relative = raw.split("  ", 1)
        expected[relative] = digest
        path = UPSTREAM / relative
        if not path.is_file():
            errors.append(f"missing {relative}")
        elif sha256(path) != digest:
            errors.append(f"hash differs {relative}")
    unexpected = source_files() - expected.keys()
    missing = expected.keys() - source_files()
    errors.extend(f"unexpected {path}" for path in sorted(unexpected))
    errors.extend(f"not listed {path}" for path in sorted(missing))
    if errors:
        raise SystemExit("public-source manifest failed:\n  " + "\n  ".join(errors))


def require(text: str, expected: str, name: str) -> None:
    if expected not in text:
        raise SystemExit(f"source-workbench invariant missing: {name}")


def verify_transform_candidate() -> None:
    patch = TRANSFORM_PATCH.read_text(encoding="utf-8")
    for marker in (
        "session_sim_step(o->objman, ms)",
        "void session_sim_step(ObjMan *om, u32 sim_ms)",
        "if (!sim_ms) return;",
        "static bool held_at_rest",
        "static bool transform_is_already_set",
        "already-settled writes elided",
    ):
        require(patch, marker, f"transform candidate: {marker}")
    # Keep the candidate a computation-elision patch, not a hidden sync/rate/map policy change.
    for forbidden in ("MP_STATE_KBPS", "MP_BG_FPS", "MP_FG_FPS", "g_last_snapshot", "MP_SYNC_STATE", "MP_HOST_MODE", "g_map_"):
        if forbidden in patch:
            raise SystemExit(f"transform candidate must not change {forbidden}")


def verify_ordered_operation_candidate() -> None:
    patch = OPERATION_PATCH.read_text(encoding="utf-8")
    for marker in (
        "operation_turn.h",
        "MAP_TURN_HOST",
        "MAP_TURN_CLIENT",
        "MAP_TURN_COLLECT",
        "g_map_handoff_pending",
        "publish_host_map",
        "begin_map_download",
        "client-issued orders (including attacks) are merged",
        "queue_remote_transform",
        "g_stat_coalesced",
    ):
        require(patch, marker, f"ordered-operation candidate: {marker}")
    # This is a source overlay, not a silent change to the existing runtime packages or transport policy.
    for forbidden in ("MiusMultiplayer_v1.0.0_INSTALL.zip", "MiusMultiplayer_beta_FPS_fix_INSTALL.zip", "MP_STATE_KBPS", "MP_BG_FPS", "MP_FG_FPS", "k_version = 13"):
        if forbidden in patch:
            raise SystemExit(f"ordered-operation candidate must not change {forbidden}")


def main() -> None:
    metadata = json.loads((UPSTREAM / "UPSTREAM.json").read_text(encoding="utf-8"))
    if metadata["published_release"]["basic_network_sha256"] != "8108bd1e9af8080ef8b1050512b8311848f5a336acb49a7479388733b4ae5723":
        raise SystemExit("public-source metadata no longer points at the checked v20 release input")
    if metadata.get("target_game", {}).get("steam_version") != "6.00 build 3598/1":
        raise SystemExit("supported game target must remain Steam 6.00 build 3598/1")
    verify_source_manifest()
    verify_transform_candidate()
    verify_ordered_operation_candidate()

    # This makes the causal upstream assumption visible before anyone ports Vitals into C++.
    session = SESSION.read_text(encoding="utf-8")
    require(session, 'GetEnvironmentVariableA("MP_SYNC_STATE", v, sizeof v)', "MP_SYNC_STATE reader")
    require(session, "return forced >= 0 ? forced != 0 : g_host_mode != HOST_MODE_OPERATION;", "operation position-only default")

    vitals = VITALS.read_text(encoding="utf-8")
    require(vitals, "static int operation_tactical_command", "strict tactical-operation classifier")
    require(vitals, 'SetEnvironmentVariableA("MP_SYNC_STATE", "0")', "absent-only operation default")
    require(vitals, "tactical operation detected; full object-state replication is disabled", "runtime evidence")

    print("OK public source", metadata["revision"])
    print("OK published v20 input", metadata["published_release"]["basic_network_sha256"])
    print("OK upstream operation default + MiusVitals sidecar policy boundary")
    print("OK source-only exact remote-transform write-elision candidate")
    print("OK source-only ordered-operation and network-hygiene candidate")


if __name__ == "__main__":
    main()

#!/usr/bin/env python3
"""Static and model checks for the source-only remote-transform optimization.

The installable codelib is intentionally not rebuilt by this check.  This test
keeps the proposed source patch narrow until a Windows build and a two-peer game
run can certify a new binary.
"""
from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[3]
PATCH = ROOT / "development/merged-source/patches/0001-elide-idempotent-remote-transform-writes.patch"


@dataclass(frozen=True)
class Record:
    pos: tuple[float, float, float]
    rot: tuple[float, ...]
    lin: tuple[float, float, float]
    ang: tuple[float, float, float]


def can_elide(previous: Record, current: Record, local: Record) -> bool:
    """The candidate's safety rule: skip only an exact, settled no-op write."""
    zero = (0.0, 0.0, 0.0)
    return (
        previous.pos == current.pos
        and previous.rot == current.rot
        and previous.lin == zero
        and current.lin == zero
        and previous.ang == zero
        and current.ang == zero
        and local.pos == current.pos
        and local.rot == current.rot
        and local.lin == zero
        and local.ang == zero
    )


def expect(value: bool, message: str) -> None:
    if not value:
        raise SystemExit(message)


def main() -> None:
    text = PATCH.read_text(encoding="utf-8")
    # It must apply cleanly to the untouched, hash-pinned public source snapshot.
    subprocess.run(["git", "apply", "--check", str(PATCH)], cwd=ROOT, check=True)

    for marker in (
        "session_sim_step(o->objman, ms)",
        "void session_sim_step(ObjMan *om, u32 sim_ms)",
        "if (!sim_ms) return;",
        "static bool held_at_rest",
        "static bool transform_is_already_set",
        "already-settled writes elided",
    ):
        expect(marker in text, f"candidate transform patch lost required marker: {marker}")

    # The change must not turn into an FPS cap, transport/snapshot-rate throttle,
    # state-sync toggle, or an operational-map change.
    for forbidden in (
        "MP_STATE_KBPS",
        "MP_BG_FPS",
        "MP_FG_FPS",
        "g_last_snapshot",
        "MP_SYNC_STATE",
        "MP_HOST_MODE",
        "g_map_",
    ):
        expect(forbidden not in text, f"candidate patch must not change {forbidden}")

    stationary = Record((1.0, 2.0, 3.0), tuple(float(i) for i in range(9)), (0.0, 0.0, 0.0), (0.0, 0.0, 0.0))
    moved = Record((1.01, 2.0, 3.0), stationary.rot, stationary.lin, stationary.ang)
    rotating = Record(stationary.pos, (9.0,) + stationary.rot[1:], stationary.lin, stationary.ang)
    rolling = Record(stationary.pos, stationary.rot, (0.1, 0.0, 0.0), stationary.ang)
    local_drift = Record((1.0, 2.0, 3.001), stationary.rot, stationary.lin, stationary.ang)

    expect(can_elide(stationary, stationary, stationary), "exact settled transform should be elided")
    expect(not can_elide(stationary, moved, stationary), "a position change must still be written")
    expect(not can_elide(stationary, rotating, stationary), "a rotation must still be written")
    expect(not can_elide(stationary, rolling, stationary), "a nonzero velocity must still be written")
    expect(not can_elide(stationary, stationary, local_drift), "local physics drift must still be corrected")
    print("OK transform patch applies to pinned public source")
    print("OK only exact settled no-op transform writes are elided")
    print("OK no transport throttle, state toggle, or map change in candidate")


if __name__ == "__main__":
    main()

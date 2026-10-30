#!/usr/bin/env python3
"""Self-test for battle_report.py: build logs by hand, then check what the report says about them.

The report is what a two-PC run is read with, so it has to be right about the cases that actually
happen: the same build on both machines and a lost launch, two different builds, a log that was cut
short, and a single log with nothing to compare against.

    python development/versions/<версия>/tools/battle_report_selftest.py
"""
from __future__ import annotations

import argparse
import subprocess
import sys
import tempfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
REPORT = HERE / "battle_report.py"

CHECKS = 0
FAILURES = 0


def check(condition: bool, what: str) -> None:
    global CHECKS, FAILURES
    CHECKS += 1
    if not condition:
        FAILURES += 1
    print(f"{'OK  ' if condition else 'FAIL'} {what}")


def line(at: float, channel: str, text: str) -> str:
    return f"21:00:{int(at) % 60:02d}.000 [t01234 f000100 +{at:6.1f}s] {channel:<7} {text}\n"


def log(image: str, shots_sent: list[int], shots_in: list[int], rejected: int = 0,
        cut: bool = False) -> str:
    text = (line(0, "boot", "=== net_v21: our own network module, v1.0.2-test ===")
            + line(0, "boot", "build: compiled Oct 26 2026, MSVC 1944")
            + line(0, "boot", f"image: 250000 bytes, fnv1a64 {image}, pe stamp 1234abcd")
            + line(0, "boot", "flags: MP_STEAM=1 MP_STEAM_FAKE=<unset>"))
    for i, at in enumerate(range(5, 5 * len(shots_sent) + 5, 5)):
        text += line(at, "summary", f"{(i + 1) * 50} frames, rx {(i + 1) * 9}/900 B, "
                                    f"tx {(i + 1) * 9}/900 B, phase 2, sim {at:.1f} s, 1 peer(s), "
                                    f"sides 0x52535355")
        # the module writes cumulative totals, not per-interval counts
        text += line(at, "summary", f"battle: shots_sent={shots_sent[i]} shots_in={shots_in[i]} "
                                    f"state_loaded={i * 3} state_rejected={rejected * (i + 1) if rejected else 0}")
    if cut:
        text += "21:01:0"          # a log that stops mid-line, the way a crash leaves it
    return text


def run(*logs: Path) -> str:
    result = subprocess.run([sys.executable, str(REPORT), *map(str, logs)],
                            capture_output=True, text=True)
    if result.returncode != 0:
        print(result.stdout)
        print(result.stderr)
    return result.stdout


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.parse_args()
    with tempfile.TemporaryDirectory() as temp:
        folder = Path(temp)
        ours = folder / "net_v21_tacgame.log"
        theirs = folder / "net_v21_tacgame_2.log"

        ours.write_text(log("aaaaaaaaaaaaaaaa", [4, 8, 12, 16], [2, 6, 10, 14]))
        text = run(ours)
        check("1111111111111111" not in text and "aaaaaaaaaaaaaaaa" in text,
              "a single log reports the image hash it ran from")
        check("event records transmitted (including retries): **16**" in text, "and accurately labels transmissions")
        check("nothing to compare them with" in text, "and says there is nothing to compare against")

        theirs.write_text(log("aaaaaaaaaaaaaaaa", [2, 6, 10, 14], [4, 8, 12, 16]))
        text = run(ours, theirs)
        check("| our event records | 16 | 16 | counts match |" in text,
              "two logs can have matching event counts")
        check("0 direction(s) have different counts" in text, "and report matching counts without inferring delivery")
        check("different builds" not in text, "the same build on both machines is not flagged")

        theirs.write_text(log("bbbbbbbbbbbbbbbb", [2, 6, 10, 14], [4, 8, 12, 16]))
        text = run(ours, theirs)
        check("different builds" in text and "bbbbbbbbbbbbbbbb" in text,
              "two different builds are called out loudly, with both hashes")

        # Different counts may be retransmissions or different log windows, not necessarily loss.
        ours.write_text(log("aaaaaaaaaaaaaaaa", [4, 8, 12, 16], [2, 6, 10, 12]))
        theirs.write_text(log("aaaaaaaaaaaaaaaa", [2, 6, 10, 12], [4, 8, 12, 14]))
        text = run(ours, theirs)
        check("| our event records | 16 | 14 | **2 fewer at peer** |" in text,
              "a counter difference is reported without diagnosing packet loss")
        check("1 direction(s) have different counts" in text and "do not prove packet loss" in text,
              "and the report explains why a mismatch is not proof of loss")

        legacy = log("aaaaaaaaaaaaaaaa", [1], [1]).replace("state_loaded=0", "trench_sent=37 state_loaded=0")
        ours.write_text(legacy)
        text = run(ours)
        check("**37** / **unknown (legacy log)**" in text and "dug locally" in text,
              "legacy trench_sent is local digging, not proof of sending")
        text = run(ours, theirs)
        check("Legacy `trench_sent`" in text and "our trenches queued / applied" not in text,
              "legacy trench totals are never compared as network traffic")
        modern = legacy.replace("trench_sent=37", "trench_dug=37 trench_queued=3")
        ours.write_text(modern)
        text = run(ours)
        check("**37** / **3**" in text, "modern counters separate local digging and queueing")

        theirs.write_text(log("aaaaaaaaaaaaaaaa", [2, 6, 10, 14], [4, 8, 12, 16], rejected=1))
        text = run(ours, theirs)
        check("object states rejected (both games) | 4" in text,
              "a rejected object state is counted across both games")

        cut = folder / "net_v21_tacgame_cut.log"
        cut.write_text(log("aaaaaaaaaaaaaaaa", [4], [2], cut=True))
        text = run(cut)
        check("did not parse and were left out" in text,
              "a log that was cut short says how many lines it could not use")

    print(f"{CHECKS} checks, {FAILURES} failures")
    return 1 if FAILURES else 0


if __name__ == "__main__":
    sys.exit(main())

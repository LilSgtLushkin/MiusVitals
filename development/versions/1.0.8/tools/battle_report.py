#!/usr/bin/env python3
"""Read the run logs of a two-PC battle and say where the two games stopped agreeing.

A log is written by net_v21 (see development/net-v21/src/netlog.h): a passport at the top, the
module's own lines mirrored in, a summary every five seconds, and battle milestones.  One log tells
what this game did.  Two logs, laid side by side, tell the thing a bug report is actually about:
where one game sent something the other never received, or received something it threw away.

    python tools/battle_report.py out/net_v21_tacgame.log
    python tools/battle_report.py ours.log theirs.log --out battle.md

What it reports:

* the passport of each log - version, image hash, PE stamp - and a loud warning when the two games
  ran different builds, because every other number in the report is meaningless then;
* the connection: who joined whom, with which sides;
* a timeline of the counters that matter (shots sent/received, object states loaded/rejected,
  trenches), every five seconds, side by side where they should agree;
* differences between observed counters, with caveats: retransmissions, distinct counting points and
  unequal log windows mean those differences do not by themselves prove packet loss.

Nothing here is guessed: if a line does not parse it is counted as unparsed and left out, so a report
built from a half-written log says so instead of inventing a battle.
"""
from __future__ import annotations

import argparse
import re
import sys
from dataclasses import dataclass, field
from pathlib import Path

LINE = re.compile(
    r"^(?P<clock>\d\d:\d\d:\d\d\.\d\d\d) \[t(?P<thread>\d+) f(?P<frame>\d+) \+ *(?P<at>[\d.]+)s\] "
    r"(?P<channel>\S+) *(?P<text>.*)$")
SUMMARY = re.compile(r"^(?P<frames>\d+) frames, rx (?P<rx>\d+)/(?P<rxb>\d+) B, tx (?P<tx>\d+)/(?P<txb>\d+) B"
                     r"(?:, phase (?P<phase>\d+), sim (?P<sim>[\d.]+) s, (?P<peers>\d+) peer\(s\), sides 0x(?P<sides>[0-9a-f]+))?$")
BATTLE = re.compile(r"^(?P<name>[a-z_]+)=(?P<value>-?\d+)")


@dataclass
class Sample:
    at: float
    frames: int
    rx: int
    rx_bytes: int
    tx: int
    tx_bytes: int
    phase: int | None = None
    sim: float | None = None
    peers: int | None = None
    sides: int | None = None
    counters: dict[str, int] = field(default_factory=dict)


@dataclass
class Log:
    path: Path
    passport: dict[str, str] = field(default_factory=dict)
    samples: list[Sample] = field(default_factory=list)
    marks: list[tuple[float, str]] = field(default_factory=list)
    notes: list[tuple[float, str]] = field(default_factory=list)
    unparsed: int = 0
    first_at: float | None = None
    last_at: float = 0.0

    @property
    def label(self) -> str:
        return self.path.name

    @property
    def program(self) -> str:
        return "tacgame" if "tacgame" in self.path.name else "opgame"

    def at(self, seconds: float) -> Sample | None:
        """The sample at or before `seconds` - the state of that game at that moment."""
        found = None
        for sample in self.samples:
            if sample.at <= seconds:
                found = sample
            else:
                break
        return found


def read(path: Path) -> Log:
    log = Log(path=path)
    if not path.is_file():
        raise SystemExit(f"no such log: {path}")
    for line in path.read_text(errors="replace").splitlines():
        match = LINE.match(line)
        if not match:
            log.unparsed += 1
            continue
        at = float(match.group("at"))
        channel, text = match.group("channel"), match.group("text")
        log.last_at = max(log.last_at, at)
        if log.first_at is None:
            log.first_at = at
        if channel == "boot":
            for key, value in (("build", r"^build: (.*)$"), ("image", r"fnv1a64 ([0-9a-f]{16})"),
                               ("flags", r"^flags: (.*)$"), ("program", r"^process: .*, program (\S+)$")):
                found = re.search(value, text)
                if found:
                    log.passport[key] = found.group(1)
        elif channel == "summary":
            if text.startswith("types:"):
                continue
            if text.startswith("battle:"):
                if log.samples:
                    for item in text[len("battle:"):].split():
                        name, _, value = item.partition("=")
                        try:
                            log.samples[-1].counters[name] = int(value)
                        except ValueError:
                            pass
                continue
            found = SUMMARY.match(text)
            if found:
                log.samples.append(Sample(
                    at=at, frames=int(found.group("frames")), rx=int(found.group("rx")),
                    rx_bytes=int(found.group("rxb")), tx=int(found.group("tx")),
                    tx_bytes=int(found.group("txb")),
                    phase=int(found.group("phase")) if found.group("phase") else None,
                    sim=float(found.group("sim")) if found.group("sim") else None,
                    peers=int(found.group("peers")) if found.group("peers") else None,
                    sides=int(found.group("sides"), 16) if found.group("sides") else None))
            else:
                log.unparsed += 1
        elif channel == "battle":
            log.marks.append((at, text))
        elif channel in ("module", "note"):
            log.notes.append((at, text))
    return log


def totals(log: Log) -> dict[str, int]:
    """The last value of every counter: the numbers a battle ends with."""
    merged: dict[str, int] = {}
    for sample in log.samples:
        merged.update(sample.counters)
    return merged


def first_phase(log: Log, phase: int) -> float | None:
    for sample in log.samples:
        if sample.phase == phase:
            return sample.at
    return None


def side_by_side(ours: Log, theirs: Log | None, out: list[str]) -> None:
    out += ["## The battle, every five seconds", ""]
    if theirs is None:
        out += ["| at | phase | sim | peers | network ticks (not FPS) | rx/tx packets | counters |",
                "|---|---|---|---|---|---|---|"]
        for sample in ours.samples:
            counters = " ".join(f"{k}={v}" for k, v in sorted(sample.counters.items()))
            out.append(f"| {sample.at:.1f} s | {sample.phase if sample.phase is not None else '-'} | "
                       f"{sample.sim if sample.sim is not None else '-'} | "
                       f"{sample.peers if sample.peers is not None else '-'} | {sample.frames} | "
                       f"{sample.rx}/{sample.tx} | {counters} |")
        out.append("")
        return

    keys = sorted(set(totals(ours)) | set(totals(theirs)))
    out += ["| at | ours: sim / rx / tx | theirs: sim / rx / tx | counters (ours / theirs) |",
            "|---|---|---|---|"]
    step = 5.0
    at = 0.0
    while at <= max(ours.last_at, theirs.last_at):
        mine, other = ours.at(at), theirs.at(at)
        if mine is None and other is None:
            at += step
            continue
        def cell(sample: Sample | None) -> str:
            if sample is None:
                return "-"
            return (f"{sample.sim if sample.sim is not None else '-'} s / {sample.rx} / {sample.tx}")
        pairs = []
        for key in keys:
            a = mine.counters.get(key) if mine else None
            b = other.counters.get(key) if other else None
            if a is None and b is None:
                continue
            pairs.append(f"{key} {a if a is not None else '-'}/{b if b is not None else '-'}")
        out.append(f"| {at:.0f} s | {cell(mine)} | {cell(other)} | {'; '.join(pairs)} |")
        at += step
    out.append("")


def divergences(ours: Log, theirs: Log | None, out: list[str]) -> None:
    out += ["## Divergences", ""]
    mine = totals(ours)
    if theirs is None:
        out += ["One log: the numbers below are this game's own counts, nothing to compare them with.",
                "",
                f"* event records transmitted (including retries): **{mine.get('shots_sent', 0)}**",
                f"* new event records accepted from peer: **{mine.get('shots_in', 0)}**",
                f"* object states loaded / rejected: **{mine.get('state_loaded', 0)}** / "
                f"**{mine.get('state_rejected', 0)}**",
                f"* trenches dug locally / queued for the peer / applied from the peer: "
                f"**{mine.get('trench_dug', mine.get('trench_sent', 0))}** / "
                f"**{mine.get('trench_queued', 0) if 'trench_sent' not in mine else 'unknown (legacy log)'}** / "
                f"**{mine.get('trench_applied', 0)}**",
                f"* Steam datagrams sent / received: **{mine.get('steam_sent', 0)}** / "
                f"**{mine.get('steam_received', 0)}**",
                ""]
        return

    other = totals(theirs)
    problems = 0
    # These are observations at different counting points, not an end-to-end loss measurement.
    # shots_sent includes retransmissions; shots_in counts newly accepted records. Legacy
    # trench_sent counted local digging even with no peer and must never be presented as traffic.
    rows = [
        ("our event records", mine.get("shots_sent", 0), other.get("shots_in", 0)),
        ("their event records", other.get("shots_sent", 0), mine.get("shots_in", 0)),
        ("our Steam datagrams", mine.get("steam_sent", 0), other.get("steam_received", 0)),
        ("their Steam datagrams", other.get("steam_sent", 0), mine.get("steam_received", 0)),
    ]
    if "trench_sent" not in mine and "trench_sent" not in other:
        rows.extend([
            ("our trenches queued / applied", mine.get("trench_queued", 0), other.get("trench_applied", 0)),
            ("their trenches queued / applied", other.get("trench_queued", 0), mine.get("trench_applied", 0)),
        ])
    else:
        out += ["Legacy `trench_sent` counts local digging, including with no peer; "
                "it cannot be compared as network traffic.", ""]
    out += ["| direction | sender count | peer count | difference |", "|---|---|---|---|"]
    for what, sent, received in rows:
        if not sent and not received:
            continue
        delta = sent - received
        if delta > 0:
            problems += 1
            out.append(f"| {what} | {sent} | {received} | **{delta} fewer at peer** |")
        elif delta < 0:
            problems += 1
            out.append(f"| {what} | {sent} | {received} | **{-delta} more at peer** |")
        else:
            out.append(f"| {what} | {sent} | {received} | counts match |")
    rejected = mine.get("state_rejected", 0) + other.get("state_rejected", 0)
    out.append(f"| object states rejected (both games) | {rejected} | - | "
               f"{'nothing was thrown away' if not rejected else '**see the state lines**'} |")
    out.append("")

    out += [f"{problems} direction(s) have different counts.", "",
            "Counts alone do not prove packet loss: event transmissions include retries, receivers "
            "count new records, trenches are counted when queued versus applied, and logs may cover "
            "different windows or stop before a summary. Inspect timestamps, ACKs and queue state.", ""]


def build_warning(ours: Log, theirs: Log | None, out: list[str]) -> None:
    if theirs is None:
        return
    fields = [("image", "image hash"), ("build", "build stamp")]
    differ = [(key, label) for key, label in fields
              if ours.passport.get(key) and theirs.passport.get(key)
              and ours.passport[key] != theirs.passport[key]]
    if differ:
        out += ["## The two games ran different builds", ""]
        for key, label in differ:
            out.append(f"* {label}: `{ours.passport.get(key, '?')}` vs `{theirs.passport.get(key, '?')}`")
        out += ["",
                "Every number below compares two different programs.  Re-run with the same archive on "
                "both machines before drawing conclusions from it.", ""]


def report(logs: list[Log]) -> str:
    ours = logs[0]
    theirs = logs[1] if len(logs) > 1 else None
    out = [f"# Battle report: {ours.label}" + (f" and {theirs.label}" if theirs else ""), ""]
    out += ["## Passport", "", "| | " + ours.label + (" | " + theirs.label if theirs else "") + " |",
            "|---|---|---|" if theirs else "|---|---|"]
    for key in ("build", "image", "flags"):
        row = f"| {key} | {ours.passport.get(key, 'not in the log')}"
        if theirs:
            row += f" | {theirs.passport.get(key, 'not in the log')}"
        out.append(row + " |")
    out.append("")
    build_warning(ours, theirs, out)

    out += ["## How it started", ""]
    for log in logs:
        joined = [text for _, text in log.marks]
        out.append(f"* {log.label}: {len(joined)} battle line(s)" +
                   ("" if not joined else ": " + "; ".join(joined[:4])))
    out.append("")

    side_by_side(ours, theirs, out)
    divergences(ours, theirs, out)

    unparsed = sum(log.unparsed for log in logs)
    if unparsed:
        out += [f"{unparsed} line(s) did not parse and were left out - a log that is still being written "
                f"or one that was cut short looks like this.", ""]
    return "\n".join(out)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("logs", nargs="+", type=Path,
                        help="one log, or two: ours first, the other game's second")
    parser.add_argument("--out", type=Path, help="write the report here instead of stdout")
    args = parser.parse_args()
    logs = [read(path) for path in args.logs[:2]]
    text = report(logs)
    if args.out:
        args.out.write_text(text)
        print(f"wrote {args.out}")
    else:
        print(text)
    return 0


if __name__ == "__main__":
    sys.exit(main())

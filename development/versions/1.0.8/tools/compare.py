#!/usr/bin/env python3
"""Compare two builds of the network module function by function.

M1 asks "does our rebuild of the published tree reproduce the released v12 module?" and M2 asks
"is our Steam layer the same code as v20's?".  Both are the same measurement: match every function
of the candidate to the closest function of the reference by structure (opcode profile, then
instruction-level similarity, strings as a tie-breaker) and report what matched and what did not.

The reference is whichever binary is authoritative for the question:

    --reference v12   the module built from the published source (tools/.../v12.codelib)
    --reference v20   the module players actually run (GTMPRelease1.zip)

Names make the report readable.  A DLL carries no symbol table, so the candidate gets its names from
the linker map (`/MAP`); the reference gets them from the v20 recovery's known-name table.

    python development/net-v21/tools/compare.py \
        --candidate build/basic_network.codelib --candidate-map build/basic_network.map \
        --reference development/v20-recovery/work/v12.codelib --label M1 --out build/compare-m1.md
"""
from __future__ import annotations

import argparse
import json
import re
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[3]
RECOVERY = ROOT / "development" / "v20-recovery"
sys.path.insert(0, str(RECOVERY / "tools"))

import v20tool  # noqa: E402  (the PE reader, tokenizer and matcher already used for the v20 work)
import rules  # noqa: E402  (milestone rules: the same ones the v20 backlog is filed under)


# The verified v20 names (docs/NETWORK_LIMITATIONS.md, docs/V20_*.md).  Kept here rather than
# imported so that this tool does not depend on the recovery pipeline's knobs.
KNOWN_V20 = {
    0x16230: "on_packet", 0x19F88: "session_frame", 0x1C544: "session_sim_step",
    0x19A98: "send_peer_packet", 0x19AC8: "send_datagram", 0x17CF0: "owns_object",
    0x27DC: "mp_log", 0x3F75C: "steam_init", 0x3E8BC: "fake_steam_open",
    0x3EB34: "steam_bind_interfaces", 0x402B0: "security_cookie_check",
}


def image_base(path: Path) -> int:
    """Preferred load address, read here so the shared PE reader stays untouched."""
    import struct
    data = path.read_bytes()
    pe = struct.unpack_from("<I", data, 0x3C)[0]
    return struct.unpack_from("<Q", data, pe + 24 + 24)[0]


def parse_map(path: Path, base: int, image_size: int) -> dict[int, str]:
    """RVA -> symbol from a MSVC linker map.

    The map prints "Rva+Base" (preferred base + RVA) and, at the top, the preferred load address;
    subtracting one from the other gives the RVA the PE reader works in.
    """
    text = path.read_text(errors="ignore")
    start = re.search(r"Preferred load address is ([0-9a-fA-F]+)", text)
    base = int(start.group(1), 16) if start else base
    names: dict[int, str] = {}
    for line in text.splitlines():
        match = re.match(r"\s+[0-9a-f]{4}:[0-9a-f]{8}\s+(\S+)\s+([0-9a-f]{16})", line)
        if not match:
            continue
        symbol, absolute = match.group(1), int(match.group(2), 16)
        rva = absolute - base
        if 0 < rva < image_size:
            names.setdefault(rva, symbol.split("@@")[0].lstrip("?"))
    return names


def reference_names(reference: str, image: v20tool.Image) -> dict[int, str]:
    """Names for the reference image: the v12 symbol table, or the v20 verified names."""
    if reference == "v20":
        return dict(KNOWN_V20)
    names = v20tool.load_names() if hasattr(v20tool, "load_names") else {}
    classified = RECOVERY / "work" / "v20-classified.json"
    if not names and classified.is_file():
        for row in json.loads(classified.read_text(encoding="utf-8")):
            label = row.get("name") or row.get("ref")
            if label:
                names[row["rva"]] = label.split("@@")[0].lstrip("?")
    return names


def backlog_rows(path: Path) -> list[dict]:
    """The v20 work items: every function the classification did not call SAME.

    Same file the backlog report is generated from, so the coverage table and
    `v20-backlog.md` cannot drift apart.
    """
    import csv
    if not path.is_file():
        return []
    rows = []
    for row in csv.DictReader(path.open(newline="", encoding="utf-8")):
        if row["status"] == "SAME":
            continue
        rows.append({"rva": int(row["rva"], 16), "size": int(row["size"]),
                     "status": row["status"], "v12_name": row["v12_name"],
                     "strings": row["strings"]})
    return rows


def coverage(rows: list[dict], backlog: list[dict], threshold: float = 0.55) -> dict:
    """Which v20 work items does this build already account for?

    Two rules, because there are two ways code arrives here:

    * the build matches the v20 function at or above the threshold the matcher uses for CHANGED -
      this covers the ported code;
    * the build has a function carrying the same name - this covers our own rewrites, which
      implement the same behaviour and are never going to score high against v20's instructions.
    """
    covered: set[int] = set()
    for row in rows:
        target = row.get("ref_rva")
        if target is None:
            continue
        if row["similarity"] >= threshold or row["status"] in ("SAME", "CHANGED"):
            covered.add(target)
    ours = {(row.get("own") or "").lower() for row in rows if row.get("own")}
    for item in backlog:
        name = item["v12_name"].split("@")[0].lstrip("?").lower()
        if name and name in ours:
            covered.add(item["rva"])
    # The backlog's name column is a structural hint (v20 and v12 were built by different
    # compilers, so a 0.3 match can point at the wrong function).  For the handful of functions
    # whose name was proven by byte-level work, the name is a fact - session_frame is in this
    # build under that name even though the matcher only scores its body 0.30 against v20's.
    for rva, name in rules.VERIFIED.items():
        if name.lower() in ours:
            covered.add(rva)
    return {"covered": covered}


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--candidate", type=Path, required=True)
    parser.add_argument("--candidate-map", type=Path)
    parser.add_argument("--reference", type=Path, required=True)
    parser.add_argument("--reference-kind", choices=("v12", "v20"), default="v12")
    parser.add_argument("--label", default="compare")
    parser.add_argument("--out", type=Path)
    parser.add_argument("--json", type=Path)
    parser.add_argument("--backlog", type=Path,
                        default=RECOVERY / "reports" / "v20-functions.csv",
                        help="the v20 classification; coverage is filed by its milestones")
    parser.add_argument("--missing-out", type=Path,
                        help="write every missing work item, grouped by milestone")
    parser.add_argument("--focus", default="",
                        help="comma separated substrings: functions whose name matches are listed first")
    args = parser.parse_args()

    candidate = v20tool.Image(args.candidate)
    reference = v20tool.Image(args.reference)
    cand_names = (parse_map(args.candidate_map, image_base(args.candidate), candidate.size_of_image)
                  if args.candidate_map else {})
    ref_names = reference_names(args.reference_kind, reference)

    rows = v20tool._classify(candidate, cand_names, reference, ref_names, args.label)
    for row in rows:
        row["own"] = cand_names.get(row["rva"], "")

    focus = [f for f in args.focus.split(",") if f]
    if focus:
        for row in rows:
            row["focus"] = any(f.lower() in row["own"].lower() for f in focus)
    else:
        for row in rows:
            row["focus"] = False

    total = len(rows)
    by_status = {s: [r for r in rows if r["status"] == s] for s in ("SAME", "CHANGED", "NEW")}
    candidate_bytes = sum(r["size"] for r in rows)
    reference_bytes = sum(end - begin for begin, end in reference.functions())

    print(f"{args.label}: candidate {total} functions / {candidate_bytes} bytes, "
          f"reference {len(reference.functions())} / {reference_bytes} bytes")
    print("  " + ", ".join(f"{s} {len(by_status[s])} ({sum(r['size'] for r in by_status[s])} B)"
                           for s in ("SAME", "CHANGED", "NEW")))

    if args.json:
        args.json.write_text(json.dumps(rows, indent=1))

    # ---- what the build does not do yet, filed the way the work is filed
    #
    # Matching is a map, not a verdict: a rewrite of the same behaviour will not score high against
    # the original instructions, so a low similarity does not mean "not implemented".  The question
    # that decides progress is different - of the v20 work items, which ones does this build have
    # something for at all?  That is a coverage question, and it is asked per milestone, because a
    # milestone is a feature, and features are what the two-PC run accepts.
    coverage_lines: list[str] = []
    if args.reference_kind == "v20":
        backlog = backlog_rows(args.backlog)
        cov = coverage(rows, backlog)["covered"]
        if backlog:
            filed = [(rules.milestone(row), row) for row in backlog]
            order = ["M2", "M3", "M4", "M5", "M6", "M8", "M7"]
            groups: dict[str, dict] = {}
            for (tag, label), row in filed:
                group = groups.setdefault(tag, {"label": label, "total": 0, "bytes": 0, "covered": 0,
                                                "covered_bytes": 0, "missing": []})
                group["total"] += 1
                group["bytes"] += row["size"]
                if row["rva"] in cov:
                    group["covered"] += 1
                    group["covered_bytes"] += row["size"]
                else:
                    group["missing"].append(row)
            missing_all = [r for tag in order for r in (groups.get(tag, {}).get("missing") or [])]
            covered_total = len(backlog) - len(missing_all)
            print(f"  coverage: {covered_total}/{len(backlog)} work items accounted for, "
                  f"{len(missing_all)} missing ({sum(r['size'] for r in missing_all)} B)")
            coverage_lines = [
                "## Feature coverage",
                "",
                "The work is filed by milestone in `development/v20-recovery/reports/v20-backlog.md`;",
                "this table says which of those items this build already has something for.  A v20",
                "function counts as **covered** when the build matches it at or above the CHANGED",
                "threshold, or when the build has a function with the same name - the second rule is what",
                "recognises our own rewrites, which are not supposed to look like v20's instructions.",
                "",
                "| milestone | work items | bytes | covered | missing | missing bytes |",
                "|---|---|---|---|---|---|",
            ]
            for tag in order:
                group = groups.get(tag)
                if not group:
                    continue
                label = group["label"].split(":")[0].split("(")[0].strip()
                coverage_lines.append(
                    f"| {tag} {label} | {group['total']} | {group['bytes']} | {group['covered']} | "
                    f"{group['total'] - group['covered']} | {group['bytes'] - group['covered_bytes']} |")
            coverage_lines += ["", f"**{covered_total} of {len(backlog)} work items are accounted for; "
                                   f"{len(missing_all)} ({sum(r['size'] for r in missing_all)} bytes) "
                                   f"are not.**", ""]
            if missing_all:
                where = {r["rva"]: tag for tag in order
                         for r in (groups.get(tag, {}).get("missing") or [])}
                coverage_lines += ["### What is missing, largest first", "",
                                   "| RVA | v20 function | bytes | milestone | strings |",
                                   "|---|---|---|---|---|"]
                for row in sorted(missing_all, key=lambda r: -r["size"])[:20]:
                    strings = " \\| ".join(row["strings"].split(" | ")[:2])[:80]
                    name = row["v12_name"].split("@@")[0].lstrip("?") or "—"
                    coverage_lines.append(
                        f"| `{row['rva']:06x}` | `{name}` | {row['size']} | {where[row['rva']]} | "
                        f"{strings} |")
                coverage_lines.append("")
            if args.missing_out:
                # The full list, grouped the way the work is done: a milestone is the unit that gets
                # built and accepted, so "which of these eight are left" has to be answerable without
                # a debugger and without downloading an artifact.
                full: list[str] = []
                for tag in order:
                    group = groups.get(tag)
                    if not group or not group["missing"]:
                        continue
                    full.append(f"# {tag} {group['label']} - {len(group['missing'])} missing "
                                f"({group['bytes'] - group['covered_bytes']} bytes)")
                    for row in sorted(group["missing"], key=lambda r: -r["size"]):
                        name = row["v12_name"].split("@@")[0].lstrip("?") or "(no name)"
                        full.append(f"{row['rva']:06x} {row['size']:6d}  {name}")
                        for text in row["strings"].split(" | ")[:4]:
                            if text.strip():
                                full.append(f"                | {text.strip()[:100]}")
                    full.append("")
                args.missing_out.write_text("\n".join(full) + "\n")
                print(f"  wrote the missing work items to {args.missing_out}")

    if args.out:
        lines = [
            f"# {args.label}: candidate vs reference",
            "",
            f"* candidate: `{args.candidate}` — {total} functions, {candidate_bytes} bytes",
            f"* reference: `{args.reference}` — {len(reference.functions())} functions, {reference_bytes} bytes",
            "",
            "| status | functions | bytes | meaning |",
            "|---|---|---|---|",
            f"| SAME | {len(by_status['SAME'])} | {sum(r['size'] for r in by_status['SAME'])} | same code |",
            f"| CHANGED | {len(by_status['CHANGED'])} | {sum(r['size'] for r in by_status['CHANGED'])} | same function, edited |",
            f"| NEW | {len(by_status['NEW'])} | {sum(r['size'] for r in by_status['NEW'])} | no counterpart in the reference |",
            "",
        ]
        lines += coverage_lines
        if focus:
            lines += ["## Our functions the focus names (" + ", ".join(f"`{f}`" for f in focus) + ")", "",
                      "These are functions **we wrote**; the reference is the v20 function the matcher",
                      "considered closest. A reimplementation is not expected to match v20 byte for byte -",
                      "what it has to match is behaviour, which the self-test and the two-PC run check.", "",
                      "| RVA | our name | bytes | closest v20 | similarity |", "|---|---|---|---|---|"]
            for row in sorted((r for r in rows if r["focus"]), key=lambda r: r["own"]):
                lines.append(f"| `{row['rva']:06x}` | `{row['own']}` | {row['size']} | "
                             f"`{row['ref'] or '—'}` | {row['similarity']:.2f} |")
            lines.append("")
        lines += ["## Largest unmatched functions", "",
                  "| RVA | name | bytes | closest reference | similarity | strings |", "|---|---|---|---|---|---|"]
        for row in sorted(by_status["NEW"], key=lambda r: -r["size"])[:25]:
            strings = " \\| ".join(row["strings"][:3])[:110]
            lines.append(f"| `{row['rva']:06x}` | `{row['name'] or '—'}` | {row['size']} | "
                         f"`{row['ref'] or '—'}` | {row['similarity']:.2f} | {strings} |")
        buckets = {"1.00": 0, ">=0.90": 0, ">=0.75": 0, ">=0.55": 0, "<0.55": 0}
        for row in rows:
            score = row["similarity"]
            if score >= 0.999:
                buckets["1.00"] += 1
            elif score >= 0.90:
                buckets[">=0.90"] += 1
            elif score >= 0.75:
                buckets[">=0.75"] += 1
            elif score >= 0.55:
                buckets[">=0.55"] += 1
            else:
                buckets["<0.55"] += 1
        lines += ["", "## How close", "", "| similarity to the closest reference function | functions |",
                  "|---|---|"]
        for label, count in buckets.items():
            lines.append(f"| {label} | {count} |")
        lines += ["", "## Closest matches", "", "| RVA | name | bytes | status | reference | similarity |",
                  "|---|---|---|---|---|---|"]
        for row in sorted(rows, key=lambda r: -r["similarity"])[:15]:
            lines.append(f"| `{row['rva']:06x}` | `{row['name'] or '—'}` | {row['size']} | {row['status']} | "
                         f"`{row['ref'] or '—'}` | {row['similarity']:.2f} |")
        args.out.write_text("\n".join(lines) + "\n")
        print(f"  wrote {args.out}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

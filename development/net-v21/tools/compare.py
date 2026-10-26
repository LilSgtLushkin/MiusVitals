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
ROOT = HERE.parents[2]
RECOVERY = ROOT / "development" / "v20-recovery"
sys.path.insert(0, str(RECOVERY / "tools"))

import v20tool  # noqa: E402  (the PE reader, tokenizer and matcher already used for the v20 work)


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


def parse_map(path: Path, base: int) -> dict[int, str]:
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
        if 0 < rva < image.size_of_image:
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


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--candidate", type=Path, required=True)
    parser.add_argument("--candidate-map", type=Path)
    parser.add_argument("--reference", type=Path, required=True)
    parser.add_argument("--reference-kind", choices=("v12", "v20"), default="v12")
    parser.add_argument("--label", default="compare")
    parser.add_argument("--out", type=Path)
    parser.add_argument("--json", type=Path)
    parser.add_argument("--focus", default="",
                        help="comma separated substrings: functions whose name matches are listed first")
    args = parser.parse_args()

    candidate = v20tool.Image(args.candidate)
    reference = v20tool.Image(args.reference)
    cand_names = parse_map(args.candidate_map, image_base(args.candidate)) if args.candidate_map else {}
    ref_names = reference_names(args.reference_kind, reference)

    rows = v20tool._classify(candidate, cand_names, reference, ref_names, args.label)

    focus = [f for f in args.focus.split(",") if f]
    if focus:
        for row in rows:
            row["focus"] = any(f.lower() in row["name"].lower() for f in focus)
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
        if focus:
            lines += ["## Focus", "", "| RVA | name | bytes | status | reference | similarity |",
                      "|---|---|---|---|---|---|"]
            for row in sorted((r for r in rows if r["focus"]), key=lambda r: -r["similarity"]):
                lines.append(f"| `{row['rva']:06x}` | `{row['name'] or '—'}` | {row['size']} | "
                             f"{row['status']} | `{row['ref'] or '—'}` | {row['similarity']:.2f} |")
            lines.append("")
        lines += ["## Largest unmatched functions", "",
                  "| RVA | name | bytes | closest reference | similarity | strings |", "|---|---|---|---|---|---|"]
        for row in sorted(by_status["NEW"], key=lambda r: -r["size"])[:25]:
            strings = " \\| ".join(row["strings"][:3])[:110]
            lines.append(f"| `{row['rva']:06x}` | `{row['name'] or '—'}` | {row['size']} | "
                         f"`{row['ref'] or '—'}` | {row['similarity']:.2f} | {strings} |")
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

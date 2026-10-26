#!/usr/bin/env python3
"""Recover the v20 source tree of `basic_network.codelib` from the shipped binary.

Why this exists
---------------
The published MiusPlayer repository contains the **v12** C++ source and MSVC object
files, while the module players run (`MiusMultiplayer`) is a **v20** build whose source
was never published.  v20 is v12 plus ~293 functions (~107 KB of code).  Without a named
map of those functions any change to the network code is a guess.

What this tool does
-------------------
1. `objects`  - reads the published MSVC `.obj` files (they carry `/Gy` COMDAT
                sections and a full symbol table) and writes every function with its
                **mangled C++ name**, size and bytes.
2. `names`    - links those names onto the published v12 binary by matching code bytes
                with wildcards at relocations.  v12 is now a named image.
3. `classify` - walks v20's `.pdata`, disassembles every function, rewrites
                RIP-relative targets, call targets and in-image constants into a
                canonical token stream, and matches each v20 function against the
                named v12 image.  Result per function: SAME / CHANGED / NEW with the
                name, the referencing strings and the size.
4. `report`   - writes `v20-functions.csv` and `v20-recovery.md`.

The classification is the working list for the port: SAME functions are already in the
public source, CHANGED ones are small edits of it, NEW ones are the ~107 KB that must be
written (and are the only place where new behaviour lives: Steam, force choice,
ceasefire, hits/anim/craters, operation map extras).

Inputs are not committed (they are binaries, 0.5-2 MB each): see `fetch_inputs.sh`.
"""
from __future__ import annotations

import argparse
import base64
import csv
import re
import hashlib
import json
import struct
import sys
from pathlib import Path

try:
    import capstone
except ImportError:  # pragma: no cover - the tool explains itself
    raise SystemExit("capstone is required: pip install -r development/v20-recovery/requirements.txt")

HERE = Path(__file__).resolve().parent
WORK = HERE.parent / "work"          # inputs and intermediate output (git-ignored)
OUT = HERE.parent / "reports"        # committed reports

OBJECTS = ("basic_network.obj", "session.obj", "ui.obj", "state.obj", "statesync.obj",
           "shots.obj", "trench.obj", "xlate.obj", "mp_join.obj")
SOURCE_OF = {"basic_network": "basic_network.cpp", "session": "session.cpp", "ui": "ui.cpp",
             "state": "state.cpp", "statesync": "statesync.cpp", "shots": "shots.cpp",
             "trench": "trench.cpp", "xlate": "xlate.cpp", "mp_join": "mp_join.cpp"}


# --------------------------------------------------------------------------- COFF objects
class Coff:
    """MSVC x64 object file: COMDAT sections, symbol table, relocations."""

    def __init__(self, path: Path):
        self.data = d = path.read_bytes()
        machine, nsec, ts, symoff, nsym, optsz, chars = struct.unpack_from("<HHIIIHH", d, 0)
        if machine != 0x8664:
            raise SystemExit(f"{path}: not x86-64")
        self.ts = ts
        self.strtab = symoff + nsym * 18
        self.strsize = struct.unpack_from("<I", d, self.strtab)[0]
        self.sections: list[dict] = []
        at = 20 + optsz
        for _ in range(nsec):
            name = d[at:at + 8].rstrip(b"\0").decode("ascii", "replace")
            if name.startswith("/"):
                name = self._string(int(name[1:]))
            vsize, va, rsize, raw, reloff, _, nrel, _, _ = struct.unpack_from("<IIIIIIHHI", d, at + 8)
            self.sections.append({"name": name, "size": rsize, "raw": raw, "reloc": reloff, "nrel": nrel})
            at += 40
        self.symbols: list[tuple[str, int, int, int, int]] = []
        for i in range(nsym):
            o = symoff + i * 18
            raw = d[o:o + 8]
            value, secnum, typ, storage, naux = struct.unpack_from("<IhHBB", d, o + 8)
            name = self._string(struct.unpack_from("<I", raw, 4)[0]) if raw[:4] == b"\0\0\0\0" else \
                raw.rstrip(b"\0").decode("ascii", "replace")
            self.symbols.append((name, value, secnum, storage, naux))

    def _string(self, offset: int) -> str:
        start = self.strtab + offset
        return self.data[start:self.data.index(b"\0", start)].decode("ascii", "replace")

    def functions(self) -> list[dict]:
        """One entry per COMDAT code section that a function symbol points at."""
        by_section: dict[int, str] = {}
        for name, value, secnum, storage, _ in self.symbols:
            if secnum <= 0 or storage not in (2, 3) or value != 0:
                continue
            if name.startswith(".") or name.startswith("$"):
                continue
            section = self.sections[secnum - 1]
            if not section["name"].startswith(".text"):
                continue
            # Several symbols may share a section (aliases); the first well-formed one wins.
            by_section.setdefault(secnum, name)
        out = []
        for secnum, name in by_section.items():
            section = self.sections[secnum - 1]
            size = section["size"]
            if not size:
                continue
            code = self.data[section["raw"]:section["raw"] + size]
            wild = set()
            for i in range(section["nrel"]):
                at = section["reloc"] + i * 10
                offset, symidx, kind = struct.unpack_from("<IIH", self.data, at)
                if kind in (0x14, 0x1, 0x2, 0x3, 0x4, 0x5, 0x9):   # REL32/ADDR32/ADDR32NB/...: a 4-byte patch
                    wild.update(range(offset, min(offset + 4, size)))
            out.append({"name": name, "section": section["name"], "size": size,
                        "bytes": base64.b64encode(code).decode("ascii"),
                        "wild": sorted(wild)})
        return out


# --------------------------------------------------------------------------- PE image
class Image:
    def __init__(self, path: Path):
        self.data = d = path.read_bytes()
        self.path = path
        pe = struct.unpack_from("<I", d, 0x3C)[0]
        self.pe = pe
        self.timestamp = struct.unpack_from("<I", d, pe + 8)[0]
        nsec = struct.unpack_from("<H", d, pe + 6)[0]
        optsz = struct.unpack_from("<H", d, pe + 20)[0]
        opt = pe + 24
        self.size_of_image = struct.unpack_from("<I", d, opt + 56)[0]
        at = opt + optsz
        self.sections = []
        for _ in range(nsec):
            name = d[at:at + 8].rstrip(b"\0").decode("ascii", "replace")
            vsize, va, rsize, raw = struct.unpack_from("<IIII", d, at + 8)
            self.sections.append({"name": name, "va": va, "vsize": max(vsize, rsize), "raw": raw, "rsize": rsize})
            at += 40

    def off(self, rva: int) -> int | None:
        for s in self.sections:
            if s["va"] <= rva < s["va"] + s["vsize"]:
                return s["raw"] + rva - s["va"]
        return None

    def read(self, rva: int, n: int) -> bytes:
        o = self.off(rva)
        if o is None or o + n > len(self.data):
            return b""
        return self.data[o:o + n]

    def cstr(self, rva: int, limit: int = 400) -> str:
        o = self.off(rva)
        if o is None:
            return ""
        end = self.data.find(b"\0", o, o + limit)
        if end < 0:
            return ""
        return self.data[o:end].decode("ascii", "replace")

    def functions(self) -> list[tuple[int, int]]:
        """(begin, end) from the exception directory - exact function bounds, no guessing."""
        directory_rva, directory_size = struct.unpack_from("<II", self.data, self.pe + 24 + 112 + 3 * 8)
        o = self.off(directory_rva)
        out = []
        if o is None:
            return out
        for i in range(directory_size // 12):
            begin, end, _ = struct.unpack_from("<III", self.data, o + i * 12)
            if begin:
                out.append((begin, end))
        return sorted(out)

    def section_of(self, rva: int) -> str:
        for s in self.sections:
            if s["va"] <= rva < s["va"] + s["vsize"]:
                return s["name"]
        return "?"


# --------------------------------------------------------------------------- canonical code
class Canon:
    """Turn machine code into tokens that survive a different build of the same source.

    A RIP-relative memory operand becomes the *content* it points at (the string, or a
    placeholder for data), never the address, and a call/jump target becomes a placeholder
    too - the v12 and v20 images were built by different compiler versions, so addresses,
    register choices and stack offsets all move while the structure of the code does not.
    """

    def __init__(self, image: Image, names: dict[int, str] | None = None):
        self.image = image
        self.names = names or {}
        self.md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_64)
        self.md.detail = True

    def target(self, rva: int) -> str:
        """What a pointer points at, in terms a rebuild can reproduce."""
        if self.names and rva in self.names:
            return "fn:" + self.names[rva].split("@@")[0]
        text = self.image.cstr(rva, 160)
        if len(text) >= 5 and all(32 <= ord(c) < 127 for c in text):
            return "str:" + text[:120]
        if 0 < rva < self.image.size_of_image:
            return "img"
        return f"abs:{rva & 0xFFFFFFFF:x}"

    def tokens(self, begin: int, end: int) -> list[str]:
        code = self.image.read(begin, end - begin)
        out = []
        for insn in self.md.disasm(code, begin):
            if insn.address >= end:
                break
            parts = []
            for op in insn.operands:
                if op.type == capstone.x86.X86_OP_MEM:
                    mem = op.mem
                    if mem.base == capstone.x86.X86_REG_RIP:
                        parts.append("@{" + self.target(insn.address + insn.size + mem.disp) + "}")
                    else:
                        base = insn.reg_name(mem.base) or ""
                        index = insn.reg_name(mem.index) or ""
                        # A frame-reference offset moves with the layout; the shape does not.
                        parts.append(f"[{base}+{index}*{mem.scale}]")
                elif op.type == capstone.x86.X86_OP_IMM:
                    if insn.group(capstone.x86.X86_GRP_JUMP) or insn.group(capstone.x86.X86_GRP_CALL):
                        parts.append("T")           # a target inside the image: not comparable by address
                    else:
                        parts.append(f"#{op.imm & 0xFFFFFFFFFFFFFFFF:x}")
                elif op.type == capstone.x86.X86_OP_REG:
                    parts.append(insn.reg_name(op.reg) or "?")
                else:
                    parts.append("?")
            out.append(f"{insn.mnemonic} {' '.join(parts)}".strip())
        return out

    def strings(self, begin: int, end: int) -> list[str]:
        """Printable strings this function can reach (directly or through a pointer load)."""
        found: list[str] = []
        for token in self.tokens(begin, end):
            for match in re.finditer(r"@\{str:(.*?)\}", token):
                if match.group(1) not in found:
                    found.append(match.group(1))
        return found


# --------------------------------------------------------------------------- matching
def masked_match(code: bytes, pattern: bytes, wild: set[int]) -> bool:
    if len(code) != len(pattern):
        return False
    for i in range(len(pattern)):
        if i in wild:
            continue
        if code[i] != pattern[i]:
            return False
    return True


def cmd_objects(_args) -> int:
    WORK.mkdir(parents=True, exist_ok=True)
    all_functions = []
    for name in OBJECTS:
        path = WORK / name
        if not path.is_file():
            print(f"missing {path} (run fetch_inputs.sh)", file=sys.stderr)
            continue
        coff = Coff(path)
        functions = coff.functions()
        for f in functions:
            f["file"] = SOURCE_OF[name[:-4]]
        all_functions.extend(functions)
        print(f"{name:20} {len(functions):5} functions")
    (WORK / "v12-objects.json").write_text(json.dumps(all_functions), encoding="utf-8")
    print(f"total {len(all_functions)} object functions -> {WORK / 'v12-objects.json'}")
    return 0


def cmd_names(_args) -> int:
    v12 = Image(WORK / "v12.codelib")
    functions = json.loads((WORK / "v12-objects.json").read_text(encoding="utf-8"))
    targets = v12.functions()
    by_size: dict[int, list[tuple[int, int]]] = {}
    for begin, end in targets:
        by_size.setdefault(end - begin, []).append((begin, end))
    names: dict[int, str] = {}
    used: set[int] = set()
    unmatched = []
    # Two passes: an exact masked match first (unique size bucket), then a second pass for
    # functions whose byte pattern is shared by several sections (template copies).
    for strict in (True, False):
        for f in functions:
            begin_list = by_size.get(f["size"], [])
            if not begin_list:
                continue
            pattern = base64.b64decode(f["bytes"])
            wild = set(f["wild"])
            hits = [b for b, _ in begin_list if b not in used and masked_match(v12.read(b, f["size"]), pattern, wild)]
            if len(hits) != 1 and strict:
                unmatched.append(f)
                continue
            if not hits:
                unmatched.append(f)
                continue
            begin = hits[0]
            names[begin] = f["name"]
            used.add(begin)
    matched = len(names)
    print(f"named {matched} of {len(functions)} object functions "
          f"({len(targets)} functions in the v12 image)")
    (WORK / "v12-names.json").write_text(json.dumps(names, indent=0), encoding="utf-8")
    rest = [t for t in targets if t[0] not in names]
    print(f"v12 functions without a name: {len(rest)}")
    for begin, end in rest[:15]:
        print(f"   {begin:06x}  {end - begin:5} bytes  section {v12.section_of(begin)}")
    return 0


def _opcode_profile(tokens: list[str]) -> dict[str, int]:
    profile: dict[str, int] = {}
    for token in tokens:
        op = token.split(" ", 1)[0]
        profile[op] = profile.get(op, 0) + 1
    return profile


def _profile_similarity(a: dict[str, int], b: dict[str, int]) -> float:
    if not a or not b:
        return 0.0
    keys = a.keys() & b.keys()
    inter = sum(min(a[k], b[k]) for k in keys)
    return 2.0 * inter / (sum(a.values()) + sum(b.values()))


def _classify(image: Image, names: dict[int, str], reference: Image | None,
              reference_names: dict[int, str], label: str) -> list[dict]:
    """Match every function of `image` to the closest function of `reference`.

    The reference is the named v12 image.  Two different compiler versions move registers,
    stack offsets and addresses, so identity is decided on structure: opcode profile first
    (cheap), then the instruction-level similarity of the surviving candidates, with the
    referenced strings as a tie-breaker.
    """
    canon = Canon(image, names)
    ref_list: list[dict] = []
    if reference:
        ref_canon = Canon(reference, reference_names)
        for begin, end in reference.functions():
            tokens = ref_canon.tokens(begin, end)
            ref_list.append({"begin": begin, "size": end - begin, "tokens": tokens,
                             "profile": _opcode_profile(tokens),
                             "name": reference_names.get(begin, ""),
                             "strings": set(ref_canon.strings(begin, end))})
    rows = []
    for begin, end in image.functions():
        tokens = canon.tokens(begin, end)
        row = {"rva": begin, "size": end - begin, "name": names.get(begin, ""),
               "status": "NEW", "ref": "", "similarity": 0.0,
               "strings": canon.strings(begin, end), "tokens": len(tokens)}
        profile = _opcode_profile(tokens)
        strings = set(row["strings"])
        best, best_score = None, 0.0
        for candidate in ref_list:
            size_ratio = min(row["size"], candidate["size"]) / max(row["size"], candidate["size"])
            if size_ratio < 0.45:
                continue
            shape = _profile_similarity(profile, candidate["profile"])
            if shape < 0.55:
                continue
            import difflib
            score = difflib.SequenceMatcher(None, tokens, candidate["tokens"], autojunk=False).ratio()
            if strings or candidate["strings"]:
                shared = len(strings & candidate["strings"])
                union = len(strings | candidate["strings"])
                score = 0.75 * score + 0.25 * (shared / union if union else 0.0)
            score *= 0.5 + 0.5 * size_ratio
            if score > best_score:
                best, best_score = candidate, score
        if best is not None:
            row["similarity"] = round(best_score, 3)
            row["ref"] = best["name"]
            row["ref_rva"] = best["begin"]
            if best_score >= 0.92 and abs(best["size"] - row["size"]) <= max(4, row["size"] // 50):
                row["status"] = "SAME"
            elif best_score >= 0.55:
                row["status"] = "CHANGED"
        rows.append(row)
    print(f"{label}: " + ", ".join(f"{s} {sum(1 for r in rows if r['status'] == s)}"
                                   for s in ("SAME", "CHANGED", "NEW")))
    return rows


def cmd_classify(_args) -> int:
    v20 = Image(WORK / "v20.codelib")
    v12 = Image(WORK / "v12.codelib")
    names12 = {int(k): v for k, v in json.loads((WORK / "v12-names.json").read_text()).items()}
    rows = _classify(v20, {}, v12, names12, "v20 against named v12")
    (WORK / "v20-classified.json").write_text(json.dumps(rows), encoding="utf-8")
    # Second pass: the v20 functions that ARE v12 functions give us v20-side names too, which
    # makes the report and later comparisons readable.
    for row in rows:
        if row["status"] == "SAME" and row["ref"]:
            row["name"] = row["ref"]
    (WORK / "v20-classified.json").write_text(json.dumps(rows), encoding="utf-8")
    return 0


def cmd_report(_args) -> int:
    rows = json.loads((WORK / "v20-classified.json").read_text(encoding="utf-8"))
    OUT.mkdir(parents=True, exist_ok=True)
    with (OUT / "v20-functions.csv").open("w", newline="", encoding="utf-8") as handle:
        writer = csv.writer(handle)
        writer.writerow(["rva", "size", "status", "v12_name", "similarity", "strings"])
        for row in sorted(rows, key=lambda r: r["rva"]):
            writer.writerow([f"{row['rva']:06x}", row["size"], row["status"], row["name"] or row["ref"],
                             row.get("similarity", ""), " | ".join(row["strings"][:4])])
    counts = {s: sum(1 for r in rows if r["status"] == s) for s in ("SAME", "CHANGED", "NEW")}
    sizes = {s: sum(r["size"] for r in rows if r["status"] == s) for s in counts}
    lines = ["# v20: what the binary says about the source", "",
             f"{len(rows)} functions, {sum(r['size'] for r in rows)} bytes of code.", "",
             "| status | functions | bytes | meaning |", "|---|---|---|---|",
             f"| SAME | {counts['SAME']} | {sizes['SAME']} | identical code to the published v12 source |",
             f"| CHANGED | {counts['CHANGED']} | {sizes['CHANGED']} | v12 function with edits |",
             f"| NEW | {counts['NEW']} | {sizes['NEW']} | code absent from the published source |", ""]
    lines.append("Full per-function list: [`v20-functions.csv`](v20-functions.csv).")
    (OUT / "v20-status.md").write_text("\n".join(lines) + "\n", encoding="utf-8")
    print("\n".join(lines[:12]))
    return 0


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = parser.add_subparsers(dest="command", required=True)
    for name, fn in (("objects", cmd_objects), ("names", cmd_names),
                     ("classify", cmd_classify), ("report", cmd_report)):
        sub.add_parser(name).set_defaults(run=fn)
    args = parser.parse_args()
    return args.run(args)


if __name__ == "__main__":
    raise SystemExit(main())

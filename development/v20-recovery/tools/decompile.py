#!/usr/bin/env python3
"""Decompile every v20 function to readable pseudo-C.

angr's decompiler is not Ghidra, but on /O1 MSVC code it produces structured code with the
string literals in place, which is what the port needs: it shows *what a function does*, and
the v12 source next to it shows *how the same thing is written in this codebase*.

Names are injected before decompiling:
  * functions matched to the published v12 image get their real C++ name,
  * the small set of addresses already documented in docs/NETWORK_LIMITATIONS.md
    (packet dispatcher, session_frame, session_sim_step, the logger, steam_init) are named,
  * everything else stays `sub_XXXX`, which is exactly the work list.
"""
from __future__ import annotations

import argparse
import json
import logging
import signal
import sys
import time
from pathlib import Path

HERE = Path(__file__).resolve().parent
WORK = HERE.parent / "work"
DECOMP = WORK / "decomp"

# Addresses proven by the earlier byte-level work (docs/NETWORK_LIMITATIONS.md and the
# mp_vitals hooks).  The dispatcher/frame/sender names come from the public v12 source.
KNOWN = {
    0x16230: "on_packet",
    0x19F88: "session_frame",
    0x1C544: "session_sim_step",
    0x19A98: "send_peer_packet",
    0x17CF0: "owns_object",
    0x27DC: "mp_log",
    0x3F75C: "steam_init",
    0x3E8BC: "fake_steam_open",
    0x3EB34: "steam_bind_interfaces",
    0x402B0: "security_cookie_check",
}


def load_names() -> dict[int, str]:
    names: dict[int, str] = dict(KNOWN)
    classified = WORK / "v20-classified.json"
    if classified.is_file():
        for row in json.loads(classified.read_text(encoding="utf-8")):
            label = row.get("name") or row.get("ref")
            if not label:
                continue
            label = label.split("@@")[0].lstrip("?")
            if label and row["rva"] not in names:
                names[row["rva"]] = label
    return names



def disassemble(image: bytes, begin: int, end: int, names: dict[int, str]) -> str:
    """Fallback listing for the functions angr cannot structure.

    Capstone is enough to read a 300-byte helper, and it never fails.  Every RIP-relative operand is
    annotated with the string, the imported function or the symbol it points at, so the listing still
    says what the code does.
    """
    import struct
    from capstone import CS_ARCH_X86, CS_MODE_64, Cs

    pe = struct.unpack_from("<I", image, 0x3C)[0]
    count = struct.unpack_from("<H", image, pe + 6)[0]
    optsize = struct.unpack_from("<H", image, pe + 20)[0]
    at = pe + 24 + optsize
    sections = []
    for _ in range(count):
        vsize, va, rsize, raw = struct.unpack_from("<IIII", image, at + 8)
        sections.append((va, max(vsize, rsize), raw, rsize))
        at += 40

    def read(rva: int, amount: int) -> bytes | None:
        for va, size, raw, rawsize in sections:
            if va <= rva < va + size and rva - va < rawsize:
                return image[raw + rva - va: raw + rva - va + amount]
        return None

    def cstring(rva: int, limit: int = 200) -> str | None:
        blob = read(rva, limit)
        if not blob:
            return None
        stop = blob.find(b"\0")
        if stop < 4:
            return None
        text = blob[:stop]
        if any(b < 0x20 or b > 0x7E for b in text):
            return None
        return text.decode("ascii")

    # import table: IAT slot RVA -> "dll!function", so `call qword ptr [rip+...]` has a name
    iat: dict[int, str] = {}
    try:
        name_rva, _ = struct.unpack_from("<II", image, pe + 24 + 112 + 1 * 8)
        if name_rva:
            index = next(raw + name_rva - va for va, size, raw, rawsize in sections
                         if va <= name_rva < va + size)
            while True:
                original, stamp, chain, dll_rva, first = struct.unpack_from("<IIIII", image, index)
                if not (original or stamp or chain or dll_rva or first):
                    break
                dll = cstring(dll_rva) or "?"
                slot = first or original
                while True:
                    blob = read(slot, 8)
                    if blob is None:
                        break
                    entry = struct.unpack("<Q", blob)[0]
                    if entry == 0:
                        break
                    if not entry >> 63:                     # import by name, not by ordinal
                        iat[slot] = f"{dll}!{cstring(entry + 2) or hex(entry)}"
                    slot += 8
                index += 20
    except Exception:  # noqa: BLE001 - an annotation we cannot build is not an error
        pass

    code = read(begin, end - begin)
    if code is None:
        return "// (code is not backed by file data)"
    md = Cs(CS_ARCH_X86, CS_MODE_64)
    md.detail = True
    lines = []
    for insn in md.disasm(code, begin):
        note = ""
        if insn.operands:
            op = insn.operands[-1]
            if op.type == 3 and op.mem.base == 41:          # X86_OP_MEM with RIP as base
                target = insn.address + insn.size + op.mem.disp
                literal = cstring(target)
                if literal is not None:
                    note = f'    ; "{literal}"'
                elif target in iat:
                    note = f"    ; {iat[target]}"
                elif target in names:
                    note = f"    ; {names[target]}"
                else:
                    note = f"    ; 0x{target:x}"
            elif op.type == 2 and insn.group(2):            # X86_OP_IMM on a jump/call
                if op.imm in names:
                    note = f"    ; {names[op.imm]}"
        lines.append(f"{insn.address:06x}  {insn.mnemonic:<7} {insn.op_str}{note}")
    return "\n".join(lines)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--only", type=lambda s: [int(x, 0) for x in s.split(",")],
                        help="decompile these RVAs (comma separated)")
    parser.add_argument("--limit", type=int, default=0)
    parser.add_argument("--min-size", type=int, default=0)
    parser.add_argument("--max-size", type=int, default=0)
    parser.add_argument("--skip-existing", action="store_true")
    parser.add_argument("--timeout", type=int, default=90, help="seconds per function")
    parser.add_argument("--shard", type=int, nargs=2, metavar=("INDEX", "COUNT"), help="worker INDEX of COUNT")
    args = parser.parse_args()

    for name in ("angr", "cle", "pyvex", "ailment", "claripy"):
        logging.getLogger(name).setLevel("ERROR")
    import angr

    image = (WORK / "v20.codelib").read_bytes()
    names = load_names()
    DECOMP.mkdir(parents=True, exist_ok=True)
    started = time.time()
    project = angr.Project(str(WORK / "v20.codelib"), auto_load_libs=False,
                           main_opts={"base_addr": 0})
    for addr, name in names.items():
        try:
            project.kb.functions[addr].name = name
        except KeyError:
            pass
    cfg = project.analyses.CFGFast(data_references=True, normalize=True, resolve_indirect_jumps=True)
    for addr, name in names.items():
        try:
            project.kb.functions[addr].name = name
        except KeyError:
            pass
    print(f"cfg ready in {time.time() - started:.0f}s, {len(project.kb.functions)} functions")

    # exact bounds from the exception directory: no guessing about where a function ends
    import struct
    pe = struct.unpack_from("<I", image, 0x3C)[0]
    nsec = struct.unpack_from("<H", image, pe + 6)[0]
    optsz = struct.unpack_from("<H", image, pe + 20)[0]
    at = pe + 24 + optsz
    sections = []
    for _ in range(nsec):
        vsize, va, rsize, raw = struct.unpack_from("<IIII", image, at + 8)
        sections.append((va, max(vsize, rsize), raw))
        at += 40
    directory_rva, directory_size = struct.unpack_from("<II", image, pe + 24 + 112 + 3 * 8)
    raw = next(r for va, size, r in sections if va <= directory_rva < va + size)
    table = raw + directory_rva - next(va for va, size, r in sections if va <= directory_rva < va + size)
    ranges = []
    for i in range(directory_size // 12):
        begin, end, _ = struct.unpack_from("<III", image, table + i * 12)
        if not begin or (args.only and begin not in args.only):
            continue
        if end - begin < args.min_size or (args.max_size and end - begin > args.max_size):
            continue
        ranges.append((begin, end))

    def write_failure(reason: str) -> None:
        """Never leave an empty file behind: a listing with resolved strings is still readable."""
        listing = disassemble(image, begin, end, names)
        out.write_text(f"// v20 RVA {begin:06x}..{end:06x}  ({end - begin} bytes)  {label}\n"
                       f"// {reason}: capstone listing, RIP-relative operands annotated\n\n"
                       + listing + "\n")

    written = 0
    if args.shard:
        ranges = [r for i, r in enumerate(ranges) if i % args.shard[1] == args.shard[0]]
    for begin, end in ranges:
        if args.limit and written >= args.limit:
            break
        function = project.kb.functions.function(addr=begin)
        label = names.get(begin, f"sub_{begin:x}")
        out = DECOMP / f"{begin:06x}_{label.replace(':', '_').replace('?', '')[:60]}.c"
        if args.skip_existing and out.exists():
            continue
        if function is None:
            # .pdata knows the bounds, but the CFG missed the function (it is called through a
            # table or only from an exception handler).  Create it and give it its own graph.
            project.kb.functions.function(addr=begin, create=True)
            project.analyses.CFGFast(function_starts=[begin], data_references=True, normalize=True)
            function = project.kb.functions.function(addr=begin, create=True)
        if function is None or function.graph is None or function.graph.number_of_nodes() == 0:
            write_failure("angr found no function")
            continue
        def _timeout(signum, frame):
            raise TimeoutError("decompiler exceeded the per-function limit")
        previous = signal.signal(signal.SIGALRM, _timeout)
        signal.alarm(args.timeout)
        try:
            code = project.analyses.Decompiler(function, cfg=cfg).codegen
        except TimeoutError:
            write_failure(f"decompiler timed out after {args.timeout}s")
            continue
        except Exception as error:  # noqa: BLE001 - a failed function must not stop the dump
            write_failure(f"decompiler failed ({type(error).__name__}: {error})")
            continue
        finally:
            signal.alarm(0)
            signal.signal(signal.SIGALRM, previous)
        if code is None:
            write_failure("no codegen")
            continue
        out.write_text(f"// v20 RVA {begin:06x}..{end:06x}  ({end - begin} bytes)  {label}\n"
                       f"// pseudo-C from angr; strings are exact, control flow is structured, names are injected\n\n"
                       + code.text + "\n")
        written += 1
        if written % 50 == 0:
            print(f"  {written} functions, {time.time() - started:.0f}s")
    print(f"wrote {written} functions to {DECOMP} in {time.time() - started:.0f}s")
    return 0


if __name__ == "__main__":
    sys.exit(main())

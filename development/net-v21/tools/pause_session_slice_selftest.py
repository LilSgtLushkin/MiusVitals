#!/usr/bin/env python3
"""Compile selected *production* session hooks with mock game functions, without a game.

This extracts whole static functions from session.cpp for a native portable test; changes to a
function signature or source layout fail the extraction instead of silently testing a copy. It
exercises the real UI/timer decisions and their shared pause model, but not Windows detours,
packet dispatch, Steam, the engine's object lifetimes, or thread scheduling.
"""
from __future__ import annotations

import argparse
import os
from pathlib import Path
import re
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
FUNCTIONS = (
    "pause_feature", "pause_managed", "pause_effective", "pause_connection_lost", "pause_peer_features",
    "gate_active", "mark_ready", "request_pause_vote", "pause_ui_enabled", "start_action_hook",
    "input_action_hook", "follows_host_clock", "set_pause_hook", "timer_step_hook", "advance_phase",
    "valid_tactical_state", "apply_state_header",
)


def function(source: str, name: str) -> str:
    signature = re.compile(r"(?m)^static [^\n;{}]*\b" + re.escape(name) + r"\s*\([^;{}]*\)\s*\{")
    matches = list(signature.finditer(source))
    if len(matches) != 1:
        raise ValueError(f"expected exactly one definition of {name}, found {len(matches)}")
    start, opening = matches[0].start(), matches[0].end() - 1
    depth, i, state = 0, opening, "code"
    while i < len(source):
        c = source[i]
        nxt = source[i + 1] if i + 1 < len(source) else ""
        if state == "code":
            if c == "/" and nxt == "/":
                state, i = "line", i + 1
            elif c == "/" and nxt == "*":
                state, i = "block", i + 1
            elif c == '"':
                state = "string"
            elif c == "'":
                state = "char"
            elif c == "{":
                depth += 1
            elif c == "}":
                depth -= 1
                if depth == 0:
                    return source[start:i + 1]
        elif state == "line":
            if c == "\n":
                state = "code"
        elif state == "block":
            if c == "*" and nxt == "/":
                state, i = "code", i + 1
        elif state in ("string", "char"):
            if c == "\\":
                i += 1
            elif (state == "string" and c == '"') or (state == "char" and c == "'"):
                state = "code"
        i += 1
    raise ValueError(f"unterminated body of {name}")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--cxx", default="cl" if os.name == "nt" else "c++")
    parser.add_argument("--sanitize", action="store_true", help="use ASan/UBSan on non-Windows hosts")
    args = parser.parse_args()
    source = (ROOT / "src" / "session.cpp").read_text(encoding="utf-8")
    extracted = "\n\n".join(function(source, name) for name in FUNCTIONS) + "\n"
    with tempfile.TemporaryDirectory(prefix="netv21-pause-slice-") as tmp:
        directory = Path(tmp)
        (directory / "pause_session_extracted.inc").write_text(extracted, encoding="utf-8")
        test = ROOT / "tests" / "pause_session_slice_test.cpp"
        executable = directory / ("pause_session_slice.exe" if os.name == "nt" else "pause_session_slice")
        if os.name == "nt":
            if args.sanitize:
                parser.error("--sanitize is only supported by the native non-Windows build")
            cmd = [args.cxx, "/nologo", "/EHsc", "/std:c++17", "/W3", f"/I{ROOT / 'src'}",
                   f"/I{directory}", str(test), f"/Fe:{executable}"]
        else:
            cmd = [args.cxx, "-std=c++17", "-Wall", "-Wextra", "-Werror", f"-I{ROOT / 'src'}",
                   f"-I{directory}", str(test), "-o", str(executable)]
            if args.sanitize:
                cmd[1:1] = ["-fsanitize=address,undefined", "-fno-omit-frame-pointer"]
        subprocess.run(cmd, cwd=directory, check=True)
        return subprocess.run([str(executable)], cwd=directory, check=False).returncode


if __name__ == "__main__":
    raise SystemExit(main())

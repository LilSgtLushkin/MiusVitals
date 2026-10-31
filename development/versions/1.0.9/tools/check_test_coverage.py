#!/usr/bin/env python3
"""Checks that every test executable the selftest target builds is actually run.

The build and the run are two different lists in two different places: build.bat compiles the tests,
run_tests.bat runs most of them, and the CI workflow runs a few more by name. Nothing keeps the three
in step, so a test can be compiled, never executed, and still count as a passing build.

Stands are reported separately: the fixtures and the peer processes are started by other tests, not by
the runner, so they are never expected in either run list.

Usage: check_test_coverage.py [--strict]   (--strict: exit 1 when a test is never run)
"""
import re
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
VERSION_DIR = HERE.parent
ROOT = VERSION_DIR.parent.parent

STAND = re.compile(r"fixture|starter|loopback|peer", re.I)


def built_by_selftest(build_bat):
    text = build_bat.read_text(encoding="utf-8", errors="replace").replace("\r\n", "\n")
    start = text.find("\n:selftest\n")
    end = text.find("\n:render_test\n")
    if start < 0 or end < 0:
        sys.exit("cannot find the :selftest target in build.bat")
    body = text[start:end]
    return {m.lower() for m in re.findall(r"/OUT:[^\s]*?([A-Za-z0-9_]+\.exe)", body)}


def run_by(runner):
    text = runner.read_text(encoding="utf-8", errors="replace").replace("\r\n", "\n")
    return {m.lower() for m in re.findall(r"%DIR%\\([A-Za-z0-9_]+\.exe)", text)}


def run_by_workflow(workflow):
    text = workflow.read_text(encoding="utf-8", errors="replace").replace("\r\n", "\n")
    return {m.lower() for m in re.findall(r"([A-Za-z0-9_]+\.exe)", text)}


def main():
    strict = "--strict" in sys.argv
    built = built_by_selftest(VERSION_DIR / "build.bat")
    runner = VERSION_DIR / "run_tests.bat"
    workflow = ROOT / ".github" / "workflows" / "build-network-module.yml"
    run = run_by(runner) if runner.exists() else set()
    if workflow.exists():
        run |= run_by_workflow(workflow)

    stands = sorted(n for n in built if STAND.search(n) and not n.endswith("_test.exe"))
    tests = sorted(n for n in built if n not in stands)
    never = [n for n in tests if n not in run]

    print("selftest builds %d executables: %d tests and %d stands" % (len(built), len(tests), len(stands)))
    print("run by run_tests.bat or by the workflow: %d" % (len(tests) - len(never)))
    if stands:
        print("stands (started by other tests, not by the runner):")
        for n in stands:
            print("  %s%s" % (n, "" if n in run else "   (not in either run list)"))
    if never:
        print("built but never run:")
        for n in never:
            print("  %s" % n)
    else:
        print("every built test is run")
    if strict and never:
        sys.exit(1)
    return 0


if __name__ == "__main__":
    sys.exit(main())

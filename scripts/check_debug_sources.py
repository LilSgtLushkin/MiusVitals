#!/usr/bin/env python3
"""Keep the debug paths in step with build.bat.

CI and the shipped installer are built by `build.bat`; the IDE debug paths are the Visual Studio
projects (net-v21.slnx) and the VS Code tasks. They list the same sources and the same tests in
three different formats, so they drift silently: a source added to build.bat is simply missing
from the IDE build, and a test added to build.bat is never run.

This compares the three lists and fails on any difference:

  * module / baseline sources: build.bat  vs  net-v21-module.vcxproj / net-v21-baseline.vcxproj
  * self-tests: build.bat  vs  run_tests.bat  vs  .github/workflows/build-network-module.yml

Run it after every change to the sources:

    python scripts/check_debug_sources.py

It is deliberately not part of the release gate: it checks the developer paths, not the shipped
module, and a mismatch must not block an installer.
"""
from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
NET = ROOT / "development" / "net-v21"
BUILD_BAT = NET / "build.bat"
RUN_TESTS = NET / "run_tests.bat"
CI = ROOT / ".github" / "workflows" / "build-network-module.yml"
MODULE_VCXPROJ = NET / "net-v21-module.vcxproj"
BASELINE_VCXPROJ = NET / "net-v21-baseline.vcxproj"

errors: list[str] = []


def text(path: Path) -> str:
    if not path.is_file():
        errors.append(f"missing file: {path.relative_to(ROOT).as_posix()}")
        return ""
    return path.read_text(encoding="utf-8", errors="replace")


def bat_text() -> str:
    return text(BUILD_BAT)


def bat_expand(value: str, body: str) -> str:
    """Substitute the %VARIABLES% build.bat sets itself (PUBLISHED is the only one used)."""
    for name, replacement in re.findall(r"^set ([A-Z]+)=([^\r\n]*)$", body, re.M):
        value = value.replace("%%%s%%" % name, replacement.strip())
    return value


def bat_sources(kind: str) -> set[str]:
    """`set BASE=src\\a.cpp src\\b.cpp` -> {'src/a.cpp', ...}."""
    body = bat_text()
    m = re.search(r"^set %s=(.*)$" % kind, body, re.M)
    if not m:
        errors.append(f"build.bat: no `set {kind}=` line")
        return set()
    value = bat_expand(m.group(1).strip(), body)
    return {s.replace("\\", "/").lower() for s in value.split() if s.endswith(".cpp")}


def vcxproj_sources(path: Path) -> set[str]:
    body = text(path)
    return {s.replace("\\", "/").lower()
            for s in re.findall(r'<ClCompile Include="([^"]+)"', body)}


def compare_sources() -> None:
    for kind, vcxproj in (("BASE", MODULE_VCXPROJ), ("BASELINE", BASELINE_VCXPROJ)):
        want = bat_sources(kind)
        if kind == "BASE":
            want |= bat_sources("STEAM")
        have = vcxproj_sources(vcxproj)
        missing = sorted(want - have)
        extra = sorted(have - want)
        label = vcxproj.name
        if missing:
            errors.append(f"{label}: not listed, so the IDE will not compile them: {', '.join(missing)}")
        if extra:
            errors.append(f"{label}: listed but build.bat does not compile them: {', '.join(extra)}")
        print(f"  {label}: {len(have)} sources, matches build.bat {kind}"
              f"{' + STEAM' if kind == 'BASE' else ''}: {'yes' if not missing and not extra else 'NO'}")


def bat_tests() -> set[str]:
    """Test executables build.bat produces: /OUT:build\\selftest%SUFFIX%\\<name>.exe."""
    body = text(BUILD_BAT)
    return {m.lower() for m in re.findall(r"/OUT:build\\selftest%SUFFIX%\\([A-Za-z0-9_]+\.exe)", body)}


def named_in(path: Path, names: set[str]) -> set[str]:
    """Which of `names` this file mentions at all.

    Pair tests are not run as bare executables: a Python harness starts two peers and compares
    their logs, so the test is covered when its executable is named in the harness command line.
    """
    body = text(path)
    return {n for n in names if n in body.lower()}


def compare_tests() -> None:
    built = bat_tests()
    local = named_in(RUN_TESTS, built)
    ci = named_in(CI, built)
    print(f"  build.bat builds {len(built)} tests, run_tests.bat runs {len(local)}, CI runs {len(ci)}")
    for name, have, want in (("run_tests.bat", local, built), ("CI (build-network-module.yml)", ci, built)):
        missing = sorted(want - have)
        extra = sorted(have - want)
        if missing:
            errors.append(f"{name}: built by build.bat but never run: {', '.join(missing)}")
        if extra:
            errors.append(f"{name}: runs a test build.bat does not build: {', '.join(extra)}")
        if not missing and not extra:
            print(f"  {name}: same list as build.bat: yes")


def main() -> int:
    print("debug paths vs build.bat")
    compare_sources()
    compare_tests()
    if errors:
        print("\n".join("FAIL " + e for e in errors))
        print(f"\n{len(errors)} mismatch(es). Debug builds would diverge from the shipped release.")
        return 1
    print("OK debug paths are in step with build.bat")
    return 0


if __name__ == "__main__":
    sys.exit(main())

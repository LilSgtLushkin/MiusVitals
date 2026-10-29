#!/usr/bin/env sh
# Verify shipped hashes and PE import/export surfaces, then run the portable safety tests.
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
python3 "$ROOT/development/kit-source/src/verify_artifacts.py" --kit "$ROOT/development/kit-source"
python3 "$ROOT/development/merged-source/verify_workspace.py"
python3 "$ROOT/scripts/check_artifacts.py"
python3 "$ROOT/scripts/check_artifacts_selftest.py"
python3 "$ROOT/scripts/package_net_v21_test_selftest.py"
python3 "$ROOT/development/net-v21/tools/extract_legacy_shots_fixture_selftest.py"
python3 "$ROOT/tests/test_historical_106_fixture.py"
if [ -f "$ROOT/MiusMultiplayer_v1.0.7_EXPERIMENTAL_NET+FPS_INSTALL.zip" ]; then
    # The single shipped installer, rebuilt by publish-installer.yml from the gated source. The
    # packager proves the kit inside is complete and consistent; the recorded hash proves this copy
    # is the one that was built, so a truncated or rewritten file cannot pass. The hash is not
    # pinned in check_artifacts.py: it is recorded beside the archive by the same job that builds
    # it, so a pin here would prove nothing.
    python3 "$ROOT/scripts/package_net_v21_test.py" --verify \
        "$ROOT/MiusMultiplayer_v1.0.7_EXPERIMENTAL_NET+FPS_INSTALL.zip"
    python3 - "$ROOT/MiusMultiplayer_v1.0.7_EXPERIMENTAL_NET+FPS_INSTALL.zip" <<'INNER'
import hashlib, pathlib, sys
archive = pathlib.Path(sys.argv[1])
record = archive.with_name(archive.name + ".sha256.txt")
lines = record.read_text().split("\n")
fields = lines[0].split()
assert len(fields) == 2, f"unexpected record format: {lines[0]!r}"
assert fields[1] == archive.name, f"record describes {fields[1]}, not {archive.name}"
assert lines[1].startswith("source commit: "), "the record does not name the source commit"
digest = hashlib.sha256(archive.read_bytes()).hexdigest()
assert digest == fields[0], f"installer SHA-256 {digest} does not match the recorded {fields[0]}"
INNER
fi
python3 "$ROOT/scripts/archive_audit_selftest.py"
python3 "$ROOT/scripts/archive_audit.py" --check
python3 "$ROOT/development/merged-source/tests/test_ordered_operation_patch.py"
python3 "$ROOT/scripts/package_unified_mod.py" --verify "$ROOT/MiusMultiplayer_OUTDATED_INSTALL.zip"
# Check the retired 1.0.1 kit's exact released payloads in a disposable fixture.
python3 "$ROOT/scripts/verify_legacy_fps_fixture_selftest.py"
python3 "$ROOT/scripts/package_gtmp_alpha.py" --verify "$ROOT/GTMP_alpha_INSTALL.zip"
"$ROOT/scripts/test_native.sh"

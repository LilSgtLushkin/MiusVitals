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
if [ -f "$ROOT/MiusMultiplayer_v1.0.7_TEST_INSTALL.zip" ]; then
    # The released 1.0.7 remains historical; new experimental sources intentionally differ.
    python3 "$ROOT/scripts/package_net_v21_test.py" --verify "$ROOT/MiusMultiplayer_v1.0.7_TEST_INSTALL.zip"
    python3 - "$ROOT/MiusMultiplayer_v1.0.7_TEST_INSTALL.zip" <<'PY'
import hashlib, pathlib, sys
assert hashlib.sha256(pathlib.Path(sys.argv[1]).read_bytes()).hexdigest() == '65f7a325095577cec98b2bab590f4674b81ad240bc5191adce346ebf8bec5ec6'
PY
fi
if [ -f "$ROOT/MiusMultiplayer_v1.0.7_EXPERIMENTAL_INSTALL.zip" ]; then
    python3 "$ROOT/scripts/package_net_v21_test.py" --verify "$ROOT/MiusMultiplayer_v1.0.7_EXPERIMENTAL_INSTALL.zip"
    python3 - "$ROOT/MiusMultiplayer_v1.0.7_EXPERIMENTAL_INSTALL.zip" <<'PY'
import hashlib, pathlib, sys
assert hashlib.sha256(pathlib.Path(sys.argv[1]).read_bytes()).hexdigest() == '60e7b7a2f2914ae75cf203f48aacc89e9469f71b15b654f70805d50e50736f78'
PY
fi
if [ -f "$ROOT/MiusMultiplayer_v1.0.7_EXPERIMENTAL_NET+FPS_INSTALL.zip" ]; then
    # Preserve the previously tested archive; it is no longer a build of the current source.
    python3 "$ROOT/scripts/package_net_v21_test.py" --verify "$ROOT/MiusMultiplayer_v1.0.7_EXPERIMENTAL_NET+FPS_INSTALL.zip"
    python3 - "$ROOT/MiusMultiplayer_v1.0.7_EXPERIMENTAL_NET+FPS_INSTALL.zip" <<'PY'
import hashlib, pathlib, sys
assert hashlib.sha256(pathlib.Path(sys.argv[1]).read_bytes()).hexdigest() == 'd733efa9f085f65d1f5db36bf8b004e5945329625d0bb6b468590d936b490bb8'
PY
fi
if [ -f "$ROOT/MiusMultiplayer_v1.0.7_EXPERIMENTAL_NET+FPS_CURRENT_INSTALL.zip" ]; then
    # The rolling installer published from the gated source. The packager proves the kit inside is
    # complete and consistent; the recorded hash proves this copy is the one that was built, so a
    # truncated or rewritten file cannot pass. The hash is not pinned in check_artifacts.py: it
    # would be written by the same job that builds the archive and would prove nothing.
    python3 "$ROOT/scripts/package_net_v21_test.py" --verify \
        "$ROOT/MiusMultiplayer_v1.0.7_EXPERIMENTAL_NET+FPS_CURRENT_INSTALL.zip"
    python3 - "$ROOT/MiusMultiplayer_v1.0.7_EXPERIMENTAL_NET+FPS_CURRENT_INSTALL.zip" <<'PY'
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
PY
fi
python3 "$ROOT/scripts/archive_audit_selftest.py"
python3 "$ROOT/scripts/archive_audit.py" --check
python3 "$ROOT/development/merged-source/tests/test_ordered_operation_patch.py"
python3 "$ROOT/scripts/package_unified_mod.py" --verify "$ROOT/MiusMultiplayer_OUTDATED_INSTALL.zip"
# Check the retired 1.0.1 kit's exact released payloads in a disposable fixture.
python3 "$ROOT/scripts/verify_legacy_fps_fixture_selftest.py"
python3 "$ROOT/scripts/package_gtmp_alpha.py" --verify "$ROOT/GTMP_alpha_INSTALL.zip"
"$ROOT/scripts/test_native.sh"

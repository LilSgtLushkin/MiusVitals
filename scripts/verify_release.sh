#!/usr/bin/env sh
# Verify shipped hashes and PE import/export surfaces, then run the portable safety tests.
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
python3 "$ROOT/development/kit-source/src/verify_artifacts.py" --kit "$ROOT/development/kit-source"
python3 "$ROOT/development/merged-source/verify_workspace.py"
python3 "$ROOT/scripts/check_artifacts.py"
python3 "$ROOT/scripts/check_artifacts_selftest.py"
python3 "$ROOT/scripts/package_net_v21_test_selftest.py"
if [ -f "$ROOT/MiusMultiplayer_v1.0.5_TEST_INSTALL.zip" ]; then
    python3 "$ROOT/scripts/package_net_v21_test.py" --verify "$ROOT/MiusMultiplayer_v1.0.5_TEST_INSTALL.zip"
fi
if [ -f "$ROOT/MiusMultiplayer_v1.0.6_TEST_INSTALL.zip" ]; then
    python3 "$ROOT/scripts/package_net_v21_test.py" --verify "$ROOT/MiusMultiplayer_v1.0.6_TEST_INSTALL.zip"
fi
if [ -f "$ROOT/MiusMultiplayer_v1.0.7_TEST_INSTALL.zip" ]; then
    # The released 1.0.7 remains historical; new experimental sources intentionally differ.
    python3 "$ROOT/scripts/package_net_v21_test.py" --verify "$ROOT/MiusMultiplayer_v1.0.7_TEST_INSTALL.zip"
    python3 - "$ROOT/MiusMultiplayer_v1.0.7_TEST_INSTALL.zip" <<'PY'
import hashlib, pathlib, sys
assert hashlib.sha256(pathlib.Path(sys.argv[1]).read_bytes()).hexdigest() == '65f7a325095577cec98b2bab590f4674b81ad240bc5191adce346ebf8bec5ec6'
PY
fi
python3 "$ROOT/scripts/archive_audit_selftest.py"
python3 "$ROOT/scripts/archive_audit.py" --check
python3 "$ROOT/development/merged-source/tests/test_ordered_operation_patch.py"
python3 "$ROOT/scripts/package_unified_mod.py" --verify "$ROOT/MiusMultiplayer_OUTDATED_INSTALL.zip"
python3 "$ROOT/scripts/package_unified_fps_mod.py" --verify "$ROOT/MiusMultiplayer_v1.0.1_INSTALL.zip"
python3 "$ROOT/scripts/package_gtmp_alpha.py" --verify "$ROOT/GTMP_alpha_INSTALL.zip"
"$ROOT/scripts/test_native.sh"

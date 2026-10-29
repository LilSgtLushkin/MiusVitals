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
    # Structure plus the SHA-256 recorded beside it. The hash is not pinned in check_artifacts.py:
    # it is recorded by the same job that builds the archive, so a pin would prove nothing.
    python3 "$ROOT/scripts/check_shipped_installer.py" \
        "$ROOT/MiusMultiplayer_v1.0.7_EXPERIMENTAL_NET+FPS_INSTALL.zip"
fi
python3 "$ROOT/scripts/archive_audit_selftest.py"
python3 "$ROOT/scripts/archive_audit.py" --check
python3 "$ROOT/development/merged-source/tests/test_ordered_operation_patch.py"
python3 "$ROOT/scripts/package_unified_mod.py" --verify "$ROOT/MiusMultiplayer_OUTDATED_INSTALL.zip"
# Check the retired 1.0.1 kit's exact released payloads in a disposable fixture.
python3 "$ROOT/scripts/verify_legacy_fps_fixture_selftest.py"
python3 "$ROOT/scripts/package_gtmp_alpha.py" --verify "$ROOT/GTMP_alpha_INSTALL.zip"
"$ROOT/scripts/test_native.sh"

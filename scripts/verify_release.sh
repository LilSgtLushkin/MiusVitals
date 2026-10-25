#!/usr/bin/env sh
# Verify shipped hashes and PE import/export surfaces, then run the portable safety tests.
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
python3 "$ROOT/development/kit-source/src/verify_artifacts.py" --kit "$ROOT/development/kit-source"
python3 "$ROOT/development/merged-source/verify_workspace.py"
python3 "$ROOT/scripts/package_unified_mod.py" --verify "$ROOT/MiusMultiplayer_v1.0.0_INSTALL.zip"
python3 "$ROOT/scripts/package_fps_beta.py" --verify "$ROOT/MiusMultiplayer_beta_FPS_fix_INSTALL.zip"
"$ROOT/scripts/test_native.sh"

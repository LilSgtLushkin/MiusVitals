#!/usr/bin/env sh
# Verify shipped hashes and PE import/export surfaces, then run the portable safety tests.
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
python3 "$ROOT/development/kit-source/src/verify_artifacts.py" --kit "$ROOT/development/kit-source"
python3 "$ROOT/scripts/package_kit.py" --verify "$ROOT/MiusVitals_v7.3.0_INSTALL.zip"
"$ROOT/scripts/test_native.sh"

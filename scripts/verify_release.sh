#!/usr/bin/env sh
# Verify shipped hashes and PE import/export surfaces, then run the portable safety tests.
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
python3 "$ROOT/MiusVitals/src/verify_artifacts.py" --kit "$ROOT/MiusVitals"
python3 "$ROOT/scripts/package_kit.py" --verify "$ROOT/MiusVitals_kit_7.1.0.zip"
"$ROOT/scripts/test_native.sh"

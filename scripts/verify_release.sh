#!/usr/bin/env sh
# Verify shipped hashes and PE import/export surfaces, then run the portable safety tests.
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
python3 "$ROOT/MiusVitals/src/verify_artifacts.py" --kit "$ROOT/MiusVitals"
"$ROOT/scripts/test_native.sh"

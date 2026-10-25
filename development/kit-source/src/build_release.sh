#!/usr/bin/env sh
# Recreate all codelib variants in the distributable kit from an original
# MiusPlayer v20 basic_network.codelib, then compile the v7 DLL.
set -eu

HERE=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
KIT=$(CDPATH= cd -- "$HERE/.." && pwd)
REPO=$(CDPATH= cd -- "$KIT/../.." && pwd)
PYTHON=${PYTHON:-python3}
ZIG_PYTHON=${ZIG_PYTHON:-$PYTHON}
INPUT=${1:?"usage: build_release.sh /path/to/original/basic_network.codelib"}
WORK=$(mktemp -d "${TMPDIR:-/tmp}/miusvitals-release.XXXXXX")
trap 'rm -rf "$WORK"' EXIT HUP INT TERM

mkdir -p "$KIT/files/alt"
"$PYTHON" "$HERE/rebase_3598_1.py" "$INPUT" "$WORK/rebased.codelib"
"$PYTHON" "$HERE/patch_import.py" "$WORK/rebased.codelib" "$KIT/files/basic_network.codelib"
"$PYTHON" "$HERE/rebase_3598_1.py" --no-start "$INPUT" "$WORK/rebased-nostart.codelib"
"$PYTHON" "$HERE/patch_import.py" "$WORK/rebased-nostart.codelib" "$KIT/files/alt/basic_network_nostart.codelib"
"$PYTHON" "$HERE/patch_import.py" "$INPUT" "$KIT/files/alt/basic_network_v5_norebase.codelib"
OUT="$KIT/files/mp_vitals.dll" ZIG_PYTHON="$ZIG_PYTHON" "$HERE/build.sh"

"$PYTHON" "$HERE/verify_artifacts.py" --kit "$KIT"
"$PYTHON" "$REPO/scripts/package_kit.py" --output "$REPO/MiusVitals_v7.5.0_INSTALL.zip"

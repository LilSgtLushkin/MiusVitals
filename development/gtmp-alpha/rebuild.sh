#!/usr/bin/env sh
# Rebuild the experimental GTMP_alpha mp_vitals.dll and prove the toolchain is the reviewed one.
#
#   rebuild.sh                       build development/gtmp-alpha/files/mp_vitals.dll from the current source
#   rebuild.sh --baseline <rev>      build the source of <rev> and compare it with the stable v1.0.1 hash
#
# The stable v1.0.1 DLL (71a1bf3a...) must come out of the source as it was at the commit this work
# branched from; the current source builds the alpha DLL.  Both runs use the same build.sh, so a
# difference means the compiler/toolchain changed, not the code.
set -eu

HERE=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
ROOT=$(CDPATH= cd -- "$HERE/../.." && pwd)
BUILD="$ROOT/development/kit-source/src/build.sh"
PYTHON=${ZIG_PYTHON:-python3}

EXPECTED_STABLE=71a1bf3a37923bbe6c8ee84448d0b91705c2bff7382b647f990b9d671ee224df
EXPECTED_ALPHA=808e1d5f6cb301ac971700e0aba6e78d1df1b13b02e748b8d01dd4bfc8a9881c

hash_of() {
    "$PYTHON" -c 'import hashlib,sys; print(hashlib.sha256(open(sys.argv[1], "rb").read()).hexdigest())' "$1"
}

build_source() {                       # build_source <source.c> <output.dll>
    src=$1
    out=$2
    work=$(mktemp -d "${TMPDIR:-/tmp}/gtmp-alpha.XXXXXX")
    cp "$BUILD" "$work/build.sh"
    cp "$src" "$work/mp_vitals.c"
    # The DLL embeds its own module name (the crash-guard text names mp_vitals.dll), so the build
    # output must be called mp_vitals.dll: any other file name changes the hash.
    OUT="$work/mp_vitals.dll" ZIG_PYTHON="$PYTHON" sh "$work/build.sh" >/dev/null
    cp "$work/mp_vitals.dll" "$out"
    rm -rf "$work"
}

if [ "${1:-}" = "--baseline" ]; then
    rev=${2:?usage: rebuild.sh --baseline <git-revision>}
    src=$(mktemp "${TMPDIR:-/tmp}/mp_vitals-baseline.XXXXXX.c")
    git -C "$ROOT" show "$rev:development/kit-source/src/mp_vitals.c" > "$src"
    out=$(mktemp "${TMPDIR:-/tmp}/mp_vitals-baseline.XXXXXX.dll")
    build_source "$src" "$out"
    found=$(hash_of "$out")
    rm -f "$src" "$out"
    echo "baseline $rev: $found"
    if [ "$found" != "$EXPECTED_STABLE" ]; then
        echo "MISMATCH: expected the stable v1.0.1 DLL $EXPECTED_STABLE" >&2
        exit 1
    fi
    echo "OK: the reviewed toolchain still reproduces the stable v1.0.1 DLL"
    exit 0
fi

OUT="$HERE/files/mp_vitals.dll"
build_source "$ROOT/development/kit-source/src/mp_vitals.c" "$OUT"
found=$(hash_of "$OUT")
echo "alpha:  $found  $OUT"
if [ "$found" != "$EXPECTED_ALPHA" ]; then
    echo "MISMATCH: expected the checked-in alpha DLL $EXPECTED_ALPHA" >&2
    exit 1
fi
echo "OK: matches the checked-in GTMP_alpha DLL"

#!/usr/bin/env sh
# Compile the portable logic tests with strict warnings and Address/Undefined sanitizers.
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
OUT=${TMPDIR:-/tmp}/miusvitals-native-test
CC=${CC:-cc}
"$CC" -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined -fno-omit-frame-pointer -g \
    -I "$ROOT/development/kit-source/src/test/stub" \
    -o "$OUT" "$ROOT/development/kit-source/src/test/t.c"
"$OUT"

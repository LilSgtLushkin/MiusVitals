#!/usr/bin/env sh
# Compile the portable logic tests with strict warnings and Address/Undefined sanitizers.
# The vitals module they exercise is parked in development/forbidden until its crashes are
# understood, so the test lives beside its source there rather than in a kit nobody ships.
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
OUT=${TMPDIR:-/tmp}/miusvitals-native-test
CC=${CC:-cc}
VITALS="$ROOT/development/forbidden/development/kit-source/src"
"$CC" -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined -fno-omit-frame-pointer -g \
    -I "$VITALS/test/stub" \
    -o "$OUT" "$VITALS/test/t.c"
"$OUT"

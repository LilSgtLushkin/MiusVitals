#!/bin/sh
# Type-check - and link - the module on a machine that has no MSVC and no game: zig's clang, aimed at
# x86_64-windows-gnu, with mingw's headers and tools/syntax/natupnp.h standing in for the SDK's.
#
#   python3 -m pip install --target /tmp/zig ziglang==0.17.0
#   ZIG=/tmp/zig/ziglang/zig tools/syntax/check.sh
#
# This is not the build: MSVC on CI is, and the module is linked into the game's codelib there.  What
# this catches is what a blind change gets wrong most often - a name, a type, a signature, a missing
# include, a function that nothing defines - in three seconds a file instead of two minutes a push.
set -e
cd "$(dirname "$0")/../.."
ZIG=${ZIG:-zig}
MODULE="basic_network.cpp netlog.cpp hooks.cpp render_guard.cpp vitals.cpp session.cpp ui.cpp state.cpp statesync.cpp statesync_legacy.cpp shots.cpp
trench.cpp xlate.cpp steam.cpp steam_module.cpp"
PROGRAMS="steam_logger.cpp mp_join.cpp protocol_selftest.cpp opsave_test.cpp netlog_selftest.cpp
steam_selftest.cpp steam_friends_test.cpp atomic_file_selftest.cpp battle_file_test.cpp retry_gate_test.cpp transform_gc_test.cpp"
FLAGS="-target x86_64-windows-gnu -std=c++17 -fms-extensions -c -Wno-unknown-attributes
-Wno-pointer-bool-conversion -Wno-unused-value -Wno-nontrivial-memcall
-Dmin(a,b)=((a)<(b)?(a):(b)) -Dmax(a,b)=((a)>(b)?(a):(b)) -Isrc -Itools/syntax"
OUT=${OUT:-/tmp/net-v21-check}
mkdir -p "$OUT"
export ZIG_GLOBAL_CACHE_DIR=${ZIG_GLOBAL_CACHE_DIR:-$OUT/zig-global}
export ZIG_LOCAL_CACHE_DIR=${ZIG_LOCAL_CACHE_DIR:-$OUT/zig-local}

fail=0
objs=""
for f in $MODULE; do
    [ -f "src/$f" ] || { echo "missing src/$f"; fail=1; continue; }
    o="$OUT/$(basename "$f" .cpp).obj"
    rc=0
    "$ZIG" c++ $FLAGS -o "$o" "src/$f" >"$OUT/compile.log" 2>&1 || rc=$?
    out=$(grep -v '^zig: warning' "$OUT/compile.log" || true)
    if [ "$rc" -ne 0 ] || [ -n "$out" ]; then
        echo "== src/$f (compiler exit $rc)"
        [ -z "$out" ] || printf '%s\n' "$out" | head -25
        fail=1
    else
        objs="$objs $o"
    fi
done
for f in $PROGRAMS; do
    [ -f "src/$f" ] || continue
    rc=0
    "$ZIG" c++ $FLAGS -o "$OUT/$(basename "$f" .cpp).obj" "src/$f" >"$OUT/compile.log" 2>&1 || rc=$?
    out=$(grep -v '^zig: warning' "$OUT/compile.log" || true)
    if [ "$rc" -ne 0 ] || [ -n "$out" ]; then
        echo "== src/$f (compiler exit $rc)"
        [ -z "$out" ] || printf '%s\n' "$out" | head -25
        fail=1
    fi
done

# Tests that expose additional production hooks must be compiled with the test define. They are
# not linked into the module and do not change the production build.
for f in render_guard_test.cpp session_loopback_peer.cpp; do
    rc=0
    "$ZIG" c++ $FLAGS -DNETV21_TEST_HOOKS -o "$OUT/$(basename "$f" .cpp).obj" "src/$f" >"$OUT/compile.log" 2>&1 || rc=$?
    out=$(grep -v '^zig: warning' "$OUT/compile.log" || true)
    if [ "$rc" -ne 0 ] || [ -n "$out" ]; then
        echo "== src/$f (compiler exit $rc)"
        [ -z "$out" ] || printf '%s\n' "$out" | head -25
        fail=1
    fi
done

# The module is a DLL the game loads: every symbol it calls has to exist somewhere, and nothing may
# need a library the game does not load.
if [ $fail = 0 ]; then
    rc=0
    "$ZIG" c++ -target x86_64-windows-gnu -shared -o "$OUT/basic_network.check.dll" $objs \
        -lole32 -loleaut32 -lws2_32 -luser32 -lgdi32 >"$OUT/link.log" 2>&1 || rc=$?
    out=$(grep -v '^zig: warning' "$OUT/link.log" || true)
    if [ "$rc" -ne 0 ] || [ -n "$out" ]; then
        echo "== link (compiler exit $rc)"
        [ -z "$out" ] || printf '%s\n' "$out" | head -25
        fail=1
    fi
fi
[ $fail = 0 ] && echo "check: 15 module files compile and link ($OUT/basic_network.check.dll)"
exit $fail

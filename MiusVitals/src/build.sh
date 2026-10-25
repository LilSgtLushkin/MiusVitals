#!/usr/bin/env sh
# Build the x64 Windows DLL with Zig's clang/lld front end. No C runtime is linked:
# the resulting DLL imports only the small KERNEL32 API surface declared below.
set -eu

HERE=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
PYTHON=${ZIG_PYTHON:-python3}
OUT=${OUT:-"$HERE/../files/mp_vitals.dll"}
WORK=$(mktemp -d "${TMPDIR:-/tmp}/miusvitals-build.XXXXXX")
trap 'rm -rf "$WORK"' EXIT HUP INT TERM
mkdir -p "$(dirname -- "$OUT")"

cat > "$WORK/k32.def" <<'DEF'
LIBRARY KERNEL32.dll
EXPORTS
DisableThreadLibraryCalls
EnterCriticalSection
FlushInstructionCache
GetCurrentProcess
GetEnvironmentVariableA
GetModuleHandleA
GetTickCount
InitializeCriticalSection
LeaveCriticalSection
VirtualAlloc
VirtualFree
VirtualProtect
VirtualQuery
DEF

"$PYTHON" -m ziglang dlltool -d "$WORK/k32.def" -l "$WORK/kernel32.lib" -m i386:x86-64
"$PYTHON" -m ziglang cc -target x86_64-windows-gnu -O2 -Wall -Wextra -fno-builtin -fno-stack-protector \
    -c -o "$WORK/mp_vitals.obj" "$HERE/mp_vitals.c"
"$PYTHON" -m ziglang lld-link /dll /entry:DllMain /nodefaultlib /export:MpVitalsInit \
    /out:"$OUT" /opt:ref /nxcompat /dynamicbase /base:0x7a0000000 /ignore:4281 \
    /implib:"$WORK/mp_vitals.lib" "$WORK/mp_vitals.obj" "$WORK/kernel32.lib"

printf '%s\n' "built $OUT"

#!/usr/bin/env sh
# Fetch the three inputs the recovery needs.  They are binaries of the published (CC0) MiusPlayer
# project, so they are downloaded instead of being committed:
#
#   v20.codelib   8108bd1e...  the module players run (GTMPRelease1.zip, "network version 20")
#   v12.codelib   eff7ae12...  the build of the published source tree
#   *.obj                      MSVC objects of that same tree - they carry the C++ symbol table
#                              that gives v12 (and through it v20) real function names
#
# Usage: development/v20-recovery/fetch_inputs.sh [clone-dir]     (default: /tmp/miusplayer-upstream)
set -eu

HERE=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
WORK="$HERE/work"
CLONE=${1:-/tmp/miusplayer-upstream}

mkdir -p "$WORK"
if [ ! -d "$CLONE/.git" ]; then
    # Blobless clone: the repository carries large binaries and half a gigabyte of history.
    git clone --quiet --filter=blob:none --no-checkout https://github.com/VampyrCat/MiusPlayer.git "$CLONE"
fi
git -C "$CLONE" checkout --quiet HEAD -- GTMPRelease1.zip _mp_research/basic_network/build _mp_research/basic_network/build.bat

python3 - "$CLONE" "$WORK" <<'PY'
import sys, zipfile
from pathlib import Path
clone, work = Path(sys.argv[1]), Path(sys.argv[2])
with zipfile.ZipFile(clone / "GTMPRelease1.zip") as archive:
    (work / "v20.codelib").write_bytes(archive.read("mpmod1/files/bin/rel/x64/basic_network.codelib"))
    (work / "v20_mp_join.exe").write_bytes(archive.read("mpmod1/files/mp_join.exe"))
build = clone / "_mp_research/basic_network/build"
(work / "v12.codelib").write_bytes((build / "basic_network.codelib").read_bytes())
for obj in sorted(build.glob("*.obj")):
    (work / obj.name).write_bytes(obj.read_bytes())
print("copied inputs into", work)
PY

python3 - "$WORK" <<'PY'
import hashlib, sys
from pathlib import Path
EXPECTED = {
    "v20.codelib": "8108bd1e9af8080ef8b1050512b8311848f5a336acb49a7479388733b4ae5723",
    "v12.codelib": "eff7ae12c5f440dc91d0fccc9d9578022a383b0485da58793bffd217922f7af5",
}
work = Path(sys.argv[1])
for name, expected in EXPECTED.items():
    found = hashlib.sha256((work / name).read_bytes()).hexdigest()
    print(f"{name:14} {found} {'OK' if found == expected else 'MISMATCH expected ' + expected}")
PY

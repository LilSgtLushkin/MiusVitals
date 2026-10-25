#!/usr/bin/env python3
"""Validate the files that are shipped in a MiusVitals kit without external tools."""
import argparse
import hashlib
import struct
from pathlib import Path

EXPECTED_MODULES = {
    "files/basic_network.codelib": "485b655662bd062e16278f9d699a3097b49a08de1948d2500dd589703a3102a3",
    "files/alt/basic_network_nostart.codelib": "e4317e630a21198a55e0e86be4aa29c71fcada6065824c858acd0e2bdef8adf9",
    "files/alt/basic_network_v5_norebase.codelib": "d2e34581b42d0573547e221873d7538b64a413267e8ed388abeb46dd42b630d2",
}
EXPECTED_DLL = "f978c2caa28429a0a4c6648cbdf9368001a683d12aa33b5bc5f9b7ad98f0df08"
V20_SHA256 = "8108bd1e9af8080ef8b1050512b8311848f5a336acb49a7479388733b4ae5723"
REBASED_MAIN_SHA256 = "f8c89ed083fee9c94576abdb7be67ec7c17a556e5c1224448fa7c2e0ab1907ae"
REBASED_NOSTART_SHA256 = "affe19a1f2fa20636b05e13515ca85a62c8a905ab77d2cede422d26b7c010aeb"
# Steam 3598/1 moves the operation Next Turn handler from +0x36b40 to +0x36b30.
# The rebased main variants must contain both the target address and its strict signature;
# otherwise basic_network deliberately disables shared operation turns at runtime.
MAP_NEXT_TURN_IMM_RVA = 0x1A327
MAP_NEXT_TURN_RVA = 0x36B30
MAP_NEXT_TURN_SIGNATURE_RVA = 0x48DA0
MAP_NEXT_TURN_SIGNATURE = bytes.fromhex("40534883ec404c8b81d8010000488bd9")
# In next_turn_hook(), a shared map used to require g_peer_on_map before the
# client had time to launch its separate map process.  This exact NOP sequence
# removes only that one predicate; the following g_connected predicate remains.
MAP_JOIN_LAUNCH_RACE_RVA = 0x15A77
MAP_JOIN_LAUNCH_RACE_PATCH = b"\x90" * 8
KERNEL32_IMPORTS = {
    "AddVectoredExceptionHandler", "DisableThreadLibraryCalls", "EnterCriticalSection", "FlushInstructionCache", "GetCurrentProcess",
    "GetEnvironmentVariableA", "GetModuleHandleA", "GetTickCount", "InitializeCriticalSection",
    "LeaveCriticalSection", "VirtualAlloc", "VirtualFree", "VirtualProtect", "VirtualQuery",
}


def sha256(data):
    return hashlib.sha256(data).hexdigest()


def pe(data):
    if data[:2] != b"MZ":
        raise ValueError("missing MZ header")
    offset, = struct.unpack_from("<I", data, 0x3C)
    if data[offset:offset + 4] != b"PE\0\0":
        raise ValueError("missing PE header")
    sections_count, = struct.unpack_from("<H", data, offset + 6)
    optional_size, = struct.unpack_from("<H", data, offset + 20)
    optional = offset + 24
    if struct.unpack_from("<H", data, optional)[0] != 0x20B:
        raise ValueError("not a PE32+ image")
    table = optional + optional_size
    sections = []
    for i in range(sections_count):
        at = table + i * 40
        name = data[at:at + 8].rstrip(b"\0").decode("ascii")
        vsize, va, raw_size, raw = struct.unpack_from("<IIII", data, at + 8)
        sections.append((name, va, max(vsize, raw_size), raw))

    def rva_to_offset(rva):
        for _, va, size, raw in sections:
            if va <= rva < va + size:
                return raw + rva - va
        raise ValueError("RVA %#x does not belong to a section" % rva)

    directories = optional + 112
    return sections, rva_to_offset, directories


def c_string(data, at):
    end = data.index(0, at)
    return data[at:end].decode("ascii")


def imports(data):
    _, rva_to_offset, directories = pe(data)
    rva, size = struct.unpack_from("<II", data, directories + 8)
    if not rva or not size:
        return {}
    at = rva_to_offset(rva)
    result = {}
    while any(data[at:at + 20]):
        original_first_thunk, _, _, name_rva, first_thunk = struct.unpack_from("<IIIII", data, at)
        dll = c_string(data, rva_to_offset(name_rva))
        thunk = rva_to_offset(original_first_thunk or first_thunk)
        symbols = []
        while True:
            value, = struct.unpack_from("<Q", data, thunk)
            if value == 0:
                break
            if value >> 63:
                symbols.append("#%d" % (value & 0xFFFF))
            else:
                symbols.append(c_string(data, rva_to_offset(value) + 2))
            thunk += 8
        result[dll.lower()] = symbols
        at += 20
    return result


def exports(data):
    _, rva_to_offset, directories = pe(data)
    rva, size = struct.unpack_from("<II", data, directories)
    if not rva or not size:
        return []
    at = rva_to_offset(rva)
    _, _, _, _, _, _, _, names, _, names_rva, _ = struct.unpack_from("<IIHHIIIIIII", data, at)
    names_at = rva_to_offset(names_rva)
    return [c_string(data, rva_to_offset(struct.unpack_from("<I", data, names_at + 4 * i)[0])) for i in range(names)]


def check_module(path, require_operation_map):
    data = path.read_bytes()
    sections, rva_to_offset, directories = pe(data)
    names = [name for name, *_ in sections]
    if ".mpimp" not in names:
        raise ValueError(".mpimp section is absent")
    import_rva, _ = struct.unpack_from("<II", data, directories + 8)
    if next(name for name, va, size, _ in sections if va <= import_rva < va + size) != ".mpimp":
        raise ValueError("the import table is not in .mpimp")
    imported = imports(data)
    if imported.get("mp_vitals.dll") != ["MpVitalsInit"]:
        raise ValueError("mp_vitals.dll!MpVitalsInit import is absent or unexpected")
    if require_operation_map:
        next_turn, = struct.unpack_from("<I", data, rva_to_offset(MAP_NEXT_TURN_IMM_RVA))
        if next_turn != MAP_NEXT_TURN_RVA:
            raise ValueError("operation Next Turn target is %#x, expected %#x" % (next_turn, MAP_NEXT_TURN_RVA))
        actual_signature = data[rva_to_offset(MAP_NEXT_TURN_SIGNATURE_RVA):rva_to_offset(MAP_NEXT_TURN_SIGNATURE_RVA) + len(MAP_NEXT_TURN_SIGNATURE)]
        if actual_signature != MAP_NEXT_TURN_SIGNATURE:
            raise ValueError("operation Next Turn signature is not the verified Steam 3598/1 handler")
        race_fix = data[rva_to_offset(MAP_JOIN_LAUNCH_RACE_RVA):rva_to_offset(MAP_JOIN_LAUNCH_RACE_RVA) + len(MAP_JOIN_LAUNCH_RACE_PATCH)]
        if race_fix != MAP_JOIN_LAUNCH_RACE_PATCH:
            raise ValueError("operation-map join-launch Next Turn wait is absent")


def check_rebase_reproducibility(kit):
    # The norebase alternative is an import-only v20 artifact.  Recover that
    # exact input and regenerate both rebased variants in memory, so the small
    # operation-map race edit is verified as a source transformation as well
    # as by the installed-artifact byte assertion above.
    import patch_import
    import rebase_3598_1

    original = patch_import.unpatch((kit / "files/alt/basic_network_v5_norebase.codelib").read_bytes())
    if sha256(original) != V20_SHA256:
        raise ValueError("the import-only module does not restore the v20 input")
    main = rebase_3598_1.rebase(original, start=True)
    nostart = rebase_3598_1.rebase(original, start=False)
    if sha256(main) != REBASED_MAIN_SHA256 or sha256(nostart) != REBASED_NOSTART_SHA256:
        raise ValueError("rebased operation-map modules are not reproducible from v20")


def check_dll(path):
    data = path.read_bytes()
    imported = imports(data)
    if set(imported) != {"kernel32.dll"}:
        raise ValueError("DLL imports unexpected libraries: " + repr(imported))
    if set(imported["kernel32.dll"]) != KERNEL32_IMPORTS:
        raise ValueError("unexpected KERNEL32 import surface")
    if exports(data) != ["MpVitalsInit"]:
        raise ValueError("DLL must export only MpVitalsInit")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--kit", type=Path, required=True)
    args = parser.parse_args()
    kit = args.kit.resolve()
    for relative, wanted in EXPECTED_MODULES.items():
        path = kit / relative
        actual = sha256(path.read_bytes())
        if actual != wanted:
            raise SystemExit(f"{relative}: sha256 {actual}, expected {wanted}")
        try:
            check_module(path, relative != "files/alt/basic_network_v5_norebase.codelib")
        except (ValueError, struct.error, UnicodeDecodeError) as error:
            raise SystemExit(f"{relative}: invalid PE/import layout: {error}")
        print(f"OK {relative}: {actual}")
    try:
        check_rebase_reproducibility(kit)
    except (ValueError, struct.error, UnicodeDecodeError) as error:
        raise SystemExit(f"rebased codelibs: not reproducible: {error}")
    print("OK rebased codelibs: reproduced from the exact v20 input, including join-launch wait")
    dll = kit / "files/mp_vitals.dll"
    actual_dll = sha256(dll.read_bytes())
    if actual_dll != EXPECTED_DLL:
        raise SystemExit(f"files/mp_vitals.dll: sha256 {actual_dll}, expected {EXPECTED_DLL}")
    try:
        check_dll(dll)
    except (ValueError, struct.error, UnicodeDecodeError) as error:
        raise SystemExit(f"files/mp_vitals.dll: invalid PE/import layout: {error}")
    print(f"OK files/mp_vitals.dll: {actual_dll}; KERNEL32 only; MpVitalsInit exported")


if __name__ == "__main__":
    main()

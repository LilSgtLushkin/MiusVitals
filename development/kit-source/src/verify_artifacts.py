#!/usr/bin/env python3
"""Validate the files that are shipped in a MiusVitals kit without external tools."""
import argparse
import hashlib
import struct
from pathlib import Path

EXPECTED_MODULES = {
    "files/basic_network.codelib": "4c91874c4b62db424ecca36586121bcb3b4cc7849cc900285e68de5151eddc36",
    "files/alt/basic_network_nostart.codelib": "f0766bec48696869d59d4a654a04f20bd7baf9f73adb6fbf139e7da31dfc1f4f",
    "files/alt/basic_network_v5_norebase.codelib": "d2e34581b42d0573547e221873d7538b64a413267e8ed388abeb46dd42b630d2",
}
EXPECTED_DLL = "f978c2caa28429a0a4c6648cbdf9368001a683d12aa33b5bc5f9b7ad98f0df08"
# Steam 3598/1 moves the operation Next Turn handler from +0x36b40 to +0x36b30.
# The rebased main variants must contain both the target address and its strict signature;
# otherwise basic_network deliberately disables shared operation turns at runtime.
MAP_NEXT_TURN_IMM_RVA = 0x1A327
MAP_NEXT_TURN_RVA = 0x36B30
MAP_NEXT_TURN_SIGNATURE_RVA = 0x48DA0
MAP_NEXT_TURN_SIGNATURE = bytes.fromhex("40534883ec404c8b81d8010000488bd9")
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

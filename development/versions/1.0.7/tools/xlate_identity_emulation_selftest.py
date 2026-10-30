#!/usr/bin/env python3
"""Emulate selected i_weapons lookup machine code with synthetic tables, without the game.

The code is from the exact user-supplied adv_game PE, but the object, strings and tables are
fabricated. Imported memcpy and the engine logger are stubbed. This checks local instruction
behaviour, not engine initialization, real identities, cross-machine equivalence, ABI or threads.
"""
from __future__ import annotations

import hashlib
from pathlib import Path
import struct
import unittest

from pe_scan import PE
from unicorn import Uc, UcError, UC_ARCH_X86, UC_MODE_64, UC_HOOK_CODE, UC_ERR_READ_UNMAPPED
from unicorn.x86_const import UC_X86_REG_RAX, UC_X86_REG_RBX, UC_X86_REG_RCX, UC_X86_REG_RDX
from unicorn.x86_const import UC_X86_REG_R8, UC_X86_REG_R10, UC_X86_REG_R13, UC_X86_REG_R14
from unicorn.x86_const import UC_X86_REG_RBP, UC_X86_REG_RDI, UC_X86_REG_RSI
from unicorn.x86_const import UC_X86_REG_RIP, UC_X86_REG_RSP, UC_X86_REG_XMM1

GAME = Path(__file__).resolve().parents[2] / "game-build-3598-1" / "x64" / "adv_game.codelib"
EXPECTED_SHA256 = "8f53c9018e7fc30a7e08daa16be5952a6aaec115412fa48245c2ee13046d1975"
RETURN = 0x10000000
OWNER = 0x11000000
STACK = 0x12000000
TEXT = 0x13000000
WEAPON_MAP = 0x14000000
SHELL_MAP = 0x15000000
WEAPONS = 0x16000000
SHELLS = 0x17000000


class Fixture:
    def __init__(self, entries):
        pe = PE(GAME)
        if hashlib.sha256(pe.data).hexdigest() != EXPECTED_SHA256:
            raise ValueError("the supplied adv_game reference image does not match")
        self.base = pe.image_base
        self.uc = Uc(UC_ARCH_X86, UC_MODE_64)
        self.uc.mem_map(self.base, 0x180000)
        for _name, rva, _vsize, raw, rawsize in pe.sections:
            self.uc.mem_write(self.base + rva, pe.data[raw:raw + rawsize])
        for address, size in ((RETURN, 0x1000), (OWNER, 0x1000), (STACK, 0x10000),
                              (TEXT, 0x1000), (WEAPON_MAP, 0x1000), (SHELL_MAP, 0x1000),
                              (WEAPONS, 0x1000), (SHELLS, 0x1000)):
            self.uc.mem_map(address, size)
        # Put the last valid row at the page boundary. Reading row[count] must fail instead
        # of accidentally appearing safe because spare memory happens to be mapped.
        weapon_map = self.table(WEAPON_MAP, entries)
        shell_map = self.table(SHELL_MAP, entries)
        self.write64(OWNER, self.base + 0x12e180)  # the actual i_weapons vtable
        self.write64(OWNER + 0xf0, WEAPONS)
        self.write32(OWNER + 0xfc, len(entries))
        self.write64(OWNER + 0x120, weapon_map)
        self.write32(OWNER + 0x12c, len(entries))
        self.write64(OWNER + 0x108, SHELLS)
        self.write32(OWNER + 0x114, len(entries))
        self.write64(OWNER + 0x138, shell_map)
        self.write32(OWNER + 0x144, len(entries))
        self.uc.hook_add(UC_HOOK_CODE, self.memcpy, begin=self.base + 0x109437,
                         end=self.base + 0x109437)
        self.uc.hook_add(UC_HOOK_CODE, self.log_stub, begin=self.base + 0x100710,
                         end=self.base + 0x100710)

    def write64(self, address, value):
        self.uc.mem_write(address, struct.pack("<Q", value))

    def write32(self, address, value):
        self.uc.mem_write(address, struct.pack("<I", value))

    def table(self, page, entries):
        if not entries or len(entries) * 0x44 > 0x1000:
            raise ValueError("fixture needs a small, nonempty index")
        if list(entries) != sorted(entries):
            raise ValueError("fixture keys must be sorted")
        start = page + 0x1000 - len(entries) * 0x44
        for i, (name, index) in enumerate(entries):
            data = name.encode("ascii")
            if len(data) > 61 or index >= len(entries):
                raise ValueError("fixture key or index out of range")
            row = bytearray(0x44)
            struct.pack_into("<H", row, 0, len(data))
            row[2:2 + len(data)] = data
            struct.pack_into("<I", row, 0x40, index)
            self.uc.mem_write(start + i * 0x44, bytes(row))
        return start

    @staticmethod
    def return_from_stub(uc):
        sp = uc.reg_read(UC_X86_REG_RSP)
        target = struct.unpack("<Q", uc.mem_read(sp, 8))[0]
        uc.reg_write(UC_X86_REG_RSP, sp + 8)
        uc.reg_write(UC_X86_REG_RIP, target)

    def memcpy(self, uc, _address, _size, _user):
        dst = uc.reg_read(UC_X86_REG_RCX)
        src = uc.reg_read(UC_X86_REG_RDX)
        count = uc.reg_read(UC_X86_REG_R8)
        if count > 61:
            raise ValueError("unexpected memcpy size from emulated lookup")
        uc.mem_write(dst, bytes(uc.mem_read(src, count)))
        uc.reg_write(UC_X86_REG_RAX, dst)
        self.return_from_stub(uc)

    def log_stub(self, uc, _address, _size, _user):
        self.return_from_stub(uc)  # no real engine logger is available in this fixture

    def call(self, rva, name=None, scalar=None, argument=TEXT, flags=0):
        if name is not None:
            encoded = name.encode("ascii") + b"\0"
            if len(encoded) > 0x1000:
                raise ValueError("fixture name too long")
            self.uc.mem_write(TEXT, encoded)
        sp = STACK + 0x8000 - 8  # Windows x64 function entry alignment and return address
        self.write64(sp, RETURN)
        for register, value in ((UC_X86_REG_RSP, sp), (UC_X86_REG_RCX, OWNER),
                                (UC_X86_REG_RDX, argument), (UC_X86_REG_R8, flags),
                                (UC_X86_REG_RAX, 0), (UC_X86_REG_RBX, 0)):
            self.uc.reg_write(register, value)
        if scalar is not None:
            self.uc.reg_write(UC_X86_REG_XMM1, struct.unpack("<I", struct.pack("<f", scalar))[0])
        self.uc.emu_start(self.base + rva, RETURN, count=20000)
        if self.uc.reg_read(UC_X86_REG_RIP) != RETURN:
            raise AssertionError("lookup did not return within the instruction limit")
        return self.uc.reg_read(UC_X86_REG_RAX)


class DescriptionIndexEmulation(unittest.TestCase):
    def test_membership_and_name_to_pointer(self):
        entries = [("alpha", 1), ("bravo", 0)]
        for membership, lookup, array, stride in ((0x6a440, 0x6c6e0, WEAPONS, 0x130),
                                                   (0x6a4d0, 0x6c790, SHELLS, 0x170)):
            with self.subTest(kind=hex(lookup)):
                self.assertEqual(Fixture(entries).call(membership, "alpha") & 0xff, 1)
                self.assertEqual(Fixture(entries).call(membership, "missing") & 0xff, 0)
                self.assertEqual(Fixture(entries).call(lookup, "alpha"), array + stride)
                self.assertEqual(Fixture(entries).call(lookup, "bravo"), array)

    def test_unknown_name_is_not_safe_to_pass_directly_to_lookup(self):
        entries = [("alpha", 1), ("bravo", 0)]
        for lookup, failing_rva in ((0x6c6e0, 0x6c769), (0x6c790, 0x6c819)):
            with self.subTest(kind=hex(lookup)):
                f = Fixture(entries)
                with self.assertRaises(UcError) as error:
                    f.call(lookup, "missing")
                self.assertEqual(error.exception.errno, UC_ERR_READ_UNMAPPED)
                self.assertEqual(f.uc.reg_read(UC_X86_REG_RIP), f.base + failing_rva)

    def test_names_with_same_first_61_bytes_cannot_be_distinguished(self):
        prefix = "x" * 61
        entries = [(prefix, 0)]
        for lookup, array in ((0x6c6e0, WEAPONS), (0x6c790, SHELLS)):
            with self.subTest(kind=hex(lookup)):
                self.assertEqual(Fixture(entries).call(lookup, prefix + "A"), array)
                self.assertEqual(Fixture(entries).call(lookup, prefix + "B"), array)

    def test_synthetic_index_can_select_equal_payloads_at_different_addresses(self):
        # Deliberately fabricated; this does not assert that the game loader produces identical
        # full records for different names. The name index alone selects distinct addresses.
        entries = [("cannon", 0), ("cannon1", 1)]
        f = Fixture(entries)
        first = f.call(0x6c6e0, "cannon")
        second = f.call(0x6c6e0, "cannon1")
        self.assertNotEqual(first, second)
        self.assertEqual(bytes(f.uc.mem_read(first, 0x130)), bytes(f.uc.mem_read(second, 0x130)))

    def test_divisions_named_descriptors_reach_one_launch_path(self):
        # This stitches together real instruction slices from i_divisions' configuration
        # and launch routines, with fabricated objects and names. It neither executes the
        # whole routine nor proves that every launch in a real battle uses these arrays.
        f = Fixture([("alpha", 0), ("bravo", 1)])
        division, carriers, fire = 0x18000000, 0x19000000, 0x1a000000
        shellphys, shell_vtable = 0x1b000000, 0x1c000000
        for address in (division, carriers, fire, shellphys, shell_vtable):
            f.uc.mem_map(address, 0x1000)
        f.write64(division + 0x118, OWNER)       # i_weapons dependency
        f.write64(division + 0x120, shellphys)   # i_shellphys dependency
        f.write64(division + 0x3e0, carriers)   # 0xa0-byte division records
        f.write64(shellphys, shell_vtable)
        f.write64(shell_vtable + 0x18, RETURN)
        row = carriers + 0xa0  # use a nonzero record index
        f.uc.reg_write(UC_X86_REG_RSP, STACK + 0x8000)
        f.uc.reg_write(UC_X86_REG_RBP, STACK + 0x7000)
        f.uc.reg_write(UC_X86_REG_R13, division)

        def run_slice(begin, end):
            f.uc.emu_start(f.base + begin, f.base + end, count=20000)
            self.assertEqual(f.uc.reg_read(UC_X86_REG_RIP), f.base + end)

        run_slice(0x83906, 0x83911)
        f.uc.reg_write(UC_X86_REG_RDX, 0xa0)
        run_slice(0x83930, 0x83936)  # select the synthetic configuration row
        self.assertEqual(f.uc.reg_read(UC_X86_REG_RDI), row)

        # The two actual i_weapons methods return entries from the synthetic named maps.
        f.uc.mem_write(TEXT, b"bravo\0")
        f.uc.reg_write(UC_X86_REG_RDX, TEXT)
        run_slice(0x839fb, 0x83a0c)  # lookup weapon by name -> carrier + 0x20
        f.uc.mem_write(TEXT, b"alpha\0")
        f.uc.reg_write(UC_X86_REG_RDX, TEXT)
        run_slice(0x83a1a, 0x83a2b)  # lookup shell by name -> carrier + 0x28
        self.assertEqual(struct.unpack("<Q", f.uc.mem_read(row + 0x20, 8))[0], WEAPONS + 0x130)
        self.assertEqual(struct.unpack("<Q", f.uc.mem_read(row + 0x28, 8))[0], SHELLS)
        # The alternative-shell loop looks up a name through a pointer and stores it in
        # another slot; give it a different synthetic shell to exercise the indexed path.
        f.uc.mem_write(TEXT + 0x100, b"bravo\0")
        f.write64(TEXT + 0x200, TEXT + 0x100)
        f.uc.reg_write(UC_X86_REG_RDX, TEXT + 0x200)
        f.uc.reg_write(UC_X86_REG_RBX, row + 0x30)
        run_slice(0x83afd, 0x83b10)
        self.assertEqual(struct.unpack("<Q", f.uc.mem_read(row + 0x30, 8))[0], SHELLS + 0x170)

        # Follow the separate firing path's record selection and its launch-record stores.
        f.write32(fire + 0x94, 1)  # synthetic carrier index
        f.write32(fire + 0x38, 1)  # synthetic shell choice
        f.uc.reg_write(UC_X86_REG_R14, division)
        f.uc.reg_write(UC_X86_REG_RDI, fire)
        run_slice(0x42dd1, 0x42de6)  # carrier = division + selected 0xa0-byte row
        self.assertEqual(f.uc.reg_read(UC_X86_REG_R13), row)
        run_slice(0x43443, 0x4345a)  # shell pointer from the selected row
        self.assertEqual(f.uc.reg_read(UC_X86_REG_RSI), SHELLS + 0x170)
        rbp = STACK + 0x7000
        f.uc.reg_write(UC_X86_REG_RBP, rbp)
        run_slice(0x438f5, 0x43901)
        record = rbp + 0x10
        self.assertEqual(struct.unpack("<Q", f.uc.mem_read(record + 0x38, 8))[0], SHELLS + 0x170)
        self.assertEqual(struct.unpack("<Q", f.uc.mem_read(record + 0x40, 8))[0], WEAPONS + 0x130)
        f.uc.reg_write(UC_X86_REG_R8, 0)
        run_slice(0x43954, 0x4398a)  # stop before calling slot 3: do not invoke a game function
        self.assertEqual(f.uc.reg_read(UC_X86_REG_RCX), shellphys)
        self.assertEqual(f.uc.reg_read(UC_X86_REG_RDX), record)
        self.assertEqual(f.uc.reg_read(UC_X86_REG_R10), shell_vtable)

    def test_weapon_enumerator_returns_separate_inline_names(self):
        f = Fixture([("cannon", 0), ("cannon1", 1)])
        f.uc.mem_write(WEAPONS + 0x42, b"cannon\0")
        f.uc.mem_write(WEAPONS + 0x130 + 0x42, b"cannon1\0")
        out = 0x18000000
        f.uc.mem_map(out, 0x1000)
        data = out + 0x100
        f.uc.mem_write(out, struct.pack("<QQQ", data, data, data + 16))
        f.call(0x6ca50, argument=out, flags=0)
        self.assertEqual(struct.unpack("<Q", f.uc.mem_read(out + 8, 8))[0], data + 16)
        self.assertEqual(struct.unpack("<QQ", f.uc.mem_read(data, 16)),
                         (WEAPONS + 0x42, WEAPONS + 0x130 + 0x42))

    def test_temperature_correction_mutates_the_fingerprinted_prefix(self):
        f = Fixture([("alpha", 0), ("bravo", 1)])
        other = SHELLS + 0x170
        f.uc.mem_write(SHELLS + 0x30, struct.pack("<4f", 1, 2, 3, 4))
        f.uc.mem_write(SHELLS + 0x20, struct.pack("<4f", 99, 99, 99, 99))
        f.uc.mem_write(other + 0x20, struct.pack("<4f", 88, 88, 88, 88))
        f.write32(other + 0xc0, 0x164)  # the tested flags exclude this synthetic shell
        f.call(0x6cb20, scalar=293.0)
        cold = bytes(f.uc.mem_read(SHELLS, 0x60))
        self.assertEqual(struct.unpack("<4f", cold[0x20:0x30]), (1, 2, 3, 4))
        f.call(0x6cb20, scalar=353.0)
        warm = bytes(f.uc.mem_read(SHELLS, 0x60))
        self.assertNotEqual(cold, warm)
        for actual, expected in zip(struct.unpack("<4f", warm[0x20:0x30]), (1.1, 2.2, 3.3, 4.4)):
            self.assertAlmostEqual(actual, expected, places=5)
        self.assertEqual(struct.unpack("<4f", f.uc.mem_read(other + 0x20, 16)), (88, 88, 88, 88))


if __name__ == "__main__":
    unittest.main(verbosity=2)

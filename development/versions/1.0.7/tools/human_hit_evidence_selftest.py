#!/usr/bin/env python3
"""Reproduce limited observations in the supplied physics image, without the game.

The human hit function and two direct helpers execute as supplied; selected tests also execute
the checked human slots +0x658 and +0x698. Other calls are stubbed, including engine callbacks,
so this cannot prove real-game reachability, callback behaviour, thread scheduling or that a
reported crash occurred here. Injected health changes or detachment in the tests are hypothetical:
they do not claim that a real callback or another thread makes those changes.
"""
from __future__ import annotations

import hashlib
from pathlib import Path
import struct
import unittest

from capstone import Cs, CS_ARCH_X86, CS_MODE_64
from capstone.x86 import X86_OP_IMM
from unicorn import Uc, UcError, UC_ARCH_X86, UC_MODE_64, UC_ERR_READ_UNMAPPED
from unicorn import UC_HOOK_CODE, UC_HOOK_MEM_READ, UC_HOOK_MEM_WRITE
from unicorn.x86_const import UC_X86_REG_RAX, UC_X86_REG_RCX, UC_X86_REG_RDX
from unicorn.x86_const import UC_X86_REG_R8, UC_X86_REG_R9, UC_X86_REG_RIP, UC_X86_REG_RSP

from pe_scan import PE, pointer_rva

GAME = Path(__file__).resolve().parents[2] / "game-build-3598-1" / "x64" / "adv_physic.codelib"
SHA256 = "c130dc38054a47b8cf1ddd9301835af552b45b96d252a528a9addd518ea3cf6e"
RETURN, STACK, HUMAN, RECORD = 0x10000000, 0x12000000, 0x13000000, 0x14000000
SQUAD, TABLE, SERVICE, SOURCE = 0x15000000, 0x16000000, 0x17000000, 0x18000000


class Fixture:
    def __init__(self):
        self.pe = PE(GAME)
        if hashlib.sha256(self.pe.data).hexdigest() != SHA256:
            raise ValueError("the supplied physics reference image does not match")
        self.base = self.pe.image_base
        self.decoded = {}
        decoder = Cs(CS_ARCH_X86, CS_MODE_64)
        decoder.detail = True
        # Calls outside the first two helpers and the two optional human slots are stubs.
        for start, end in ((0x66C90, 0x670CB), (0x66AF0, 0x66BDD), (0x67100, 0x678A8),
                           (0x75820, 0x7597D), (0x62900, 0x62908)):
            raw = self.pe.data[self.pe.offset(start):self.pe.offset(end)]
            for insn in decoder.disasm(raw, self.base + start):
                self.decoded[insn.address] = insn

    def run_hit(self, kind, part, flag, squad, source, inject_detachment=False, *,
                callback_health=None, getter=True, execute_human_slots=False, seed=0):
        u = Uc(UC_ARCH_X86, UC_MODE_64)
        u.mem_map(self.base, 0xF2000)
        for _name, rva, _vsize, raw, size in self.pe.sections:
            if rva + size <= 0xF2000:
                u.mem_write(self.base + rva, self.pe.data[raw:raw + size])
        for address, size in ((RETURN, 0x1000), (STACK, 0x10000), (HUMAN, 0x4000),
                              (RECORD, 0x1000), (SQUAD, 0x4000), (TABLE, 0x1000),
                              (SERVICE, 0x2000), (SOURCE, 0x4000)):
            u.mem_map(address, size)

        def write64(at, value):
            u.mem_write(at, struct.pack("<Q", value))

        def write32(at, value):
            u.mem_write(at, struct.pack("<I", value))

        def writef(at, value):
            u.mem_write(at, struct.pack("<f", value))

        # A fake world and interface table permit the real instructions to run; calls are
        # intercepted before entering unmodelled code. Nothing here is an engine simulation.
        for obj in (HUMAN, SQUAD, SERVICE, SOURCE):
            write64(obj, TABLE)
        for off in range(0, 0x1000, 8):
            write64(TABLE + off, RETURN)
        if execute_human_slots:
            write64(TABLE + 0x658, self.base + 0x75820)
            write64(TABLE + 0x698, self.base + 0x62900)
        for rva in range(0xD9B00, 0xD9D00, 8):
            write64(self.base + rva, SERVICE)
        writef(HUMAN + 0x7C8, 1.0)
        u.mem_write(HUMAN + 0x228C, bytes([flag]))
        u.mem_write(HUMAN + 0x228E, b"\x01")
        u.mem_write(HUMAN + 0x22C8, bytes([int(getter)]))
        write32(HUMAN + 0x2768, seed)
        write64(HUMAN + 0x2388, SQUAD if squad else 0)
        write64(HUMAN + 0x7D0, SQUAD if squad else 0)
        write32(HUMAN + 0x7D8, 0x20)
        write32(RECORD + 0x60, part)
        write64(RECORD + 0x68, SERVICE if source else 0)
        write64(RECORD + 0x70, SERVICE if source else 0)
        for offset in (0x44, 0x48, 0x54):
            writef(RECORD + offset, 0.5)
        sp = STACK + 0xF000 - 8  # x64 function-entry alignment
        write64(sp, RETURN)
        write32(sp + 0x28, 0)
        write32(sp + 0x30, 0)
        for register, value in ((UC_X86_REG_RSP, sp), (UC_X86_REG_RCX, HUMAN),
                                (UC_X86_REG_RDX, RECORD), (UC_X86_REG_R8, kind),
                                (UC_X86_REG_R9, SOURCE if source else 0)):
            u.reg_write(register, value)
        reads, calls, writes = [], [], []
        helpers = {self.base + 0x66AF0, self.base + 0x67100}

        def on_read(_u, _access, address, size, _value, _user):
            reads.append((address - RECORD, size))

        def on_write(uc, _access, address, size, _value, _user):
            writes.append((uc.reg_read(UC_X86_REG_RIP) - self.base, address - HUMAN, size))

        def on_code(uc, address, size, _user):
            insn = self.decoded.get(address)
            if insn is None:
                raise AssertionError("unexpected or undecoded instruction at %#x" % address)
            if insn.mnemonic != "call":
                return
            target = insn.operands[0].imm if insn.operands[0].type == X86_OP_IMM else None
            if target in helpers:
                return
            calls.append((address - self.base, target - self.base if target is not None else None))
            if execute_human_slots and address in (self.base + 0x6721D, self.base + 0x66ED9):
                return  # execute the checked human implementation, not a fake return value
            if address == self.base + 0x66ECC:
                if callback_health is not None:
                    # Hypothetical health-only change while the squad callback is in progress.
                    # The supplied instructions, not this injection, may then clear the pointers.
                    writef(HUMAN + 0x7C8, callback_health)
                if inject_detachment:
                    # Separate hypothetical Vitals-like detachment, not evidence of scheduling.
                    writef(HUMAN + 0x7C8, 0.25)
                    write64(HUMAN + 0x2388, 0)
                    write64(HUMAN + 0x7D0, 0)
            # Stub all other calls, including the squad and service callbacks.
            uc.reg_write(UC_X86_REG_RAX, int(getter) if address == self.base + 0x66ED9 else 0)
            uc.reg_write(UC_X86_REG_RIP, address + size)

        u.hook_add(UC_HOOK_CODE, on_code)
        u.hook_add(UC_HOOK_MEM_READ, on_read, begin=RECORD, end=RECORD + 0xFF)
        u.hook_add(UC_HOOK_MEM_WRITE, on_write, begin=HUMAN, end=HUMAN + 0x2FFF)
        error = None
        try:
            u.emu_start(self.base + 0x66C90, RETURN, count=200000)
        except UcError as exc:
            error = exc.errno
        rip = u.reg_read(UC_X86_REG_RIP)
        return error, rip, reads, calls, bytes(u.mem_read(HUMAN, 0x3000)), writes


class HumanHitEvidence(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.f = Fixture()

    def test_slot_points_to_checked_human_hit(self):
        off = self.f.pe.offset(0xC64D8 + 0x288)
        value = struct.unpack_from("<Q", self.f.pe.data, off)[0]
        self.assertEqual(pointer_rva(self.f.pe, value), 0x66C90)

    def test_checked_slots_and_kind8_helper_do_not_write_health_in_sampled_runs(self):
        for slot, expected in ((0x658, 0x75820), (0x698, 0x62900)):
            off = self.f.pe.offset(0xC64D8 + slot)
            value = struct.unpack_from("<Q", self.f.pe.data, off)[0]
            self.assertEqual(pointer_rva(self.f.pe, value), expected)
        for seed in (0, 1, 1000, 123456789, 0xFFFFFFFF):
            with self.subTest(seed=seed):
                run = self.f.run_hit(8, 0xFFFFFFFF, 0, True, False,
                                     execute_human_slots=True, seed=seed)
                self.assertIsNone(run[0])
                self.assertEqual(run[1], RETURN)
                self.assertIn((0x6721D, None), run[3])
                self.assertIn((0x66ED9, None), run[3])
                if seed in (0, 1000):
                    self.assertEqual(any(pc == 0x7596A for pc, _target in run[3]), seed == 0)
                self.assertEqual(struct.unpack_from("<f", run[4], 0x7C8)[0], 1.0)
                self.assertFalse(any(off < 0x7CC and off + size > 0x7C8
                                     for _pc, off, size in run[5]))

    def test_component_word_is_not_read_in_sampled_stubbed_paths(self):
        for kind in range(10):
            for flag in (0, 1):
                for squad in (False, True):
                    for source in (False, True):
                        with self.subTest(kind=kind, flag=flag, squad=squad, source=source):
                            a = self.f.run_hit(kind, 0xFFFFFFFF, flag, squad, source)
                            b = self.f.run_hit(kind, 0x12345678, flag, squad, source)
                            self.assertIsNone(a[0])
                            self.assertEqual(a[1], RETURN)
                            self.assertEqual(a, b)
                            self.assertFalse(any(offset < 0x64 and offset + size > 0x60
                                                 for offset, size in a[2]))

    def test_null_dereference_is_conditional_on_injected_side_effect(self):
        normal = self.f.run_hit(8, 0xFFFFFFFF, 0, True, False)
        hypothetical = self.f.run_hit(8, 0xFFFFFFFF, 0, True, False, inject_detachment=True)
        self.assertIsNone(normal[0])
        self.assertEqual(normal[1], RETURN)
        self.assertEqual(hypothetical[0], UC_ERR_READ_UNMAPPED)
        self.assertEqual(hypothetical[1], self.f.base + 0x66F60)
        self.assertEqual(struct.unpack_from("<Q", hypothetical[4], 0x2388)[0], 0)

    def test_health_only_injection_can_make_game_clear_its_own_squad_pointer(self):
        for health, getter, should_fault in ((0.25, True, True), (0.5, True, False),
                                             (0.25, False, False)):
            with self.subTest(health=health, getter=getter):
                run = self.f.run_hit(8, 0xFFFFFFFF, 0, True, False,
                                     callback_health=health, getter=getter,
                                     execute_human_slots=True)
                squad = struct.unpack_from("<Q", run[4], 0x2388)[0]
                other = struct.unpack_from("<Q", run[4], 0x7D0)[0]
                if should_fault:
                    self.assertEqual((run[0], run[1]), (UC_ERR_READ_UNMAPPED, self.f.base + 0x66F60))
                    self.assertEqual((squad, other), (0, 0))
                    self.assertIn((0x66F15, 0x2388, 8), run[5])
                    self.assertIn((0x66F1C, 0x7D0, 8), run[5])
                else:
                    self.assertIsNone(run[0])
                    self.assertEqual(run[1], RETURN)
                    self.assertEqual((squad, other), (SQUAD, SQUAD))
                    self.assertFalse(any(pc in (0x66F15, 0x66F1C) for pc, _off, _size in run[5]))


if __name__ == "__main__":
    unittest.main(verbosity=2)

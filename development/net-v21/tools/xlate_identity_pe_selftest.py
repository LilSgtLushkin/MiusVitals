#!/usr/bin/env python3
"""Recheck offline evidence about description identities in the supplied 3598/1 images.

This inspects pinned PE bytes/instructions, not a running game. It does not install hooks, call
engine methods, extract private logs, prove ABI/thread safety, or validate a proposed matcher.
Requires capstone (already installed by the Windows network-module CI).
"""
from __future__ import annotations

import hashlib
from pathlib import Path
import struct
import unittest

from capstone import Cs, CS_ARCH_X86, CS_MODE_64
from capstone.x86 import X86_OP_MEM, X86_REG_RIP
from pe_scan import PE, pointer_rva

GAME = Path(__file__).resolve().parents[2] / "game-build-3598-1" / "x64"
SHA256 = {
    "base_shell": "9606f69c38f997b7ae24a7152055f2bf612d5a38080d1e92be9c8bf0be7747e2",
    "adv_game": "8f53c9018e7fc30a7e08daa16be5952a6aaec115412fa48245c2ee13046d1975",
    "basic_physic": "a51aa13d94a89d776e12434a613b7bfa606525e50e473be8254b70995d98aa04",
}


class DescriptionIdentityEvidence(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.images = {name: PE(GAME / (name + ".codelib")) for name in SHA256}
        cls.md = Cs(CS_ARCH_X86, CS_MODE_64)
        cls.md.detail = True

    def instruction(self, name: str, rva: int):
        pe = self.images[name]
        self.assertTrue(pe.executable_rva(rva), f"{name}+{rva:#x} is not executable")
        offset = pe.offset(rva)
        self.assertIsNotNone(offset)
        ins = next(self.md.disasm(pe.data[offset:offset + 15], rva, count=1), None)
        self.assertIsNotNone(ins, f"no instruction at {name}+{rva:#x}")
        return ins

    def instructions(self, name: str, cases):
        for address, mnemonic, operands in cases:
            with self.subTest(module=name, rva=hex(address)):
                ins = self.instruction(name, address)
                self.assertEqual((ins.mnemonic, ins.op_str), (mnemonic, operands))

    def rip_string(self, name: str, address: int) -> bytes:
        ins = self.instruction(name, address)
        candidates = [op.mem.disp for op in ins.operands
                      if op.type == X86_OP_MEM and op.mem.base == X86_REG_RIP]
        self.assertEqual(len(candidates), 1)
        pe = self.images[name]
        target = ins.address + ins.size + candidates[0]
        offset = pe.offset(target)
        self.assertIsNotNone(offset)
        return pe.data[offset:offset + 128].split(b"\0", 1)[0]

    def slot(self, name: str, table: int, index: int, expected: int):
        pe = self.images[name]
        offset = pe.offset(table + index * 8)
        self.assertIsNotNone(offset)
        value = struct.unpack_from("<Q", pe.data, offset)[0]
        self.assertEqual(pointer_rva(pe, value), expected)

    def test_exact_images(self):
        for name, expected in SHA256.items():
            with self.subTest(module=name):
                self.assertEqual(hashlib.sha256(self.images[name].data).hexdigest(), expected)

    def test_manager_registration_and_lookup(self):
        # i_objman slots 55..57: register pointer, pointer -> ID, ID -> pointer.
        for slot, target in ((55, 0x12280), (56, 0x12120), (57, 0x121d0)):
            self.slot("base_shell", 0x4ca10, slot, target)
        self.instructions("base_shell", [
            (0x12147, "mov", "edi, dword ptr [rcx + 0x176c]"),
            (0x12156, "mov", "rsi, qword ptr [rcx + 0x1760]"),
            (0x12173, "cmp", "r10, qword ptr [rsi + rcx*8]"),
            (0x121ba, "mov", "r11d, dword ptr [rcx + 8]"),
            (0x121f1, "mov", "edi, dword ptr [rcx + 0x1754]"),
            (0x12200, "mov", "rsi, qword ptr [rcx + 0x1748]"),
            (0x12223, "cmp", "r10d, dword ptr [rsi + rcx*8]"),
            (0x1226c, "mov", "r11, qword ptr [rcx + 8]"),
            (0x1228d, "lea", "r14, [rcx + 0x1760]"),
            (0x122c3, "cmp", "rbx, qword ptr [r10 + rcx*8]"),
            (0x122f2, "jne", "0x1240e"),  # an already registered pointer skips insertion
            (0x1231b, "mov", "r10d, dword ptr [rdi + 0x1778]"),
            (0x1238b, "mov", "qword ptr [rax + rcx*8 + 8], rbx"),
            (0x123f1, "mov", "eax, dword ptr [rdi + 0x1778]"),
            (0x12404, "mov", "dword ptr [rcx + rdx*8 + 8], eax"),
            (0x12408, "inc", "dword ptr [rdi + 0x1778]"),
        ])

    def test_weapon_and_shell_name_indices(self):
        self.assertEqual(self.rip_string("adv_game", 0x6a5e3), b"i_weapons")
        self.assertEqual(self.rip_string("adv_game", 0x6a944), b"Reserve %u weapons")
        self.assertEqual(self.rip_string("adv_game", 0x6a960), b"Reserve %u shells")
        # Constructor installs this vtable; slots 8/9 test membership and 3/4 obtain a pointer.
        for slot, target in ((3, 0x6c6e0), (4, 0x6c790), (7, 0x6ca50),
                             (8, 0x6a440), (9, 0x6a4d0)):
            self.slot("adv_game", 0x12e180, slot, target)
        self.instructions("adv_game", [
            (0x6a5f7, "lea", "rax, [rip + 0xc3b82]"),  # points at the table above
            (0x6a603, "mov", "qword ptr [rbx + 0xf0], rax"),
            (0x6a611, "mov", "qword ptr [rbx + 0x108], rax"),
            (0x6a976, "lea", "rcx, [r14 + 0xf0]"),
            (0x6a989, "lea", "rcx, [r14 + 0x108]"),
            (0x6b595, "imul", "rdi, rax, 0x130"),
            (0x6b5ba, "call", "qword ptr [rax + 0x1b8]"),
            (0x6b5c0, "add", "rbx, 0x130"),
            (0x6c69c, "imul", "rdi, rax, 0x170"),
            (0x6c6ba, "call", "qword ptr [rax + 0x1b8]"),
            (0x6c6c0, "add", "rbx, 0x170"),
            (0x194f8, "imul", "rcx, rcx, 0x44"),
            (0x19635, "mov", "dword ptr [rdx + rcx + 0x40], r8d"),
            (0x6c70c, "cmp", "eax, 0x3d"),  # names are capped at 61 bytes
            (0x6c740, "lea", "rcx, [rdi + 0x120]"),
            (0x6c747, "call", "0x19650"),
            (0x6c74c, "mov", "ecx, dword ptr [rdi + 0x12c]"),
            (0x6c760, "cmovb", "ecx, eax"),
            (0x6c765, "imul", "rcx, rax, 0x44"),
            (0x6c769, "mov", "eax, dword ptr [rcx + rdx + 0x40]"),
            (0x6c76d, "imul", "rax, rax, 0x130"),
            (0x6c774, "add", "rax, qword ptr [rdi + 0xf0]"),
            (0x6c7bc, "cmp", "eax, 0x3d"),
            (0x6c7f0, "lea", "rcx, [rdi + 0x138]"),
            (0x6c7f7, "call", "0x19650"),
            (0x6c7fc, "mov", "ecx, dword ptr [rdi + 0x144]"),
            (0x6c810, "cmovb", "ecx, eax"),
            (0x6c815, "imul", "rcx, rax, 0x44"),
            (0x6c819, "mov", "eax, dword ptr [rcx + rdx + 0x40]"),
            (0x6c81d, "imul", "rax, rax, 0x170"),
            (0x6c824, "add", "rax, qword ptr [rdi + 0x108]"),
        ])

    def test_weapon_name_is_not_always_a_unique_descriptor_identity(self):
        self.assertEqual(self.rip_string("adv_game", 0x6bc44), b"Bad weapon name for shell '%C'")
        self.assertEqual(self.rip_string("adv_game", 0x6b3d4), b"%s%u")
        self.instructions("adv_game", [
            (0x6bc6f, "imul", "r9, rax, 0x130"),
            (0x6bc90, "mov", "rax, qword ptr [rdi + 0x80]"),
            (0x6ab45, "mov", "r14d, 2"),
            (0x6b2ed, "call", "0x194a0"),
            (0x6b335, "mov", "rdx, r14"),
            (0x6b347, "lea", "rax, [rbp - 0x50]"),  # original record's copy source
            (0x6b3a1, "jne", "0x6b350"),
            (0x6b3c5, "mov", "r12d, 2"),
            (0x6b3de, "lea", "rcx, [rbp - 0x10]"),  # source + 0x40: the inline key
            (0x6b40e, "mov", "eax, dword ptr [r13 + 0xfc]"),
            (0x6b423, "call", "0x194a0"),  # generated numbered name -> next index
            (0x6b450, "mov", "rdx, r12"),
            (0x6b453, "imul", "rcx, rax, 0x130"),
            (0x6b462, "lea", "rax, [rbp - 0x50]"),  # generated record copies same source
            (0x6b4b7, "jne", "0x6b466"),
            (0x6cab7, "lea", "rbx, [rsi + 0x42]"),  # separate inline weapon name
            (0x6cadc, "mov", "qword ptr [rdx], rbx"),
        ])

    def test_temperature_correction_changes_bytes_used_by_the_fingerprint(self):
        self.slot("adv_game", 0x12e180, 10, 0x6cb20)
        self.slot("adv_game", 0x12e180, 11, 0x6cbe0)
        self.assertEqual(self.rip_string("adv_game", 0x6cb59),
                         b"Velocity correction to %f%% by temp %f C")
        self.instructions("adv_game", [
            (0x6cb39, "call", "qword ptr [rax + 0x58]"),
            (0x6cb81, "mov", "rcx, qword ptr [rbx + 0x108]"),
            (0x6cb88, "mov", "eax, dword ptr [rbx + 0x114]"),
            (0x6cb8e, "imul", "rdx, rax, 0x170"),
            (0x6cba0, "test", "dword ptr [rcx + 0xc0], 0x164"),
            (0x6cbaf, "shufps", "xmm0, xmm0, 0"),
            (0x6cbb3, "mulps", "xmm0, xmmword ptr [rcx + 0x30]"),
            (0x6cbb7, "movups", "xmmword ptr [rcx + 0x20], xmm0"),
        ])

    def test_named_divisions_records_feed_one_launch_path(self):
        # i_divisions.generate loads named descriptors from i_weapons into 0xa0-byte
        # rows; one ctrl_tick path later pulls those pointers into the launch record.
        # This is evidence for that path, not all other callers or cross-install identity.
        self.assertEqual(self.rip_string("adv_game", 0x81ccf), b"i_divisions")
        self.assertEqual(self.rip_string("adv_game", 0x82079), b"generate")
        self.assertEqual(self.rip_string("adv_game", 0x82091), b"ctrl_tick")
        self.assertEqual(self.rip_string("adv_game", 0x82289), b"i_weapons")
        self.assertEqual(self.rip_string("adv_game", 0x822c5), b"i_shellphys")
        self.instructions("adv_game", [
            (0x82085, "lea", "rax, [rip - 0xc1c]"),  # generate -> 0x81470
            (0x8209d, "lea", "rax, [rip - 0xba4]"),  # ctrl_tick -> 0x81500
            (0x822a5, "lea", "rax, [rdi + 0x118]"),
            (0x822e1, "lea", "rax, [rdi + 0x120]"),
            (0x81470, "jmp", "0x82a30"),
            (0x82bd0, "call", "0x82bf0"),
            (0x8150e, "jmp", "0x85b00"),
            (0x8614b, "call", "0x42d40"),
            (0x82c56, "mov", "r13, rcx"),
            (0x837a1, "lea", "rcx, [r9 + r9*4]"),
            (0x837a5, "shl", "rcx, 5"),  # allocate count * 0xa0
            (0x83906, "lea", "r10, [r13 + 0x3e0]"),
            (0x83930, "mov", "rdi, qword ptr [r10]"),
            (0x83933, "add", "rdi, rdx"),
            (0x839fb, "mov", "rcx, qword ptr [r13 + 0x118]"),
            (0x83a05, "call", "qword ptr [rax + 0x18]"),  # named weapon lookup
            (0x83a08, "mov", "qword ptr [rdi + 0x20], rax"),
            (0x83a1a, "mov", "rcx, qword ptr [r13 + 0x118]"),
            (0x83a24, "call", "qword ptr [rax + 0x20]"),  # named shell lookup
            (0x83a27, "mov", "qword ptr [rdi + 0x28], rax"),
            (0x83a2e, "lea", "rbx, [rdi + 0x28]"),
            (0x83b0a, "call", "qword ptr [rax + 0x20]"),
            (0x83b0d, "mov", "qword ptr [rbx], rax"),
            (0x83b22, "add", "rbx, 8"),
            (0x83b2a, "cmp", "r15d, 5"),
            (0x42dae, "mov", "r14, rcx"),
            (0x42dd1, "mov", "ecx, dword ptr [rdi + 0x94]"),
            (0x42dd7, "lea", "r13, [rcx + rcx*4]"),
            (0x42ddb, "shl", "r13, 5"),
            (0x42ddf, "add", "r13, qword ptr [r14 + 0x3e0]"),
            (0x4344f, "mov", "rsi, qword ptr [r13 + rax*8 + 0x28]"),
            (0x438f5, "mov", "qword ptr [rbp + 0x48], rsi"),  # rec + 0x38
            (0x438f9, "mov", "rax, qword ptr [r13 + 0x20]"),
            (0x438fd, "mov", "qword ptr [rbp + 0x50], rax"),  # rec + 0x40
            (0x43954, "mov", "rcx, qword ptr [r14 + 0x120]"),
            (0x43986, "lea", "rdx, [rbp + 0x10]"),
            (0x4398a, "call", "qword ptr [r10 + 0x18]"),
        ])

    def test_launch_uses_fields_outside_existing_fingerprint_and_ammo_check(self):
        self.instructions("basic_physic", [
            (0x185c8, "mov", "rdi, qword ptr [rdx + 0x40]"),
            (0x185d4, "mov", "r15, qword ptr [rdx + 0x38]"),
            (0x185ef, "movzx", "eax, byte ptr [rdi + 0x88]"),
            (0x18657, "comiss", "xmm12, dword ptr [rdi + 0x90]"),
            (0x18720, "mov", "ecx, dword ptr [r15 + 0xc0]"),
            (0x18730, "shr", "eax, 0x1f"),
            (0x18889, "mov", "eax, dword ptr [r15 + 0xc4]"),
            (0x18890, "cmp", "eax, 0x54414548"),  # little-endian "HEAT"
            (0x188bc, "cmp", "eax, 0x47415246"),  # little-endian "FRAG"
        ])


if __name__ == "__main__":
    unittest.main(verbosity=2)

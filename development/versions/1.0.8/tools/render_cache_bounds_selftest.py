#!/usr/bin/env python3
"""Verify and emulate the actual x64 cache patch against the supplied Steam renderer PE.

The emitter template/offsets and verified signatures are read from production C++ source; the
PE instructions and allocator run in Unicorn. Objects, memory and matrix contents are synthetic.
This is not a game, a concurrency test, a timing benchmark, or a proof of allocation ownership.
"""
from __future__ import annotations

import hashlib
from pathlib import Path
import re
import struct
import unittest

from capstone import Cs, CS_ARCH_X86, CS_MODE_64
from unicorn import (Uc, UcError, UC_ARCH_X86, UC_MODE_64, UC_ERR_WRITE_PROT,
                     UC_ERR_WRITE_UNMAPPED, UC_PROT_READ, UC_HOOK_CODE)
from unicorn.x86_const import (UC_X86_REG_RAX, UC_X86_REG_RBX, UC_X86_REG_RCX, UC_X86_REG_RDX,
                               UC_X86_REG_RBP, UC_X86_REG_RSI, UC_X86_REG_RDI, UC_X86_REG_RSP,
                               UC_X86_REG_R8, UC_X86_REG_RIP, UC_X86_REG_XMM0, UC_X86_REG_XMM1)
from pe_scan import PE
from tactical_hook_selftest import cpp_signature, executable_hits

NET = Path(__file__).resolve().parent.parent
GAME = NET.parents[1] / 'game-build-3598-1/x64/basic_render.codelib'  # NET = versions/<version>
EXPECTED = 'ba8d68cf2b84b890dd88f6f3035c8c070070248e22c86c7481ba1262f8a3db11'
STUB, COUNTER = 0x10000000, 0x11000000
RENDER, GROUPS, RECORD, POOL = 0x12000000, 0x13000000, 0x14000000, 0x15000000
MATRIX, STACK, LIVE, FREE = 0x16000000, 0x17000000, 0x18000000, 0x19000000
OTHER = 0x1a000000


def production_stub(next_address: int, allocator: int, counter: int) -> bytes:
    text = (NET / 'src/render_cache_bounds.h').read_text(encoding='utf-8')
    match = re.search(r'k_stub\[\]\s*=\s*\{(.*?)\};', text, re.S)
    if not match:
        raise ValueError('production stub template not found')
    # Numeric addresses in human comments must not be mistaken for bytes.
    parts = re.sub(r'//[^\n]*', '', match.group(1))
    stub = bytearray(int(x, 16) for x in re.findall(r'0x([0-9a-fA-F]{2})', parts))
    enum = re.search(r'enum\s*\{([^}]+)\}', text).group(1)
    constants = dict((name, int(value)) for name, value in re.findall(r'(k_\w+)\s*=\s*(\d+)', enum))
    if len(stub) != constants['k_size']:
        raise ValueError('production stub size differs from builder constant')
    for name, value in (('k_continue_ptr', next_address), ('k_allocate_ptr', allocator), ('k_counter_ptr', counter)):
        struct.pack_into('<Q', stub, constants[name], value)
    return bytes(stub)


class EngineFixture:
    def __init__(self, slot: int, count: int = 4, capacity: int = 8, *, patched=True, free=None):
        pe = PE(GAME)
        if hashlib.sha256(pe.data).hexdigest() != EXPECTED:
            raise ValueError('renderer PE is not the supplied reference')
        self.base = pe.image_base
        self.uc = Uc(UC_ARCH_X86, UC_MODE_64)
        self.uc.mem_map(self.base, 0x64000)
        for _name, rva, _vsize, raw, rawsize in pe.sections:
            self.uc.mem_write(self.base + rva, pe.data[raw:raw + rawsize])
        for address, size in ((STUB, 0x1000), (COUNTER, 0x1000), (RENDER, 0x1000),
                              (GROUPS, 0x1000), (RECORD, 0x1000), (POOL, 0x10000),
                              (MATRIX, 0x1000), (STACK, 0x10000), (LIVE, 0x1000),
                              (FREE, 0x1000), (OTHER, 0x10000)):
            self.uc.mem_map(address, size)
        self.u64(RENDER + 0x240, GROUPS)
        self.u64(GROUPS, RECORD)
        self.u32(RECORD + 0x34, slot)
        self.u64(RENDER + 0x168, POOL)
        self.u32(RENDER + 0x174, count)
        self.u32(RENDER + 0x178, capacity)
        self.u64(RENDER + 0x138, LIVE)  # native allocator's live-slot bookkeeping
        self.u32(RENDER + 0x144, 0)
        self.u32(RENDER + 0x148, 100)
        if free is not None:
            self.u64(RENDER + 0x150, FREE)
            self.u32(RENDER + 0x15c, 1)
            self.u32(FREE, free)
        self.matrix = bytes(range(64))
        self.uc.mem_write(MATRIX, self.matrix)
        self.uc.reg_write(UC_X86_REG_RCX, RENDER)
        self.uc.reg_write(UC_X86_REG_RDX, 0)  # node 0
        self.uc.reg_write(UC_X86_REG_R8, MATRIX)
        self.uc.reg_write(UC_X86_REG_RSP, STACK + 0x8000 - 8)
        self.uc.reg_write(UC_X86_REG_RBX, 0x1234)
        if patched:
            self.uc.mem_write(STUB, production_stub(self.base + 0x1299e, self.base + 0x129c6, COUNTER))
            self.uc.mem_write(self.base + 0x1298f, b'\xff\x25\0\0\0\0' + struct.pack('<Q', STUB) + b'\x90')

    def u32(self, address, value):
        self.uc.mem_write(address, struct.pack('<I', value))

    def u64(self, address, value):
        self.uc.mem_write(address, struct.pack('<Q', value))

    def read32(self, address):
        return struct.unpack('<I', self.uc.mem_read(address, 4))[0]

    def run(self):
        self.uc.emu_start(self.base + 0x12925, self.base + 0x129da, count=1000)
        if self.uc.reg_read(UC_X86_REG_RIP) != self.base + 0x129da:
            raise AssertionError('renderer did not reach the end of the cache operation')


class CacheBoundsPE(unittest.TestCase):
    def test_reference_signatures_and_boundaries(self):
        pe = PE(GAME)
        self.assertEqual(hashlib.sha256(pe.data).hexdigest(), EXPECTED)
        e = struct.unpack_from('<I', pe.data, 0x3c)[0]
        self.assertEqual((pe.timestamp, struct.unpack_from('<I', pe.data, e + 24 + 56)[0]), (0x66f2d710, 0x64000))
        for name, rva in (('sig_cache_complete', 0x12925), ('sig_cache_rebuild', 0x129c6),
                          ('sig_cache_allocator', 0x193c0), ('sig_cache_growth', 0x19aa0)):
            with self.subTest(signature=name):
                sig = cpp_signature('render_guard.cpp', name)
                self.assertEqual(executable_hits(pe, sig), [rva])
        # The 15 displaced bytes occur twice; they are NOT enough to resolve a hook by
        # themselves. The unique full cache-function signature and fixed image locate the site.
        site = cpp_signature('render_guard.cpp', 'sig_cache_inline_site')
        self.assertEqual(executable_hits(pe, site), [0x1298f, 0x12cff])
        start, end = 0x1298f, 0x1299e
        code = pe.data[pe.offset(start):pe.offset(end)]
        insns = list(Cs(CS_ARCH_X86, CS_MODE_64).disasm(code, start))
        self.assertEqual([(i.address, i.mnemonic, i.op_str) for i in insns], [
            (0x1298f, 'and', 'edx, 0xffffff'), (0x12995, 'mov', 'rax, qword ptr [rdi + 0x168]'),
            (0x1299c, 'mov', 'ecx, edx')])
        self.assertEqual(insns[-1].address + insns[-1].size, end)
        self.assertEqual(pe.data[pe.offset(0x129a2):pe.offset(0x129a2)+4], b'\x0f\x29\x04\x01')

    def test_valid_write_matches_unpatched_engine(self):
        registers = (UC_X86_REG_RAX, UC_X86_REG_RBX, UC_X86_REG_RCX, UC_X86_REG_RDX,
                     UC_X86_REG_RBP, UC_X86_REG_RSI, UC_X86_REG_RDI, UC_X86_REG_RSP,
                     UC_X86_REG_R8, UC_X86_REG_XMM0, UC_X86_REG_XMM1)
        for slot in (0, 1, 3, 0xab000002):
            with self.subTest(slot=slot):
                original = EngineFixture(slot, patched=False)
                fast = EngineFixture(slot)
                original.run(); fast.run()
                self.assertEqual(bytes(fast.uc.mem_read(POOL, 0x300)), bytes(original.uc.mem_read(POOL, 0x300)))
                self.assertEqual(fast.read32(RECORD + 0x34), original.read32(RECORD + 0x34))
                self.assertEqual([fast.uc.reg_read(r) for r in registers], [original.uc.reg_read(r) for r in registers])
                self.assertEqual(fast.read32(COUNTER), 0)

    def test_bad_slot_allocates_fresh_or_reuses_free_list(self):
        for free, expected in ((None, 4), (2, 2)):
            with self.subTest(free=free):
                f = EngineFixture(0x00de741b, free=free)
                f.run()
                self.assertEqual(f.read32(RECORD + 0x34), expected)
                self.assertEqual(bytes(f.uc.mem_read(POOL + expected * 64, 64)), f.matrix)
                self.assertEqual(f.read32(COUNTER), 1)
                self.assertEqual(f.read32(RENDER + 0x174), 5 if free is None else 4)

    def test_native_allocator_grows_the_matrix_pool(self):
        # Run the actual allocator and resize instructions. Only the external allocation/free
        # and memcpy functions are simulated; this is not the game's memory manager.
        f = EngineFixture(0x00de741b, count=4, capacity=4)
        allocate_api, free_api = 0x1b000000, 0x1c000000
        f.uc.mem_map(allocate_api, 0x1000); f.uc.mem_map(free_api, 0x1000)
        f.u64(f.base + 0x42388, allocate_api)
        f.u64(f.base + 0x423a8, free_api)
        f.u32(RENDER + 0x17c, 1)  # vector owns/copies existing elements
        original = bytes((i * 7) & 0xff for i in range(4 * 64))
        f.uc.mem_write(POOL, original)
        calls = []

        def return_to_caller(uc):
            sp = uc.reg_read(UC_X86_REG_RSP)
            target = struct.unpack('<Q', uc.mem_read(sp, 8))[0]
            uc.reg_write(UC_X86_REG_RSP, sp + 8)
            uc.reg_write(UC_X86_REG_RIP, target)

        def allocator(uc, _address, _size, _user):
            calls.append(('alloc', uc.reg_read(UC_X86_REG_RCX), uc.reg_read(UC_X86_REG_RDX)))
            uc.reg_write(UC_X86_REG_RAX, OTHER)
            return_to_caller(uc)

        def release(uc, _address, _size, _user):
            calls.append(('free', uc.reg_read(UC_X86_REG_RCX)))
            return_to_caller(uc)

        def copy(uc, _address, _size, _user):
            dst, src, count = (uc.reg_read(r) for r in (UC_X86_REG_RCX, UC_X86_REG_RDX, UC_X86_REG_R8))
            calls.append(('copy', dst, src, count))
            uc.mem_write(dst, bytes(uc.mem_read(src, count)))
            uc.reg_write(UC_X86_REG_RAX, dst)
            return_to_caller(uc)

        f.uc.hook_add(UC_HOOK_CODE, allocator, begin=allocate_api, end=allocate_api)
        f.uc.hook_add(UC_HOOK_CODE, release, begin=free_api, end=free_api)
        f.uc.hook_add(UC_HOOK_CODE, copy, begin=f.base + 0x3fc96, end=f.base + 0x3fc96)
        f.run()
        self.assertEqual(calls, [('alloc', 8 * 64, 16), ('copy', OTHER, POOL, 4 * 64), ('free', POOL)])
        self.assertEqual(struct.unpack('<Q', f.uc.mem_read(RENDER + 0x168, 8))[0], OTHER)
        self.assertEqual((f.read32(RENDER + 0x174), f.read32(RENDER + 0x178)), (5, 8))
        self.assertEqual(bytes(f.uc.mem_read(OTHER, 4 * 64)), original)
        self.assertEqual(bytes(f.uc.mem_read(OTHER + 4 * 64, 64)), f.matrix)
        self.assertEqual((f.read32(RECORD + 0x34), f.read32(COUNTER)), (4, 1))

    def test_boundaries_and_sentinel(self):
        # The function's existing sentinel check precedes the patch; sentinels must not be
        # counted as bounds repairs. Capacity can be lower than count in a damaged object;
        # a valid free slot avoids asking the damaged fresh allocator to grow that object.
        for slot, count, cap, free, repair in ((3, 4, 8, None, 0), (4, 4, 8, None, 1),
                                               (3, 6, 3, 1, 1), (0xfffffffe, 4, 8, None, 0),
                                               (0xffffffff, 4, 8, None, 0)):
            with self.subTest(slot=slot, count=count, capacity=cap):
                f = EngineFixture(slot, count, cap, free=free); f.run()
                self.assertEqual(f.read32(COUNTER), repair)
                selected = f.read32(RECORD + 0x34)
                self.assertEqual(bytes(f.uc.mem_read(POOL + selected * 64, 64)), f.matrix)

    def test_inaccessible_pool_still_faults_at_original_exact_write(self):
        for freed in (False, True):
            with self.subTest(freed=freed):
                f = EngineFixture(1)
                if freed:
                    f.uc.mem_unmap(POOL, 0x10000)
                else:
                    f.uc.mem_protect(POOL, 0x10000, UC_PROT_READ)
                with self.assertRaises(UcError) as error:
                    f.run()
                self.assertEqual(error.exception.errno, UC_ERR_WRITE_UNMAPPED if freed else UC_ERR_WRITE_PROT)
                self.assertEqual(f.uc.reg_read(UC_X86_REG_RIP), f.base + 0x129a2)
                self.assertEqual(f.read32(COUNTER), 0)

    def test_corrupt_metadata_can_write_foreign_writable_memory(self):
        # Intentional characterization of the acknowledged fast-mode risk, NOT a safety check.
        f = EngineFixture(5, count=1000, capacity=1000)
        f.u64(RENDER + 0x168, OTHER)
        f.run()
        self.assertEqual(bytes(f.uc.mem_read(OTHER + 5 * 64, 64)), f.matrix)
        self.assertEqual(f.read32(COUNTER), 0)


if __name__ == '__main__':
    unittest.main(verbosity=2)

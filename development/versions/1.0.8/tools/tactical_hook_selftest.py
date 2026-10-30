#!/usr/bin/env python3
"""Verify the *actual C++* tactical hook signatures against the supplied game's PE code.

This is a file-identity check, not an engine-hook or behaviour test. The signatures are parsed
from production source rather than duplicated here so a code change cannot silently escape it.
"""
from pathlib import Path
import re
import struct
import unittest
from pe_scan import PE, pointer_rva

TOOLS = Path(__file__).resolve().parent
NET = TOOLS.parent
GAME = NET.parents[1] / "game-build-3598-1/x64"  # NET = versions/<version>; above it, development/


def cpp_signature(file: str, array: str, after: str = "") -> bytes:
    text = (NET / "src" / file).read_text(encoding="utf-8")
    if after:
        text = text[text.index(after):]
    match = re.search(r"(?:static\s+)?const\s+unsigned\s+char\s+" + re.escape(array)
                      + r"\s*\[\s*\]\s*=\s*\{([^}]*)\}", text, re.S)
    if not match:
        raise ValueError(f"{file}: cannot find {array}")
    return bytes(int(part, 16) for part in re.findall(r"0x([0-9a-fA-F]+)", match.group(1)))


def executable_hits(pe: PE, sig: bytes) -> list[int]:
    hits = []
    for _name, rva, raw, size in pe.executable():
        data = pe.data[raw:raw + size]
        at = data.find(sig)
        while at != -1:
            hits.append(rva + at)
            at = data.find(sig, at + 1)
    return hits


class TacticalSignatures(unittest.TestCase):
    def test_current_game_hook_signatures(self):
        cases = [
            ("basic_physic.codelib", "shots.cpp", "launch_sig", "", 0x18430),
            ("basic_physic.codelib", "shots.cpp", "explode_sig", "", 0x19260),
            ("adv_game.codelib", "trench.cpp", "sig", "void trench_install", 0xDBC20),
            ("adv_game.codelib", "session.cpp", "sig", "static void update_ai_commander", 0xBEC30),
            ("adv_opgame.codelib", "session.cpp", "next_sig", "static void install_map_detours", 0x369E0),
            ("adv_opgame.codelib", "session.cpp", "worker_sig", "static void install_map_detours", 0x76510),
            ("adv_physic.codelib", "state.cpp", "sig", "static void install_part_guard", 0x82020),
            ("basic_render.codelib", "state.cpp", "lookup_sig", "", 0x13AB0),
            ("basic_render.codelib", "render_guard.cpp", "sig_walk_node", "", 0x12FD0),
            ("basic_render.codelib", "render_guard.cpp", "sig_node", "", 0x13AB0),
            ("basic_render.codelib", "render_guard.cpp", "sig_cache_complete", "", 0x12925),
            ("basic_render.codelib", "render_guard.cpp", "sig_triangle", "", 0x39A40),
            ("base_shell.codelib", "xlate.cpp", "obj_lookup_sig", "", 0x98E0),
            ("base_shell.codelib", "xlate.cpp", "desc_lookup_sig", "", 0x121D0),
            ("adv_interf.codelib", "session.cpp", "start_sig", "static void install_detours", 0x3B320),
            ("adv_interf.codelib", "session.cpp", "input_sig", "static void install_detours", 0x3B460),
            ("adv_interf.codelib", "session.cpp", "box_sig", "static void install_detours", 0x41630),
            ("base_main.codelib", "session.cpp", "timer_sig", "static void install_detours", 0x4200),
        ]
        cache = {}
        for game, source, name, after, rva in cases:
            with self.subTest(module=game, signature=name):
                if game not in cache:
                    cache[game] = PE(GAME / game)
                sig = cpp_signature(source, name, after)
                self.assertGreaterEqual(len(sig), 9)
                self.assertEqual(executable_hits(cache[game], sig), [rva])

    def test_renderer_recovery_paths(self):
        pe = PE(GAME / "basic_render.codelib")
        for name, rva in (("sig_walk_fallback", 0x13038), ("sig_node_fallback", 0x13BC9),
                          ("sig_cache_rebuild", 0x129C6), ("sig_triangle_loop", 0x39B20),
                          ("sig_triangle_attr0", 0x39CF2), ("sig_triangle_attr1", 0x39D23),
                          ("sig_triangle_fallback", 0x39DE5)):
            with self.subTest(signature=name):
                sig = cpp_signature("render_guard.cpp", name)
                offset = pe.offset(rva)
                self.assertIsNotNone(offset)
                self.assertEqual(pe.data[offset:offset + len(sig)], sig)
                self.assertTrue(pe.executable_rva(rva))

    def test_object_manager_vtable_slots(self):
        pe = PE(GAME / "base_shell.codelib")
        for index, target in ((54, 0x98E0), (57, 0x121D0)):
            with self.subTest(slot=index):
                offset = pe.offset(0x4CA10 + index * 8)
                self.assertIsNotNone(offset)
                value = struct.unpack_from("<Q", pe.data, offset)[0]
                self.assertEqual(pointer_rva(pe, value), target)


if __name__ == "__main__":
    unittest.main(verbosity=2)

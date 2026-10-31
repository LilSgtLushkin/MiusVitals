# Disassembles a codelib of the game build at an RVA. RVA and file offset differ in these modules
# (adv_interf .rdata: rva 0x83000, raw 0x81a00), so an address seen in the running game has to be
# mapped through the section table before it can be read out of the file.
#
#   python3 -m venv /tmp/venv && /tmp/venv/bin/pip install capstone
#   /tmp/venv/bin/python development/v20-recovery/tools/dis_at.py <codelib> <rva-hex> [length-hex]
#
# The AI commander, and the loop that steps one side of the battle per tick:
#   .../dis_at.py development/game-build-3598-1/x64/adv_game.codelib 0xBEC30 0x140
import struct, sys
from capstone import *

path = sys.argv[1]
rva = int(sys.argv[2], 16)
length = int(sys.argv[3], 16) if len(sys.argv) > 3 else 0x100

d = open(path, 'rb').read()
lfanew = struct.unpack_from('<I', d, 0x3c)[0]
nsec = struct.unpack_from('<H', d, lfanew + 6)[0]
opt = struct.unpack_from('<H', d, lfanew + 20)[0]
secs = []
for i in range(nsec):
    o = lfanew + 24 + opt + i * 40
    name = d[o:o + 8].rstrip(b'\x00').decode(errors='replace')
    vsize, vaddr, rsize, raddr = struct.unpack_from('<IIII', d, o + 8)
    secs.append((name, vaddr, vsize, raddr, rsize))


def rva_to_offset(x):
    for name, vaddr, vsize, raddr, rsize in secs:
        if vaddr <= x < vaddr + vsize:
            return raddr + (x - vaddr)
    sys.exit("RVA %#x is outside every section" % x)


md = Cs(CS_ARCH_X86, CS_MODE_64)
for i in md.disasm(d[rva_to_offset(rva):][:length], rva):
    note = ''
    if i.mnemonic == 'call':
        try:
            target = int(i.op_str, 16)
            note = "   -> %#x" % target
            if 0 < target - rva < length:
                note += "  (inside this window)"
        except ValueError:
            pass
    print("%08x  %-20s %s %s%s" % (i.address, i.bytes.hex(), i.mnemonic, i.op_str, note))

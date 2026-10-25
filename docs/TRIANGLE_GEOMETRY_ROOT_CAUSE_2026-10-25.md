# Корневая причина нового renderer crash: invalid geometry access — 2026-10-25

## Scope and inputs

This analysis concerns the newer report, not the earlier recursive-material and
transform-cache reports:

| Input | SHA-256 |
| --- | --- |
| `gt3_crashreport.zip` | `a73bd1da29e21bc6a909fe6e45160ec1f048d8218014bb6a8f8b62fdd443c881` |
| `minidump.dmp` inside it | `01051fc8ea6a1ca9dbb5da72acbd55c6bfe1a09577b0ee1891b0abae0edda2c7` |
| checked Steam 3598/1 `basic_render.codelib` | `ba8d68cf2b84b890dd88f6f3035c8c070070248e22c86c7481ba1262f8a3db11` |

The report starts a tactical battle and logs that the older Vitals renderer
measures were armed. It nevertheless terminates in a new `basic_render` routine.
Therefore the older invalid-child and transform-cache fixes were neither removed
nor treated as an explanation for this crash.

## What the dump proves

The exception stream supplies the exception-thread x64 `CONTEXT` directly:

| Field | Value |
| --- | --- |
| Exception | `0xc0000005`, read access violation |
| Instruction | `basic_render+0x39b52` |
| Instruction bytes | `44 0f 10 0c 30` — `movups xmm9,[rax+rsi]` |
| Fault address | `0x00000001bfa72560` |
| `RSI` | `0x00000000c4f85f30` |
| `RAX` / `RCX` | `0x00000000faaec630` |
| `R8` | `0x00000000c4f8820c` |
| `R10` | `0x0000000000000000` |
| `R11` / `R15` | `1` / `128` |
| query/output pointer `RDX` | `0x00000000055a6900` |

`RSI + RAX` is exactly the reported invalid read address. `RAX` was produced by
32-bit arithmetic before the load, so the failure is not an arbitrary CPU
register change at the load itself.

The fully disassembled Steam 3598/1 triangle-query routine begins at `+0x39a40`:

```asm
mov rsi, [rcx+78]       ; vertex base
mov r8,  [rcx+70]       ; triples of 32-bit indices
mov r15d,[rcx+84]       ; triangle count
mov r10d,[rcx+94]       ; common index base
...
mov ecx, [rdx+88]       ; vertex stride
mov ebp, [r8]           ; first stored index
mov eax, [r8+8]         ; third stored index
mov r14d,[r8+4]         ; second stored index
add eax, r10d
imul eax, ecx           ; 32-bit scaled vertex offset
movups xmm12,[rsi+rbp]  ; +0x39b4d
movups xmm9, [rax+rsi]  ; +0x39b52  <- crash
movups xmm8, [r14+rsi]  ; +0x39b5b
```

The engine never verifies that the calculated offsets lie in the current vertex
or optional-attribute buffers. Here `R10` is zero, `R11` is the second
triangle-loop iteration, and `RAX` is already the impossible scaled offset
`0xfaaec630`. The minimal dump deliberately contains no heap pages for `R8` or
`RDX`; it cannot distinguish whether the original bad value was an index, a
stride, or prior memory corruption. It *does* prove the crash mechanism:
**the stock triangle reader trusts geometry index/stride data and performs an
unbounded derived read.**

This is a different root mechanism from:

* the old `+0x12ffd`/`+0x13adb` recursive material child reads; and
* the old `+0x129a2` stale transform-cache write.

## v7.3.0 fix: preflight the engine's complete geometry read set

The fix detours only the exact, signature-verified triangle-query entry
`basic_render+0x39a40`. Before it calls the untouched engine function, it reads
the query's documented inputs used by that routine and verifies every exact
memory range the loop can load:

1. the index-triple array (`count * 12` bytes);
2. all three index-derived primary 16-byte vertices for every triangle;
3. if the engine flags request them, all three 16-byte reads in each optional
   attribute buffer.

The entry detour is installed only after five independent byte signatures match
the supported Steam 3598/1 image: query entry, index/vertex loop, both attribute
blocks, and the engine's no-hit epilogue. Any module mismatch leaves the feature
off.

If an index-derived address is not readable, the hook writes only query byte
`+0xa4` to zero and returns `false`. This is the function's own defined *no hit*
result; it does not create a vertex, change an index, retain an unknown object,
or continue executing through the invalid read. The direct caller checks that
boolean and follows its normal failure path.

This is a pre-access invariant check, not a broad catch-all exception policy.
The existing vectored handler is retained only for a narrow race where another
engine thread changes a previously checked geometry buffer between preflight and
one of the exact load instructions. It accepts only a **read** AV at one of these
twelve instructions with the computed effective address exactly equal to the AV
address and a writable real `+0xa4` output byte:

```text
39b26 39b29 39b30         index triple loads
39b4d 39b52 39b5b         primary vertices
39cfb 39cff 39d04         optional attribute buffer 0
39d2c 39d30 39d35         optional attribute buffer 1
```

Only then does it branch to the same native no-hit epilogue at `+0x39de5`.
Every other fault — wrong direction, another renderer instruction, mismatching
effective address, invalid output object, or another build — remains visible to
the game's normal crash handling.

## Verification performed

`native test` now constructs a real query-layout fixture and asserts all of the
following under ASan/UBSan:

* readable primary and optional geometry calls the original triangle function;
* the dump-shaped third stored index `0x0faaec63` with a 16-byte stride produces
  offset `0xfaaec630`, is rejected *before* the original function is called,
  and yields only no-hit;
* the exception-race fallback accepts the exact current `+0x39b52` context plus
  representative index and attribute loads, and redirects to `+0x39de5`;
* an effective-address mismatch is propagated instead of being hidden.

Static release verification also asserts the supported codelib timestamp,
byte signatures, DLL import/export surface, archive tree, and SHA-256 hashes.
A Linux sandbox cannot execute the Windows game, so the first two-player run is
still required to validate game behaviour; it should retain full
`basic_network_tacgame_*.log` files and confirm that no `skipped malformed
basic_render geometry read` message occurs during normal geometry.

# Корневая причина предыдущего GT3 crash — stale transform-cache renderer, 2026-10-25

> **Статус:** этот документ относится к отдельному write-AV `basic_render+0x129a2`.
> Более поздний supplied dump — read-AV `+0x39b52` — имеет другую geometry/index
> причину и разобран в [`TRIANGLE_GEOMETRY_ROOT_CAUSE_2026-10-25.md`](TRIANGLE_GEOMETRY_ROOT_CAUSE_2026-10-25.md).

## Вывод

Новый crash не является ещё одним вариантом Vitals packet, object-number exchange или native type 10/11. Это конкретное нарушение инварианта времени жизни в Steam 3598/1 `basic_render.codelib`: **в корректной node record остаётся старый 24-битный номер matrix/transform cache slot от другого renderer pool**. Код движка использует это число как смещение без проверки, и пытается записать матрицу за пределы текущего pool.

Это отличается от двух ранее зарегистрированных read-AV в recursive material walkers. Все три случая происходят в `basic_render`, но новый dump впервые показывает источник недопустимого индекса и штатный путь, который корректно его пересоздаёт.

## Что доказано новым dump

Вход: `gt3_crashreport.zip`, `minidump.dmp`, SHA-256:

```text
0a75452e613ea9b100506c1e450b765138c11525d0c369cb66b3af0aba1b2840
```

| Поле | Значение |
| --- | --- |
| Exception | `0xC0000005`, **write** AV |
| RIP | `basic_render.codelib + 0x129a2` |
| Instruction | `movaps [rax + rcx], xmm0` |
| Fault destination | `0x00000000f9b24710` |
| `RAX` (current matrix pool base) | `0x00000000c2154050` |
| cached slot from record (`EDX`) | `0x00de741b` |
| `RBP` | valid record address `0x00000000ab3f1080` |
| source node (`EBX`) | `0x00052006` — normal group/index encoding |
| renderer binary | Steam 3598/1 timestamp `0x66f2d710` |

Disassembly captured directly in the dump gives the relevant data flow:

```asm
; node 00052006 selects a valid record in renderer's node table
mov edx, [rbp+34]                 ; stale cache slot = 00de741b
cmp edx, fffffffd                 ; -1/-2 means cache miss / allocation required
ja  cache_miss
and edx, 00ffffff
mov rax, [rdi+168]                ; current matrix pool
mov ecx, edx
shl rcx, 6                        ; 64 bytes per matrix
movaps [rax+rcx], xmm0            ; +129a2: unchecked write, faults

cache_miss:
lea rcx, [rdi+138]
mov rdx, r8
call basic_render+193c0           ; engine allocator
mov edx, eax
mov [rbp+34], edx
```

`0xc2154050 + (0x00de741b << 6) = 0xf9b24710`, exactly matching the AV address. Thus this is not a guessed interpretation of a register: the cache value, shift, pool base and fault address agree exactly.

## Почему это именно lifecycle defect, а не повреждённая save/network data

1. `0x00052006` is the source node used to select the record and is structurally valid; the bad number is the **separate transient field** `record+0x34`.
2. `0x00de741b` and `0x00052006` do not occur in the supplied `tacgame_image.cfgpack` or operation saves. The stale value is created in process memory, not received from the campaign save.
3. The report fails immediately during `Deploy is started`, before the custom Vitals channel can apply a tactical snapshot.
4. The older standalone report (`gt3_crashreport2`, SHA-256 `2d0d4da64feee2898ea54f3999787438d80479e7617f62bf7acba45508e171b0`) has no `mp_vitals.dll` at all and crashes in the same stock renderer during deployment. It uses a different save, battle and non-`mp_guest` profile.

The proven root defect is therefore that the renderer treats every non-sentinel cache value as belonging to the current matrix pool. The immediate bad value can be a retained handle after a pool restart **or** an earlier writer corrupting this transient field; this minidump cannot name that earlier writer. Both origins violate the same missing ownership invariant. The engine already has a correct cache-miss allocator path but fails to enter it when a non-sentinel handle does not belong to the current pool.

## Corrective invariant, not an AV skip

The accompanying source change is intentionally not a generic vectored-exception patch:

1. It detours only the exact `basic_render+0x12925` cache-sync function after timestamp and 149 bytes of function/rebuild signatures match the dump-verified Steam 3598/1 image.
2. For every cache use, it resolves the same node record as the engine and asks `VirtualQuery` whether `matrix_pool + (slot & 0xffffff) * 64` is writable **and is in the same Windows allocation as the current pool base**.
3. If this invariant fails, it atomically writes the engine's existing `-2` cache-miss sentinel to `record+0x34`.
4. The untouched engine code follows its normal `+0x129c6` allocator branch, writes a new slot back to the record, and then continues its ordinary recursive renderer flow.
5. A second, even narrower exact-write fallback exists only for a concurrent change after this preflight. It verifies the exact `movaps` effective address and record field before redirecting to that same engine allocation branch.

It neither fabricates a slot nor skips a renderer operation. Valid slots and all unrecognised builds remain untouched. The native ASan/UBSan test covers a valid cache, an out-of-allocation cache, the exact race fallback, and rejection of an address mismatch.

## Boundary

This restores the missing invariant for the newly demonstrated stale-cache class and prevents its invalid write regardless of the numeric slot value. It does not claim that every possible renderer bug, GPU-driver issue, or malformed material tree has the same source. The previous two independently proven recursive-child read cases retain their strict guards. A real Windows two-player deploy is still required after installation: the log must say that cache-lifetime repair and invalid-child guards are armed, and it will report any rebuilt stale cache entries.

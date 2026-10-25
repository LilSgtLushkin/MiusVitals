# Basic-render crash analysis and v7.0.1 guard

This records the evidence behind the v7.0.1 crash guard. It is deliberately scoped to the two supplied Windows minidumps; it is not a claim that every renderer failure has the same cause.

## Inputs

| Report | SHA-256 of minidump | Faulting thread | Exception |
| --- | --- | ---: | --- |
| 2026-10-24 (also supplied directly as `minidump.dmp.txt`) | `c5befc49e378137c43b754ab7c49fcc4b6c382947d402040b17f7de77b41c5cc` | 31832 | read AV `0xC0000005` |
| 2026-10-18 | `2d0d4da64feee2898ea54f3999787438d80479e7617f62bf7acba45508e171b0` | 7048 | read AV `0xC0000005` |

The newer direct dump is byte-for-byte identical to the dump in its crash-report ZIP. The tactical log associated with it uses thread `t31832` and reaches normal multiplayer reporting immediately before process termination.

## What both dumps prove

The two faults have the same relative address in the same engine module:

| Report | Loaded `basic_render` base | Fault IP | RVA | Invalid read |
| --- | ---: | ---: | ---: | ---: |
| 2026-10-24 | `0x047d0000` | `0x047e3adb` | `0x13adb` | `0x00000001af2fc898` |
| 2026-10-18 | `0x055e0000` | `0x055f3adb` | `0x13adb` | `0x000000018adc8a68` |

The Steam 3598/1 `basic_render.codelib` disassembles that RVA as:

```asm
basic_render+0x13ad7  add rsi, qword ptr [r10+r9]
basic_render+0x13adb  mov ecx, dword ptr [rsi+0x38]  ; fault
```

It belongs to the recursive material-node routine beginning at `basic_render+0x13ab0`. The stack return address in both dumps is `basic_render+0x13b1e`, immediately after that routine recursively calls itself. At the exception:

* `R8D`, the recursive child-node number, is `0x8b219fc2` / `0x3fe24291`, although the normal node index is masked to 10 bits by its non-recursive caller;
* `RDI` remains the already-computed parent material ID (`0x00e1a7c2` / `0x00496dae`), which fits the routine's 24-bit material-ID representation;
* the AV target is exactly `RSI + 0x38`.

So the confirmed failure is an invalid recursive render child, not an instruction inside `basic_network.codelib` or `mp_vitals.dll`. The older report's module list has no loaded `mp_vitals.dll`, which rules out attributing this recurring instruction-level failure solely to that DLL. It does not, by itself, identify the earlier producer of the corrupt render-node state.

## v7.0.1 mitigation

`mp_vitals.dll` now waits until the normal frame hook sees `basic_render.codelib`, and arms the guard only when all of the following hold:

1. the module has the Steam 3598/1 timestamp (`0x66f2d710`) and a sufficiently large image;
2. the full 46-byte node-reader signature at `0x13ab0` and 18-byte normal epilogue signature at `0x13bc9` match;
3. the first-chance exception is a *read* access violation at `base + 0x13adb`, whose target equals `RSI + 0x38`;
4. the child number is outside its valid 10-bit range (`R8 > 0x3ff`) and the preserved parent material ID is valid (`RDI <= 0xffffff`).

Only then the handler changes `RIP` to the routine's ordinary epilogue at `base + 0x13bc9`. That epilogue returns the still-valid parent material in `EAX` and restores the frame normally. No code or game data are patched. All non-matching exceptions continue to the game's own handlers unchanged.

The native ASan/UBSan test constructs the matching exception context and proves that it resumes at the real epilogue; it also proves that a normal index, a write AV, another instruction address, or an invalid parent ID is rejected.

The guard is on by default. `MP_RENDER_GUARD=0` disables only this recovery path for diagnosis. The runtime log confirms arming and records each actual recovery.

## Limits

This is a fault-containment fix for the exact recurring signature, not proof of the original memory-state producer. It may make the affected object's material inherit its parent's material for that frame/tree, so a local visual defect is preferable to process termination. The actual Graviteam game cannot run in the Linux build environment; game-session validation remains required.

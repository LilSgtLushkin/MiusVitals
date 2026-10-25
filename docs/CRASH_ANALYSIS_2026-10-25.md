# GT3 crash analysis: second `basic_render` invalid-child path — 2026-10-25

> **Дополнение после нового dump:** отдельный write-AV `basic_render+0x129a2` выявил stale transform-cache slot и его штатный engine allocator path. Это не третий «угаданный» child guard; полный разбор и инвариантное исправление находятся в [`RENDER_CACHE_ROOT_CAUSE_2026-10-25.md`](RENDER_CACHE_ROOT_CAUSE_2026-10-25.md).

## Evidence examined

The supplied `gt3_crashreport.zip` was inspected as a ZIP container without broad extraction. Only its `minidump.dmp` and the associated engine metadata were read. The report is the joining tactical game (`mp_guest`); the host-side operational and tactical `basic_network` logs were correlated by the battle start and subsequent connection loss.

The dump has a normal first-chance-unhandled read access violation:

| Field | Value |
| --- | --- |
| Exception | `0xc0000005` (read AV) |
| Faulting instruction | `basic_render.codelib + 0x12ffd` |
| Instruction bytes | `8b 4f 38` — `mov ecx,[rdi+0x38]` |
| Fault address | `0x000000010b5f22f8` |
| `RDI` | `0x000000010b5f22c0` |
| `R8` | `0x000000003f800000` |
| `basic_render` timestamp | `0x66f2d710` (supported Steam 3598/1 image) |
| `mp_vitals.dll` | loaded in the crashing process |

The caller constructs `RDI` from a recursive child-node number and reads its node flags at `+0x38`. `R8 = 0x3f800000` is not a plausible node number, so the calculated node address is invalid. This is an engine-side invalid recursive-child read, not a Vitals packet or native reliable-stream failure.

## Relation to the existing guard

The previous guard was intentionally scoped to the independently confirmed reader at `basic_render+0x13adb`. This dump faults in a separate but structurally parallel walker at `+0x12ffd`; the old guard correctly did not catch it, because accepting every renderer AV would be unsafe.

The exact supported image was checked against the supplied static `basic_render`:

```text
+0x12fd0  recursive walker signature
+0x12ffd  mov ecx,[rdi+0x38]       <- confirmed new fault
+0x13038  normal epilogue: restore RBX/RBP/RSI/RDI and return
+0x13ab0  previously guarded recursive material reader
+0x13adb  prior confirmed fault
+0x13bc9  prior reader's normal epilogue
```

## v7.1.1 mitigation

v7.1.1 adds one more narrow branch to the existing vectored handler. It acts only when all of these conditions hold:

1. `basic_render.codelib` has the expected 3598 timestamp and all four reader/epilogue byte signatures;
2. the exception is a **read** AV at exactly `+0x12ffd` or `+0x13adb`;
3. the reported fault address is exactly the relevant register plus `0x38`;
4. `R8 > 0x3ff`, excluding plausible child indices.

For the new walker it resumes at `+0x13038`, its own normal epilogue. This restores every callee-saved register and returns from only the malformed child branch; no renderer code is patched in memory. A valid child index, any other instruction/address, a write AV, another renderer build, or a missing signature is left to the game’s normal exception handling.

The runtime log identifies that both exact guards were armed:

```text
vitals: basic_render invalid-child crash guards armed (rvas 12ffd, 13adb)
```

`MP_RENDER_GUARD=0` disables both branches for controlled diagnosis.

## Log correlation and unrelated findings

The host log shows that it issued the operation map and started the tactical battle; after the guest crash it reports a network timeout. The host’s Vitals transport telemetry is idle at the start of this battle, so these logs do not implicate Vitals delivery in this crash.

The host operation log also says `map: unknown game build, ending the turn is not coordinated between the players`. That is a separate, previously documented operational-map detour-signature issue. This renderer guard does not change map logic or native packet types 10/11.

## Limits

This is a targeted mitigation for the exact dump shape and known Steam 3598/1 binary. It is not a claim that all `basic_render` crashes have this cause, and a real two-player run remains required to establish gameplay behaviour after recovery.

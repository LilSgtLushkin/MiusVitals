# Лист переноса MiusVitals в исходный MiusPlayer

Это единственное место, где фиксируется соответствие между текущим binary-sidecar и public C++ source. До появления новой проверенной source-built codelib **не редактируйте vendored upstream задним числом и не применяйте старые RVA к его build output**.

| Задача | Нынешняя реализация | Исходная точка MiusPlayer | Условие переноса |
| --- | --- | --- | --- |
| Operation tactical full-state policy | `kit-source/src/mp_vitals.c`: strict command-line classifier, absent-only `MP_SYNC_STATE=0` | `basic_network/session.cpp`, `sync_state_mode()` | Сохранить explicit `MP_SYNC_STATE`; C++ default должен быть выбран до state sync cache и проверен двумя peers |
| Vitals loss/damage exchange | `kit-source/src/mp_vitals.c`, packet 0x34/0x35 | `session.cpp`, `state.cpp`, `statesync.cpp` | Сначала описать protocol compatibility и ACK/retry transfer, затем добавить C++ unit/native tests |
| Tactical remote-transform write elision | `merged-source/patches/0001-elide-idempotent-remote-transform-writes.patch` — source-only candidate | `basic_network.cpp`, `session.cpp`, `common.h` | Только `ms == 0` no-simulation и byte-identical settled no-op; не менять snapshot cadence/KBPS/frame pacing; проверить moving units, combat, reconnect и оба host roles до binary release |
| Object-number readiness latch | `kit-source/src/mp_vitals.c` | `xlate.cpp`, `session.cpp` | Не включать Vitals save state до native number exchange на обеих сторонах |
| Renderer cache/geometry guards | `kit-source/src/mp_vitals.c` | Нет в public MiusPlayer C++ tree: это другой engine module (`basic_render.codelib`) | Оставить отдельным строго signature-bound sidecar guard до появления engine-backed source/API |
| Operational map ordering / handoff | Source-only overlay `patches/0002-ordered-operation-turns-and-network-hygiene.patch` | `session.cpp`, `joiner.h`, `protocol.h`, new C++ `operation_turn.h` | Host → authoritative handoff → client → collect/host resolve; rebuild codelib **and** launcher, then validate two peers before any archive release |

## Минимальный checklist для нового C++ изменения

1. Сделать отдельный commit поверх vendored baseline, не менять `SOURCE_MANIFEST.sha256` как способ скрыть локальную правку.
2. Описать source-level invariant и добавить test, который можно выполнить без игры.
3. Собрать MSVC candidate через `build_public_source.bat`; сохранить hash, timestamp, imports/exports и diff к published v20 artifact.
4. Создать новый release profile вместо использования `RVA_*` старого Vitals DLL.
5. Выполнить two-peer game test и только после него обновлять install kit.

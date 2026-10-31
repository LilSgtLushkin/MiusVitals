# Аудит сетевого кода и source-only candidate — 2026-10-25

> Исторический аудит старой схемы поочерёдных ходов. Она отменена: в 1.0.6 только хост
> выполняет оперативные ходы и инициирует бои, клиент наблюдает и присоединяется к его боям.
> Упомянутые ниже client-initiated attack и handoff больше не являются требованиями 1.0.6.
> Актуальные ограничения: [NET_V21_SPECTATOR_1_0_6.md](NET_V21_SPECTATOR_1_0_6.md).

## Граница аудита

Проверен весь runtime source публичного `VampyrCat/MiusPlayer@43b2736d3b4a1e208e6c80f45f6ebbdd476de3aa` (`basic_network/*.cpp`, `*.h`, включая launcher `mp_join.cpp`) и действующий C sidecar `development/forbidden/development/kit-source/src/mp_vitals.c` (припаркован вместе со своим тестом).

Закреплённый upstream tree не редактируется: изменения оформлены отдельным C++ overlay `0002-ordered-operation-turns-and-network-hygiene.patch`, который применяется **после** FPS overlay `0001`. Это не бинарный release и не изменение player archives.

| Участок | Проверка | Результат в candidate |
| --- | --- | --- |
| `protocol.h` | размер/packing UDP packets, version negotiation, свободные bits `Welcome.map_flags` | ABI и `k_version=12` не меняются; добавлен компактный C++ codec `operation_turn.h` для bits 2–3 (`host`, `client`, `collect`) |
| `session.cpp`: operation map | ownership Next Turn, map upload/download, map epoch, merge/restart, disconnect path | строгая state machine **host → authoritative handoff → client → collect/host resolution**; stock Next Turn не может обойти очередь после выдачи карты |
| `session.cpp`: client attacks | three-way merge, в том числе когда first `user_orders` создан client | используется существующий проверяемый `opsave::merge_moves` и его first-order fallback; host вызывает turn worker только после обеих фаз и collection |
| `session.cpp`: tactical transforms | receive burst, pending storage, unit lifetime | C++ coalescing хранит newest snapshot для каждого unit до real simulation step; stale `g_held`/`g_sent` entry удаляется после disappearance unit |
| `basic_network.cpp` | ctrl_tick, sim_ms, input isolation, frame pipeline | сохраняется source FPS invariant: не делать transform stores на `sim_ms == 0`; `ui_blocks_input()` уже проходит через raw input, key state и window procedure, поэтому phase gate блокирует реальный input |
| `ui.cpp` | `session_map_blocking()` и map busy | новых D3D/UI detours не требуется: existing raw-input gate получает phase decision из session C++ state machine |
| `joiner.h`, `mp_join.cpp` | map launcher environment и protocol interpretation | map epoch, map stage и authoritative map hash переносятся в новый opgame process; matching `mp_join.exe` компилируется Windows workflow вместе с candidate |
| `opsave.h`, `opsave_test.cpp` | tags, sidecar, `user_orders`, value three-way merge | формат save не переписан и не поллингится из памяти; overlay вызывает existing typed C++ API и не меняет `ai`/`vis_mode` при merge |
| `state.cpp`, `statesync.cpp` | full-state stream и fragment validation | не менялись: operation tactical position-only policy остаётся явной existing boundary; никаких глобальных `MP_SYNC_STATE`, KBPS или packet-rate изменений |
| `shots.cpp`, `trench.cpp`, `xlate.cpp` | ordered reliable event streams, mapping resend/readiness | не менялись: они уже отделяют shots/trenches/object-number mapping от transform stream; coalescing касается только replaceable transforms, не events |
| `engine.h`, `detour.h`, `image.h` | engine ABI/RVA safety and ownership mirror | не добавлено guessed engine memory offset; existing exact signature checks сохранены |
| `mp_vitals.c` | C sidecar hooks / Vitals guard | intentionally не переносится на новый source codelib: его RVA/import profile привязан к released v20 module и нуждается в отдельном C++ port или новом profile |
| Python/BAT packaging tools | reproducibility, installers, release checks | остаются tooling, а не hot-path runtime. Переписывать их в C++ не улучшит игру и ухудшит reproducible packaging; весь изменённый game/network runtime написан на C++ |

## Исправленные hot-path проблемы

1. **Очередь remote transforms.** Несколько UDP state packets, пришедших между двумя simulation steps, раньше добавляли повторяющиеся `UnitRec` в vector. В same step они всё равно последовательно заменяли один и тот же newest held snapshot. Новый `unordered_map<unit, UnitRec>` + stable first-seen order сохраняет последний record каждого unit и исключает бесполезную allocation/work. Moving units, rotation и velocity не фильтруются.
2. **Рост cache уничтоженных units.** `g_held` и `g_sent` очищаются при rebuild, когда unit исчез из object manager. Это устраняет неограниченный рост в долгих skirmish/mission/operation battles.
3. **Operation race и одновременный ввод.** Passive side блокируется до raw input. После host phase клиент получает новый checked operation save и только затем получает право на input. После client phase host collection/merge выполняется перед единственным call engine turn worker.
4. **FPS overlay сохраняется.** Exact settled no-op write elision и `sim_ms == 0` path не понижают snapshot cadence, state KBPS, `MP_SYNC_STATE`, frame pacing или global transport.

## Обязательные safety boundaries

- Проверка C++ overlay строит `basic_network.codelib` **дважды** `/Brepro` и отдельно компилирует matching `mp_join.exe`; workflow artifact помечен source-only/unvalidated и не публикуется в player archive.
- Новая map stage зависит от обоих новых modules. Смешивать candidate с released stable codelib, прежним experimental FPS codelib (beta_FPS_fix, ныне удалён) или старым launcher нельзя.
- `mp_vitals.dll` released v20 profile несовместим с source-built candidate. Его hooks не должны загружаться без нового exact PE/RVA profile.
- Не реализован unsafe «per-frame live operation-memory synchronization»: public source не раскрывает validated engine API для безопасной записи active opgame units. Полный save/reload polling в ходе одной фазы создал бы lag, races и риск повреждения save. Candidate делает безопасный authoritative handoff перед client phase, поэтому client не выдаёт orders по stale host positions. Полноценный live visual sync требует отдельного reverse-engineered API, signature checks и двух-peer evidence.

## Выполненные проверки

```sh
python3 development/merged-source/tests/test_transform_sync_patch.py
python3 development/merged-source/tests/test_ordered_operation_patch.py
python3 development/merged-source/verify_workspace.py
scripts/test_native.sh
scripts/verify_release.sh
```

> Дерево `development/merged-source/` убрано 2026-10-31; три верхние команды остаются в отчёте
> как фиксация того, что проверялось на дату аудита.

`test_ordered_operation_patch.py` применяет оба overlay к disposable copy pinned source, компилирует standalone C++ codec, моделирует порядок host/client/collect/host-resolve и проверяет newest-per-unit receive coalescing. Windows workflow `.github/workflows/build-ordered-operation-candidate.yml` предназначен для реальной VS2022 x64 compilation всего codelib и matching launcher.

## Перед любым release

Нужен живой Windows two-peer matrix: host movement → handoff → client movement; client-initiated attack; host-initiated attack; both completed turns without battle; disconnect/reconnect in every phase; battle entry/exit; operation reload; skirmish and custom mission transform/shot/state regression. До этого ветка остаётся source-only и **не должна merge в `main`**.

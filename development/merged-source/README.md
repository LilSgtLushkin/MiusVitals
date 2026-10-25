# Объединённая исходная рабочая область MiusPlayer + MiusVitals

Эта директория снимает прежнее ограничение «только 16 файлов» **для разработки**: рядом с исходником Vitals теперь хранится закреплённая, проверяемая копия публичного исходника MiusPlayer. Она предназначена для чтения, правки, white-box проверки и будущей полной сборки. Это не папка установки игры и не замена player archive `MiusMultiplayer_v1.0.1_INSTALL.zip`.

## Состав и граница слоёв

| Слой | Путь | Назначение |
| --- | --- | --- |
| Публичный MiusPlayer | [`../upstream/miusplayer-public-43b2736/`](../upstream/miusplayer-public-43b2736/) | C++ `basic_network`, исходники program-pack tooling и шаблоны mpmod1 |
| MiusVitals sidecar | [`../kit-source/src/mp_vitals.c`](../kit-source/src/mp_vitals.c) | Узкие проверяемые guards и operation tactical policy для установленного бинаря |
| Проверяемый игроком kit | [`../kit-source/`](../kit-source/) | Существующий install flow; не используйте его как базу для C++-редактирования |

Публичный source snapshot — `VampyrCat/MiusPlayer` commit `43b2736d3b4a1e208e6c80f45f6ebbdd476de3aa`, лицензия CC0. `UPSTREAM.json` и `SOURCE_MANIFEST.sha256` фиксируют происхождение и каждый включённый файл. Из upstream намеренно исключены object/exe/codelib outputs, Ghidra database и screenshots: они не являются редактируемым исходным кодом и только скрывают настоящую разницу сборки.

## Почему это действительно связано с FPS-исправлением

`basic_network/session.cpp` публичного источника содержит целевую native semantics:

```cpp
return forced >= 0 ? forced != 0 : g_host_mode != HOST_MODE_OPERATION;
```

То есть operation tactical battle должен реплицировать positions без full saved state, если `MP_SYNC_STATE` не задан. MiusVitals v7.5 восстанавливает только этот **отсутствующий** process default для строго распознанного operation-tactical command line; он не меняет `g_host_mode`, map state или codelib. Так C++ источник и sidecar теперь лежат в одном редактируемом дереве и проверяются одной командой:

```sh
python3 development/merged-source/verify_workspace.py
```

Проверка утверждает hash каждого vendored source file, связь опубликованного базового codelib с v20 input и оба конца policy boundary. Она не является игровым тестом.

Полный file-by-file audit runtime source, исправленные hot paths и оставшиеся safety boundaries находятся в [`../../docs/NETWORK_CODE_AUDIT_2026-10-25.md`](../../docs/NETWORK_CODE_AUDIT_2026-10-25.md).

## Candidate: устранение холостых записей remote transforms

В `session.cpp` устранена конкретная лишняя работа: `session_sim_step` запускался на каждом `ctrl_tick`, включая `ms == 0`, и снова записывал transform каждого followed unit; во время battle это также делалось для уже точного неподвижного состояния.

Проверяемый source-only candidate лежит в [`patches/0001-elide-idempotent-remote-transform-writes.patch`](patches/0001-elide-idempotent-remote-transform-writes.patch). Он:

- хранит newest snapshots, но не пишет transforms, пока engine не сделал simulation step;
- пропускает только **byte-identical settled no-op** запись: две одинаковые snapshots с нулевыми velocities и уже идентичные local position/matrices/velocities;
- не меняет 66 ms snapshot cadence, `MP_STATE_KBPS`, `MP_SYNC_STATE`, global map или frame pacing;
- добавляет счётчик `already-settled writes elided` в пятисекундный log для live-measurement.

Проверка candidate без игры:

```sh
python3 development/merged-source/tests/test_transform_sync_patch.py
```

Это patch для будущей source rebuild и **не** часть текущего install archive: нужны Windows build, новый binary/Vitals profile и two-peer tactical validation до выпуска runtime binary. Для создания disposable copy без изменения pinned provenance tree:

```sh
python3 development/merged-source/prepare_transform_sync_candidate.py
```

Она создаёт игнорируемый `development/merged-source/candidates/transform-sync/basic_network/`; его output всё ещё analysis candidate, не installable artifact.

## Полная пересборка: безопасный путь

На Windows с Visual Studio 2022 C++ tools публичный C++ кандидат строится штатным upstream script:

```bat
cd development\upstream\miusplayer-public-43b2736\basic_network
build.bat
```

Перед этим запускайте `build_public_source.bat` из этой директории: он верифицирует source manifest, запускает upstream build и печатает hash кандидата.

**Не копируйте этот C++ output в игру и не пропускайте его через текущий `patch_import.py`.** Публичный source build, уже лежащий upstream, имеет SHA-256 `eff7ae…f7af5`, тогда как опубликованный v20 module имеет `8108bd…5723`. Последний совпадает с проверенным input текущего MiusVitals release pipeline, но source-build output не совпадает. Поэтому source tree делает исправления обозримыми, но пока не доказывает бинарную совместимость адресных Vitals guards.

Точки переноса каждой текущей функции Vitals в C++ и обязательные условия собраны в [`PORTING_LEDGER.md`](PORTING_LEDGER.md).

Чтобы превратить полную C++ пересборку в installable release, нужно выполнить все этапы:

1. внести изменение в vendored C++ source и добавить его как явный patch/commit в этой рабочей области;
2. собрать candidate MSVC toolchain и закрепить его timestamp, PE layout, exports и hashes;
3. либо перенести нужную Vitals логику в C++, либо создать новый проверенный sidecar profile — не применять старые RVA/signatures к новому binary;
4. добавить native/PE tests и двух-peer game evidence;
5. только затем расширять player archive или выпускать новый install kit.

## Candidate: упорядоченная operational map и network hygiene

Пользовательский scope для этой ветки расширен на operational map. Второй source overlay [`patches/0002-ordered-operation-turns-and-network-hygiene.patch`](patches/0002-ordered-operation-turns-and-network-hygiene.patch) применяет C++ state machine поверх transform overlay:

- operational input принадлежит только одному игроку: **host → client → collection/resolution**; raw-input gate блокирует пассивную сторону, а host Next Turn не может обойти protocol, пока client process запускается;
- host после своей фазы сохраняет новый authoritative operation save и передаёт его client до разблокирования client input. Поэтому client видит позиции host units из завершённой host phase, а не старую initial copy;
- client orders, включая атаку на host, возвращаются в host save через existing three-way `opsave::merge_moves`; только после обеих завершённых фаз host вызывает engine turn worker, который решает, начинается ли battle;
- transform receive burst теперь coalesces только duplicate snapshots одной и той же unit до следующего real simulation step, а stale sent/held entries уничтоженных units удаляются. Это применяется к skirmish, custom mission и operation tactical battle без изменения cadence, KBPS, state policy или frame pacing.

Новый wire state занимает ранее неиспользуемые bits 2–3 существующего `Welcome.map_flags`; packet layout и `k_version` не меняются. Этот source candidate требует одинаковую новую codelib у обоих игроков. Перед реальным выпуском нужно пересобрать и проверить также launcher `mp_join.exe`, а не смешивать его со stable archive.

```sh
python3 development/merged-source/tests/test_ordered_operation_patch.py
python3 development/merged-source/prepare_ordered_operation_candidate.py
```

Последняя команда создаёт игнорируемую `candidates/ordered-operation-network/` copy. Она не пригодна для установки: необходимы Windows MSVC build, new PE/Vitals profile, two-peer operation test (host attack, client attack, disconnect/reconnect, map handoff) и отдельная проверка live map engine API. У public source есть безопасный handoff snapshot только на границе фаз; он не даёт права выдавать неподтверждённую memory polling/reload loop за непрерывную live-render synchronization внутри одной фазы.

## Граница выпуска

Ни один operational-map edit из этого source overlay не добавлен в player archives (v1.0.1 и outdated v1.0.0). Ветка намеренно остаётся отдельной до полной Windows/two-peer validation.

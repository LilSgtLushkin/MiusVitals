# Файлы разработки — не для установки

`kit-source/` — каноническая исходная структура проверяемого MiusVitals v7.5.0 kit: manifest сборки, Vitals sidecar, тесты и входные player artifacts. **Не копируйте development-дерево целиком в папку игры.**

## Объединённая исходная рабочая область

По разрешению на выход за прежний предел 16 файлов в репозиторий добавлен закреплённый публичный исходник MiusPlayer вместе с MiusVitals:

- [`upstream/miusplayer-public-43b2736/`](upstream/miusplayer-public-43b2736/) — C++ source/basic_network, program-pack tooling и launcher templates из публичного `VampyrCat/MiusPlayer` commit `43b2736d3b4a1e208e6c80f45f6ebbdd476de3aa` (CC0); `UPSTREAM.json` и `SOURCE_MANIFEST.sha256` фиксируют происхождение;
- [`merged-source/`](merged-source/) — единая документация, source-boundary verifier и Windows entry point для сборки **analysis candidate** из C++ source;
- [`kit-source/src/mp_vitals.c`](kit-source/src/mp_vitals.c) — текущий проверяемый Vitals sidecar для опубликованного v20 binary baseline.

Проверка объединённой рабочей области:

```sh
python3 development/merged-source/verify_workspace.py
python3 scripts/check_artifacts.py     # одинаковые бинарники во всех kit-источниках, хеши архивов, пути из .gitattributes
```

Публичный release archive содержит `basic_network.codelib` с SHA-256 `8108bd…5723`, совпадающий с input v20 текущего release pipeline. Но имеющийся upstream C++ build output имеет другой hash, поэтому не считается replacement binary: перед выпуском source rebuild нужны новая бинарная mapping, native/PE проверки и two-peer evidence. Подробности — в [`merged-source/README.md`](merged-source/README.md).

## Установка игрока

Текущий экспериментальный установщик: [`../MiusMultiplayer_v1.0.7_EXPERIMENTAL_NET+FPS_INSTALL.zip`](../MiusMultiplayer_v1.0.7_EXPERIMENTAL_NET%2BFPS_INSTALL.zip). Он сразу устанавливает `net_v21`; инструкции и ограничения — в [корневом README](../README.md). Это единственная сборка 1.0.7 в дереве: прежние архивы 1.0.7 удалены и остаются в истории Git. Сохранены `_known_good` и исторический [`../MiusMultiplayer_OUTDATED_INSTALL.zip`](../MiusMultiplayer_OUTDATED_INSTALL.zip).

Текущий установщик создаётся и проверяется `../scripts/package_net_v21_test.py`. `kit-source/` и старые unified-kit-деревья сохраняются как исторические источники и тестовые входы, а не как отдельная player install directory.

## Steam-preserving FPS candidate

`fps-candidate/` хранит воспроизводимый candidate `basic_network.codelib`: `scripts/build_steam_compatible_fps_candidate.py` получает его детерминированно из проверенного Steam 3598/1 module, а workflow `../.github/workflows/verify-fps-codelib.yml` заново собирает файл и сравнивает его byte-for-byte с checked-in копией. Этот candidate входил в исторический `MiusMultiplayer_v1.0.1_INSTALL.zip`; проверка `scripts/verify_legacy_fps_fixture.py` воспроизводит временный архив и сравнивает содержимое с закреплёнными хешами, не сохраняя старый ZIP.

Отдельный `MiusMultiplayer_beta_FPS_fix_INSTALL.zip` и `fps-beta-kit-source/` удалены: второй installer с собственным codelib больше не публикуется. В игре, где остался прежний beta module (включая withdrawn `78e271…2916e`), `MiusMultiplayer\Enable_Multiplayer.bat` из v1.0.1 всё равно заменяет codelib и program packs безусловно.

## Separate ordered-operation source candidate

`merged-source/patches/0002-ordered-operation-turns-and-network-hygiene.patch` — отдельный C++ overlay, не включённый ни в один player archive. Он serializes operational turns `host → client → host resolution`, передаёт authoritative host snapshot перед client input, оставляет client attack в merged orders и coalesces tactical receive bursts во всех battle modes. Подготовка/проверка candidate описана в [`merged-source/README.md`](merged-source/README.md); до rebuild matching codelib + `mp_join.exe` и live two-peer tests это не installable runtime.

## Восстановление исходника v20 (`v20-recovery/`)

Каталог отвечает на вопрос «что именно делает v20 и что из этого надо написать», а не
«где лежит готовый исходник». Он воспроизводим с нуля: `fetch_inputs.sh` забирает из публичного
репозитория v20-бинарник, v12-бинарник и MSVC `.obj` (в них — настоящие C++-имена), остальное
считается скриптами.

```sh
development/v20-recovery/fetch_inputs.sh
python3 development/v20-recovery/tools/v20tool.py objects && ... names && ... classify
python3 development/v20-recovery/tools/backlog.py     # дельта по этапам -> reports/v20-backlog.md
```

- `tools/` — разбор PE, сопоставление v12↔v20, декомпиляция, чистка дампа, отбор;
- `reports/v20-functions.csv` — все 895 функций v20: RVA, размер, статус (SAME/CHANGED/NEW), имя;
- `reports/v20-backlog.md` — 323 работы, разложенные по этапам M2–M8;
- `reports/decomp/` — 155 файлов псевдо-C: те функции, которые предстоит писать (индекс — `README.md` внутри);
- `docs/V20_STEAM_LAYER.md`, `docs/V20_NETWORK_LAYER.md` — спецификации слоёв, которые нельзя потерять;
- `work/` — бинарники и полный дамп: **не коммитится**, воспроизводится скриптами.

Смысл в цифрах: 572 из 895 функций v20 структурно совпадают с публичным деревом (их исходник уже
есть), 105 отличаются правками, 218 новые, из которых 143 — логика, 39 — вывод компилятора
(STL-инстанцирования и инициализаторы вернутся сами), 36 — пока безымянные. План и критерии
приёмки — [`docs/V20_SOURCE_RECOVERY.md`](../docs/V20_SOURCE_RECOVERY.md),
Steam-слой отдельно — [`docs/V20_STEAM_LAYER.md`](../docs/V20_STEAM_LAYER.md).

## Ревью 2026-10-30: какая версия современнее и что можно удалять

### Самая современная версия — [`net-v21/`](net-v21/)

Дата коммита тут бесполезна: весь `development/` попал в репозиторий одним импортом, поэтому
сравнивать пришлось по содержимому. Решающая улика — метка провода внутри бинарника:

| модуль | размер | метка |
| --- | --- | --- |
| shipped `basic_network.codelib` из установщика 1.0.7 | 424 960 | `GTMF_TA_A1` + **`NETV21_WIRE_PROTOCOL_18`** |
| `kit-source/files/basic_network.codelib` | 377 856 | `GTMF_TA_A1` |
| `unified-fps-kit-source/…/basic_network.codelib` | 378 368 | `GTMF_TA_A1` |
| `game-build-3598-1/x64/basic_network.codelib` | 250 368 | `GTMF_TA_A1` |

`NETV21_WIRE_PROTOCOL_18` задаётся в `net-v21/src/protocol.h` (`k_wire_tag`) и больше нет ни в
одном бинарнике внутри `development/`. Значит: **весь современный код — в `net-v21/src`, а все
`.codelib` в `development/` — это более старые сборки.** `upstream/miusplayer-public-43b2736` —
vendored baseline публичного MiusPlayer (v12), `net-v21` уже вобрал оба патча из `merged-source`
(`0001`, `0002`): в нём есть `src/operation_turn.h` и elision-счётчики в `session.cpp`.

### Есть ли фиксы мимо самой современной версии — да, есть

Условие «самая современная включает все фиксы» **не выполняется**, поэтому «оставить только её»
сейчас нельзя. Что осталось в старых сборках:

* `03be00` (720 Б) — синхронизация анимаций и разворотов, подмена vtable `i_animch`;
* `01e530` (291 Б) — запрос паузы, требует новый тип пакета;
* ~102 КБ / 261 функция из v20 не портированы вообще (`docs/V20_SOURCE_RECOVERY.md`, раздел
  «Остаток M3: что осознанно не портировано»);
* `vitals: applied 0 refused 50` (`docs/CRASH_DUMP_2026-10-28.md:166`) — не исправлено ни в одной
  копии.

Носитель этих фиксов — не исходники, а отчёты восстановления в `v20-recovery/reports/` (160 файлов)
и та самая старая бинарная база. Пока они не перенесены, удалять старые копии значит выбросить
единственное описание того, что ещё предстоит написать.

### Карта каталогов

| каталог | размер | зачем | вердикт |
| --- | --- | --- | --- |
| `game-build-3598-1/` | 8,5 МБ | родные бинарники игры 3598/1 — вход для `net-v21/tools/hook_audit.py`, `vitals_hook_audit.py` и шести selftest'ов, сверяющих адреса хуков | **держать** |
| `net-v21/` | 1,9 МБ | современный исходник модуля, проекты VS 2026, 48 тестов | **держать** |
| `kit-source/` | 1,4 МБ | сборщик kit: `src/build_release.sh`, `src/verify_artifacts.py`, `src/mp_vitals.c` (85 КБ) + входной v20-бинарь, чей SHA-256 проверяется до изменений | **держать** |
| `v20-recovery/` | 1,3 МБ | инструменты и отчёты восстановления v20; `work/` не коммитится | **держать** |
| `unified-fps-kit-source/` | 773 К | снимок файлов FPS-установщика; вход `scripts/package_unified_fps_mod.py` | держать |
| `unified-kit-source/` | 769 К | снимок файлов NET-установщика; **выход** `kit-source/src/build_release.sh` (шаги 28-29 пишут прямо сюда) и одновременно тестовый фикстур `net-v21/tests/force_test.cpp` | держать (см. ниже) |
| `upstream/` | 508 К | vendored публичный baseline, база для `git apply` патчей `0001`/`0002` | **держать** |
| `fps-candidate/` | 373 К | воспроизводимый FPS-кандидат, пересобирается и сверяется `verify-fps-codelib.yml` | держать |
| `merged-source/` | 112 К | патчи, `PORTING_LEDGER.md`, verifier границ исходника | **держать** |
| `net-v21-kit/` | 104 К | скрипты установщика `net_v21`, `Switch_Pair.bat`; потребители — `unified-installer-smoke.yml`, `package_net_v21_test.py`, `test_switch_pair_windows.py` | **держать** |
| `gtmp-alpha/` | 88 К | альфа-сборка vitals; `package_gtmp_alpha.py`, `verify-alpha-dll.yml` | держать |

Ни один каталог не висит мёртвым: у каждого есть потребитель в `.github/workflows/`, `scripts/`
или тестах, а бинарники вдобавок хэш-закреплены в `scripts/check_artifacts.py` (17 путей).

### Что реально дублируется

10 групп байт-в-байт одинаковых файлов, 1,08 МБ лишнего:

* `unified-kit-source/files/bin/rel/x64/basic_network.codelib` == `kit-source/files/basic_network.codelib`;
* `unified-fps-kit-source/files/bin/rel/x64/basic_network.codelib` == `fps-candidate/basic_network.codelib`;
* `mp_join.exe`, `mp_vitals.dll` и оба `*.progpack` повторены в обоих unified-деревьях.

Два unified-дерева — это один и тот же kit с разным codelib и немного разными текстами
(`README.txt`, `Check_Install.ps1`, `Enable_Multiplayer.bat` у них различаются). Коммитить
бинарные сборочные выходы здесь сделано осознанно: их пересобирают по SHA-256, так что
изменение сборки видно в diff.

### Порядок обновления debug-исходников

При каждом изменении исходников или тестов — [`net-v21/DEBUG_UPKEEP.md`](net-v21/DEBUG_UPKEEP.md)
и `python scripts/check_debug_sources.py` (сверяет списки исходников и тестов в `build.bat`,
проектах VS и `run_tests.bat`).

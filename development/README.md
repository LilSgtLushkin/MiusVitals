# Файлы разработки — не для установки

## Раскладка с 1.0.9

```
development/
  versions/current.txt  текущая версия (сейчас 1.0.9)
  versions/1.0.8/       бэкап: исходник установщика 1.0.8
  versions/1.0.9/       рабочая папка: правки 1.0.9 вносятся здесь
  forbidden/            парковка vitals (исходник, его тест и DLL), работа отложена
  game-build-3598-1/  родные codelib игры 3598/1 — не изменять
  upstream/           vendored публичный baseline v12, база для baseline-сборки
  unified-kit-source/ снимок файлов NET-установщика
  unified-fps-kit-source/ proven-дерево v1.0.1, откуда паковщик берёт runtime-файлы
  net-v21-kit/        скрипты установщика net_v21
  v20-recovery/       отчёты восстановления исходника v20
```

Правила — [`versions/README.md`](versions/README.md): новая версия делается копией предыдущей,
правки вносятся в копию, позапрошлая папка удаляется. Установочные архивы в корне живут по тому же
правилу: актуальная 1.0.9, предыдущая 1.0.8, исторический 1.0.0.

`development/versions/current.txt` — единственный источник текущей версии. Воркфлоу CI держат
её в `env: VERSION:` наверху файла и собирают `development/versions/<версия>/`, так что больше
ни один путь не привязан к имени папки. Скрипты (`check_debug_sources.py`,
`package_net_v21_test.py`, `verify_release.sh`) читают версию из `current.txt`.


Исходник Vitals sidecar и его портируемый тест припаркованы в [`forbidden/development/kit-source/src/`](forbidden/development/kit-source/src/): работа над vitals отложена до выяснения падений, а тест живёт рядом с исходником, потому что тянет его относительным путём. **Не копируйте development-дерево целиком в папку игры.**

## Объединённая исходная рабочая область

По разрешению на выход за прежний предел 16 файлов в репозиторий добавлен закреплённый публичный исходник MiusPlayer вместе с MiusVitals:

- [`upstream/miusplayer-public-43b2736/`](upstream/miusplayer-public-43b2736/) — C++ source/basic_network, program-pack tooling и launcher templates из публичного `VampyrCat/MiusPlayer` commit `43b2736d3b4a1e208e6c80f45f6ebbdd476de3aa` (CC0); `UPSTREAM.json` и `SOURCE_MANIFEST.sha256` фиксируют происхождение;
- [`forbidden/development/kit-source/src/mp_vitals.c`](forbidden/development/kit-source/src/mp_vitals.c) — проверяемый Vitals sidecar для опубликованного v20 binary baseline, припаркован вместе со своим тестом.

Проверка объединённой рабочей области:

```sh
python3 scripts/check_artifacts.py     # одинаковые бинарники во всех kit-источниках, хеши архивов, пути из .gitattributes
```

Публичный release archive содержит `basic_network.codelib` с SHA-256 `8108bd…5723`, совпадающий с input v20 текущего release pipeline. Но имеющийся upstream C++ build output имеет другой hash, поэтому не считается replacement binary: перед выпуском source rebuild нужны новая бинарная mapping, native/PE проверки и two-peer evidence. Подробности — в [`upstream/README.md`](upstream/README.md) и отчётах `v20-recovery/`.

## Установка игрока

Текущий экспериментальный установщик: [`../MiusMultiplayer_v1.0.7_EXPERIMENTAL_NET+FPS_INSTALL.zip`](../MiusMultiplayer_v1.0.7_EXPERIMENTAL_NET%2BFPS_INSTALL.zip). Он сразу устанавливает `net_v21`; инструкции и ограничения — в [корневом README](../README.md). Это единственная сборка 1.0.7 в дереве: прежние архивы 1.0.7 удалены и остаются в истории Git. Сохранены `_known_good` и исторический [`../MiusMultiplayer_OUTDATED_INSTALL.zip`](../MiusMultiplayer_OUTDATED_INSTALL.zip).

Текущий установщик создаётся и проверяется `../scripts/package_net_v21_test.py`: сам модуль и `mp_join.exe` он получает аргументами `--codelib`/`--launcher`, а остальные runtime-файлы — `mp_vitals.dll`, оба `*.progpack`, `steam_appid.txt` и тексты установщика — берёт из `unified-fps-kit-source/` (proven-дерево v1.0.1) и `unified-kit-source/`. Оба дерева — источники и тестовые входы, а не отдельная player install directory.

## Steam-preserving FPS candidate

FPS-кандидат `basic_network.codelib` (SHA-256 `7b42a69a…01f67`) хранится внутри `unified-fps-kit-source/` — того самого proven-дерева v1.0.1, откуда паковщик берёт runtime-файлы; его хеш закреплён в `scripts/check_artifacts.py`. Отдельная папка `fps-candidate/`, скрипт детерминированной пересборки и workflow `verify-fps-codelib.yml` убраны 2026-10-31: кандидат больше не публикуется отдельным архивом, а повторная сборка ничего не доказывала без двухпировых прогонов.

Отдельный `MiusMultiplayer_beta_FPS_fix_INSTALL.zip` и `fps-beta-kit-source/` удалены раньше: второй installer с собственным codelib больше не публикуется. В игре, где остался прежний beta module (включая withdrawn `78e271…2916e`), `MiusMultiplayer\Enable_Multiplayer.bat` из v1.0.1 всё равно заменяет codelib и program packs безусловно.

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
| `unified-fps-kit-source/…/basic_network.codelib` | 378 368 | `GTMF_TA_A1` |
| `game-build-3598-1/x64/basic_network.codelib` | 250 368 | `GTMF_TA_A1` |

`NETV21_WIRE_PROTOCOL_18` задаётся в `net-v21/src/protocol.h` (`k_wire_tag`) и больше нет ни в
одном бинарнике внутри `development/`. Значит: **весь современный код — в `net-v21/src`, а все
`.codelib` в `development/` — это более старые сборки.** `upstream/miusplayer-public-43b2736` —
vendored baseline публичного MiusPlayer (v12); рабочая версия в `versions/` уже вобрала оба
исторических патча (`0001`, `0002`): в ней есть `src/operation_turn.h` и elision-счётчики в
`session.cpp`.

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
| `versions/` | 3,7 МБ | актуальная и предыдущая версии, полный исходник и тесты каждой | **держать** |
| `v20-recovery/` | 1,3 МБ | инструменты и отчёты восстановления v20; `work/` не коммитится | **держать** |
| `unified-fps-kit-source/` | 773 К | proven-дерево v1.0.1: отсюда `scripts/package_net_v21_test.py` берёт `mp_vitals.dll`, `mp_join.exe`, progpack'и и тексты для публикуемого установщика | **держать** |
| `unified-kit-source/` | 769 К | снимок файлов NET-установщика; поставщик `mp_vitals.dll` и progpack'ов для паковщика, тестовый фикстур `tests/force_test.cpp` и образец PE для `tools/pe_scan_selftest.py` | держать (см. ниже) |
| `upstream/` | 508 К | vendored публичный baseline, база для `git apply` патчей `0001`/`0002` | **держать** |
| `net-v21-kit/` | 104 К | скрипты установщика `net_v21`, `Switch_Pair.bat`; потребители — `unified-installer-smoke.yml`, `package_net_v21_test.py`, `test_switch_pair_windows.py` | **держать** |

Ни один каталог не висит мёртвым: у каждого есть потребитель в `.github/workflows/`, `scripts/`
или тестах, а бинарники вдобавок хэш-закреплены в `scripts/check_artifacts.py`. Удалять папку из
этой таблицы можно только вместе с тем, что её читает: 2026-10-31 так ушли `kit-source/`,
`fps-candidate/`, `merged-source/`, `gtmp-alpha/` и три workflow, которые их проверяли.

### Что реально дублируется

10 групп байт-в-байт одинаковых файлов, 1,08 МБ лишнего:

* `mp_join.exe`, `mp_vitals.dll` и оба `*.progpack` повторены в обоих unified-деревьях;
* codelib в двух деревьях различаются: в `unified-kit-source` — стабильный `485b655…102a3`,
  в `unified-fps-kit-source` — FPS-кандидат `7b42a69a…01f67`.

Два unified-дерева — это один и тот же kit с разным codelib и немного разными текстами
(`README.txt`, `Check_Install.ps1`, `Enable_Multiplayer.bat` у них различаются). Коммитить
бинарные сборочные выходы здесь сделано осознанно: их пересобирают по SHA-256, так что
изменение сборки видно в diff.

### Порядок обновления debug-исходников

При каждом изменении исходников или тестов — [`versions/1.0.9/DEBUG_UPKEEP.md`](versions/1.0.9/DEBUG_UPKEEP.md)
и `python scripts/check_debug_sources.py` (сверяет списки исходников и тестов в `build.bat`,
проектах VS и `run_tests.bat`).

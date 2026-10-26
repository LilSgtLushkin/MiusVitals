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

Актуальный installable archive: [`../MiusMultiplayer_v1.0.1_INSTALL.zip`](../MiusMultiplayer_v1.0.1_INSTALL.zip). Единственный устаревший архив — [`../MiusMultiplayer_OUTDATED_INSTALL.zip`](../MiusMultiplayer_OUTDATED_INSTALL.zip) (бывший v1.0.0, только для rollback/provenance). Оба содержат один `MiusMultiplayer/` root с runtime MiusPlayer assets, проверенным `basic_network.codelib` и `mp_vitals.dll`; отдельные `mpmod1`/`MiusVitals` archives больше не устанавливаются.

Архив создаётся и проверяется `../scripts/package_unified_mod.py`. `kit-source/` сохраняется как implementation layer и test input, а не как отдельная player install directory.

## Steam-preserving FPS candidate

`fps-candidate/` хранит воспроизводимый candidate `basic_network.codelib`: `scripts/build_steam_compatible_fps_candidate.py` получает его детерминированно из проверенного Steam 3598/1 module, а workflow `../.github/workflows/verify-fps-codelib.yml` заново собирает файл и сравнивает его byte-for-byte с checked-in копией. Этот же candidate входит в актуальный `MiusMultiplayer_v1.0.1_INSTALL.zip`, поэтому `scripts/package_unified_fps_mod.py` требует с ним полного совпадения.

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

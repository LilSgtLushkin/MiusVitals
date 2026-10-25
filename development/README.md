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
```

Публичный release archive содержит `basic_network.codelib` с SHA-256 `8108bd…5723`, совпадающий с input v20 текущего release pipeline. Но имеющийся upstream C++ build output имеет другой hash, поэтому не считается replacement binary: перед выпуском source rebuild нужны новая бинарная mapping, native/PE проверки и two-peer evidence. Подробности — в [`merged-source/README.md`](merged-source/README.md).

## Установка игрока

Единственный installable archive: [`../MiusMultiplayer_v1.0.0_INSTALL.zip`](../MiusMultiplayer_v1.0.0_INSTALL.zip). Он содержит один `MiusMultiplayer/` root с runtime MiusPlayer assets, проверенным `basic_network.codelib` и `mp_vitals.dll`; отдельные `mpmod1`/`MiusVitals` archives больше не устанавливаются.

Архив создаётся и проверяется `../scripts/package_unified_mod.py`. `kit-source/` сохраняется как implementation layer и test input, а не как отдельная player install directory.

## Отдельный FPS beta candidate

`fps-beta-build/` хранит только reproducible Windows VS2022 candidate `basic_network.codelib`, автоматически созданный workflow `../.github/workflows/build-fps-beta-codelib.yml` из закреплённого public source и transform-write-elision patch. `fps-beta-kit-source/` и `../scripts/package_fps_beta.py` создают второй, явно experimental installer `MiusMultiplayer_beta_FPS_fix_INSTALL.zip`: он автоматически меняет только codelib и умеет восстановить предыдущий recognised module.

Это не runtime replacement для stable MiusPlayer + Vitals: candidate не несёт released `mp_vitals.dll` import/RVA profile и до Windows/two-peer gameplay certification предназначен лишь для FPS comparison. Не смешивайте его installer/remover со stable installer: сначала используйте `Disable_Beta_FPS_Fix.bat`.

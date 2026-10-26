# Наш сетевой модуль (M1 → M2)

Здесь начинается **свой** `basic_network.codelib`: дерево, которое мы собираем, правим и
проверяем, вместо опубликованного v20, в который нельзя внести изменение.

## Что заморожено

Всё, что было до этого каталога, — **бэкап и справочник**, а не рабочая площадка:

* `development/upstream/miusplayer-public-43b2736/` — опубликованный исходник v12, байт в байт;
* `development/v20-recovery/` — разбор v20 (отчёты, декомпиляция, инструменты);
* осн. ветка `main` не меняется.

Ничего из перечисленного здесь не правится. Всё, что нужно изменить, копируется в `src/`
(и это видно в истории git) или пишется с нуля.

## Что внутри

| путь | что это |
|---|---|
| `src/` | копия опубликованного дерева v12 (`SOURCE_MANIFEST.sha256` фиксирует, что копия не тронута) + наши новые файлы |
| `src/steam.h`, `src/steam.cpp` | Steam-слой: 21 `GetProcAddress`, 4 callback'а, rich presence, `+gtmf_join`, стенд `MP_STEAM_FAKE` — по спецификации `docs/V20_STEAM_LAYER.md` |
| `src/steam_module.cpp` | прослойка: `steam_log` → `mp_log`, `steam_install()` / `steam_tick()`, экспорт `gtmf_steam_layer[]` (якорь, чтобы линкер не выбросил слой) |
| `src/steam_selftest.cpp` | автотест слоя без Steam и без игры |
| `build.bat` | `baseline` / `module` / `selftest` — те же ключи `cl`, что в авторском `build.bat` |
| `tools/fetch_references.py` | скачивает v12 и v20 (пин по коммиту, сверка SHA-256) |
| `tools/compare.py` | сравнение двух сборок по функциям: SAME / CHANGED / NEW |

## M1 — базлайн

Вопрос: *воспроизводит ли наша сборка опубликованного дерева релизный v12?*

```bat
build.bat baseline
python tools\compare.py --candidate build\baseline\basic_network.codelib ^
    --candidate-map build\baseline\basic_network.map ^
    --reference references\v12.codelib --reference-kind v12 --label M1
```

Смысл: пока это не сходится, все дальнейшие расхождения нельзя отличить от шума сборки.

## M2 — Steam

Вопрос: *совпадает ли наш Steam-слой с тем, что делает v20?*

```bat
build.bat module
build.bat selftest
build\selftest\steam_selftest.exe
python tools\compare.py --candidate build\module\basic_network.codelib ^
    --candidate-map build\module\basic_network.map ^
    --reference references\v20.codelib --reference-kind v20 ^
    --focus steam,gtmf,presence,upnp,find_local --label M2
```

Самопроверка идёт без Steam: она поднимает стенд `MP_STEAM_FAKE` на loopback и прогоняет
датаграммы в обе стороны, разбор `+gtmf_join=` и выключатель `MP_STEAM=0`.

Обе работы выполняет CI: `.github/workflows/build-network-module.yml`.

## Результаты (2026-10-26)

| | наш | эталон | SAME / CHANGED / NEW |
|---|---|---|---|
| **M1** базлайн vs v12 | 602 функции, 156 379 Б | 602, 156 379 Б | **380 / 192 / 30** |
| **M2** модуль vs v20 | 634 функции, 160 871 Б | 895, 263 242 Б | **354 / 181 / 99** |

Самотест Steam-слоя: **21 проверка, 0 падений**.

Читать цифры так:

* M1: набор функций и их размеры совпали в точности; расхождение по инструкциям — только
  разница компилятора (у автора toolset старше, у нас 14.44).
* M2: 243 функции совпали с v20 побайтово, 120 — на ≥ 0.90. Оставшаяся работа — 261 функция,
  ~102 КБ (M3–M5).
* «NEW» в M2 — это в том числе наши собственные Steam-функции: переписанная реализация не
  обязана совпадать с v20 по инструкциям, её приёмка — строки лога и живой прогон.

Подробный разбор — [`docs/V20_SOURCE_RECOVERY.md` §6a](../../docs/V20_SOURCE_RECOVERY.md) и
[`docs/V20_STEAM_LAYER.md` §8–9](../../docs/V20_STEAM_LAYER.md).

## Правила

1. `src/` правится только копированием опубликованного файла или новым файлом — без правок
   в `development/upstream/`.
2. Любой новый код сначала проходит self-test, потом сравнение с v20, и только потом
   попадает в собранный модуль.
3. Строки в логе — часть контракта: по ним формулируется приёмка (`steam: ready`, `steam:
   switched off`, `net: player connected from ...`).

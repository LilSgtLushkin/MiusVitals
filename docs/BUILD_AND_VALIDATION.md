# Сборка и проверка MiusMultiplayer v1.0.0

## Воспроизводимые входы и артефакты

`development/kit-source/src/build_release.sh` принимает только оригинальный MiusPlayer v20 `basic_network.codelib`; SHA-256 входа проверяется до всех изменений.

| Вариант | Преобразование | SHA-256 |
| --- | --- | --- |
| Исходный MiusPlayer v20 | входной файл | `8108bd1e9af8080ef8b1050512b8311848f5a336acb49a7479388733b4ae5723` |
| Main | rebase 3598/1, verified operation Next Turn, join-launch wait, импорт DLL | `485b655662bd062e16278f9d699a3097b49a08de1948d2500dd589703a3102a3` |
| `nostart` | тот же rebase без Start group, join-launch wait, импорт DLL | `e4317e630a21198a55e0e86be4aa29c71fcada6065824c858acd0e2bdef8adf9` |
| `norebase` | только импорт DLL; диагностический вариант | `d2e34581b42d0573547e221873d7538b64a413267e8ed388abeb46dd42b630d2` |
| `mp_vitals.dll` | Zig 0.17.0, no CRT, KERNEL32 only | `71a1bf3a37923bbe6c8ee84448d0b91705c2bff7382b647f990b9d671ee224df` |

В v7.5.0 codelib-варианты не изменены: release script воспроизводимо получает те же main/`nostart`/`norebase` байты, а изменение находится только в `mp_vitals.dll`. Поэтому текущая проблема global map не выдаётся за исправленную и не является критерием этого выпуска. `verify_artifacts.py` продолжает проверять их exact hashes, DLL-import и сохранённые operation-map signatures, чтобы выпуск случайно не регрессировал существующий artifact.

Новый DLL допускает только KERNEL32 imports, включая `GetCommandLineA` и `SetEnvironmentVariableA`. Во время dependency attach (до кэширования state policy в `basic_network`) он устанавливает отсутствующий `MP_SYNC_STATE=0` исключительно для strict operation-tactical command line; explicit environment setting не меняется. Ни `g_host_mode`, ни map state, ни codelib не записываются.

## Публичный исходный MiusPlayer и полная source workspace

`development/upstream/miusplayer-public-43b2736/` содержит отобранный редактируемый public source snapshot `VampyrCat/MiusPlayer@43b2736d3b4a1e208e6c80f45f6ebbdd476de3aa`: C++ `basic_network`, headers, program-pack Python tooling и mpmod1 templates. Лицензия CC0 и origin зафиксированы в `UPSTREAM.json`; `SOURCE_MANIFEST.sha256` защищает каждый vendored source file. Это расширяет development workspace за пределы player kit, но не добавляет непроверенные файлы в игру.

```sh
python3 development/merged-source/verify_workspace.py
```

Опубликованный public release `basic_network.codelib` имеет SHA-256 `8108bd1e9af8080ef8b1050512b8311848f5a336acb49a7479388733b4ae5723`, что совпадает с проверенным v20 input. Однако C++ output, приложенный самим upstream к source snapshot, имеет другой SHA-256 `eff7ae12c5f440dc91d0fccc9d9578022a383b0485da58793bffd217922f7af5`. Поэтому source rebuild — только candidate для правки/анализа: он не может быть установлен или пропущен через RVA-bound `patch_import.py` до новой binary mapping, PE/native validation и two-peer проверки. Полная карта процесса находится в [`../development/merged-source/README.md`](../development/merged-source/README.md).

## Единый downloadable multiplayer archive

`development/unified-kit-source/` — единственный runtime package source. Он объединяет опубликованные MiusPlayer v20 runtime assets с проверенными MiusVitals `basic_network.codelib` и `mp_vitals.dll`; `kit-source/` остаётся только implementation/test layer и не устанавливается отдельно.

`scripts/package_unified_mod.py` строго проверяет runtime hashes, наличие `basic_network.codelib` в обоих program packs, CRC, fixed ZIP metadata, allow-list и byte identity. В archive ровно 13 файлов под единственным `MiusMultiplayer/` root:

```text
Enable_Multiplayer.bat
Disable_Multiplayer.bat
Check_Install.bat
Check_Install.ps1
README.txt
LICENSE_CC0.txt
game_build_3598_1.sha256
files/bin/rel/x64/basic_network.codelib
files/mp_vitals.dll
files/mp_join.exe
files/root/programs/opgame.progpack
files/root/programs/tacgame.progpack
files/steam_appid.txt
```

Install script сравнивает Steam 6.00 build 3598/1 с historical full-file reference list, но это диагностическая сводка, а не hard gate: актуальный Steam 3598/1 может отличаться от опубликованного старого catalogue. Он по-прежнему принимает только stock/известные предыдущие multiplayer modules, сохраняет foreign program/utility files, копирует DLL до importing codelib и сверяет runtime hashes после copy. Engine-specific hooks защищены своими exact runtime signatures; `unknown game build` в логах означает, что конкретный hook не включился. `Disable_Multiplayer.bat` удаляет только exact unified hashes и восстанавливает сохранённые файлы.

```sh
python3 scripts/package_unified_mod.py --output MiusMultiplayer_v1.0.0_INSTALL.zip
python3 scripts/package_unified_mod.py --verify MiusMultiplayer_v1.0.0_INSTALL.zip
sha256sum MiusMultiplayer_v1.0.0_INSTALL.zip
```

## Локальная проверка

```sh
scripts/test_native.sh
scripts/verify_release.sh
```

Native harness запускается с ASan/UBSan. Помимо прежних тестов смертей/ранений/техники, ACK/retry, duplicate mailbox и transparent type 10/11, он утверждает:

- strict classifier принимает только `tacgame.progpack` + отдельный `19YY_MM_campaign:battle` token (case-insensitive path), отвергает opgame и custom mission; absent-only policy sets `MP_SYNC_STATE=0`, while an explicit value remains byte-for-byte unchanged;
- Vitals state не уходит, не помещается в mailbox и не ACK-ается, пока первый object-number exchange не готов;
- запрос exchange начинается через три секунды и повторяется до первого success;
- success latched для tactical session: последующий raw-ready=0 не создаёт nudge, не закрывает state sync и не отбрасывает receive; disconnect обязательно сбрасывает latch и снова требует exchange;
- default idle policy оставляет оба native immediate нетронутыми, а explicit `MP_IDLE_MS=0`/`500` меняет только проверенные два immediate;
- перед statistic hook выполняется final forced scan;
- stale transform-cache slot вне allocation текущего `basic_render` matrix pool становится engine sentinel `-2`, затем штатно пересоздаётся allocator'ом; exact write-race fallback допускает только подтверждённый effective address;
- dump-shaped malformed third triangle index produces scaled offset `0xfaaec630` and is rejected at the exact triangle-query entry *before* the original renderer executes; valid primary and optional geometry reaches the original function;
- the post-preflight race fallback accepts only the 12 verified geometry loads with an exact effective address and native no-hit epilogue; a mismatch still propagates.

Доказательство старого transform-cache root cause: [`RENDER_CACHE_ROOT_CAUSE_2026-10-25.md`](RENDER_CACHE_ROOT_CAUSE_2026-10-25.md). Доказательство нового geometry root cause и границы фикса: [`TRIANGLE_GEOMETRY_ROOT_CAUSE_2026-10-25.md`](TRIANGLE_GEOMETRY_ROOT_CAUSE_2026-10-25.md).

## Полная пересборка

```sh
python3 -m venv /tmp/mius-v7-venv
/tmp/mius-v7-venv/bin/pip install -r development/kit-source/src/requirements-build.txt
ZIG_PYTHON=/tmp/mius-v7-venv/bin/python development/kit-source/src/build_release.sh \
  /path/to/mpmod1/files/bin/rel/x64/basic_network.codelib
scripts/verify_release.sh
```

Скрипт регенерирует все варианты codelib, собирает DLL, проверяет hashes/PE/map detour и создаёт exact install ZIP. Он не устанавливает ничего в игровую папку.

Нативные и static проверки не заменяют живую пару Graviteam. До заявления, что FPS-причина устранена, нужен two-peer **operational tactical** battle: на обоих ПК должен быть `object-number lists exchanged: yes`, один startup log новой policy, отсутствие `state:` saved-state traffic после exchange и рабочие transforms, shots, damage/death Vitals. Global map сознательно не тестируется и не изменяется в этом выпуске. Финальный forced scan проверен нативно, но это ещё не отдельный блокирующий ACK-barrier при выходе из tactical game: его нельзя считать доказанным без отдельной живой проверки перехода карты, если работа с картой будет вновь разрешена.

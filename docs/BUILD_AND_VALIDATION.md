# Сборка и проверка MiusMultiplayer v1.0.1

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

## Единый тестовый архив net_v21 (v1.0.5-test)

`MiusMultiplayer_v1.0.5_TEST_INSTALL.zip` — экспериментальная сборка, не стабильный выпуск.
Вся папка `MiusMultiplayer` распаковывается рядом с `starter_x64.exe`; один
`Enable_Multiplayer.bat` ставит мод целиком, включая **net_v21 и соответствующий launcher**.
Отдельный switch не нужен и не входит в этот архив. `Disable_Multiplayer.bat` возвращает
предыдущее состояние; `Check_Install.bat` и `Collect_Logs.bat` лежат рядом.

Шесть устанавливаемых файлов проверяются по `FILES_SHA256.txt`. Перед записью создаются и
проверяются backup и снимок транзакции; если откат не удался, требуется повторное восстановление.
При сбое питания/диска или на сетевой ФС полная атомарность не гарантируется. Старые 1.0.3,
1.0.4 и `_known_good` не перезаписываются и сохраняют свою историческую схему переключения.

```sh
python3 scripts/package_net_v21_test.py \
    --codelib development/net-v21/build/module/basic_network.codelib \
    --launcher development/net-v21/build/join/mp_join.exe \
    --output /tmp/MiusMultiplayer_v1.0.5_TEST_LOCAL.zip
python3 scripts/package_net_v21_test.py --verify /tmp/MiusMultiplayer_v1.0.5_TEST_LOCAL.zip
```

ZIP детерминированный: фиксируются время и порядок записей; `TEST_BUILD.txt` содержит SHA-256
всех остальных файлов. Игра, настоящий Steam, совместный бой и реальные сохранения не проверены;
полный порт `mp_vitals` не завершён, а причина падения FPS не доказана. Результаты и пределы
тестов — в `NET_V21_M5_VALIDATION_2026-10-27.md`.

## Публичный исходный MiusPlayer и полная source workspace

`development/upstream/miusplayer-public-43b2736/` содержит отобранный редактируемый public source snapshot `VampyrCat/MiusPlayer@43b2736d3b4a1e208e6c80f45f6ebbdd476de3aa`: C++ `basic_network`, headers, program-pack Python tooling и mpmod1 templates. Лицензия CC0 и origin зафиксированы в `UPSTREAM.json`; `SOURCE_MANIFEST.sha256` защищает каждый vendored source file. Это расширяет development workspace за пределы player kit, но не добавляет непроверенные файлы в игру.

```sh
python3 development/merged-source/verify_workspace.py
```

Опубликованный public release `basic_network.codelib` имеет SHA-256 `8108bd1e9af8080ef8b1050512b8311848f5a336acb49a7479388733b4ae5723`, что совпадает с проверенным v20 input. Однако C++ output, приложенный самим upstream к source snapshot, имеет другой SHA-256 `eff7ae12c5f440dc91d0fccc9d9578022a383b0485da58793bffd217922f7af5`. Поэтому source rebuild — только candidate для правки/анализа: он не может быть установлен или пропущен через RVA-bound `patch_import.py` до новой binary mapping, PE/native validation и two-peer проверки. Полная карта процесса находится в [`../development/merged-source/README.md`](../development/merged-source/README.md).

## Устаревший unified archive (outdated)

`development/unified-kit-source/` — сохранённый source legacy v1.0.0 package; его player artifact публикуется как `MiusMultiplayer_OUTDATED_INSTALL.zip`. Актуальный v1.0.1 source лежит отдельно в `development/unified-fps-kit-source/`, чтобы v1.0.0 оставался доступным для deterministic rollback. Оба объединяют опубликованные MiusPlayer runtime assets с проверенными MiusVitals files; `kit-source/` остаётся только implementation/test layer и не устанавливается отдельно.

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

Install script не содержит проверок, влияющих на замену: все шесть runtime files заменяются безусловно (`attrib` → `del` → `copy`, повтор, копирование через `*.put` + `move`). Ни historical full-file reference list, ни allow-list известных модулей, ни `certutil`, ни SHA-256 после copy не могут отменить замену или оборвать установку. Порядок копирования сохраняет работоспособность при обрыве: DLL → codelib → `mp_join` → program packs → `steam_appid.txt`. Backup прежних файлов best-effort и не блокирует установку; после copy снимается Mark of the Web. `Disable_Multiplayer.bat` удаляет файлы по записи `state\installed.txt`, без сравнения hash-ей, и восстанавливает сохранённые файлы; hash-режим остался только для установок прежних архивов, где такой записи нет. Engine-specific hooks защищены своими exact runtime signatures; `unknown game build` в логах означает, что конкретный hook не включился. Подробный разбор кнопки MULTIPLAYER: [`MULTIPLAYER_MENU_BUTTON_2026-10-25.md`](MULTIPLAYER_MENU_BUTTON_2026-10-25.md).

```sh
python3 scripts/package_unified_mod.py --output MiusMultiplayer_OUTDATED_INSTALL.zip
python3 scripts/package_unified_mod.py --verify MiusMultiplayer_OUTDATED_INSTALL.zip
sha256sum MiusMultiplayer_OUTDATED_INSTALL.zip
```

## Актуальный unified Steam + FPS + Vitals archive v1.0.1

`MiusMultiplayer_v1.0.1_INSTALL.zip` — актуальный single installer. Его source tree — `development/unified-fps-kit-source/`; archive содержит те же Vitals DLL, launcher и program packs, что проверенный unified kit, но заменяет runtime `basic_network.codelib` на exact Steam-preserving FPS candidate:

- candidate SHA-256: `7b42a69abe58deb1583d2f5320eebbecd9718ffa27ea08933613e67df1901f67`;
- stable Steam base SHA-256: `485b655662bd062e16278f9d699a3097b49a08de1948d2500dd589703a3102a3`;
- `.fpsfix` trampoline skips `session_sim_step` only for zero-ms `ctrl_tick`; a nonzero step uses the original function;
- the exact Steam Friends, rich-presence, Join Game, SteamNetworking P2P and Vitals import paths are retained from the base module.

`scripts/package_unified_fps_mod.py` rejects a simple binary substitution: it reconstructs and byte-compares the deterministic transformation, checks the PE trampoline plus every required Steam marker, requires byte identity with `development/fps-candidate/basic_network.codelib`, validates all runtime hashes, both program-pack module references, ZIP allow-list and fixed metadata. The v1.0.1 installer has no migration allow-list and no replacement gate: any active codelib (v1.0.0, withdrawn beta, another mod or an unknown build) is backed up best-effort and replaced. `docs/MULTIPLAYER_MENU_BUTTON_2026-10-25.md` explains why a skipped or half-applied replacement leaves the main menu without the MULTIPLAYER entry.

```sh
python3 scripts/package_unified_fps_mod.py --output MiusMultiplayer_v1.0.1_INSTALL.zip
python3 scripts/package_unified_fps_mod.py --verify MiusMultiplayer_v1.0.1_INSTALL.zip
sha256sum MiusMultiplayer_v1.0.1_INSTALL.zip
```

`MiusMultiplayer_OUTDATED_INSTALL.zip` (the former v1.0.0 archive, unchanged payload) remains retained as a rollback artifact only. The standalone `MiusMultiplayer_beta_FPS_fix_INSTALL.zip` was removed: its only payload is the Steam-preserving FPS `basic_network.codelib`, which the v1.0.1 archive already ships, and two installers with different codelibs only invited mixed installs.

## Steam-preserving FPS candidate (без отдельного архива)

`MiusMultiplayer_beta_FPS_fix_INSTALL.zip`, `scripts/package_fps_beta.py` и `development/fps-beta-kit-source/` удалены: отдельный experimental installer больше не публикуется. Steam-preserving FPS `basic_network.codelib` — единственный payload того архива — входит в v1.0.1, а второй установщик с другим codelib приводил к смешанным установкам и к отсутствию кнопки MULTIPLAYER.

Сам candidate остаётся воспроизводимым и проверяемым:

- `scripts/build_steam_compatible_fps_candidate.py` начинает с exact verified Steam 3598/1 module (`485b655…102a3`) и добавляет отдельную executable `.fpsfix` PE section; её 23-byte trampoline пропускает исходный `session_sim_step` только когда `on_ctrl_tick` получает zero-ms step, а non-zero path вызывает original function без изменения;
- checked-in кандидат лежит в `development/fps-candidate/basic_network.codelib`;
- workflow `.github/workflows/verify-fps-codelib.yml` заново собирает его, сравнивает byte-for-byte с checked-in файлом и затем проверяет v1.0.1 archive через `scripts/package_unified_fps_mod.py --verify`;
- `scripts/package_unified_fps_mod.py` по-прежнему допускает только exact transformation: base hash, original call signature, redirected call, `.fpsfix` code, retained Vitals import и Steam runtime markers `MP_STEAM_PEER`, `+gtmf_join=`, SteamFriends, SteamUser, SteamNetworking, `SendP2PPacket`, `ReadP2PPacket`, `JoinRequested`.

Retained binary surface не заменяет live evidence: до Windows Steam two-peer regression (rich presence host, friends Join Game, P2P lobby, tactical session, reconnect, обе стороны host) candidate остаётся FPS test candidate, не gameplay-certified compatibility release.

## GTMP_alpha: экспериментальный архив поверх v1.0.1

`GTMP_alpha_INSTALL.zip` — не новая версия мода, а **копия v1.0.1 с одной заменой**:
пересобранной `mp_vitals.dll` и перемаркированными установщиками. Модуль
`basic_network.codelib` и все остальные runtime-файлы берутся из стабильного комплекта без
изменений, поэтому alpha физически не может привезти другой codelib.

| Файл | SHA-256 |
|---|---|
| `GTMP_alpha_INSTALL.zip` | `aacfe7dfd0cca82c0ed771e5a9088faa75565f3bf7d5757ceef8d3f532955924` |
| `development/gtmp-alpha/files/mp_vitals.dll` | `808e1d5f6cb301ac971700e0aba6e78d1df1b13b02e748b8d01dd4bfc8a9881c` |
| `files/mp_vitals.dll` в v1.0.1 (для сравнения) | `71a1bf3a37923bbe6c8ee84448d0b91705c2bff7382b647f990b9d671ee224df` |

Сборка DLL воспроизводима побайтово: `development/gtmp-alpha/rebuild.sh` собирает её тем же
`build.sh` (Zig 0.17.0, только `KERNEL32`, без CRT) и сверяет хеш. Тот же скрипт с
`--baseline <rev>` собирает исходник более раннего коммита и должен дать ровно
`71a1bf3a…` — это проверка, что «съехал» не инструмент, а код.

```sh
python3 -m venv .venv && .venv/bin/pip install -r development/kit-source/src/requirements-build.txt
development/gtmp-alpha/rebuild.sh                      # alpha DLL == 808e1d5f…
development/gtmp-alpha/rebuild.sh --baseline e8d4a52   # stable DLL == 71a1bf3a…
python3 scripts/package_gtmp_alpha.py --output GTMP_alpha_INSTALL.zip
python3 scripts/package_gtmp_alpha.py --verify GTMP_alpha_INSTALL.zip
```

`package_gtmp_alpha.py` сначала вызывает `package_unified_fps_mod.require_sources()`, то есть
alpha нельзя собрать поверх непроверенного стабильного комплекта. Затем он подменяет DLL,
заменяет в текстах `v1.0.1` на `GTMP_alpha` и старый хеш DLL на новый, добавляет `ALPHA.txt`
и требует, чтобы в текстах не осталось `v1.0.1`. Замена хеша обязательна и функциональна, а
не косметическая: `Disable_Multiplayer.bat` удаляет DLL только при совпадении `VITALS=`.

Что исправлено в alpha-DLL (подробно — `development/gtmp-alpha/README.md`): чтение
`MP_VITALS`/`MP_VEHICLES`/`MP_STATS`/`MP_RENDER_GUARD` при слишком длинном значении,
верхняя граница `MP_IDLE_MS` 6000 мс, защита `hex_of()` от пустого указателя, отбрасывание
записи короче `VitalsHead` в mailbox, плюс одна строка лога
`vitals: GTMP_alpha-1 experimental build (bugfix series 1)` вместе со значениями
`MP_*`, которые реально получил процесс игры. Сетевой протокол, типы `0x34`/`0x35`,
RVA-профили и сигнатуры не менялись.

В alpha-DLL добавлено ещё два диагностических сообщения: строка
`rx: last 5 s, datagrams from the other game (type:count/bytes) …; total N` пишется
каждые пять секунд, если трафик был (без `MP_LOG_VITALS`), а
`rx: no datagrams from the other game in the first 10 s of this session` — один раз,
если за первые 10 секунд сессии не пришло ни одного пакета. Первая строка сразу
отвечает на вопрос «доходит ли до нас второй игрок и чем именно».

Для сбора отчёта в alpha-архив добавлен `Collect_Logs.bat` (+ `.ps1`): он собирает
**один** файл `MiusMultiplayer\GTMP_alpha_log.txt` — система, версия игры и
`steam_api64.dll`, хеши установленных файлов против эталона alpha, переменные `MP_*`,
`build_report.txt`, выжимка по событиям и полные логи `out\basic_network_*.log`.
Именно этот файл и присылают тестировщики; отдельных логов собирать не нужно.

`sh scripts/verify_release.sh` проверяет и alpha-архив (см. `scripts/check_artifacts.py`:
9 ролей бинарников, 3 архива). Отдельный workflow `verify-alpha-dll.yml` пересобирает DLL на
Ubuntu и сравнивает её с закоммиченной.

## Локальная проверка

```sh
scripts/test_native.sh
scripts/verify_release.sh
```

`scripts/verify_release.sh` дополнительно запускает `scripts/check_artifacts.py`: он сверяет SHA-256 каждой копии одного и того же бинарника в `development/` (один и тот же codelib/DLL/launcher/progpack лежит в нескольких kit-источниках), требует, чтобы **любой** бинарник в дереве был перечислен и проверен, сверяет хеши обоих архивов и проверяет, что пути из `.gitattributes`/`.gitignore` существуют. Это закрывает класс ошибок «обновили один kit-source, а второй остался с прежним файлом» и «правило .gitattributes ссылается на переименованный файл».

`scripts/package_unified_mod.py` и `scripts/package_unified_fps_mod.py` теперь не только проверяют байты и ZIP tree, но и **запрещают возврат гейтов замены**: если в `Enable_Multiplayer.bat` снова появится `certutil`, `known_network`, `copy_checked`, `foreign_network`, `ODD`, `game_build_3598_1.sha256` или `SHA-256 verification failed`, сборка архива падает. Это превращает «файлы заменяются всегда» в проверяемый инвариант, а не в обещание.

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

Нативные и static проверки не заменяют живую пару Graviteam. До заявления, что FPS-причина устранена, нужен two-peer **operational tactical** battle: на обоих ПК должен быть `object-number lists exchanged: yes`, один startup log новой policy, отсутствие `state:` saved-state traffic после exchange и рабочие transforms, shots, damage/death Vitals. Stable v7.5/global-map archive по-прежнему не изменён. Отдельный C++ source candidate для ordered operational map требует самостоятельного Windows/two-peer прогона host phase, authoritative handoff, client attack, обе завершённые фазы, disconnect/reconnect и battle exit перед любым выпуском. Финальный forced scan проверен нативно, но это ещё не отдельный блокирующий ACK-barrier при выходе из tactical game: его нельзя считать доказанным без отдельной живой проверки перехода карты.

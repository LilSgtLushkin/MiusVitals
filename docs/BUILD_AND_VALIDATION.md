# Сборка и проверка MiusVitals v7.5.0

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

## Exact downloadable kit

`development/kit-source/` — редактируемая development структура, не каталог установки. `scripts/package_kit.py` — allow-list единственного installable download: `MiusVitals_v7.5.0_INSTALL.zip`. В архив попадают ровно эти 16 файлов под единственным `MiusVitals/` root:

```text
Check_Build.ps1
Uninstall_Vitals.bat
known_3598.sha256
Check_Build.bat
Install_Vitals.bat
README_VITALS.txt
src/mp_vitals.c
src/patch_import.py
src/build.sh
src/rebase_3598_1.py
src/test/t.c
src/test/stub/windows.h
files/mp_vitals.dll
files/basic_network.codelib
files/alt/basic_network_nostart.codelib
files/alt/basic_network_v5_norebase.codelib
```

ZIP использует фиксированный timestamp 2026-10-25 00:00:00, явный порядок записей, фиксированные mode bits и deflate level 9. `--verify` проверяет CRC, дерево, timestamp и точную byte identity со входами kit.

```sh
python3 scripts/package_kit.py --output MiusVitals_v7.5.0_INSTALL.zip
python3 scripts/package_kit.py --verify MiusVitals_v7.5.0_INSTALL.zip
sha256sum MiusVitals_v7.5.0_INSTALL.zip
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

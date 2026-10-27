# Аудит истории GitHub и сопоставление с tactical net_v21 — 27.10.2026

## Объём и метод

На момент инвентаризации: 23 remote refs, 23 PR (включая открытые #2 и #23), 100 уникальных достижимых commit-объектов, в том числе 75 в выдаче API основной ветки. Инвентарь строился по GitHub refs и PR heads; объекты загружены без переключения текущей ветки. Ниже индекс **всех 100**: метаданные и список изменённых файлов каждого проверены автоматически, функционально значимые изменения отдельно изучены по diff и сопоставлены с текущим C++-кодом. Строка индекса не означает, что каждый бинарный артефакт или декомпиляция подтверждены исполнением. Открытый PR #2 содержит тот же patch-id, что слитый `4f4b1d7`, а не отдельное исправление.

Этот аудит не объявляет старые названия «fix», старые CI или коммиты доказательством работоспособности в игре. Область изменений — tactical M3/M4 и их общая транспортная инфраструктура; логика операционной карты M5 сознательно не переносилась.

## Что сопоставлено и результат

| История | Текущий код и решение | Проверка / граница |
|---|---|---|
| `427ea3c`, #9: не блокировать после первого завершённого обмена ID и не заливать сеть неподвижными объектами | `xlate.cpp`: адреса и готовность проверяются до разрешения полной синхронизации; частичные таблицы повторяются, сообщения ограничены. `session.cpp`: прежний staggered idle cadence, `transform_inbox.h` coalescing с лимитом; одинаковые удалённые записи не трогают граф сцены (`transform_write.h`). | Специальные тесты таблиц и guard-page, unit-тесты и PE-проверка; другая семантика флага readiness, поэтому старый latch не копировался. В игре не доказано. |
| `cd369df`, #10: штатный position-only для тактической операции | `tactical_mode.h` определяет прямой `tacgame.progpack` + `19YY_MM_...:...`; явный `MP_SYNC_STATE` имеет приоритет. Карта M5 не меняется. | 15 сценариев классификации; в игре не подтверждено. |
| `fae26b2`, #21: переполненная env-строка, пределы чисел и диагностика; G7: очередь состояний | В tactical чтения используют ограниченные `env_value.h`, бюджет вычисляется без переполнения `kbps*1024`. Проверяются размеры и версии фрагментов, повторный старый state не откатывает новый, ошибка декодирования допускает повтор, а смена peer очищает старые данные. M5 не меняется. | Граничные unit-тесты и production-path тест с mock-движком; суммарный размер карты входящих состояний всё ещё не ограничен. |
| `4f4b1d7`/`f2b03ad`, `a58807c`, `dbc89f9`, `6cd5418`, #2/#5/#6/#8: renderer/cache/geometry | `render_guard.cpp` повторяет узкие отказы для исследованных инструкций, валидирует фиксированный RVA или единственную сигнатуру, проверяет внутренние участки и диапазон исполнения; логирует срабатывания. | Подтверждены байты предоставленной 3598/1, есть Windows synthetic tests. Живая многопоточная установка detour не атомарна и не проверялась в игре. Исключение не делает произвольную геометрию корректной. |
| `0823f2d`, patch `0001`: не писать прежний transform повторно; `docs/NETWORK_LIMITATIONS.md` G4: пропущенный крен/тангаж | Применено только к tactical remote unit; сравниваются точные биты трансформа и скоростей. Отправитель проверяет все три строки ориентации, сохраняя старую периодичность повтора. | 13 сценариев записи, включая 1000 входов, сопоставленных с прежним алгоритмом, и 11 сценариев отправки; влияние на настоящий renderer не измерено. |
| `69f5c8e`, patch `0002`; `ee9dc54`, `abf2cd1`: очередность и Next Turn | Идеи ограниченных/упорядоченных очередей сопоставлены с M3: `reliable_inbox.h` и `shots.cpp`. Изменения ходов карты и её save/ack не переносились. | Unit и двухпроцессный loopback не доказывают реальный бой или M5. |
| `9324763`, `7ac3bd8`, `a32e6c3`, `47e2caf`: Steam seam, join, счётчики | `steam.cpp` и `session.cpp`: vtable callback по официальному SDK, правильные callback ID, живой broadcast, fallback IP; tactical Steam не требует успешно открытого отдельного UDP-сокета. | ABI/stand-in selftests и 2 процесса с реальным `session.cpp` через Steam stand-in. Настоящий Steam клиент, NAT и игра не проверены. |
| `00a6eac`, `b5c1fe3`, `dc24328`, `8ded800`: retransmit, старый peer, M3/M4, логирование; `NETWORK_LIMITATIONS.md` G1 | Bounded inbox, EndState и подтверждение конца только у движка; тактический хост отклоняет чужой HELLO пока настоящий peer отвечает и допускает смену порта после 3 с тишины. M4 `force.h` остаётся на синтетических/доступных данных. | 8 сценариев защиты и 2 процесса без игры с поддельным третьим отправителем; native hit по частям и анимации ещё не реализованы. |
| `d77050f`, `c7d978e`, `a90ff6f`, `8bb3089` и упаковочные/CI-коммиты | Сохранены 100% replacement, доказанная связка Steam в стандартном установочном маршруте, отдельный экспериментальный модуль, архив отката и проверка хешей. Сборщик логов проверяет отсутствующие файлы и идентичность. | Тестовые архивы не эквивалентны стабильному завершённому 1.0.3: стандартная установка использует прежний проверенный модуль. |

## Риски, которые нельзя честно объявить исправленными

* Нативные попадания по частям и анимации M3: адреса и некоторые байты известны, но ABI, владение и многопоточность пока не доказаны; неверный вызов потенциально повреждает бой. Они не включены в экспериментальный модуль.
* При установке машинного detour другие потоки не останавливаются: trampoline публикуется заранее, но запись нескольких байт не атомарна. Неизвестно, возможно ли в этот момент одновременное исполнение соответствующей функции игрой. Аналогичный риск у существующих detours, в том числе установки трёх UI-хуков, где отказ выделения/защиты может оставить неполную группу.
* `steam_shutdown()` закрывает fake-сокет и снимает readiness; callbacks не unregister при выгрузке DLL. Потенциальный use-after-free при выгрузке во время жизни Steam ещё не исключён; вызывать сторонние API из `DllMain` под loader lock опасно. Процессное завершение и ручная выгрузка — разные случаи.
* Повторяемое EndState облегчает восстановление после потерь, но без подтверждённого протокола распределённого согласования не гарантирует синхронного решения при длительном разделении сети. Реальный Steam/UPnP/IP и результаты двух живых игр требуют отдельного подтверждения.
* В экстраполяции трансформов ещё не учтено состояние паузы (G3), а отдельного общего бюджета для потока трансформов нет (G5); грубое отключение предсказания или ограничение без игрового теста может ухудшить синхронизацию. Карта `g_incoming` в полном состоянии пока не имеет общего лимита памяти.
* При асимметричном разрыве и перезапуске только одного процесса полное восстановление потока событий требует явного согласования нового epoch: без него нельзя одновременно безопасно сбрасывать очереди и сохранять неподтверждённые выстрелы. При текущем reconnect epoch гостя не сбрасывается вслепую; этот случай не доказан.

## Полный индекс коммитов

Категория выводится из изменённых путей и является указателем для ревизии, а не рейтингом качества коммита. Merge и бинарные/архивные обновления сохранены в индексе, но не дублируют самостоятельную реализацию.

| Дата (UTC) | Commit | Описание | Основные области |
|---|---|---|---|
| 2026-10-27 | `3a0f36a` | build: refresh the net_v21 test installer [skip ci] | архив/документы/история |
| 2026-10-27 | `8ded800` | Harden tactical event delivery and 1.0.3 test diagnostics | сетевой исходник, логи/установка, CI/инструменты |
| 2026-10-27 | `ce1476c` | Add supplied x64 game reference and verify its hashes | CI/инструменты |
| 2026-10-27 | `6529dce` | tools: pe_scan takes an address as well as a signature | CI/инструменты |
| 2026-10-27 | `d7d9982` | Merge pull request #22 from LilSgtLushkin/arena/97cfb6a3-miusvitals | сетевой исходник, исторический Vitals, кандидат-патч, FPS/установщик, логи/установка, CI/инструменты |
| 2026-10-26 | `716b112` | docs: what M3 and M4 do, what was proved, and what they cannot do yet | архив/документы/история |
| 2026-10-26 | `7c22a3b` | build: refresh the net_v21 test installer [skip ci] | архив/документы/история |
| 2026-10-26 | `b5c1fe3` | net_v21: a battle left over from the last opponent, and a download that gives up | сетевой исходник |
| 2026-10-26 | `c9261bd` | build: refresh the net_v21 test installer [skip ci] | архив/документы/история |
| 2026-10-26 | `dc24328` | net_v21: a battle can end by agreement, and both players' forces go into it (M3, M4) | сетевой исходник, CI/инструменты |
| 2026-10-26 | `df47fa6` | net_v21: the known-good archive stays, and the wire format has a test | сетевой исходник, CI/инструменты |
| 2026-10-26 | `4d4a609` | Merge pull request #21 from LilSgtLushkin/arena/82e742ee-miusvitals | архив/документы/история |
| 2026-10-26 | `c369d0a` | build: refresh the net_v21 test installer [skip ci] | архив/документы/история |
| 2026-10-26 | `00a6eac` | net_v21: 1.0.0 archive, retransmit backoff, and the hook chart | сетевой исходник, CI/инструменты |
| 2026-10-26 | `c78cb5a` | build: refresh the net_v21 test installer [skip ci] | архив/документы/история |
| 2026-10-26 | `0dba06f` | net_v21: hooks that survive a game update, honest traffic counters, version 1.0.0 | сетевой исходник |
| 2026-10-26 | `72dcc9a` | Merge remote-tracking branch 'origin/arena/97cfb6a3-miusvitals' into arena/1cf0c195-miusvitals Co-authored-by: arena-agent <297053741+arena-agent@users.noreply.github.com> | архив/документы/история |
| 2026-10-26 | `ff89a8c` | docs: the seam is real, and the two M3 pieces are deliberately not ported | архив/документы/история |
| 2026-10-26 | `060d280` | tools: read a two-PC battle log and say where the two games stopped agreeing | CI/инструменты |
| 2026-10-26 | `732ed1c` | build: refresh the net_v21 test installer [skip ci] | архив/документы/история |
| 2026-10-26 | `a32e6c3` | steam: count what the transport actually moves, the way v20 does | сетевой исходник |
| 2026-10-26 | `a99ef75` | build: refresh the net_v21 test installer [skip ci] | архив/документы/история |
| 2026-10-26 | `ee1e542` | build: the join target has to reach the compiler setup too | архив/документы/история |
| 2026-10-26 | `d46c35b` | ci: mirror the build output into whatever pull request is open for the branch | CI/инструменты |
| 2026-10-26 | `9324763` | steam: make the transport seam real - a Steam peer is a sockaddr_in, and everything else stays | сетевой исходник, CI/инструменты |
| 2026-10-26 | `d99ae1f` | compare: list every missing work item, not the top twenty | CI/инструменты |
| 2026-10-26 | `5d1acd8` | build: refresh the net_v21 test installer [skip ci] | архив/документы/история |
| 2026-10-26 | `c400490` | packaging: os.replace needs os imported, even after the commit field is gone | CI/инструменты |
| 2026-10-26 | `17b2073` | build: keep the baseline published, and tell people about the log and the test archive | CI/инструменты |
| 2026-10-26 | `c9950cd` | build: refresh the net_v21 test installer [skip ci] | архив/документы/история |
| 2026-10-26 | `0fd6f12` | netlog selftest: read the file after closing it, the way a report does | сетевой исходник |
| 2026-10-26 | `db33dc5` | log: make the module's log the thing a bug report needs; package a test installer | сетевой исходник, логи/установка, CI/инструменты |
| 2026-10-26 | `cd9e094` | compare: count the verified v20 names as facts, not hints | CI/инструменты |
| 2026-10-26 | `1e89cfa` | compare: report coverage per milestone, the way the work is actually accepted | CI/инструменты |
| 2026-10-26 | `ee00ef9` | docs: the M1 and M2 numbers, and what they do not mean | архив/документы/история |
| 2026-10-26 | `47e2caf` | build: stop the linker from dropping the Steam layer it just compiled | сетевой исходник |
| 2026-10-26 | `7ac3bd8` | steam: put the Join Game text where friends' clients look for it | сетевой исходник |
| 2026-10-26 | `46e6779` | ci: put both comparison reports in the PR comment, logs behind a fold | CI/инструменты |
| 2026-10-26 | `10141ee` | compare: report how close the match is, and name our own functions in the focus table | CI/инструменты |
| 2026-10-26 | `eeac681` | compare.py: parse_map needs the image size, not an image it cannot see | CI/инструменты |
| 2026-10-26 | `2dc89ff` | ci: capture the comparison output too | CI/инструменты |
| 2026-10-26 | `263a97b` | steam selftest: send the reply to the stand-in's port, not to the helper's own socket | сетевой исходник |
| 2026-10-26 | `a0b6296` | ci: capture the self-test's own output, not just the compiler's | CI/инструменты |
| 2026-10-26 | `b7a061b` | net-v21: prefix every source with src\, not just the first one | архив/документы/история |
| 2026-10-26 | `c366831` | ci: mirror the MSVC output into a pull-request comment | CI/инструменты |
| 2026-10-26 | `bb9b37e` | net-v21: make the build script survive a cmd step and keep its output | CI/инструменты |
| 2026-10-26 | `ffbcb40` | net-v21: own module tree, M1 baseline build and M2 Steam layer | сетевой исходник, CI/инструменты |
| 2026-10-26 | `c2cedc2` | v20: one set of classification rules for every report | архив/документы/история |
| 2026-10-26 | `271d611` | v20: complete decompilation, curated dumps and the network layer | архив/документы/история |
| 2026-10-26 | `3456fda` | Recovery tooling in CI, Steam transport seam, docs index | CI/инструменты |
| 2026-10-26 | `91a77a7` | Recover v20: named breakdown, backlog and readable decompilation | архив/документы/история |
| 2026-10-26 | `fae26b2` | Analyze the v20 module, fix small mp_vitals defects, ship GTMP_alpha | исторический Vitals, FPS/установщик, логи/установка, CI/инструменты |
| 2026-10-25 | `e8d4a52` | Merge pull request #20 from LilSgtLushkin/arena/7406dde3-miusvitals | архив/документы/история |
| 2026-10-25 | `b56313c` | Merge pull request #19 from LilSgtLushkin/arena/3d3e9417-miusvitals | архив/документы/история |
| 2026-10-25 | `0dad0bc` | Merge pull request #18 from LilSgtLushkin/arena/d9d56237-miusvitals | архив/документы/история |
| 2026-10-25 | `58ab92d` | Merge pull request #17 from LilSgtLushkin/arena/38b87943-miusvitals | архив/документы/история |
| 2026-10-25 | `a4deb0c` | Merge pull request #16 from LilSgtLushkin/arena/42e67aee-miusvitals | архив/документы/история |
| 2026-10-25 | `c302dad` | Merge pull request #15 from LilSgtLushkin/arena/ffb3a2ec-miusvitals | архив/документы/история |
| 2026-10-25 | `b87cf28` | Merge pull request #14 from LilSgtLushkin/arena/e79a50d5-miusvitals | архив/документы/история |
| 2026-10-25 | `7243ce5` | Merge pull request #13 from LilSgtLushkin/arena/ee83ba06-miusvitals | архив/документы/история |
| 2026-10-25 | `f9428f8` | Merge pull request #12 from LilSgtLushkin/arena/25af671f-miusvitals | архив/документы/история |
| 2026-10-25 | `221ca30` | Merge pull request #11 from LilSgtLushkin/arena/b80c8c72-miusvitals | архив/документы/история |
| 2026-10-25 | `cb17c41` | Merge pull request #10 from LilSgtLushkin/arena/6a7ce181-miusvitals | архив/документы/история |
| 2026-10-25 | `d5e40a7` | Merge pull request #9 from LilSgtLushkin/arena/a496e0bd-miusvitals | архив/документы/история |
| 2026-10-25 | `be3daa6` | Merge pull request #8 from LilSgtLushkin/arena/2eec7710-miusvitals | архив/документы/история |
| 2026-10-25 | `b9838c8` | Merge pull request #7 from LilSgtLushkin/arena/8617d90b-miusvitals | архив/документы/история |
| 2026-10-25 | `4918181` | Merge pull request #6 from LilSgtLushkin/arena/497dd299-miusvitals | архив/документы/история |
| 2026-10-25 | `3f0f2a8` | Merge pull request #5 from LilSgtLushkin/arena/27f4fc16-miusvitals | архив/документы/история |
| 2026-10-25 | `43b7912` | Drop the beta_FPS_fix archive and publish v1.0.0 as outdated | FPS/установщик, CI/инструменты |
| 2026-10-25 | `7714190` | Merge pull request #4 from LilSgtLushkin/arena/b54c37f7-miusvitals | архив/документы/история |
| 2026-10-25 | `d77050f` | Replace multiplayer files unconditionally and diagnose the menu entry | FPS/установщик, логи/установка, CI/инструменты |
| 2026-10-25 | `8bb3089` | publish unified Steam FPS installer v1.0.1 | FPS/установщик, логи/установка, CI/инструменты |
| 2026-10-25 | `c7d978e` | preserve Steam friends in FPS beta candidate | FPS/установщик, CI/инструменты |
| 2026-10-25 | `ebd7a6e` | Merge pull request #3 from LilSgtLushkin/arena/b551d0f9-miusvitals | архив/документы/история |
| 2026-10-25 | `69f5c8e` | stage ordered operation network source candidate | кандидат-патч, CI/инструменты |
| 2026-10-25 | `f66a8b1` | allow beta installer to restore unknown codelibs | FPS/установщик, CI/инструменты |
| 2026-10-25 | `ed87303` | publish reproducible beta FPS fix installer | FPS/установщик, CI/инструменты |
| 2026-10-25 | `10b0de9` | build reproducible FPS beta codelib [skip ci] | FPS/установщик |
| 2026-10-25 | `1dd6235` | add reproducible FPS beta codelib build | FPS/установщик, CI/инструменты |
| 2026-10-25 | `d5c4fc8` | Merge pull request #1 from LilSgtLushkin/arena/33c8e36e-miusvitals | архив/документы/история |
| 2026-10-25 | `0823f2d` | stage safe transform sync candidate | исторический Vitals, кандидат-патч |
| 2026-10-25 | `a90ff6f` | allow supported 3598/1 install past stale hash catalogue | CI/инструменты |
| 2026-10-25 | `7df3223` | target Steam build 6.00 3598/1 consistently | исторический Vitals, CI/инструменты |
| 2026-10-25 | `d81fa1f` | unify MiusPlayer and Vitals install archive | логи/установка, CI/инструменты |
| 2026-10-25 | `dc64e4c` | vendor public MiusPlayer source workbench | CI/инструменты |
| 2026-10-25 | `6a9a295` | update CI artifact for v7.5 kit | CI/инструменты |
| 2026-10-25 | `cd369df` | v7.5 restore operation tactical state policy | исторический Vitals, логи/установка, CI/инструменты |
| 2026-10-25 | `427ea3c` | Fix tactical sync load and harden kit install | исторический Vitals, логи/установка, CI/инструменты |
| 2026-10-25 | `67c5fd2` | Correct release version to v7.3.0 | логи/установка, CI/инструменты |
| 2026-10-25 | `dbc89f9` | Release v7.1.0 geometry crash fix | исторический Vitals, логи/установка, CI/инструменты |
| 2026-10-25 | `ee9dc54` | Fix operation map Next Turn launch race | логи/установка, CI/инструменты |
| 2026-10-25 | `6cd5418` | Repair stale basic render transform cache | исторический Vitals, логи/установка, CI/инструменты |
| 2026-10-25 | `abf2cd1` | Release v7.2.0 map sync and Vitals readiness gate | исторический Vitals, логи/установка, CI/инструменты |
| 2026-10-25 | `a58807c` | Guard confirmed second basic_render crash | исторический Vitals, логи/установка, CI/инструменты |
| 2026-10-25 | `2f55625` | Clarify development and installation layouts | исторический Vitals, логи/установка, CI/инструменты |
| 2026-10-25 | `8e9efb5` | Release MiusVitals v7.1.0 kit | исторический Vitals, логи/установка, CI/инструменты |
| 2026-10-25 | `4f4b1d7` | Guard recurring basic_render crash | исторический Vitals |
| 2026-10-25 | `f2b03ad` | Guard recurring basic_render crash | исторический Vitals |
| 2026-10-25 | `1a99607` | Build and document MiusVitals v7 | исторический Vitals, логи/установка, CI/инструменты |
| 2026-10-25 | `eeb42d4` | Initial commit | архив/документы/история |

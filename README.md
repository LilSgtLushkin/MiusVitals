# MiusMultiplayer

## 1.0.7 EXPERIMENTAL net+fps — единый тестовый установщик

**Скачать:** [`MiusMultiplayer_v1.0.7_EXPERIMENTAL_NET+FPS_CURRENT_INSTALL.zip`](MiusMultiplayer_v1.0.7_EXPERIMENTAL_NET%2BFPS_CURRENT_INSTALL.zip)
— текущая сборка из `main`. Её собирает CI из коммита, прошедшего полный gate, и публикует в ту же
ветку, поэтому архив обновляется вместе с исходником. SHA-256 сборки и коммит, из которого она
собрана, записаны рядом, в
[`…_CURRENT_INSTALL.zip.sha256.txt`](MiusMultiplayer_v1.0.7_EXPERIMENTAL_NET%2BFPS_CURRENT_INSTALL.zip.sha256.txt);
совпадение проверяется при каждой проверке релиза.

Закреплённый архив
[`MiusMultiplayer_v1.0.7_EXPERIMENTAL_NET+FPS_INSTALL.zip`](MiusMultiplayer_v1.0.7_EXPERIMENTAL_NET%2BFPS_INSTALL.zip)
(SHA-256 `d733efa9f085f65d1f5db36bf8b004e5945329625d0bb6b468590d936b490bb8`) остаётся прежним и не
перезаписывается: это сборка, испытанная в сетевой игре 27–29.10.2026, без последующих правок.
Новый архив отличается от неё составом модуля.

Это отдельный экспериментальный архив на основе 1.0.7 EXPERIMENTAL со всеми её сетевыми
изменениями и новым быстрым контролем кэша рендера. Используйте один и тот же архив на обоих ПК;
установщик сразу включает `net_v21`. Сохранённые резервные копии не заменяются; архивы
1.0.1–1.0.6 удалены из текущего дерева. Реальная игра, Steam, FPS и отсутствие вылетов не
проверены; заранее сохраните профили и операции.

По умолчанию строгая дорогостоящая проверка кэша не включается. Она возможна только при явно
заданном `MP_RENDER_CACHE_GUARD=strict`. Если быстрый перехват не установлен, строгий режим сам
не включится: останется лишь узкий обработчик write-AV, а при несовпадении его сигнатур — никакой.
Быстрая проверка не обнаружит запись в чужую доступную память при повреждении метаданных пула.
Подробности, переключение, откат и пределы тестов — в [описании net+fps](docs/NET_V21_107_EXPERIMENTAL_NET_FPS.md).

## Предыдущая 1.0.7 EXPERIMENTAL — единый тестовый установщик

**Скачать:** [`MiusMultiplayer_v1.0.7_EXPERIMENTAL_INSTALL.zip`](MiusMultiplayer_v1.0.7_EXPERIMENTAL_INSTALL.zip)

SHA-256: `60e7b7a2f2914ae75cf203f48aacc89e9469f71b15b654f70805d50e50736f78`.

Это эксперимент для **Graviteam Tactics: Mius-Front**, Steam build 3598/1, а не стабильный релиз.
Игра и настоящий Steam с этой сборкой не проверены: возможны вылеты, рассинхронизация, неверные
итоги боя и повреждение сохранений. На обоих ПК используйте **один и тот же новый архив**;
предыдущий [`1.0.7 TEST`](MiusMultiplayer_v1.0.7_TEST_INSTALL.zip) сохранён отдельно.

1. Закройте игру на обоих ПК и отдельно скопируйте профили и сохранения операции.
2. Распакуйте папку `MiusMultiplayer` из архива рядом со `starter_x64.exe` и запустите
   `MiusMultiplayer\Enable_Multiplayer.bat`. `net_v21` уже входит в установку: `Switch` не нужен.
3. Для проверки файлов запустите `Check_Install.bat`. Хост отдаёт приказы и выполняет ходы на
   оперативной карте, клиент наблюдает и участвует в бою. F9 не завершает бой автоматически:
   оба игрока завершают его штатными средствами игры.
4. Для отката после закрытия игры запустите `Disable_Multiplayer.bat`. Если EXPERIMENTAL стоял
   поверх прежнего мода, этот шаг вернёт его файлы; для возврата к исходной игре затем распакуйте
   сохранённый архив прежней версии и выполните её `Disable_Multiplayer.bat`, сохраняя `state`.
   Архивы 1.0.1–1.0.6 в текущем дереве удалены, но остаются в истории Git.

Протокол остаётся 17. Именное сопоставление выстрелов используется только при выполнении его
проверок; иначе остаётся рискованная старая эвристика. Старый приёмник 1.0.7 не получает защиту
этого эксперимента. Подробности, условия проверки и ограничения — в
[`docs/NET_V21_NAMED_SHOTS_EXPERIMENTAL.md`](docs/NET_V21_NAMED_SHOTS_EXPERIMENTAL.md)
и `README.txt` внутри архива. Отдельного переключения модулей и замены архива 1.0.7 нет.

## История: MiusMultiplayer v1.0.1

Разделы ниже относятся к **старой v1.0.1**, а не к установщику 1.0.7 EXPERIMENTAL; их описание
файлов, диагностики, импорта DLL, глобальной карты и отката не следует переносить на новую сборку.

Единый multiplayer-мод для **Graviteam Tactics: Mius-Front** (Steam game build 3598/1). v1.0.1 объединяет Steam-compatible FPS candidate, MiusPlayer runtime и проверенный MiusVitals sidecar в одном устанавливаемом наборе.

## Историческое описание v1.0.1 — не для установки

Архивы 1.0.1–1.0.6 удалены из текущего дерева репозитория. Описание этого старого
установщика сохранено для истории и не является инструкцией для текущей версии.

Архив v1.0.1 содержал единый корень `MiusMultiplayer/` и имел SHA-256 `13dd5b317f05ee9f6852cb75671af0f3c847b040e622799b7afb19efe07479b6`. Для текущей установки используйте раздел 1.0.7 EXPERIMENTAL net+fps выше.

Внутри v1.0.1 находится `basic_network.codelib` SHA-256 `7b42a69abe58deb1583d2f5320eebbecd9718ffa27ea08933613e67df1901f67`: это Steam-capable 3598/1 module с проверенным `.fpsfix` trampoline. Он пропускает `session_sim_step` только при zero-ms `ctrl_tick` и оставляет original path для non-zero steps. Steam Friends, rich presence, Join Game, SteamNetworking P2P, Vitals import/signature surface и остальные текущие runtime fixes остаются в том же codelib.

`Enable_Multiplayer.bat` заменяет все шесть runtime files **безусловно**: в установщике больше нет ни allow-list известных модулей, ни `certutil`, ни SHA-256 после copy, ни чтения `game_build_3598_1.sha256`. Любой прежний `basic_network.codelib` (v1.0.0, withdrawn beta, другой мод, неизвестная сборка) сохраняется best-effort и заменяется; сбой backup-а установку не прерывает. Если файл физически не записывается, установщик печатает `NOT REPLACED` и всё равно заменяет остальные. Historical full-file game hash list остался только диагностикой в `Check_Install.bat`, а engine-specific hooks включаются только после собственных точных runtime signature checks. Причина этого изменения и разбор того, почему кнопка MULTIPLAYER может не появиться, — в [`docs/MULTIPLAYER_MENU_BUTTON_2026-10-25.md`](docs/MULTIPLAYER_MENU_BUTTON_2026-10-25.md).

### Какие архивы сохранены

| Архив | Назначение |
|---|---|
| [`MiusMultiplayer_v1.0.7_EXPERIMENTAL_NET+FPS_CURRENT_INSTALL.zip`](MiusMultiplayer_v1.0.7_EXPERIMENTAL_NET%2BFPS_CURRENT_INSTALL.zip) | текущий установщик из `main`, публикуется CI после зелёного gate |
| [`MiusMultiplayer_v1.0.7_EXPERIMENTAL_NET+FPS_INSTALL.zip`](MiusMultiplayer_v1.0.7_EXPERIMENTAL_NET%2BFPS_INSTALL.zip) | закреплённая сборка 27.10.2026, испытанная в игре, больше не перезаписывается |
| [`MiusMultiplayer_v1.0.7_EXPERIMENTAL_INSTALL.zip`](MiusMultiplayer_v1.0.7_EXPERIMENTAL_INSTALL.zip), [`MiusMultiplayer_v1.0.7_TEST_INSTALL.zip`](MiusMultiplayer_v1.0.7_TEST_INSTALL.zip) | предыдущие сборки 1.0.7 |
| [`MiusMultiplayer_v1.0.0_TEST_INSTALL_known_good.zip`](MiusMultiplayer_v1.0.0_TEST_INSTALL_known_good.zip) | сохранённая контрольная копия для отката |
| [`MiusMultiplayer_v1.0.0_TEST_INSTALL.zip`](MiusMultiplayer_v1.0.0_TEST_INSTALL.zip) | прежняя тестовая сборка `net_v21` |
| [`GTMP_alpha_INSTALL.zip`](GTMP_alpha_INSTALL.zip) | исторический эксперимент с другой `mp_vitals.dll` |
| [`MiusMultiplayer_OUTDATED_INSTALL.zip`](MiusMultiplayer_OUTDATED_INSTALL.zip) | прежний установщик v1.0.0, только для отката и истории |

У этих архивов есть различия в составе или бинарных файлах; одинаковый базовый установщик внутри тестовых ZIP не делает их одинаковыми сборками. Результат проверки и удалённый дубликат — в [инвентаре архивов](docs/ARCHIVE_INVENTORY_2026-10-27.md).

Отдельный experimental `MiusMultiplayer_beta_FPS_fix_INSTALL.zip` **удалён**: его единственный payload (Steam-preserving FPS `basic_network.codelib`) давно входит в v1.0.1, а два installer'а с разными codelib только путали установку. Вместе с архивом удалены `scripts/package_fps_beta.py` и `development/fps-beta-kit-source/`; воспроизводимая сборка самого FPS candidate остаётся в [`development/fps-candidate/`](development/fps-candidate/) и проверяется workflow [`verify-fps-codelib.yml`](.github/workflows/verify-fps-codelib.yml).

### GTMP_alpha — экспериментальная сборка поверх v1.0.1

[`GTMP_alpha_INSTALL.zip`](GTMP_alpha_INSTALL.zip) — это копия v1.0.1, в которой заменён только `mp_vitals.dll` (пересобранная из исходника с мелкими исправлениями) и перемаркированы установщики. `basic_network.codelib` и остальные runtime-файлы побайтово совпадают с v1.0.1, поэтому alpha не может привезти другой модуль. SHA-256 архива `aacfe7dfd0cca82c0ed771e5a9088faa75565f3bf7d5757ceef8d3f532955924`, alpha-DLL `808e1d5f6cb301ac971700e0aba6e78d1df1b13b02e748b8d01dd4bfc8a9881c`.

Alpha — исторический эксперимент и не является текущим установщиком 1.0.7. Признак загрузки alpha в логе — строка `vitals: GTMP_alpha-1 experimental build (bugfix series 1)`.

После прогона запустите `MiusMultiplayer\Collect_Logs.bat` — он соберёт один файл `MiusMultiplayer\GTMP_alpha_log.txt` (система, версия игры, хеши установленных файлов, переменные `MP_*`, отчёт Check_Install, выжимка по событиям и полные логи). Для разбора достаточно этого файла с каждого ПК. Подробности, список исправлений и откат — [`development/gtmp-alpha/README.md`](development/gtmp-alpha/README.md) и `ALPHA.txt` внутри архива.

Исторически для замены withdrawn beta module использовали установщик v1.0.1; этот архив удалён из текущего дерева.

Перед campaign/FPS comparison обязателен Steam two-PC regression: rich-presence host → friend Join Game → P2P lobby → tactical session → reconnect/exit, в обеих host directions. До этой живой проверки Steam/FPS результат не следует считать gameplay-certified.

> Не устанавливайте [`development/`](development/). Это расширенная рабочая область: source MiusPlayer/MiusVitals, manifests и проверки. Для старой v1.0.1 использовался `MiusMultiplayer_v1.0.1_INSTALL.zip`; для редактирования/полной source rebuild цепочки см. [`development/merged-source/`](development/merged-source/). Переданная папка игры `x64` сохранена как справочный снимок в [`development/game-build-3598-1/`](development/game-build-3598-1/) и не является установщиком.

Для обоих игроков нужен один и тот же архив; инструкции для текущей сборки и отката приведены в начале этого файла.

## Кнопка MULTIPLAYER в главном меню

Кнопку рисует сам `basic_network.codelib` внутри главного меню, поэтому она существует только если Windows загрузила модуль и engine создал объект `i_network`. Наиболее частые причины её отсутствия:

- установщик прежней версии отказался заменить `basic_network.codelib` (`existing ... is unknown`) или оборвался на проверке SHA-256 после copy — например, когда `certutil` недоступен, запрещён политикой или не разобрал путь; в результате оставался `codelib` без `root\programs\opgame.progpack` или без `mp_vitals.dll`;
- нет `mp_vitals.dll` рядом со `starter_x64.exe`: у `basic_network.codelib` **жёсткий статический импорт** `mp_vitals.dll!MpVitalsInit`, и без него Windows не загружает сам модуль;
- нет Microsoft Visual C++ 2015-2022 x64 runtime (`vcruntime140.dll`, `vcruntime140_1.dll`, `msvcp140.dll`);
- Smart App Control, SmartScreen, антивирус или Mark of the Web заблокировали загрузку неподписанного модуля (теперь установщик снимает Mark of the Web, а `Check_Install.bat` это диагностирует);
- `root\programs\opgame.progpack` остался стоковым/чужим: stock-программа сама грузит `basic_network.codelib`, поэтому модуль пишет лог, но её `r_main` не вызывает `on_render`, и кнопка не рисуется;
- игра другой сборки: смещения главного меню в модуле и program packs рассчитаны на Steam 6.00 build 3598/1.

Диагностика: `Check_Install.bat` (только чтение) печатает все импорты модуля и неразрешённые имена, наличие `mp_vitals.dll` и VC++ runtime, содержимое `opgame.progpack`, Mark of the Web, состояние Smart App Control и последний `out\basic_network_opgame_*.log`. Модуль пишет лог при загрузке, поэтому **нет лога после запуска игры = Windows не загрузила модуль**; лог есть, но нет строки `ui: Direct3D device` = модуль загружен, но render call не вызван (не заменён `opgame.progpack`) или главное меню не распознано (другая сборка игры).

## Что входит в единый мод

### Глобальная карта — без изменений в этом выпуске

Stable v7.5.0 по-прежнему **не** выдаёт global-map проблему за исправленную: его main/`nostart` codelib и hash `485B655662BD062E16278F9D699A3097B49A08DE1948D2500DD589703A3102A3` не менялись. Отдельный, пока неinstallable C++ source candidate для ordered host → client operational turns, authoritative handoff и client-initiated attacks описан в [`development/merged-source/README.md`](development/merged-source/README.md). Он не входит ни в current, ни в outdated archive до Windows/two-peer validation.

Проверка карты не является критерием для данного FPS-исправления. Не подменяйте codelib вручную и не вводите map throttle или guessed ownership patch. Когда работа с картой будет вновь разрешена, потребуются обе полные `basic_network_opgame_*.log`, `MiusMultiplayer/build_report.txt` с обоих ПК и созданный после hand-off `.opsave`.

### Номера объектов — обязательное условие Vitals

Нативная загрузка object state ждёт object/description table. Поэтому Vitals с первых секунд боя запрашивает native exchange каждые 3 секунды, пока он не завершится, и до этого не посылает, не применяет и не ACK-ает Vitals state.

**Исправление v7.5.0:** `ready` v20 — это transient transfer-cycle flag. После первого успеха он способен штатно кратко стать `0`; раньше DLL ошибочно воспринимала это как новый session и каждые ~3 секунды снова запускала перевод примерно 3 000 descriptions и 632 objects. Теперь первый успешный exchange latched на весь tactical session: Vitals продолжает безопасный state sync и больше не нудит native exchange, пока не сменятся tactical object manager, выбранный peer или соединение не закроется.

В логе нормального старта остаются только:

```text
vitals: waiting for object-number exchange before tactical state sync
vitals: object-number exchange complete; tactical state sync enabled
```

Если первый exchange не завершается, сохраните оба полных `basic_network_tacgame_*.log`: бой не следует считать пригодным для проверки синхронизации результата.

### Тактический FPS: устранена выбранная первопричина

Логи 6–10 FPS показали не просто «много юнитов»: один и тот же **operational tactical** запуск (например, `1942_07_Voronezh:04_06_07_shilovo_na_ger`) работал с полным saved-object state replication. Это противоречит назначению operation: там авторитетны позиции, shots/hits и Vitals, а тяжёлые object states не должны сериализоваться. В slow окнах это давало 5.2–6.4 тыс. `state:` объектов за 5 секунд, 1.28–1.35 MB/s и повторную работу remote transforms.

**v7.5.0 не ставит rate limit и не отключает state sync глобально.** При attach DLL до кэширования native state policy распознаёт только проверенную форму operation-tactical command line: `tacgame.progpack` плюс отдельный token `19YY_MM_campaign:battle`. Только если `MP_SYNC_STATE` **отсутствует**, она устанавливает для этого процесса `MP_SYNC_STATE=0`. Skirmish, custom mission, `opgame` и любое явно заданное `MP_SYNC_STATE` остаются нетронутыми; `g_host_mode`, map-mode и codelib не записываются.

В подходящем бою после появления normal logger должна быть ровно одна строка:

```text
vitals: tactical operation detected; full object-state replication is disabled (native position, shots, hits and Vitals remain)
```

Это возвращает уже предусмотренную native operation semantics, а не снижает частоту корректной работы. `MP_IDLE_MS` по-прежнему только явная диагностика: без него v7.5.0 сохраняет штатную staggered cadence неподвижных transforms; `MP_LOG_VITALS=1` включает подробные counters.

**Обязательное живое подтверждение перед утверждением результата:** в двух-peer operational battle после object-number exchange `state:` saved-state traffic должен исчезнуть, а transforms, shots и Vitals должны продолжать работать на обоих ПК. Linux native tests проверяют классификацию command line, absent-only override и explicit override, но не заменяют этот игровой прогон.

### Итог тактического боя и трупы

Перед вычислением послебоевой статистики DLL принудительно делает последний монотонный scan смертей/ранений и публикует его; броски статистики получают одинаковый seed у обоих игроков. На глобальной карте итоговым источником является обработанный хостом operation save, который клиент получает целиком на следующем map epoch.

**Граница текущей реализации:** этот final scan использует обычный ACK/retry канала Vitals, но ещё не является отдельным блокирующим ACK-barrier в момент выхода из tactical game. Поэтому нельзя выдавать его за доказательство, что peer успел подтвердить последний пакет, если бой аварийно/мгновенно закрыт. Отдельный безопасный engine-level barrier требует живой двухигроковой проверки точки перехода карты.

Состояние смерти синхронизируется как здоровье и squad record. Отображение отдельных тел, ragdoll и локальные render/physics-объекты остаётся задачей движка и не является авторитетом результата кампании. v7.5.0 не делает догадочных записей в corpse lifecycle; если клиентское тело всё ещё отсутствует при совпадающем `dead`, нужны логи и engine-backed адреса. Для диагностики смотрите итог `dead here`, `dead in the other game` и `still apart` в Vitals telemetry: после завершённого обмена `still apart` должен быть `0`.

### Корректный lifetime transform-cache renderer

Новый dump показал write-AV `basic_render+0x129a2` во время deployment: node record содержала foreign/stale 24-bit matrix-cache slot, не принадлежащий текущему pool, а stock renderer записывал в `pool + slot * 64` без проверки владельца. Это локальный engine cache-ownership defect, а не Vitals/network packet; аналогичный старый deployment crash произошёл без `mp_vitals.dll`.

v7.5.0 проверяет, что cache slot находится в том же writable Windows allocation, что и текущий pool. Только при нарушении этого инварианта он атомарно устанавливает штатный engine sentinel `-2`; исходный `basic_render` сам вызывает свой allocator и записывает свежий slot обратно. Exact write-race fallback ведёт в тот же allocator, а не пропускает render operation. Проверка включается лишь при совпадении timestamp и 149 байтов кода Steam 3598/1. Подробный разбор: [`docs/RENDER_CACHE_ROOT_CAUSE_2026-10-25.md`](docs/RENDER_CACHE_ROOT_CAUSE_2026-10-25.md).

В логе ожидается `basic_render cache-lifetime, geometry-index and invalid-child guards armed`; строка про rebuilt stale transform cache entries означает, что cache был штатно пересоздан.

### Новый crash: некорректная геометрия triangle query

Это **отдельный** crash после уже включённого cache guard. Новый dump имеет read-AV в `basic_render+0x39b52` (`movups xmm9,[rax+rsi]`): `RSI + RAX` точно совпадает с invalid address, а `RAX=0xfaaec630` уже был получен 32-bit умножением третьего индекса triangle на vertex stride. Это не Vitals packet и не ещё один recursive material node.

Корень падения — отсутствие в stock triangle reader проверки, что каждый index/stride-derived адрес принадлежит readable primary vertex и optional attribute buffer. v7.5.0 строго сверяет пять фрагментов Steam 3598/1 кода, detour-ит только вход этой функции и **до обращения** проверяет все index triples и все 16-byte vertex/attribute loads, которые выполнит engine loop. При некорректной геометрии он возвращает документированный engine result `no hit` (`false`, output byte `+0xa4=0`); не создаёт объектов, не подменяет index и не продолжает небезопасное чтение.

VEH сохранён лишь как узкий race backup между preflight и одним из 12 конкретных read instructions: должны совпасть type read-AV, RIP, effective address и writable engine output. Никакой другой exception не перехватывается. В логе нормальный запуск пишет `geometry-index ... guards armed`; строка `skipped ... malformed basic_render geometry read ... no-hit path` — диагностический факт, который следует приложить к полному tactical log.

Полная register/disassembly evidence, границы minidump и тесты: [`docs/TRIANGLE_GEOMETRY_ROOT_CAUSE_2026-10-25.md`](docs/TRIANGLE_GEOMETRY_ROOT_CAUSE_2026-10-25.md).

### Сохранённые меры

- ACK/retry/dedup для Vitals type 52/53;
- штатная повторная отправка неподвижных unit transforms без every-tick flood;
- точечная защита подтверждённых invalid-child read-AV в `basic_render`.

## Проверка

```sh
scripts/test_native.sh
scripts/verify_release.sh
```

Проверка включает ASan/UBSan native test, SHA-256 всех артефактов, PE import/export surface, проверку точного Next Turn адреса и сигнатуры, byte-for-byte сверку одинаковых бинарников во всех kit-источниках, а также exact ZIP tree.

Оглавление всей документации: [`docs/README.md`](docs/README.md). Реальный двухигроковый прогон Graviteam в Linux sandbox не выполняется; перед кампанией необходим тест обоими игроками.

Подробности сборки: [`docs/BUILD_AND_VALIDATION.md`](docs/BUILD_AND_VALIDATION.md). Проверенные причины тактической нагрузки и precondition карты: [`docs/TACTICAL_PERFORMANCE_AND_MAP_EVIDENCE_2026-10-25.md`](docs/TACTICAL_PERFORMANCE_AND_MAP_EVIDENCE_2026-10-25.md).

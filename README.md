# MiusMultiplayer v1.0.0

Единый multiplayer-мод для **Graviteam Tactics: Mius-Front** (Steam game build 3598/1). Он объединяет runtime MiusPlayer v20 и проверенный MiusVitals sidecar в одном устанавливаемом наборе.

## Скачать и установить

**Устанавливаемый файл только один:** [`MiusMultiplayer_v1.0.0_INSTALL.zip`](MiusMultiplayer_v1.0.0_INSTALL.zip).

Архив содержит единый корень `MiusMultiplayer/` и имеет SHA-256 `40a4ab0b7abca5fed80e0ec642ebd81e7a3bbf297fa86385e657b1f225f82210`. Распакуйте его в корень игры рядом со `starter_x64.exe`, затем выполните `MiusMultiplayer\Check_Install.bat` и `MiusMultiplayer\Enable_Multiplayer.bat`. Не устанавливайте отдельно старые `mpmod1` и `MiusVitals`.

> Не устанавливайте [`development/`](development/). Это расширенная рабочая область: source MiusPlayer/MiusVitals, manifest и проверки. Для игры подходит только единый INSTALL-архив. Для редактирования/полной source rebuild цепочки см. [`development/merged-source/`](development/merged-source/).

Оба игрока должны установить один и тот же v1.0.0 archive. Откат: после закрытия игры запустите `MiusMultiplayer\Disable_Multiplayer.bat`.

## Что входит в единый мод

### Глобальная карта — без изменений в этом выпуске

Текущая жалоба на то, что ходы глобальной карты не синхронизируются, **не исправляется и не маскируется в v7.5.0**. По явному решению этот выпуск не меняет ни `adv_opgame`, ни `basic_network.codelib`, ни map-mode/ownership/Next Turn логику: main и `nostart` codelib оставлены байт-в-байт прежними. В том числе hash основного модуля остаётся `485B655662BD062E16278F9D699A3097B49A08DE1948D2500DD589703A3102A3`.

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

Проверка включает ASan/UBSan native test, SHA-256 всех артефактов, PE import/export surface, проверку точного Next Turn адреса и сигнатуры, а также exact ZIP tree. Реальный двухигроковый прогон Graviteam в Linux sandbox не выполняется; перед кампанией необходим тест обоими игроками.

Подробности сборки: [`docs/BUILD_AND_VALIDATION.md`](docs/BUILD_AND_VALIDATION.md). Проверенные причины тактической нагрузки и precondition карты: [`docs/TACTICAL_PERFORMANCE_AND_MAP_EVIDENCE_2026-10-25.md`](docs/TACTICAL_PERFORMANCE_AND_MAP_EVIDENCE_2026-10-25.md).

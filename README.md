# MiusVitals v7.4.0

Надстройка для [MiusPlayer](https://github.com/VampyrCat/MiusPlayer) в **Graviteam Tactics: Mius-Front**. Версия рассчитана на Steam game build 3598/1 и MiusPlayer v20.

## Скачать и установить

**Устанавливаемый файл только один:** [`MiusVitals_v7.4.0_INSTALL.zip`](MiusVitals_v7.4.0_INSTALL.zip).

Архив содержит ровно 16 файлов в корне `MiusVitals/` и имеет SHA-256 `444f091f64d8ce909b0d0164eb171e4755fd5312319eb64ec3cfa16a8339a228`. Распакуйте его в корень игры, рядом с `starter_x64.exe` и `mpmod1`, затем выполните `MiusVitals\Check_Build.bat` и `MiusVitals\Install_Vitals.bat`.

> Не устанавливайте [`development/kit-source/`](development/kit-source/). Это дерево разработки с manifest, проверками и промежуточными файлами. Для игры подходит только INSTALL-архив.

Оба игрока должны установить один и тот же v7.4.0. Откат: после закрытия игры запустите `MiusVitals\Uninstall_Vitals.bat`.

## Что исправляет v7.4.0

### Совместный ход на глобальной карте

На фактическом `adv_opgame.codelib` Steam 3598/1 обработчик **Next Turn** расположен на `+0x36b30`, а не на старом `+0x36b40`. Из-за старого адреса MiusPlayer намеренно отказывался ставить detour и писал:

```text
map: unknown game build, ending the turn is not coordinated between the players
```

Главный и `nostart` варианты содержат строго проверенную пару: новый адрес и точный 16-байтный пролог целевой функции. Это включает штатную логику MiusPlayer для операции:

- хост отдаёт клиенту зеркальную копию текущей operational save;
- оба игрока одновременно перемещают **только свои** formations и battle groups;
- save клиента возвращается хосту, который трёхсторонне объединяет перемещения, числовые изменения и новые строки `user_orders`;
- затем хост обрабатывает ход и передаёт клиенту полную карту следующего epoch.

**v7.4.0 устраняет доказанную гонку запуска карты.** После `map: shared with ...` отдельному `opgame` клиента требуется время, чтобы скачать save, стартовать и послать map-mode Hello (`phase 0xfe`). В v20 host gate для Next Turn требовал уже полученный `g_peer_on_map`, хотя карта уже была выдана (`g_map_given`) и lobby соединение ещё было живо. Нажатие host в этот промежуток уходило в stock Next Turn: ход сразу обрабатывался, а клиент вместо карты получал следующую tactical battle.

Патч снимает **только** проверку `g_peer_on_map` в этом единственном Next Turn gate. Он по-прежнему требует, что карта уже выдана и соединение активно. Сбор возвращённых приказов всё так же требует настоящего map-mode Hello, совпадающего epoch, и Next Turn обоих игроков. Поэтому это не таймер, не фальшивое подтверждение и не обход merge: host просто остаётся в состоянии «готов и ждёт», пока уже выданная карта запускается у клиента.

После установки v7.4.0 проверяйте последовательность:

```text
host:   map: shared with ...
host:   map: this player has ended the turn, waiting for ...
joiner: net: joining ...
joiner: map: this player's side of the operational map; the turn ends when both players have pressed next turn
```

До Next Turn joiner на host **не** должно появиться `map: ending the turn` или сразу стартовать tactical battle. После Next Turn joiner host начинает collection; тогда normal `map: ending the turn - asking ... for his moves` допустима.

Строка `map: unknown game build, ending the turn is not coordinated between the players` всё ещё означает, что Next Turn detour не включён на неизвестной сборке. Другая строка — `map: unknown game build, what the other player's intelligence finds out is not kept for him` — относится только к сохранению разведданных и **не** отключает корректно установленный Next Turn hook.

Подкрепления не передаются отдельным неполным списком: существующие состояния formations/groups входят в полную operation save и её числовой merge. **Ограничение, требующее живого теста:** текущий merge структурно добавляет только новые строки `user_orders`; появление новой строки подкрепления в ходе совместного turn ещё не доказано. Поэтому не следует обещать сохранение такого нового reinforcement до проверки обоими игроками.

#### Сообщение «за вторым человеком действует бот»

В присланном host `basic_network_opgame_2236.log` есть `map: shared with ... 391 formations and battle groups turned round`. В MiusPlayer именно эта операция меняет command tags противоположной стороны на `USER`; тактическая строка `AI commander switched off` — отдельный механизм и не является владельцем operational-map приказов. Значит, этот факт сам по себе не доказывает, что `mirror` не выдал joiner human command tags.

Зато в host log за весь `opgame` нет ни одной строки `vitals:`, а в логе joiner она есть. DLL может появиться в процессе только через импорт текущего `basic_network.codelib`; это прямой признак разных или неполно установленных комплектов на двух ПК. Это и объясняет запуск stock path/бота за клиента без небезопасной догадки об ownership patch.

v7.4.0 после установки проверяет **оба** файла. До теста карты на host и joiner должны совпасть main/nostart variant и строки:

```text
basic_network.codelib 485B655662BD062E16278F9D699A3097B49A08DE1948D2500DD589703A3102A3
mp_vitals.dll          D22E931575A119136CCE9F40622EE90F8919EBD250F2738E3E50D7561C4FC486
```

На обеих сторонах в `basic_network_opgame_*.log` должна быть `vitals:` строка. Не используйте `norebase` для обычного Steam 3598/1 и не подменяйте `basic_network.codelib` вручную. Если при этих доказательствах второй map всё ещё управляется ботом, нужны обе full opgame logs, оба `MiusVitals/build_report.txt` и `.opsave`, созданный после hand-off.

### Номера объектов — обязательное условие Vitals

Нативная загрузка object state ждёт object/description table. Поэтому Vitals с первых секунд боя запрашивает native exchange каждые 3 секунды, пока он не завершится, и до этого не посылает, не применяет и не ACK-ает Vitals state.

**Исправление v7.4.0:** `ready` v20 — это transient transfer-cycle flag. После первого успеха он способен штатно кратко стать `0`; раньше DLL ошибочно воспринимала это как новый session и каждые ~3 секунды снова запускала перевод примерно 3 000 descriptions и 632 objects. Теперь первый успешный exchange latched на весь tactical session: Vitals продолжает безопасный state sync и больше не нудит native exchange, пока не сменятся tactical object manager, выбранный peer или соединение не закроется.

В логе нормального старта остаются только:

```text
vitals: waiting for object-number exchange before tactical state sync
vitals: object-number exchange complete; tactical state sync enabled
```

Если первый exchange не завершается, сохраните оба полных `basic_network_tacgame_*.log`: бой не следует считать пригодным для проверки синхронизации результата.

### FPS, сеть и логирование

Логи показали вторую независимую причину нагрузки: прежний default patch отправлял неподвижные unit **на каждом** network tick. Это превратило штатную staggered cadence около 2 секунд в тысячи transforms, тысячи Steam datagrams и до мегабайт в секунду даже когда никто не двигался.

v7.4.0 по умолчанию больше не трогает этот native sender. Aggressive режим доступен только как явная диагностика `MP_IDLE_MS=0`; `MP_IDLE_MS=N` задаёт другой период сознательно. Нормальные Vitals aggregate counters раз в 5 секунд также выключены: ошибки и ключевые переходы остаются в логе, а полная telemetry включается только `MP_LOG_VITALS=1`.

### Итог тактического боя и трупы

Перед вычислением послебоевой статистики DLL принудительно делает последний монотонный scan смертей/ранений и публикует его; броски статистики получают одинаковый seed у обоих игроков. На глобальной карте итоговым источником является обработанный хостом operation save, который клиент получает целиком на следующем map epoch.

**Граница текущей реализации:** этот final scan использует обычный ACK/retry канала Vitals, но ещё не является отдельным блокирующим ACK-barrier в момент выхода из tactical game. Поэтому нельзя выдавать его за доказательство, что peer успел подтвердить последний пакет, если бой аварийно/мгновенно закрыт. Отдельный безопасный engine-level barrier требует живой двухигроковой проверки точки перехода карты.

Состояние смерти синхронизируется как здоровье и squad record. Отображение отдельных тел, ragdoll и локальные render/physics-объекты остаётся задачей движка и не является авторитетом результата кампании. v7.4.0 не делает догадочных записей в corpse lifecycle; если клиентское тело всё ещё отсутствует при совпадающем `dead`, нужны логи и engine-backed адреса. Для диагностики смотрите итог `dead here`, `dead in the other game` и `still apart` в Vitals telemetry: после завершённого обмена `still apart` должен быть `0`.

### Корректный lifetime transform-cache renderer

Новый dump показал write-AV `basic_render+0x129a2` во время deployment: node record содержала foreign/stale 24-bit matrix-cache slot, не принадлежащий текущему pool, а stock renderer записывал в `pool + slot * 64` без проверки владельца. Это локальный engine cache-ownership defect, а не Vitals/network packet; аналогичный старый deployment crash произошёл без `mp_vitals.dll`.

v7.4.0 проверяет, что cache slot находится в том же writable Windows allocation, что и текущий pool. Только при нарушении этого инварианта он атомарно устанавливает штатный engine sentinel `-2`; исходный `basic_render` сам вызывает свой allocator и записывает свежий slot обратно. Exact write-race fallback ведёт в тот же allocator, а не пропускает render operation. Проверка включается лишь при совпадении timestamp и 149 байтов кода Steam 3598. Подробный разбор: [`docs/RENDER_CACHE_ROOT_CAUSE_2026-10-25.md`](docs/RENDER_CACHE_ROOT_CAUSE_2026-10-25.md).

В логе ожидается `basic_render cache-lifetime, geometry-index and invalid-child guards armed`; строка про rebuilt stale transform cache entries означает, что cache был штатно пересоздан.

### Новый crash: некорректная геометрия triangle query

Это **отдельный** crash после уже включённого cache guard. Новый dump имеет read-AV в `basic_render+0x39b52` (`movups xmm9,[rax+rsi]`): `RSI + RAX` точно совпадает с invalid address, а `RAX=0xfaaec630` уже был получен 32-bit умножением третьего индекса triangle на vertex stride. Это не Vitals packet и не ещё один recursive material node.

Корень падения — отсутствие в stock triangle reader проверки, что каждый index/stride-derived адрес принадлежит readable primary vertex и optional attribute buffer. v7.4.0 строго сверяет пять фрагментов Steam 3598 кода, detour-ит только вход этой функции и **до обращения** проверяет все index triples и все 16-byte vertex/attribute loads, которые выполнит engine loop. При некорректной геометрии он возвращает документированный engine result `no hit` (`false`, output byte `+0xa4=0`); не создаёт объектов, не подменяет index и не продолжает небезопасное чтение.

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

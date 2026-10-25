# MiusVitals v7.1.0

Надстройка для [MiusPlayer](https://github.com/VampyrCat/MiusPlayer) в **Graviteam Tactics: Mius-Front**. Версия рассчитана на Steam game build 3598/1 и MiusPlayer v20.

## Скачать и установить

**Устанавливаемый файл только один:** [`MiusVitals_v7.1.0_INSTALL.zip`](MiusVitals_v7.1.0_INSTALL.zip).

Архив содержит ровно 16 файлов в корне `MiusVitals/` и имеет SHA-256 `f06e83284494ddc4f8ac16c57d7a4bed4ecb48af4b5aa0f1ad27fc8a953a5a4c`. Распакуйте его в корень игры, рядом с `starter_x64.exe` и `mpmod1`, затем выполните `MiusVitals\Check_Build.bat` и `MiusVitals\Install_Vitals.bat`.

> Не устанавливайте [`development/kit-source/`](development/kit-source/). Это дерево разработки с manifest, проверками и промежуточными файлами. Для игры подходит только INSTALL-архив.

Оба игрока должны установить один и тот же v7.1.0. Откат: после закрытия игры запустите `MiusVitals\Uninstall_Vitals.bat`.

## Что исправляет v7.1.0

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

**v7.1.0 устраняет доказанную гонку запуска карты.** После `map: shared with ...` отдельному `opgame` клиента требуется время, чтобы скачать save, стартовать и послать map-mode Hello (`phase 0xfe`). В v20 host gate для Next Turn требовал уже полученный `g_peer_on_map`, хотя карта уже была выдана (`g_map_given`) и lobby соединение ещё было живо. Нажатие host в этот промежуток уходило в stock Next Turn: ход сразу обрабатывался, а клиент вместо карты получал следующую tactical battle.

Патч снимает **только** проверку `g_peer_on_map` в этом единственном Next Turn gate. Он по-прежнему требует, что карта уже выдана и соединение активно. Сбор возвращённых приказов всё так же требует настоящего map-mode Hello, совпадающего epoch, и Next Turn обоих игроков. Поэтому это не таймер, не фальшивое подтверждение и не обход merge: host просто остаётся в состоянии «готов и ждёт», пока уже выданная карта запускается у клиента.

После установки v7.1.0 проверяйте последовательность:

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

Разбор присланного host `basic_network_opgame_32004.log` нашёл `map: shared with ... 391 formations and battle groups turned round`. В исходниках MiusPlayer именно эта операция меняет command tags противоположной стороны на `USER`; тактическая строка `AI commander switched off` — отдельный механизм и не является владельцем operational-map приказов. Поэтому эти логи **не доказывают**, что `mirror` не выдал второй стороне human command tags.

В то же время ряд map logs содержит `map: unknown game build, ending the turn is not coordinated between the players`. Это доказательство несовместимого/старого установленного `basic_network.codelib`, а не повод добавлять догадочный ownership patch. v7.1.0 устанавливает на обеих машинах один и тот же строго проверенный main hash `485B6556…3102A3`; именно это включает map detour, mirroring и устранение launch race. Не устанавливайте `norebase` для обычного Steam 3598/1 и не копируйте поверх kit вручную другой `basic_network.codelib`.

Если после подтверждения этого hash на **host и joiner** второй map всё ещё управляется ботом, приложите обе full `basic_network_opgame_*.log`, `MiusVitals/build_report.txt` и одну созданную после hand-off `.opsave`: это отделит UI/tag state от installation/runtime mismatch без скрывающего gameplay patch.

### Номера объектов — обязательное условие Vitals

Ранее объектные/description tables могли долго оставаться в состоянии `NO`. Теперь Vitals:

- с первых секунд боя запрашивает нативный object-number exchange каждые 3 секунды, пока он не завершится;
- не отправляет, не применяет и не ACK-ает Vitals state до `object-number lists exchanged: yes`;
- после успешного exchange запускает полный монотонный snapshot состояния.

Нативная загрузка object state уже ждёт эту таблицу; v7.1.0 добавляет такое же жёсткое правило для отдельного канала Vitals. В логе ожидаются строки:

```text
vitals: waiting for object-number exchange before tactical state sync
vitals: object-number exchange complete; tactical state sync enabled
```

Если `yes` не появляется, сохраните оба полных `basic_network_tacgame_*.log`: бой не следует считать пригодным для проверки синхронизации результата.

### Итог тактического боя и трупы

Перед вычислением послебоевой статистики DLL принудительно делает последний монотонный scan смертей/ранений и публикует его; броски статистики получают одинаковый seed у обоих игроков. На глобальной карте итоговым источником является обработанный хостом operation save, который клиент получает целиком на следующем map epoch.

**Граница текущей реализации:** этот final scan использует обычный ACK/retry канала Vitals, но ещё не является отдельным блокирующим ACK-barrier в момент выхода из tactical game. Поэтому нельзя выдавать его за доказательство, что peer успел подтвердить последний пакет, если бой аварийно/мгновенно закрыт. Отдельный безопасный engine-level barrier требует живой двухигроковой проверки точки перехода карты.

Состояние смерти синхронизируется как здоровье и squad record. Отображение отдельных тел, ragdoll и локальные render/physics-объекты остаётся задачей движка и не является авторитетом результата кампании. v7.1.0 не делает догадочных записей в corpse lifecycle; если клиентское тело всё ещё отсутствует при совпадающем `dead`, нужны логи и engine-backed адреса. Для диагностики смотрите итог `dead here`, `dead in the other game` и `still apart` в Vitals telemetry: после завершённого обмена `still apart` должен быть `0`.

### Корректный lifetime transform-cache renderer

Новый dump показал write-AV `basic_render+0x129a2` во время deployment: node record содержала foreign/stale 24-bit matrix-cache slot, не принадлежащий текущему pool, а stock renderer записывал в `pool + slot * 64` без проверки владельца. Это локальный engine cache-ownership defect, а не Vitals/network packet; аналогичный старый deployment crash произошёл без `mp_vitals.dll`.

v7.1.0 проверяет, что cache slot находится в том же writable Windows allocation, что и текущий pool. Только при нарушении этого инварианта он атомарно устанавливает штатный engine sentinel `-2`; исходный `basic_render` сам вызывает свой allocator и записывает свежий slot обратно. Exact write-race fallback ведёт в тот же allocator, а не пропускает render operation. Проверка включается лишь при совпадении timestamp и 149 байтов кода Steam 3598. Подробный разбор: [`docs/RENDER_CACHE_ROOT_CAUSE_2026-10-25.md`](docs/RENDER_CACHE_ROOT_CAUSE_2026-10-25.md).

В логе ожидается `basic_render cache-lifetime, geometry-index and invalid-child guards armed`; строка про rebuilt stale transform cache entries означает, что cache был штатно пересоздан.

### Новый crash: некорректная геометрия triangle query

Это **отдельный** crash после уже включённого cache guard. Новый dump имеет read-AV в `basic_render+0x39b52` (`movups xmm9,[rax+rsi]`): `RSI + RAX` точно совпадает с invalid address, а `RAX=0xfaaec630` уже был получен 32-bit умножением третьего индекса triangle на vertex stride. Это не Vitals packet и не ещё один recursive material node.

Корень падения — отсутствие в stock triangle reader проверки, что каждый index/stride-derived адрес принадлежит readable primary vertex и optional attribute buffer. v7.1.0 строго сверяет пять фрагментов Steam 3598 кода, detour-ит только вход этой функции и **до обращения** проверяет все index triples и все 16-byte vertex/attribute loads, которые выполнит engine loop. При некорректной геометрии он возвращает документированный engine result `no hit` (`false`, output byte `+0xa4=0`); не создаёт объектов, не подменяет index и не продолжает небезопасное чтение.

VEH сохранён лишь как узкий race backup между preflight и одним из 12 конкретных read instructions: должны совпасть type read-AV, RIP, effective address и writable engine output. Никакой другой exception не перехватывается. В логе нормальный запуск пишет `geometry-index ... guards armed`; строка `skipped ... malformed basic_render geometry read ... no-hit path` — диагностический факт, который следует приложить к полному tactical log.

Полная register/disassembly evidence, границы minidump и тесты: [`docs/TRIANGLE_GEOMETRY_ROOT_CAUSE_2026-10-25.md`](docs/TRIANGLE_GEOMETRY_ROOT_CAUSE_2026-10-25.md).

### Сохранённые меры v7.1.1

- ACK/retry/dedup для Vitals type 52/53;
- повторная отправка неподвижных unit transforms;
- точечная защита двух подтверждённых invalid-child read-AV в `basic_render`.

## Проверка

```sh
scripts/test_native.sh
scripts/verify_release.sh
```

Проверка включает ASan/UBSan native test, SHA-256 всех артефактов, PE import/export surface, проверку точного Next Turn адреса и сигнатуры, а также exact ZIP tree. Реальный двухигроковый прогон Graviteam в Linux sandbox не выполняется; перед кампанией необходим тест обоими игроками.

Подробности сборки: [`docs/BUILD_AND_VALIDATION.md`](docs/BUILD_AND_VALIDATION.md). Анализ operation-map: [`docs/OPERATION_MAP_ANALYSIS_2026-10-25.md`](docs/OPERATION_MAP_ANALYSIS_2026-10-25.md).

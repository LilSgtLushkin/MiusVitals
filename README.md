# MiusVitals v7.2.1

Надстройка для [MiusPlayer](https://github.com/VampyrCat/MiusPlayer) в **Graviteam Tactics: Mius-Front**. Версия рассчитана на Steam game build 3598/1 и MiusPlayer v20.

## Скачать и установить

**Устанавливаемый файл только один:** [`MiusVitals_v7.2.1_INSTALL.zip`](MiusVitals_v7.2.1_INSTALL.zip).

Архив содержит ровно 16 файлов в корне `MiusVitals/`. Распакуйте его в корень игры, рядом с `starter_x64.exe` и `mpmod1`, затем выполните `MiusVitals\Check_Build.bat` и `MiusVitals\Install_Vitals.bat`.

> Не устанавливайте [`development/kit-source/`](development/kit-source/). Это дерево разработки с manifest, проверками и промежуточными файлами. Для игры подходит только INSTALL-архив.

Оба игрока должны установить один и тот же v7.2.1. Откат: после закрытия игры запустите `MiusVitals\Uninstall_Vitals.bat`.

## Что исправляет v7.2.1

### Совместный ход на глобальной карте

На фактическом `adv_opgame.codelib` Steam 3598/1 обработчик **Next Turn** расположен на `+0x36b30`, а не на старом `+0x36b40`. Из-за старого адреса MiusPlayer намеренно отказывался ставить detour и писал:

```text
map: unknown game build, ending the turn is not coordinated between the players
```

Главный и `nostart` варианты v7.2.1 содержат строго проверенную пару: новый адрес и точный 16-байтный пролог целевой функции. Это вновь включает штатную логику MiusPlayer для операции:

- хост отдаёт клиенту зеркальную копию текущей operational save;
- оба игрока одновременно перемещают **только свои** formations и battle groups;
- ход не завершается, пока оба не нажмут Next Turn;
- save клиента возвращается хосту, который трёхсторонне объединяет перемещения, числовые изменения и новые строки `user_orders`;
- затем хост обрабатывает ход и передаёт клиенту полную карту следующего epoch.

Подкрепления не передаются отдельным неполным списком: существующие состояния formations/groups входят в полную operation save и её числовой merge. **Ограничение, требующее живого теста:** текущий merge структурно добавляет только новые строки `user_orders`; появление новой строки подкрепления в ходе совместного turn ещё не доказано. Поэтому не следует обещать сохранение такого нового reinforcement до проверки обоими игроками.

В логе обеих сторон после открытия карты должна появиться строка о том, что ход заканчивается после нажатия Next Turn обоими игроками. Строка `map: unknown game build...` означает несовпадение сборки и безопасно оставляет координацию выключенной.

### Номера объектов — обязательное условие Vitals

Ранее объектные/description tables могли долго оставаться в состоянии `NO`. Теперь Vitals:

- с первых секунд боя запрашивает нативный object-number exchange каждые 3 секунды, пока он не завершится;
- не отправляет, не применяет и не ACK-ает Vitals state до `object-number lists exchanged: yes`;
- после успешного exchange запускает полный монотонный snapshot состояния.

Нативная загрузка object state уже ждёт эту таблицу; v7.2.1 добавляет такое же жёсткое правило для отдельного канала Vitals. В логе ожидаются строки:

```text
vitals: waiting for object-number exchange before tactical state sync
vitals: object-number exchange complete; tactical state sync enabled
```

Если `yes` не появляется, сохраните оба полных `basic_network_tacgame_*.log`: бой не следует считать пригодным для проверки синхронизации результата.

### Итог тактического боя и трупы

Перед вычислением послебоевой статистики DLL принудительно делает последний монотонный scan смертей/ранений и публикует его; броски статистики получают одинаковый seed у обоих игроков. На глобальной карте итоговым источником является обработанный хостом operation save, который клиент получает целиком на следующем map epoch.

**Граница текущей реализации:** этот final scan использует обычный ACK/retry канала Vitals, но ещё не является отдельным блокирующим ACK-barrier в момент выхода из tactical game. Поэтому нельзя выдавать его за доказательство, что peer успел подтвердить последний пакет, если бой аварийно/мгновенно закрыт. Отдельный безопасный engine-level barrier требует живой двухигроковой проверки точки перехода карты.

Состояние смерти синхронизируется как здоровье и squad record. Отображение отдельных тел, ragdoll и локальные render/physics-объекты остаётся задачей движка и не является авторитетом результата кампании. v7.2.1 не делает догадочных записей в corpse lifecycle; если клиентское тело всё ещё отсутствует при совпадающем `dead`, нужны логи и engine-backed адреса. Для диагностики смотрите итог `dead here`, `dead in the other game` и `still apart` в Vitals telemetry: после завершённого обмена `still apart` должен быть `0`.

### Корректный lifetime transform-cache renderer

Новый dump показал write-AV `basic_render+0x129a2` во время deployment: node record содержала foreign/stale 24-bit matrix-cache slot, не принадлежащий текущему pool, а stock renderer записывал в `pool + slot * 64` без проверки владельца. Это локальный engine cache-ownership defect, а не Vitals/network packet; аналогичный старый deployment crash произошёл без `mp_vitals.dll`.

v7.2.1 проверяет, что cache slot находится в том же writable Windows allocation, что и текущий pool. Только при нарушении этого инварианта он атомарно устанавливает штатный engine sentinel `-2`; исходный `basic_render` сам вызывает свой allocator и записывает свежий slot обратно. Exact write-race fallback ведёт в тот же allocator, а не пропускает render operation. Проверка включается лишь при совпадении timestamp и 149 байтов кода Steam 3598. Подробный разбор: [`docs/RENDER_CACHE_ROOT_CAUSE_2026-10-25.md`](docs/RENDER_CACHE_ROOT_CAUSE_2026-10-25.md).

В логе ожидается `basic_render cache-lifetime repair and invalid-child guards armed`; строка про rebuilt stale transform cache entries означает, что cache был штатно пересоздан.

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

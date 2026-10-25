# Операционная карта: причина преждевременного Next Turn — 2026-10-25

> **Итог для v7.1.0:** supplied host evidence `391 formations and battle groups
> turned round` proves that `opsave::mirror` did execute its USER-tag hand-off;
> tactical `AI commander switched off` is a different system. The supplied logs
> do not prove a map-command ownership defect in that function. In contrast,
> `map: unknown game build, ending the turn is not coordinated between the
> players` proves an old/incompatible installed `basic_network.codelib` for
> those runs. The install kit now reports both installed module and DLL hashes,
> and writes the same checked main module (`485b6556…3102a3`) on both players.
> A remaining bot symptom with both exact hashes needs paired opgame logs plus
> the handed-off `.opsave`, not a guessed ownership patch.

## Итог: два разных дефекта в разных наборах файлов

Логи содержат сессии от нескольких поколений MiusPlayer/MiusVitals. Их нельзя
сводить к одному сообщению `unknown game build`.

1. В старых сессиях `basic_network_opgame_34232.log`, `35204.log`, `20248.log`
   и `14864.log` есть:

   ```text
   map: unknown game build, ending the turn is not coordinated between the players
   ```

   Там старый target Next Turn (`+0x36b40`) не совпадал с Steam 3598/1. Stock
   handler действительно завершает ход без ожидания. Это исправлено в v7.2.0:
   target перенесён на `adv_opgame+0x36b30`, его точный 16-byte prologue
   проверяется до установки detour.

2. Новая host-сессия `basic_network_opgame_32004.log` уже доказывает, что этот
   detour был установлен:

   ```text
   map: the turn ends when both players have pressed next turn; the other player's moves are collected then
   ```

   Поэтому её отдельная строка
   `map: unknown game build, what the other player's intelligence finds out is
   not kept for him` **не** относится к Next Turn. Она означает лишь, что
   detour persistence разведданных не распознал build; выдача карты и
   turn-handshake ею не выключаются.

## Доказанная причина сессии PID 32004 / 15436

Host выдал карту `zloy1971` в `15:42:14.638`:

```text
map: shared with zloy1971: ...opsave, 3998509 bytes, 391 formations and battle groups turned round
```

Однако joiner `opgame` (PID 15436) появился только через примерно 39 секунд в
своих часах и не содержит map-mode lifecycle. Вместо строк `net: joining ...`,
`net: connected to host ...` и `map: this player's side ...` он сразу сообщает:

```text
ui: rejoining the lobby ...
ui: battle received ..., starting it
```

Host к тому времени уже начал tactical battle. То есть клиент не был «картой с
плохим вводом»: он вообще не успел стать map-mode process и поэтому закономерно
получил battle из лобби.

Исходный код v20 объясняет точную гонку. До v7.2.2 host `next_turn_hook()`
перехватывал click лишь когда `other_player_on_map()` был true:

```cpp
return g_role == ROLE_HOST && !g_tactical && g_map_given &&
       g_peer_on_map && g_connected;
```

`g_peer_on_map` становится true только после map-mode Hello клиента (`phase ==
0xfe`) с текущим `map_epoch`. Между `g_map_given` (save уже передан launcher'у)
и этим Hello неизбежно есть окно скачивания и запуска отдельного `opgame`.
Нажатие host Next Turn в этом окне шло в оригинальный `g_next_turn`, а не в
`press_next_turn()`. Результат в логах точно соответствует этому пути: host
немедленно продвинул operation, а поздний joiner увидел уже battle.

Это не проблема transport Vitals, object numbers, intelligence persistence или
renderer. Это один ошибочный предикат перехода, расположенный до самой игровой
кнопки Next Turn.

## Что дополнительные tactical logs говорят о crash

В присланных `basic_network_*.log` нет записи exception, access violation,
fatal error или crash. `vitals: basic_render invalid-child crash guards armed`
— это только startup-status включённой защиты, а не сообщение о новом вылете.

`basic_network_tacgame_9916.log` один раз записывает
`vitals: skipped an invalid recursive basic_render node ...; parent material
used`. Это подтверждает, что прежний узкий invalid-child guard действительно
встретил его доказанный плохой node и безопасно выбрал штатный parent material.
Он не является evidence stale transform-cache write-AV из minidump и не
объясняет operation-map race.

Большие счётчики `not loaded (render handles differ)` также ожидаемы: MiusPlayer
намеренно не переносит локальные render handles между независимыми движками.
Несколько старых сессий показывают Steam `error 4`/`connection lost`, но в PID
32004/15436 непосредственная последовательность уже завершилась до запуска
map-mode клиента; это не конкурирующая причина немедленного Next Turn.

Все эти логи содержат старую строку только с invalid-child guards и не содержат
v7.2.1 `basic_render cache-lifetime repair ... armed`. Следовательно, они не
тестировали опубликованный v7.2.1 renderer DLL и тем более не могут подтвердить
v7.2.2 до установки полного одинакового архива на обеих машинах.

## Исправление v7.2.2

В обеих rebased main/`nostart` вариациях проверен и изменён ровно один участок
кода release v20:

- `basic_network+0x15a77`, исходные байты:
  `38 0d bc 5d 04 00 74 13`;
- они были сравнением `g_peer_on_map` и его conditional branch внутри
  `next_turn_hook()`;
- v7.2.2 требует точного совпадения этих байтов и заменяет только их на восемь
  `NOP`.

Следующее сравнение `g_connected`, предшествующие проверки host / non-tactical /
`g_map_given` и все условия collection остаются неизменными. Поэтому после выдачи
карты активному lobby joiner host click теперь вызывает `press_next_turn()` и
логирует ожидание. Он не объявляет peer готовым, не запускает merge и не вызывает
turn worker. `begin_collect()` по-прежнему требует и `g_peer_on_map`, и
`g_peer_turn_done`; это происходит только когда map-mode joiner реально
запустился, подтвердил текущий epoch и сам нажал Next Turn.

Патч не устанавливает таймер и не маскирует ошибку: disconnected peer сохраняет
штатный solo path, поскольку `g_connected` остаётся обязательным. Каждый этап
проверяем воспроизводимо:

- `rebase_3598_1.py` принимает только SHA-256 оригинального v20, asserts старые
  восемь байтов и emits known hashes;
- `patch_import.py` принимает только эти exact raw artifacts и обратимо удаляет
  import;
- `verify_artifacts.py` asserts `+0x36b30`, verified handler signature и ровно
  восемь `NOP` в main/`nostart`.

## Ожидаемый живой тест v7.2.2

Оба игрока должны установить **один и тот же полный v7.2.2 INSTALL-архив**. В
присланных логах нет строки v7.2.1 renderer repair
`basic_render cache-lifetime repair and invalid-child guards armed`; это
подтверждает, что там работал прежний DLL/kit, а не опубликованный v7.2.1.
Нельзя проверять новую карту на смешанном наборе файлов.

После `map: shared with ...` host намеренно может нажать Next Turn, пока joiner
ещё запускает карту. Правильная последовательность:

```text
host:   map: this player has ended the turn, waiting for <peer>
joiner: net: joining ...
joiner: map: this player's side of the operational map; the turn ends when both players have pressed next turn
```

До click joiner не должно быть `map: ending the turn` и не должна стартовать
следующая tactical battle. После click обоих ожидаются request/return map,
`map: ending the turn - asking ... for his moves`, merge и новая карта с новым
epoch. Если последовательность не получается, сохраните полные
`basic_network_opgame_*.log` с обеих сторон и `build_report.txt`; нужны также
точный hash installed `basic_network.codelib` и `mp_vitals.dll`.

## Сохраняющиеся границы

Этот fix исправляет только переход host в ожидание пока уже выданная карта
запускается. Он не доказывает обработку произвольной новой structural row
подкрепления: текущий `opsave::merge_moves` структурно добавляет только
`user_orders`. Он также не создаёт отдельный Vitals final-result ACK barrier при
быстром окончании tactical battle. Оба вопроса требуют отдельного engine-backed
живого теста, а не широкого catch или догадочного binary patch.

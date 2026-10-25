# Тактические фризы и карта — проверенная цепочка причин (2026-10-25)

Этот документ фиксирует только выводы, которые следуют из приложенных runtime-логов и проверяемого исходника `mp_vitals.c`. Живой двух-PC прогон Graviteam в Linux sandbox невозможен; он остаётся release-gate для поведения игрового UI.

## 1. Ритмичный tactical hitch: повторный object-number exchange

`basic_network_tacgame_2316.log` фиксирует успешный исходный обмен в `17:10:21.018`:

```text
numbers: 3091 descriptions ...
numbers: 632 objects ...
vitals: object-number exchange complete; tactical state sync enabled
```

Но затем Vitals снова запрашивает exchange в `17:10:23.649`, `17:10:26.660`, `17:10:29.655` и далее. Каждый такой cycle вновь печатает/выполняет перевод приблизительно 3 091 descriptions и 632 objects, причём в примере видны парные native table reports. В `basic_network_tacgame_22432.log` диагностический счётчик достигает как минимум `asked again 238 times`.

Причина была в неверной семантике `g_num_ready`: это raw v20 transfer-cycle flag, а не permanent session-complete flag. DLL безопасно ждала первый успешный exchange, но затем ошибочно снова закрывала собственный Vitals gate при каждом штатном кратком `ready=0`.

### Исправление v7.4.0

`g_num_complete` теперь latched после **первого** `g_num_ready=1` и используется одинаково для send/apply/ACK/final scan. Он сбрасывается только при уходе из tactical session, смене object manager или selected peer. Пока latch не установлен, нативный exchange всё ещё запрашивается через 3 секунды и Vitals state всё ещё не принимается — исходный safety invariant сохранён.

Native harness проверяет именно последовательность:

1. initial `ready=0` создаёт один nudge и блокирует sync;
2. `ready=1` открывает latch;
3. последующий raw `ready=0` спустя 3 секунды **не** создаёт nudge и не блокирует sync;
4. disconnect сбрасывает latch, и новый session снова ждёт exchange.

## 2. Постоянная нагрузка: every-tick idle override

В тех же tactical logs есть строка прежнего default patch:

```text
net: units that stand still are sent on every tick (patched at start)
```

Измеренный host interval непосредственно после старта: `25 088` sent transforms за 5 секунд, `12 049` Steam datagrams и `2206.7 KB/s`. Позже client применяет `49–52k` transforms за 5 секунд. Это не мягкая оптимизация: native sender обычно разносит stationary repeat примерно на 2.0–2.45 секунды, а прежний default заставлял его пересылать неподвижные unit на каждом tick.

### Исправление v7.4.0

При отсутствующем `MP_IDLE_MS` `patch_idle()` не пишет ни одного байта: сохраняются native imm8/imm32 и штатное staggered behaviour. Только явный `MP_IDLE_MS=N` применяет override; `MP_IDLE_MS=0` оставлен как диагностический every-tick режим и видимо отмечается в логе.

Unit test утверждает, что absent/`2000` не меняют target code, `0` и `500` меняют ровно два signature-verified immediate, а несовпавший signature не трогается.

## 3. Логирование

До v7.4.0 DLL каждые 5 секунд записывала три длинные telemetry строки даже при нормальном бою. Они не были первопричиной packet flood, но добавляли синхронную файловую работу в момент hitches. Теперь errors, startup, first exchange и state transitions остаются normal-runtime событиями; aggregate telemetry выдаётся только с `MP_LOG_VITALS=1`. Counters всё равно сбрасываются каждые 5 секунд, поэтому диагностика после включения не использует старые totals.

## 4. Global map / "бот за клиента"

Host `basic_network_opgame_2236.log` подтверждает mirror path:

```text
map: shared with zloy1971: ... 391 formations and battle groups turned round
```

Это именно преобразование command tags opposing side в `USER`; оно противоречит гипотезе, что mirror вообще не выполнялся. Строка о неизвестной сборке и разведданных относится к persistence scouting и не отменяет mirror/Next Turn hook.

Но за всё время host `opgame` в этом логе нет ни одной строки `vitals:`. В клиентском `basic_network_opgame_21972.log` строка есть (`vitals: basic_render ... guards armed`). DLL загружается только import-ом проверенного patched `basic_network.codelib`; следовательно, представленная пара уже доказывает asymmetric/partial install и не является валидной проверкой общей map logic.

v7.4.0 installer после copy hashes both files, not just the codelib. Before treating a map symptom as code regression, both machines must report the same selected main/nostart variant, current DLL SHA and at least one `vitals:` line in their opgame logs. This is a verified installation precondition, not a speculative gameplay patch.

## Acceptance test after installation

1. Both players run `Check_Build.bat` and `Install_Vitals.bat` from the same v7.4.0 archive with the game closed.
2. Save both installer success outputs or `build_report.txt`; confirm matching main/nostart and DLL hashes.
3. Start an operation. Host shares map; joiner log reaches `map: this player's side of the operational map...`.
4. Before joiner has loaded map, host presses Next Turn once: host must wait, not start a tactical battle.
5. Give orders on both sides, press Next Turn on both sides, confirm collection/merge and next map epoch.
6. Start a tactical battle. Each side sees one initial object-number completion; no repeating table conversion every ~3 seconds; normal stationary traffic stays near native cadence.

If step 3–5 fails with both hashes proven, retain both complete opgame logs, both reports and a post-handoff `.opsave`; that is the missing evidence necessary for a safe map code change.

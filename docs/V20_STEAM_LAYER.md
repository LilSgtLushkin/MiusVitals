# Steam-слой v20: спецификация по декомпиляции

Дата: **2026-10-26**. Источник: `basic_network.codelib` версии 20 (`8108bd1e…5723`), псевдо-C
angr с подставленными строками (`development/v20-recovery/work/decomp/`).

> Уточнение 27.10: пользователь установил внутреннюю приёмку всех этапов до M6. Упоминание
> реального Steam и двух ПК ниже — отдельная будущая проверка поведения, а не условие внутренней
> приёмки. ABI и stand-in уже покрыты CI; настоящий Steam этой автоматикой не подтверждён.
> Текущий статус: [STATUS_AND_BUILD_RULES_2026-10-27.md](STATUS_AND_BUILD_RULES_2026-10-27.md).

Это самый рискованный участок порта и единственный, который нельзя «взять из публичного кода»:
в v12 Steam нет. Ниже — всё, что бинарник говорит о слое, в виде, достаточном для написания
файла `steam.cpp` без гаданий.

---

## 1. Точки входа и режимы

`steam_init` (**0x3F75C**, 802 Б) — единственная функция инициализации, вызывается лениво
(из `snapshot_operation_saves` и из места, где нужен Steam ID). Устройство — три режима,
проверяемые строго по порядку:

| Условие | Режим | Что делает |
|---|---|---|
| `MP_STEAM=0` | **выключен** | пишет `steam: switched off (MP_STEAM=0)`, выходит; сеть идёт по IP/UDP |
| `MP_STEAM_FAKE=<n>` (число ≠ 0) | **стенд** | `fake_steam_open(...)` поднимает UDP-сокет, «личность» = `steam:<n>`, всё равно UDP |
| иначе | **настоящий Steam** | `GetModuleHandleA("steam_api64.dll")` → привязка интерфейсов → регистрация callback'ов |

Если `steam_api64.dll` в процессе нет, пишется
`steam: the game has not loaded Steam's library - no Steam joining` — это ровно тот случай,
когда игра запущена без Steam, и джойн по Steam невозможен, а по IP — возможен.

В настоящем режиме после успешной привязки:

```c
g_user   = SteamAPI_ISteamUser_GetSteamID(user);
g_appid  = SteamAPI_ISteamUtils_GetAppID(utils);
strncpy_s(g_persona, 64, SteamAPI_ISteamFriends_GetPersonaName(friends), 63);
if (!g_user) mp_log("steam: no Steam user - no Steam joining");
SteamAPI_ISteamNetworking_AllowP2PPacketRelay(net, 1);
SteamAPI_RegisterCallback(&g_5a308, 1202);   // P2PSessionRequest_t
SteamAPI_RegisterCallback(&g_5a318, 1203);   // P2PSessionConnectFail_t
SteamAPI_RegisterCallback(&g_5a328, 336);    // FriendRichPresenceUpdate_t
SteamAPI_RegisterCallback(&g_5a338, 337);    // GameRichPresenceJoinRequested_t
SteamAPI_ISteamFriends_RequestFriendRichPresence(friends, g_user);
mp_log("steam: ready - user %llu (%s), application %u, %d friends, logged on: %s", ...);
```

Номера 1202/1203 и 336/337 — это `P2PSessionRequest_t` / `P2PSessionConnectFail_t` и
`FriendRichPresenceUpdate_t` / `GameRichPresenceJoinRequested_t`: базовые значения 1200 и 300,
смещения 2/3 и 36/37. `GameConnectedFriendChatMsg_t` имеет номер **343**, не 337. Ранее эти
два имени в документе и реализации были перепутаны; исправление в net_v21 сопоставляет обработчики
фактическим структурам SDK. См. [steam_api_internal.h](https://github.com/rlabrecque/SteamworksSDK/blob/df2baabf574a738ef1ea90a7e89339107fc0a279/public/steam/steam_api_internal.h),
[isteamfriends.h](https://github.com/rlabrecque/SteamworksSDK/blob/df2baabf574a738ef1ea90a7e89339107fc0a279/public/steam/isteamfriends.h) и
[steam_api_common.h](https://github.com/rlabrecque/SteamworksSDK/blob/df2baabf574a738ef1ea90a7e89339107fc0a279/public/steam/steam_api_common.h)
(у `CCallbackBase` также нет виртуального деструктора). Windows MSVC размещает две перегрузки `Run` в обратном относительно объявления порядке; обе проверяются через реальные слоты. GNU/MinGW использует другой порядок, поэтому его кросс-сборка годится только для синтаксиса и stand-in: реальный Steam там отклоняется с логом, используется MSVC build. Это проверка ABI по заголовкам, не живой прогон Steam.

Режим «стенд» нужен автору для отладки на одной машине и **нам полезен напрямую**: он позволяет
гонять два процесса на одном ПК без второго аккаунта Steam.

## 2. Привязка интерфейсов: 21 вызов, фиксированная таблица слотов

`steam_bind_interfaces` (**0x3EB34**, 595 Б) делает `GetProcAddress` по списку и складывает
указатели в последовательные глобалы. Список точный (порядок и слоты — из кода):

| Слот | Символ |
|---|---|
| `g_5ddc0` | `SteamAPI_SteamFriends_v017` |
| `g_5ddc8` | `SteamAPI_SteamUser_v021` |
| `g_5ddd0` | `SteamAPI_SteamNetworking_v006` |
| `g_5ddd8` | `SteamAPI_SteamUtils_v010` |
| `g_5dde0` | `SteamAPI_ISteamUser_GetSteamID` |
| `g_5dde8` | `SteamAPI_ISteamUser_BLoggedOn` |
| `g_5ddf0` | `SteamAPI_ISteamUtils_GetAppID` |
| `g_5ddf8` | `SteamAPI_ISteamFriends_GetPersonaName` |
| `g_5de00` | `SteamAPI_ISteamFriends_SetRichPresence` |
| `g_5de08` | `SteamAPI_ISteamFriends_GetFriendRichPresence` |
| `g_5de10` | `SteamAPI_ISteamFriends_RequestFriendRichPresence` |
| `g_5de18` | `SteamAPI_ISteamFriends_GetFriendCount` |
| `g_5de20` | `SteamAPI_ISteamFriends_GetFriendByIndex` |
| `g_5de28` | `SteamAPI_ISteamFriends_GetFriendPersonaName` |
| `g_5de30` | `SteamAPI_ISteamFriends_GetFriendGamePlayed` |
| `g_5de38` | `SteamAPI_ISteamNetworking_SendP2PPacket` |
| `g_5de40` | `SteamAPI_ISteamNetworking_IsP2PPacketAvailable` |
| `g_5de48` | `SteamAPI_ISteamNetworking_ReadP2PPacket` |
| `g_5de50` | `SteamAPI_ISteamNetworking_AcceptP2PSessionWithUser` |
| `g_5de58` | `SteamAPI_ISteamNetworking_AllowP2PPacketRelay` |
| `g_5de60` | `SteamAPI_RegisterCallback` |

Следствия для порта:

* **Не нужен `steam_api64.lib` и SDK.** Достаточно `LoadLibrary`/`GetModuleHandle` +
  `GetProcAddress` по этим 21 имени — ровно тот же ABI, что у автора.
* Версии интерфейсов (`v017`, `v021`, `v006`, `v010`) взяты из живой `steam_api64.dll` игры,
  а не из наших догадок: если игра обновится, `GetProcAddress` вернёт `NULL`, и мы это увидим
  в логе — то есть отказ будет диагностируемым, а не «мод не работает».
* Приём/передача данных — **старый ISteamNetworking (P2P)**, не `ISteamNetworkingSockets`:
  это UDP-датаграммы через Steam-релей, тот же формат пакетов, что и при подключении по IP.

## 3. Приём соединения и ошибки

```
P2PSessionRequest_t  -> AcceptP2PSessionWithUser(net, steam_id)
                        "steam: session asked for by %llu - accepted"
P2PSessionConnectFail -> "steam: no connection to %llu could be made (error %u)"
```

Принятие сессии (`0x3E804`, 90 Б), ошибка (`0x3E7CC`, 53 Б), факт регистрации callback'ов
(`0x3E7A4`, 37 Б, пишет `steam: Steam's callbacks arrive in this program`). При отказе в приёме
счётчик `g_5df30` увеличивается — по нему потом пишется сводка
`steam: 5 s: %llu datagrams sent through Steam, %llu received%s` (`0x3FBA0`).

## 4. Rich presence: как друзья узнают, куда подключаться

`snapshot_operation_saves` (**0x3EFA8**, 568 Б) собирает строку `"%u %s"` (операция) и:

```c
if (SetRichPresence(friends, "gtmf", buf) == false ||
    GetFriendRichPresence(friends, "gtmf", check) != buf)
    mp_log("steam: Steam did not take the note for friends (asked for '%s', it holds '%s')");
```

Проверка **чтением обратно** — характерная деталь: автор не доверяет `SetRichPresence`
и сообщает, если Steam сохранил не то. Ключ присутствия — `gtmf`, значение — `<id операции> <хранилище>`.

`end_turn_here` (**0x3F5CC**, 230 Б) разбирает запрос на джойн: ищет в строке `+gtmf_join=`
(параметр запуска из Steam), и если значение начинается на `steam:`, парсит Steam ID и пишет

```
steam: Steam's Join Game asks to join %s%s
        (" (on the command line)" | " (a friend's game, while this game runs)")
```

Иначе — `steam: a join request that is not a multiplayer game's ('%.80s') - ignored`.
То есть «Join Game» у друга — это параметр `-gtmf_join=steam:<id>` плюс немедленный переход
в лобби при старте.

## 5. Стенд без Steam (полезен для наших двух-ПК тестов)

`fake_steam_open` (**0x3E8BC**) + `0x3EA40`:

* поднимает UDP-сокет на порту `MP_STEAM_FAKE` (значение = «номер личности»),
* при занятом порте — `steam: test stand-in: UDP port %llu is taken`,
* имя сессии: `steam:<n>`, лог
  `steam: TEST STAND-IN for Steam: this game is 'steam:%llu', its datagrams travel over UDP on this PC`,
* «файл сессии» — `%TEMP%\gtmf_fake_steam_<n>.txt`.

Это готовый способ проверить логику Steam-джойна **без второго аккаунта**, и он уже есть в
бинарнике — нам нужно только повторить.

## 6. Что это даёт плану

1. M2 перестаёт быть «написать Steam с нуля»: это ~600–800 строк C++, из которых половина —
   обёртки над 21 `GetProcAddress`, и все строки известны дословно.
2. Никакого SDK и линковки: только `steam_api64.dll` игры.
3. Отказ в каждом месте **логируется** — приёмка M2 формулируется как «в логе есть `steam: ready`,
   друг видит игру и `+gtmf_join` доводит его в лобби».
4. Стенд `MP_STEAM_FAKE` даёт автотест, не требующий двух ПК.

## 7. Транспортный шов: как Steam встроен в общий код (доказано)

Самое важное для порта — не сами вызовы Steam, а **как они встроены**. v20 не имеет отдельной
ветки «Steam-сеть»: у него один транспортный интерфейс, и Steam — одна из его реализаций.

* Пир описывается обычным `sockaddr_in`, но с `sa_family = 0x5354` (21332, «TS»). При такой
  семье первые 8 байт после заголовка — Steam ID, дальше в HELLO приходит имя.
* Единственная точка отправки (`0x19AC8`, ~50 Б) ветвится ровно один раз:

```c
if (peer->sa_family != 0x5354)
    sendto(g_sock, buf, len, 0, peer, 16);          // обычный UDP
else
    SendP2PPacket(net, steam_id(peer), buf, len, ...);  // Steam P2P
```

* HELLO (`0x16230`) так же различает источник: `family == 0x5354` → Steam ID в строку,
  `SetEnvironmentVariableA("MP_STEAM_PEER", id)` и лог
  `net: player connected through Steam (steam:%s, %s)`; иначе `inet_ntop` и
  `net: player connected from %s:%u (%s)`.
* Имя пира чистится: всё, кроме букв, цифр, `_` и `-`, заменяется на `_` — то есть имя приходит
  из Steam без доверия.
* `session_frame` (**0x19F88**, 4743 Б) на каждом кадре: если режим Steam и пир «Steam-овский»,
  `steam_init(0x5354)` и затем цикл «взять датаграмму → разобрать»; в обычном режиме — то же
  самое через `recvfrom`.

Вывод для M2: **транспорт надо делать не «поверх» существующей сети, а тем же швом.** Наш
`steam.cpp` реализует ровно две операции — `send(peer, buf, len)` и `recv(buf)`, — а весь
протокол (`on_packet`, HELLO, `MP_STEAM_PEER`) остаётся общим. Это и есть причина, по которой
Steam-джойн в v20 работает так же надёжно, как IP: для верхнего кода разницы нет.

## 8. Что было не доказано — и чем кончилось

* **Формат presence.** Доказано: ключей два, а не один.
  `gtmf` = `"<номер операции> game"` (`sprintf_s(buf, 20, "%u %s", op, "game")` в
  `snapshot_operation_saves`, `0x3EFA8`), а `connect` =
  `"root\\programs\\opgame.progpack, +gtmf_join=steam:<наш SteamID>"`. Второй ключ Steam
  показывает другу как пункт «Join Game»; нажатие запускает игру с `+gtmf_join=steam:<id>` в
  командной строке. Первый пишется и тут же перечитывается `GetFriendRichPresence`, и если Steam
  его не принял, в лог идёт `steam: Steam did not take the note for friends (asked for '%s', it
  holds '%s')` — то есть проверка записи чтением не наша выдумка, а поведение v20.
  При выходе оба ключа очищаются, лог `steam: no longer shown to friends as a game to join`.
  Реализовано в `steam_broadcast()` и проверено самотестом.
* **Как P2P-канал складывается в `on_packet`.** Доказано: `on_packet` (`0x16230`) различает
  источник по `sa_family == 0x5354`, берёт Steam ID из адреса (+8 байт), имя пира — из самого
  пакета (смещение +32), чистит его (всё, кроме `[A-Za-z0-9_-]`, заменяется на `_`) и пишет
  `SetEnvironmentVariableA("MP_STEAM_PEER", id)` плюс строку
  `net: player connected through Steam (steam:%s, %s)`. Для IP-пира — та же ветка с `inet_ntop`
  и `net: player connected from %s:%u (%s)`. Никакого отдельного «Steam-протокола» нет: тот же
  буфер датаграммы, другой транспорт.
* **Имена callback'ов в прежнем разборе были неверны:** 336 — обновление Rich Presence,
  337 — запрос Join Game; чат друзей — 343. По одному номеру регистрации нельзя доказать
  содержимое обработчика в v20. Новая реализация обрабатывает Join Game и отдельно игнорирует
  уведомление об обновлении присутствия; настоящий Steam ещё требует проверки.

## 9. Что сделано в M2

`development/net-v21/src/steam.cpp` + `steam_module.cpp` + `steam_selftest.cpp`:

* 21 `GetProcAddress` — **те же имена**, что в v20 (`SteamAPI_SteamFriends_v017`,
  `..._SteamUser_v021`, `..._SteamNetworking_v006`, `..._SteamUtils_v010` и 17 методов);
  сверено с дампом `steam_bind_interfaces` (`0x3EB34`), список совпал дословно.
* `MP_STEAM=0` — выключено; `MP_STEAM_FAKE=<n>` — стенд (личность `steam:<n>`, датаграммы по
  loopback, файл-записка `%TEMP%\gtmf_fake_steam_<n>.txt` с ключами `persona`, `gtmf`,
  `connect`); иначе — настоящий Steam из `steam_api64.dll` игры.
* P2P: `SEND_UNRELIABLE`, канал 0, `AllowP2PPacketRelay(true)`, `AcceptP2PSessionWithUser` на
  `SessionRequest`, счётчик неудач на `SessionFail`.
* Join: `+gtmf_join=steam:<id>` с командной строки и из `GameJoinRequested`; запрос отдаётся
  ровно один раз.
* Самотест: **21 проверка, 0 падений**, на машине без Steam.
* Якорь `gtmf_steam_layer[]` (экспорт) — чтобы `/OPT:REF` не выбрасывал слой из образа, пока его
  никто не вызывает.

Чего это не доказывает: настоящий Steam, P2P-ретрансляцию и переход друга в лобби. Это M6 —
два ПК, см. [`TWO_PC_TEST_PLAN_2026-10-26`](TWO_PC_TEST_PLAN_2026-10-26.md).

## 10. Шов теперь настоящий (2026-10-26)

Слой перестал быть библиотекой, которую никто не вызывает, и стал транспортом.

| где | что |
|---|---|
| `steam.h` | `steam_peer_make()` / `steam_peer_id()`: пир Steam — обычный `sockaddr_in` с `sa_family=0x5354` и Steam ID на +8, ровно как у v20 |
| `session.cpp` `send_to()` | адрес решает транспорт: `0x5354` → `steam_send()`, иначе `sendto()` |
| `session.cpp` `session_frame()` | пакеты P2P сливаются в тот же `on_packet()`; HELLO, передача образа, состояние и выстрелы общие для обоих транспортов |
| `session.cpp` `MP_PEER` | принимает `steam:<id>` (проверяется до разбора `host:port`) |
| `joiner.h` | `start("steam:<id>")` и `send_host()`; приём — из Steam, когда хост в Steam |
| `ui.cpp` | отложенный Join Game забирается с главного меню и ведёт в лобби — строка `ui: Steam's Join Game - into the lobby of %s when the main menu is up` дословно как у v20 |
| `steam_module.cpp` | `steam_install()` / `steam_tick()` вызываются из `session_start`/`session_frame` **и** из `ui_frame`: запрос друга приходит именно в процесс меню |
| `build.bat join` | `mp_join.exe` снова собирается, уже со слоем Steam |

Что доказывается самотестом (27 проверок): кодировка адреса, датаграммы в обе стороны через стенд,
presence в двух ключах, разбор `+gtmf_join=`, счётчики трафика (`steam: 5 s: … datagrams`).
Что по-прежнему доказывается только двумя ПК: настоящий Steam, ретрансляция P2P и переход друга в
лобби.

Совместимость: формат пакетов не менялся (наш `protocol.h` — копия опубликованного), поэтому наш
модуль понимает v20-й `mp_join.exe`, а модуль v20 понимает наш лаунчер.

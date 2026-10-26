# Сетевой слой v20: транспорт, UPnP, поиск адреса

Дата: **2026-10-26**. Источник — декомпиляция `basic_network.codelib` v20 и сверка с публичным
деревом v12. Задача раздела: зафиксировать то, что нельзя потерять при пересборке, — подключение
по IP и UPnP.

---

## 1. Что уже есть в публичном дереве (и переписывать не нужно)

| Функция v20 | RVA | Статус против v12 | Следствие |
|---|---|---|---|
| `network_create` | `0x0028DC` | **SAME** (0.967) | создание сетевого объекта и регистрация `i_network` — исходник есть |
| `upnp_thread` | `0x022D78` | CHANGED (0.969) | UPnP почти тот же; дельта — 641 Б, видна в [`reports/decomp`](../development/v20-recovery/reports/decomp) |
| `host_listen` | `0x011148` | NEW | приём соединения, ретрай занятого порта |
| `find_local_ip` | `0x022C84` | NEW (242 Б) | внешний адрес через UPnP — единственная новая функция в этой области |
| `on_packet` | `0x016230` | NEW (4906 Б) | разбор пакетов; HELLO, выборы стороны, карта, синхронизация |

Порт `1943` — значение по умолчанию: `0x01C3B4` берёт `MP_PORT`, иначе `1943`, и логирует
`net: UDP port %u is still in use, retrying` при занятом порту.

## 2. UPnP сделан через COM, а не через SSDP

Это не догадка: `upnp_thread` и `find_local_ip` вызывают `ole32!CoInitializeEx`,
`ole32!CoCreateInstance`, `ole32!CoUninitialize`, а в данных лежат два GUID'а:

| Смещение | Байты | Что это |
|---|---|---|
| `0x47A08` | `AA 00 1E AE D5 3F 3C 40 8A 27 2B BD C3 0C D0 E1` | **CLSID `{AE1E00AA-3FD5-403C-8A27-2BBDC30CD0E1}` — `UPnPNAT`** |
| `0x47A18` | `12 C8 71 B1 76 CC 5A 48 94 D8 B6 B3 A2 79 4E 99` | **IID `{B171C812-CC76-485A-94D8-B6B3A2794E99}` — `IUPnPNAT`** |

Строки результата (дословно):

```
net: UPnP mapped UDP %u (external address %s)
net: UPnP mapping refused (0x%08lx); relying on a manual port forward of UDP %u
net: UPnP not available on this network (0x%08lx); internet play needs UDP %u forwarded by hand
```

Для порта это важно по двум причинам:

1. **Не надо писать SSDP/HTTP-клиент.** Windows-компонент `UPnPNAT` уже умеет пробивать
   проброс; мы вызываем его так же, как v20, и получаем тот же `IStaticPortMappingCollection`.
2. **Отказ UPnP не роняет игру.** Все три исхода — это лог; при отказе сеть продолжает работать
   на прямом IP:порт, ровно как сейчас. Это и есть поведение, которое нужно сохранить.

## 3. Транспортный шов: один `sockaddr_in` на два транспорта

Единая точка отправки (`0x19AC8`, ~50 Б) различает транспорт по `sa_family`:

```c
if (peer->sa_family != 0x5354)
    sendto(g_sock, buf, len, 0, peer, 16);                 // обычный UDP, порт 1943
else
    SendP2PPacket(net, steam_id(peer), buf, len, ...);     // Steam P2P
```

`sa_family = 0x5354` (21332) — маркер «это Steam-пир»: тогда по смещению +8 лежит Steam ID.
`on_packet` разбирает HELLO так же: Steam-пир даёт
`net: player connected through Steam (steam:%s, %s)` и `MP_STEAM_PEER=<id>`, обычный —
`net: player connected from %s:%u (%s)`.

Вывод для пересборки: **сетевой код не должен знать, какой транспорт под ним.** Порт сохраняет
это свойство — тогда Steam и IP не расходятся по поведению, а UPnP остаётся независимым
(он говорит про UDP-порт, а не про Steam).

## 4. Что это означает для риска «потерять Steam/UPnP/IP»

| Риск | Почему не теряется |
|---|---|
| IP-подключение | `network_create` — SAME, `on_packet` разобран, порт по умолчанию 1943 известен |
| UPnP | тот же COM-объект и те же GUID'ы; три исхода логируются и не блокируют игру |
| Steam | спецификация из 21 вызова и 4 callback'ов, см. [`V20_STEAM_LAYER.md`](V20_STEAM_LAYER.md) |
| «тихая» деградация | каждая ветка пишет строку; приёмка формулируется по строкам в логе, а не «на глаз» |

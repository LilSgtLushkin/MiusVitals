# План двух-ПК прогона v1.0.1 — 2026-10-26

Цель: зафиксировать baseline рабочей версии и впервые проверить **Steam-путь двумя
реальными аккаунтами** (у upstream он не проверялся ни разу). Результат этого прогона —
точка отсчёта для всех дальнейших правок и откатов.

## 0. Перед запуском (на обоих ПК)

- Один и тот же архив `MiusMultiplayer_v1.0.1_INSTALL.zip`.
- Запустить `Check_Install.bat`, сохранить `MiusMultiplayer\build_report.txt`.
- Сверить хеши (должны совпасть у обоих):

| Файл | SHA-256 |
|---|---|
| `basic_network.codelib` | `7b42a69abe58deb1583d2f5320eebbecd9718ffa27ea08933613e67df1901f67` |
| `mp_vitals.dll` | `71a1bf3a37923bbe6c8ee84448d0b91705c2bff7382b647f990b9d671ee224df` |
| `mp_join.exe` | `6aa417cf6e1f01480c997446f2691a58ab55b90fa36e1ff511945989aadfeeaa` |
| `opgame.progpack` | `eca75b740f7b11d9e13f2e2001f1b2b133427d0860fb3770220b89b0b8909535` |
| `tacgame.progpack` | `01923eabdf6a9d5fa37dc70a3d76740242dbc9a7f26c0298ca3faa8cca87600d` |

- Игра Steam 6.00 build **3598/1**; Steam запущен, аккаунты в друзьях.
- Закрыть другие моды/оверлеи; освободить `out\` от старых логов (или записать время начала).
- Записать конфигурацию: CPU/GPU, разрешение, примерный FPS в одиночной игре.
- Для одного тяжёлого боя можно включить `MP_LOG_SHOTS=1` и `MP_LOG_TRENCH=1`
  (пишут файлы в `out\`; для остальных сценариев лучше выключить).

## 1. Сценарии (по возможности — в обе стороны, кто хостит)

| # | Сценарий | Что считается успехом |
|---|---|---|
| S1 | **Адресный вход (baseline):** host skirmish → гость вводит IP → лобби → READY → бой 3 мин → выход | подключение, лобби, бой без падений, юниты не расходятся > 5 м |
| S2 | **Steam-друзья (критично):** host открывает лобби → гость видит `STEAM FRIENDS HOSTING A GAME` → JOIN → бой 3 мин | гость в списке, JOIN без адреса/порта, P2P-сессия, бой |
| S3 | **Steam Join Game:** правый клик по другу в Steam → Join Game (игра запущена и закрыта) | игра входит в лобби хоста сама |
| S4 | **Операция:** host открывает операцию → оба на карте → оба «next turn» → бой → возврат на карту | карта у обоих, ход считается, бой и возврат |
| S5 | **Ceasefire + Withdraw** в скирмише | предложение/принятие, одинаковый финал, экраны совпадают |
| S6 | **Выход/реконнект:** host закрывает игру в лобби и в бою | гость возвращается в меню без зависания |
| S7 | **Тяжёлый бой** 150+ юнитов (лучше операция 400+), 3–4 мин на x2–x3 | FPS не хуже одиночной, нет ритмичных фризов |

S2 — главный сценарий дня: если Steam-путь не работает, лог покажет, на каком шаге
(`steam: ready` / `friends are told` / `no connection` / `player connected through Steam`).

## 2. Что сохранить с каждого ПК

**Один файл:** после прогона запустите `MiusMultiplayer\Collect_Logs.bat`. Он соберёт
`MiusMultiplayer\GTMP_alpha_log.txt` — система, версия игры, хеши установленных файлов,
переменные `MP_*`, `build_report.txt`, выжимка по событиям и полные логи. Для разбора
достаточно по одному такому файлу с каждого ПК. Ниже — что именно он забирает.

- `out\basic_network_opgame_*.log` — все за прогон;
- `out\basic_network_tacgame_*.log` — все за прогон;
- `out\mp_shots_*.txt`, `out\mp_trench_*.txt` — если включали;
- `MiusMultiplayer\build_report.txt`;
- `users\mp_saves\*.opsave` — после hand-off карты (S4);
- скриншоты/видео видимых проблем + время.

Быстрый сборщик (PowerShell, путь игры поправить):

```powershell
$g = "C:\Program Files (x86)\Steam\steamapps\common\Graviteam Tactics Mius-Front"
$dst = "$env:USERPROFILE\Desktop\mp_logs_$env:COMPUTERNAME.zip"
Compress-Archive -Force -DestinationPath $dst -Path `
  "$g\out\*.log","$g\out\mp_*.txt","$g\MiusMultiplayer\build_report.txt","$g\users\mp_saves\*.opsave"
```

## 3. Что я проверю в логах

- `steam: ready - user <id> (<name>), application 312980, ...` — Steam загрузился;
- `steam: friends are told that this game can be joined` — presence выставлен;
- `net: player connected through Steam (steam:..., ...)` — P2P-сессия;
- `net: player connected from ...` — адресный вход;
- `vitals: object-number exchange complete` — Vitals-поток включился;
- `state:` — объём state-трафика;
- `connection lost`, `unknown game build`, `could not be hooked` — ошибки.

## 4. Критерии приёмки baseline

- S1, S4, S5, S6 проходят без падений;
- S2 либо проходит, либо даёт конкретную строку отказа — обе ситуации полезны;
- S7: нет деградации FPS относительно одиночной игры и нет ритмичных фризов;
- у обоих одинаковые `build_report.txt` и совпадающие хеши.

## 5. Если что-то упало

1. Не перезапускать игру до копирования `out\` (логи per-PID, но лучше сохранить сразу).
2. Записать: кто хостил, сценарий, время, что было на экране.
3. Приложить `.dmp`, если появился.
4. После сбора — можно перезапускать и повторять; каждый прогон = новый набор логов.

# Стыки целей сборки

Как цели `build.bat` соединяются друг с другом, с прогоном тестов, с CI и с паковщиком.
Версия задаётся переменной `env: VERSION:` в workflows; пути ниже — для
`development/versions/<VERSION>/`.

## Цели `build.bat`

| Цель | Что собирает | Куда | Из чего |
|---|---|---|---|
| `baseline` | `basic_network.codelib` | `build\baseline[-debug]\` | **замороженная копия** `..\..\upstream\miusplayer-public-43b2736\basic_network` (8 файлов) |
| `module` | `basic_network.codelib` (+ `.lib`, `.map`) | `build\module[-debug]\` | наши `src\` — 15 файлов `%BASE%` + 2 файла `%STEAM%` |
| `selftest` | 49 выходов: тесты и стенды | `build\selftest[-debug]\` | `tests\` и часть `src\` |
| `render_test` | `render_guard_test.exe` с `/DNETV21_TEST_HOOKS` | `build\render_test[-debug]\` | `render_guard.cpp`, `hooks.cpp`, `tests\render_guard_test.cpp` |
| `join` | `mp_join.exe` (`/MT /O1 /EHsc`) | `build\join[-debug]\` | `mp_join.cpp`, `steam.cpp`, `steam_logger.cpp` |
| `all` | `baseline` → `module` → `selftest` → `join` | — | `render_test` **не входит** |

`baseline` собирается не из `src\`, а из замороженной копии: он измеряет инструмент, а не наши
правки. Если каталог `upstream\` отсутствует, `baseline` — а значит и `all` — падает.

Конфигурация (`debug` / `release`) добавляет суффикс к каталогу: `build\module-debug\` против
`build\module\`. **Без суффикса — только release**, и паковщик ищет именно его.

## Кто что потребляет

- `run_tests.bat [release|debug]` запускает тесты из `build\selftest[-debug]\`; предварительно
  нужна цель `selftest` **той же конфигурации** (иначе «каталог не существует — сначала
  `build.bat selftest <CONFIG>`»).
- `scripts/package_net_v21_test.py` забирает модуль по пути
  `development/versions/<VERSION>/build/module/basic_network.codelib` — release, без суффикса.
- Цель `module` в debug дополнительно кладёт модуль рядом с игрой, когда задан `MIUS_GAME_DIR`
  (в release символов нет, поэтому копируется только debug).

## CI

`build-network-module.yml` вызывает цели по отдельности, каждый вызов пишет свой лог
в `build\<цель>.log`, ненулевой код завершает шаг:

| Строка | Цель |
|---|---|
| :133 | `baseline` |
| :154 | `module` |
| :175 | `selftest` |
| :389 | `join` |

Сразу после `selftest` несколько тестов запускаются по именам (:322–334: `state_memory_test`,
`hooks_selftest`, `render_guard_test`, `xlate_selftest`). Они должны существовать в
`build\selftest\` — то есть цель `selftest` обязана их собирать: правка списка в `build.bat`
без правки этих строк (или наоборот) расходится молча, если exe просто не запускается.

Срез «пауза и заголовки состояния» выполняется **внутри** `build.bat selftest` (:211) — ему нужен
компилятор, поэтому он живёт в цели, а не отдельным шагом.

`publish-installer.yml` (:115) **пересобирает** модуль целью `build.bat module` своими настройками,
а не берёт артефакт, прошедший проверки. Это развилка: установщик может уйти с модулем, собранным
не той конфигурацией, что тестировалась.

## Стенды в `build\selftest\`

Помимо тестов цель собирает исполняемые стенды, которые нужны другим прогонам:
`late_input_fixture.dll`, `late_input_test.exe`, `starter_fixture.exe`, `loopback_peer.exe`,
`session_loopback_peer.exe`, `session_event_peer.exe`. Их отсутствие ломает не сам `selftest`,
а шаги, которые их запускают.

## Что стоит помнить при правках

1. Новая цель в `build.bat` не попадает в CI сама: надо добавить вызов в
   `build-network-module.yml` и имя лога.
2. `all` и CI расходятся: цель `render_test` не входит в `all`, и CI её не вызывает — но одноимённый
   `render_guard_test.exe` собирается внутри цели `selftest` (с тем же `/DNETV21_TEST_HOOKS`, но
   с `%CL_T%` вместо `%CL_MD%` и в другом каталоге). Один и тот же тест собирается дважды: править
   надо обе сборки либо убрать лишнюю цель.
3. Конфигурация — часть пути: release-паковщик не найдёт debug-модуль.
4. `%CL_MD%` (динамическая рантайм) у всех целей, кроме `join` (`/MT`): `mp_join` не зависит от
   рантайма модуля и может собираться отдельным шагом.

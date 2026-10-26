# Документация MiusVitals

Указатель по разборам и решениям. Имена файлов не переименовываются: на них ссылаются
`README.md`, `development/README.md`, `docs/BUILD_AND_VALIDATION.md` и рабочие процессы CI.

| Документ | Тема | Статус |
|---|---|---|
| [`BUILD_AND_VALIDATION.md`](BUILD_AND_VALIDATION.md) | входы, артефакты, локальная проверка и полная пересборка обоих архивов | **действует** |
| [`MULTIPLAYER_MENU_BUTTON_2026-10-25.md`](MULTIPLAYER_MENU_BUTTON_2026-10-25.md) | почему кнопка MULTIPLAYER может не появиться: гейты замены файлов, `mp_vitals.dll`, VC++ runtime, MOTW | **действует** |
| [`NETWORK_CODE_AUDIT_2026-10-25.md`](NETWORK_CODE_AUDIT_2026-10-25.md) | граница аудита сетевого кода, исправленные hot-path проблемы, safety boundaries source-only candidate | **действует** |
| [`OPERATION_MAP_ANALYSIS_2026-10-25.md`](OPERATION_MAP_ANALYSIS_2026-10-25.md) | операционная карта: причина преждевременного Next Turn, исправление v7.2.2 | **действует** |
| [`TACTICAL_OPERATION_STATE_POLICY_2026-10-25.md`](TACTICAL_OPERATION_STATE_POLICY_2026-10-25.md) | почему в тактической операции полный state-синк выключен (`MP_SYNC_STATE=0`) | **действует** |
| [`TACTICAL_PERFORMANCE_AND_MAP_EVIDENCE_2026-10-25.md`](TACTICAL_PERFORMANCE_AND_MAP_EVIDENCE_2026-10-25.md) | ритмичные фризы, повторный обмен object numbers, политика idle | **действует** |
| [`RENDER_CACHE_ROOT_CAUSE_2026-10-25.md`](RENDER_CACHE_ROOT_CAUSE_2026-10-25.md) | root cause падения: stale transform-cache slot в `basic_render` | **действует** |
| [`TRIANGLE_GEOMETRY_ROOT_CAUSE_2026-10-25.md`](TRIANGLE_GEOMETRY_ROOT_CAUSE_2026-10-25.md) | root cause падения: некорректная геометрия в triangle query | **действует** |
| [`CRASH_ANALYSIS_2026-10-25.md`](CRASH_ANALYSIS_2026-10-25.md) | второй invalid-child путь `basic_render`, guard v7.1.1 | **действует** |
| [`CRASH_ANALYSIS_2026-10-24.md`](CRASH_ANALYSIS_2026-10-24.md) | первый разбор падения `basic_render`, guard v7.0.1 | исторический: перекрыт документами от 2026-10-25 |
| [`UPSTREAM_V20_REPO_ANALYSIS.md`](UPSTREAM_V20_REPO_ANALYSIS.md) | полный разбор `VampyrCat/MiusPlayer`: публичный исходник — v12, v20 — только бинарник в `GTMPRelease1.zip`, дельта 293 функции | **действует** |
| [`V20_SOURCE_RECOVERY.md`](V20_SOURCE_RECOVERY.md) | восстановление исходника v20: именованный разбор 895 функций, дельта к публичному дереву (323 работы), план порта M0–M6 с критериями приёмки | **действует** |
| [`SOURCE_REBUILD_REQUIREMENTS.md`](SOURCE_REBUILD_REQUIREMENTS.md) | границы нашего процесса: что можно менять, что снято решением 2026-10-26, сверка Steam ABI по присланной DLL | **действует** |
| [`VITALS_CPP_PORT_PLAN.md`](VITALS_CPP_PORT_PLAN.md) | поэтапный перенос логики `mp_vitals` в C++: модули, инварианты, точки отката, E0–E8 | **действует** |
| [`TWO_PC_TEST_PLAN_2026-10-26.md`](TWO_PC_TEST_PLAN_2026-10-26.md) | план двух-ПК прогона v1.0.1: сценарии S1–S7, что сохранять, критерии приёмки | **действует** |
| [`NETWORK_LIMITATIONS.md`](NETWORK_LIMITATIONS.md) | карта точных сайтов в релизном модуле и известные ограничения сетевого кода (G1–G7) | **действует** |
| [`MIUSPLAYER_ANALYSIS.md`](MIUSPLAYER_ANALYSIS.md) | независимый разбор MiusPlayer v20: нативный надёжный поток, зачем изменён Vitals-поток | **действует** |

## Как читать

1. Установка и «почему не работает» — `README.md` в корне, затем `MULTIPLAYER_MENU_BUTTON…`.
2. Сборка, хеши, проверки — `BUILD_AND_VALIDATION.md`.
3. Падения и защитные обёртки — три документа `*ROOT_CAUSE*` и `CRASH_ANALYSIS_2026-10-25.md`.
4. Границы сетевого кода и что считается доказанным — `NETWORK_CODE_AUDIT…`.

## Дата в имени файла

`*_2026-10-24.md` и `*_2026-10-25.md` — дата разбора, а не версия мода: документы
описывают наблюдения на конкретной сборке игры (Steam 6.00 build 3598/1) и на конкретном
релизе мода. Если поведение меняется, добавляется новый документ, а старый помечается
здесь как исторический.

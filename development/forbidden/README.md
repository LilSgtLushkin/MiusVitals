# Парковка: vitals

Сюда сложено всё, что относится к vitals (`mp_vitals.dll` и модуль показателей бойцов).
Работа по vitals отложена до разбора падений: в дампе 2026-10-28 модуль пишет
`applied 0 refused 50` — ни один пакет не применяется. Когда дойдём, материалы берём отсюда.

Пути внутри повторяют исходное расположение файлов в репозитории, чтобы ничего не потерялось:

* `development/net-v21/src/vitals.{cpp,h}`, `vitals_core.h` — модуль vitals внутри `basic_network`;
* `development/net-v21/tests/vitals_*_test.cpp` — три теста модуля;
* `development/net-v21/tools/vitals_hook_audit*.py` — аудит хуков vitals;
* `development/kit-source/src/mp_vitals.c` и `*.bat`/README — sidecar и его установка;
* `*_kit-source/files/mp_vitals.dll`, `gtmp-alpha/files/mp_vitals.dll` — собранные sidecar;
* `docs/NET_V21_VITALS_1_0_7.md`, `docs/VITALS_CPP_PORT_PLAN.md` — спецификация и план порта.

**Это копии, а не перенос.** Оригиналы пока на своих местах: на них завязаны CI-проверки
(в том числе хэш-закреплённые пути в `scripts/check_artifacts.py`), а их мы трогаем только
после отдельного разговора об объёме проверок. Оригиналы удалим, когда перестроим CI — тогда
единственным местом хранения vitals останется эта папка.

# Сборка в Visual Studio Code: debug и release

Всё, что нужно для работы в VS Code, лежит в репозитории: `.vscode/tasks.json`,
`.vscode/launch.json`, `.vscode/c_cpp_properties.json`. Открываете папку репозитория — и
собираете. Сборка по-прежнему идёт через `build.bat` (Visual Studio 2022 C++), VS Code только
вызывает его и показывает ошибки.

## Что нужно на машине

* **Visual Studio 2022** с «Разработка классических приложений на C++» (нужен `cl.exe`).
  `build.bat` сам находит инструменты: сначала Community/Professional/Enterprise, потом через
  `vswhere.exe`.
* Расширение **C/C++ Extension Pack** (`ms-vscode.cpptools-extension-pack`) — VS Code предложит
  его при открытии папки.
* Для портируемого теста под Linux/WSL — обычный `g++`.

## Сборка

`Ctrl+Shift+B` — задача по умолчанию: **module (Release)**, то есть ровно то, что собирает CI.

| Задача VS Code | Команда | Результат |
|---|---|---|
| Build: module (Release) | `build.bat module release` | `build\module\basic_network.codelib` — то, что идёт в игру |
| **Build: module (Debug)** | `build.bat module debug` | `build\module-debug\basic_network.codelib` + `.pdb` |
| Build: baseline (Release/Debug) | `build.bat baseline …` | исходники v12 без изменений — эталон для сравнения |
| Build: selftests (Release/Debug) | `build.bat selftest …` | все тесты в `build\selftest[-debug]\` |
| Build: mp_join (Release/Debug) | `build.bat join …` | `mp_join.exe` |
| Build: render guard test (Debug) | `build.bat render_test debug` | только тест защиты рендерера, быстро |
| Build: everything (Release) | `build.bat all release` | baseline + module + selftest + join |
| Run: self-tests (Release/Debug) | `run_tests.bat …` | собирает и прогоняет все тесты |
| Clean: build output | — | удаляет `build\` |

Задача **Run: self-tests** назначена тестовой по умолчанию, поэтому запускается из
`Ctrl+Shift+P` → «Tasks: Run Test Task» (соберёт тесты и прогонит их).

### Чем отличается debug

| | Release (по умолчанию) | Debug |
|---|---|---|
| Оптимизация | `/MD /O1 /Brepro` | `/Od /MD /Zi /DNETV21_DEBUG` |
| Символы | нет | `/Zi` + `.pdb`, компоновка с `/DEBUG` |
| Выходной каталог | `build\<цель>\` | `build\<цель>-debug\` |
| Среда выполнения C | обычная (`/MD`) | **та же обычная**, а не отладочная |

Отладочная сборка намеренно линкуется с обычной CRT (`/MD`, а не `/MDd`): модуль загружает игра,
и тащить в её процесс отладочные DLL среды выполнения ни к чему. Пошаговая отладка при этом
работает полностью — символы есть.

**Важно:** без второго аргумента `build.bat` ведёт себя в точности как раньше — те же флаги, те же
каталоги. Командные строки компилятора для CI не изменились ни на один символ, проверено
сравнением развёрнутого release-варианта с прежним файлом.

## Отладка в игре

1. Соберите **Build: module (Debug)**.
2. Положите `build\module-debug\basic_network.codelib` в игру вместо штатного модуля
   (так же, как ставится обычный: `bin\rel\x64\basic_network.codelib`; перед подменой сделайте
   копию штатного файла).
3. Запустите игру, зайдите в бой.
4. `F5` → **Attach to tacgame (Debug module)** (для карты операции — **Attach to opgame**).
5. Ставьте точки останова в `development/net-v21/src/*.cpp` — символы подхватятся из
   `build\module-debug`.

Если процесс игры не находится по имени, есть конфигурация **Attach to any game process (pick)** —
она покажет список процессов.

Полезные переменные окружения (задаются до запуска игры, пишут в `out\basic_network_*.log`):
`MP_DEBUG=1` — опись объектов и отладочный вывод,
`MP_APPLY_DELAY`, `MP_SYNC_STATE=0`, `MP_RENDER_GUARD=0` — диагностика по
`docs/CRASH_ANALYSIS_2026-10-25.md` и `docs/RENDER_CACHE_ROOT_CAUSE_2026-10-25.md`.

## Тесты

`run_tests.bat` прогоняет тот же список тестов, что и CI (44 штуки), и печатает
`ok` / `FAIL` / `MISSING` для каждого:

```
build.bat selftest debug
run_tests.bat debug
```

Отдельно стоит `tests/transform_object_check_test.cpp` — построчный прогон проверки объекта перед
записью трансформа (проверка на реальной карте памяти: Windows — `VirtualQuery`, Linux —
`/proc/self/maps`). Он собирается и там, и там, причём без `windows.h`:

```
cl /Isrc tests\transform_object_check_test.cpp     (входит в build.bat selftest)
g++ -std=c++11 -O0 -g tests/transform_object_check_test.cpp -o /tmp/toc-test   (Linux/WSL)
```

Вывод — по шагу, с причиной отказа:

```
   0  object pointer is not null         ok
   1  object memory is readable          REFUSED  object memory is not readable
```

## Настройки для чтения кода

`c_cpp_properties.json` содержит три конфигурации IntelliSense: **Windows (MSVC, Release)**,
**Windows (MSVC, Debug)** и **Portable (g++, no windows.h)**. Переключаются через
`Ctrl+Shift+P` → «C/C++: Select a Configuration». Для портируемых заголовков (тех, что без
`windows.h`) удобна третья: она сразу показывает, если в заголовок случайно попала
платформенная зависимость.

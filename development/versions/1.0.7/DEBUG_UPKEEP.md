# Что обновлять при каждом обновлении мода (чтобы debug оставался рабочим)

Release собирает CI через `build.bat`. Debug живёт в трёх местах: `net-v21.slnx`
(Visual Studio 2026), `.vscode/` (VS Code) и `run_tests.bat`. Все они перечисляют те же
исходники и те же тесты, но в трёх разных форматах — поэтому расходятся молча: файл,
добавленный в `build.bat`, просто не попадёт в сборку студии, а новый тест никто не запустит.
Обе сборки при этом останутся «зелёными», но отлаживать будете уже не то, что shipped.

## Автоматическая проверка

```bat
python scripts/check_debug_sources.py
```

Сравнивает три списка и возвращает 1 при любом расхождении:

* исходники модуля и baseline — `build.bat` (`set BASE` / `set STEAM` / `set BASELINE`)
  против `net-v21-module.vcxproj` и `net-v21-baseline.vcxproj`;
* тесты — что собирает `build.bat`, что прогоняет `run_tests.bat` и что прогоняет
  `.github/workflows/build-network-module.yml`.

Гонять после каждого изменения исходников, до коммита. В гейт релиза проверка не входит:
она смотрит путь разработчика, а не shipped-модуль.

## Ручной чек-лист

1. **Новый или удалённый `.cpp` в `src/`** → `net-v21-module.vcxproj` (для baseline —
   `net-v21-baseline.vcxproj`). Иначе студия его не соберёт, а IntelliSense не увидит.
   Ловится скриптом выше.
2. **Новый тест** → три места сразу: `build.bat` (цель `selftest`), `run_tests.bat`,
   `.github/workflows/build-network-module.yml`. Парный тест (два процесса) запускается
   не напрямую, а через Python-харнесс из `tools\` — см. четыре строки `:runpair` в
   `run_tests.bat`.
3. **Новые флаги или дефайны** → `net-v21-common.props` (общие) плюс конфигурации в
   vcxproj; для подсветки — три конфигурации в `.vscode/c_cpp_properties.json`.
4. **Новые каталоги включения** → `AdditionalIncludeDirectories` в проектах и
   `includePath` в `c_cpp_properties.json`.
5. **После правок прогнать** `build.bat all debug` и `run_tests.bat debug`. Командные строки
   release при этом меняться не должны — если `git diff build.bat` показывает изменения
   внутри release-ветви, это повод разобраться, а не коммитить.
6. **Смена протокола** (`protocol.h`, `k_wire_tag`) → проверить, что debug и release одинаково
   проходят handshake; симптом рассинхрона — расхождение версий в логе.
7. **PDB** — после debug-сборки в папку игры должны попасть вместе `basic_network.codelib`
   и `basic_network.pdb` (либо задайте `MIUS_GAME_DIR`, и post-build скопирует сам).

## Ограничения

* Парные тесты требуют Python в `PATH` (`python` или `py -3`) и UDP на `127.0.0.1` —
  Windows Firewall при первом запуске может спросить разрешение.
* `standalone_loopback` требует собранный `mp_join.exe` (`build.bat join`); если его нет,
  тест честно печатается как MISSING.

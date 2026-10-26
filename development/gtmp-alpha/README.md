# GTMP_alpha — экспериментальная сборка поверх стабильного v1.0.1

Каталог хранит **только то, чем alpha отличается от стабильного комплекта**: пересобранную
`mp_vitals.dll`, текст для игроков и скрипт пересборки. Сам архив собирается из
`development/unified-fps-kit-source` (это и есть копия стабильной версии) скриптом
`scripts/package_gtmp_alpha.py`.

## 1. Состав и хеши

| Файл | SHA-256 | Откуда |
|---|---|---|
| `files/mp_vitals.dll` | `808e1d5f6cb301ac971700e0aba6e78d1df1b13b02e748b8d01dd4bfc8a9881c` | сборка из `development/kit-source/src/mp_vitals.c` |
| `GTMP_alpha_INSTALL.zip` | см. `scripts/check_artifacts.py` | `scripts/package_gtmp_alpha.py --output` |

В alpha-архиве совпадают с v1.0.1 побайтово: `basic_network.codelib`
(`7b42a69a…1f67`), `mp_join.exe`, `opgame.progpack`, `tacgame.progpack`,
`steam_appid.txt`. Меняется только DLL и тексты (метка `GTMP_alpha` и новый hash).

## 2. Что исправлено в DLL

Alpha отличается от `71a1bf3a…` только исправлениями без изменения поведения:

1. `MP_VITALS`, `MP_VEHICLES`, `MP_STATS`, `MP_RENDER_GUARD` — значение длиннее
   принимаемого буфера больше не разбирается как неинициализированная память; оно
   считается «не задано».
2. `MP_IDLE_MS` — верхняя граница 6000 мс теперь соблюдается строго (раньше
   `60000`, `12000` и т.п. проходили проверку).
3. `hex_of()` — защита от пустого указателя.
4. `drain_mailbox()` — запись короче `VitalsHead` не разбирается как пакет.
5. Баннер при старте: `vitals: GTMP_alpha-1 experimental build (bugfix series 1)` плюс
   значения `MP_*`, которые получил процесс игры (пишется из первого кадра, а не из
   `DllMain`: логгер модуля к этому моменту гарантированно готов).
6. Строка `rx: last 5 s, datagrams from the other game (type:count/bytes) …; total N`
   каждые пять секунд при наличии трафика — **без** `MP_LOG_VITALS`.
7. Один раз за сессию: `rx: no datagrams from the other game in the first 10 s of this session`.

Сетевой протокол, набор типов пакетов (`0x34`/`0x35`), все RVA-профили и сигнатуры
не менялись — alpha и v1.0.1 совместимы между собой по сети.

## 3. Пересборка DLL

```sh
python3 -m venv .venv && .venv/bin/pip install -r development/kit-source/src/requirements-build.txt
development/gtmp-alpha/rebuild.sh
```

`rebuild.sh` собирает `files/mp_vitals.dll` тем же `build.sh`, что и стабильный комплект
(Zig 0.17.0, `-target x86_64-windows-gnu`, `/nodefaultlib`, `/Brepro`, `/timestamp:0`), и
печатает SHA-256. Сборка детерминирована: повторный запуск даёт тот же файл.

Важно: DLL встраивает **собственное имя файла** (оно попадает в текст crash-guard), поэтому
собирать нужно всегда в файл с именем `mp_vitals.dll`. Сборка в `mp_vitals_rebuild.dll` даёт
другой SHA-256 при том же коде — именно на этом ломалась первая версия `--baseline`.

Проверка, что инструмент не «съехал»: сборка **исходника стабильной версии** должна давать
ровно `71a1bf3a…`. Стабильный исходник — это версия файла в коммите, от которого ответвилась
текущая работа:

```sh
mkdir -p /tmp/baseline && git show e8d4a52:development/kit-source/src/mp_vitals.c > /tmp/baseline/mp_vitals.c
```

## 4. Сборка и проверка архива

```sh
python3 scripts/package_gtmp_alpha.py --output GTMP_alpha_INSTALL.zip
python3 scripts/package_gtmp_alpha.py --verify GTMP_alpha_INSTALL.zip
sh scripts/verify_release.sh
```

Архив детерминирован: фиксированное время записей, фиксированный порядок, один корень
`MiusMultiplayer/`. Скрипт проверяет, что все файлы, кроме DLL и текстов, побайтово равны
стабильному комплекту, а текст не содержит остатков `v1.0.1`.

## 5. Что входит в alpha и чего в ней нет

* **Входит:** исправления DLL из §2, метка alpha в установщике, `ALPHA.txt` для игроков,
  `Collect_Logs.bat`/`.ps1` — сборщик одного отчётного файла `GTMP_alpha_log.txt`.
* **Не входит:** правки `basic_network.codelib` (дефекты G1–G7 остаются как в v1.0.1),
  перенос Vitals в C++ (`docs/VITALS_CPP_PORT_PLAN.md`), изменения карты операции.

## 6. Один файл для разбора

`MiusMultiplayer\Collect_Logs.bat` собирает `MiusMultiplayer\GTMP_alpha_log.txt`:
система, версия игры и `steam_api64.dll`, хеши установленных файлов против эталона
alpha, переменные `MP_*`, `build_report.txt`, выжимку по событиям и полные логи
`out\basic_network_*.log`. Скрипт только читает и хеширует файлы; в игре он ничего
не меняет. Это единственный файл, который нужно присылать с каждого ПК.

## 7. Откат

Alpha не меняет ни одного файла, кроме `mp_vitals.dll`, поэтому откат — это установка
`MiusMultiplayer_v1.0.1_INSTALL.zip` тем же `Enable_Multiplayer.bat` (он заменяет всё
безусловно) либо `Disable_Multiplayer.bat` из этого же архива: его `VITALS=` уже указывает
на alpha-хеш, поэтому удаляется именно alpha-DLL.

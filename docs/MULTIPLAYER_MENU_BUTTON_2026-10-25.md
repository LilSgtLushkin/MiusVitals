# Почему может не появиться кнопка MULTIPLAYER в главном меню

Дата: 2026-10-25. Относится к `MiusMultiplayer_v1.0.1_INSTALL.zip` (Steam Mius-Front 6.00 build 3598/1)
и к тому же коду установщика в `development/unified-kit-source` (v1.0.0).

## 1. Что именно рисует кнопку

Кнопку не рисует ни один файл игры. Её рисует сам `basic_network.codelib` через render pipeline,
и только в том случае, если engine его загрузил и создал объект `i_network`:

1. `root\programs\opgame.progpack` — это loose program pack, который engine предпочитает
   упакованному `root\programs.flatdata` (`development/upstream/miusplayer-public-43b2736/tools/make_progpack.py:1-8`).
   В нём:
   * `codelib_list` содержит `basic_network.codelib` (11 записей в shipped pack);
   * `obj_list` содержит `i_network:network`;
   * в `r_main` добавлена строка `i_network` сразу под `i_cursor` (`make_progpack.py:27-31`), поэтому
     `on_render` вызывается из render pipeline.
2. Engine загружает `bin\rel\x64\basic_network.codelib` и создаёт `i_network`; затем вызывает его
   команды `sync`, `tick`, `ctrl_tick`, `render`, `generate` (`basic_network.cpp:554-576`,
   `basic_network.cpp:594-600`, `basic_network.cpp:674-712`).
3. `on_tick` вызывает `ui_start`/`ui_frame`, `on_render` вызывает `ui_render`
   (`basic_network.cpp:554-576`, `basic_network.cpp:594-600`).
4. `ui_frame` каждый кадр вычисляет `g_on_main_menu = on_main_menu(om)` (`ui.cpp:446`, `ui.cpp:465`).
   `on_main_menu` — это **жёстко зашитые смещения** Steam 3598/1:
   `i_opgame + 0x100` → menu controller, `controller + 0x1258 == 0` (main menu) и
   `controller + 0x1200 != 0` (`ui.cpp:59-79`).
5. `ui_render` рисует entry только если `hint = !g_tactical && g_screen == SCR_NONE && g_on_main_menu`
   (`ui.cpp:815-820`), и если `find_device` нашёл `IDirect3DDevice9` внутри `i_render`
   (`ui.cpp:121-175`).

Отсюда все причины отказа делятся на две группы: **модуль не загрузился** (engine не создал
`i_network`) или **модуль загрузился, но главное меню не распознано** (другой build игры).

Проверено, что кнопку не прячут:

* `mp_vitals.dll` — её `DllMain` всегда возвращает `TRUE` (`development/forbidden/development/kit-source/src/mp_vitals.c:1364-1372`,
  то есть сама DLL не отказывается загружаться и не может «выключить» кнопку своими signature checks;
* переменные окружения `MP_MENU` (выставляется только при закрытии меню, `ui.cpp:52`) и `MP_AUTOJOIN`
  (только повторно открывает lobby, `ui.cpp:441`) не скрывают entry;
* `open_stock_screen` проверяет байты `adv_opgame.codelib` (`ui.cpp:81-107`), но это влияет только на
  открытие Skirmish/Missions/Operations после клика, а не на отрисовку самой кнопки.

## 2. Причины, по которым кнопки нет

### 2.1 Установка не заменила файлы (главная причина, и она была заложена в установщике)

До этого изменения `Enable_Multiplayer.bat` содержал четыре независимых «проверки», каждая из которых
могла остановить установку или оборвать её на середине:

| Проверка (старый код) | Что происходило |
| --- | --- |
| `:known_network` — allow-list из 10 известных SHA-256 | Если в игре лежал **любой другой** `basic_network.codelib` (другой выпуск, другой мод, файл после Steam/AV/правки), установщик печатал `Existing basic_network.codelib is unknown` и выходил с `RC=2`, **не скопировав ничего**. Кнопки нет. |
| `:copy_checked` — SHA-256 после каждого `copy` | Проверка возвращала ошибку не только при реальном сбое копирования: `certutil` может отсутствовать, быть запрещён политикой, вернуть локализованный вывод или не разобраться в пути с не-ASCII символами. Тогда `H` пустой → `SHA-256 verification failed` → `goto :failed`. Файлы копировались по порядку `mp_vitals.dll` → `codelib` → `mp_join` → `opgame` → `tacgame`, поэтому обрыв оставлял **codelib без program pack** — engine вообще не создаёт `i_network`. |
| `:backup_if_foreign` | Ошибка `mkdir`/`copy` в `state\backup` (read-only папка, AV, OneDrive) прерывала установку **до** копирования файлов. |
| `for /f` по `game_build_3598_1.sha256` | Сам по себе не блокировал (это только fingerprint), но добавлял `certutil`-проход по 20 файлам игры; при отсутствии списка `for /f` тоже даёт ошибку. |

Отдельный, легко не заметный случай того же класса: codelib заменён, а `root\programs\opgame.progpack` — нет.
Stock-программа `opgame` сама перечисляет `basic_network.codelib` в `codelib_list` и `i_network:network`
в `obj_list`, поэтому модуль **загрузится и напишет лог**, но в `r_main` у неё нет строки `i_network`,
и `on_render` никогда не вызывается. Снаружи это ровно «кнопки нет», хотя модуль жив. У нашего pack'а
`i_network` стоит в `r_main` (индекс 16, то есть 17-я из 19 записей), у `tacgame` — индекс 38 из 42, поэтому проверка хеша
`opgame.progpack` (`eca75b74…9535`) в `Check_Install.bat` сразу показывает такую половинчатую установку.

Плюс `Disable_Multiplayer.bat` удалял только файлы с **точным** ожидаемым SHA-256 (`:is_ours`). Если
файл был изменён (обновление, AV, ручная правка), он оставался в игре с сообщением `left changed file`.
Следующий `Enable_Multiplayer.bat` упирался в `:known_network` и отказывался ставить мод. Это и есть
самый частый сценарий «раньше работало, теперь кнопки нет».

### 2.2 `mp_vitals.dll` отсутствует или не загружается

`basic_network.codelib` из kit'а имеет **жёсткий статический импорт** `mp_vitals.dll!MpVitalsInit`
(это проверял `development/kit-source/src/verify_artifacts.py:check_module`, скрипт убран 2026-10-31). Windows разрешает
зависимости DLL по папке exe (`starter_x64.exe`), затем System32 и PATH — **не** по `bin\rel\x64`.
Поэтому `mp_vitals.dll` обязана лежать в корне игры. Если её нет, повреждена, заблокирована или
несовместима по разрядности, загрузится **ни** DLL, **ни** codelib. Engine молча продолжит без
`i_network`: главное меню стоковое, кнопки нет, лога модуля тоже нет.

Воспроизвести проверку импорта без Windows:

```sh
python3 - <<'EOF'
import struct
p='development/unified-fps-kit-source/files/bin/rel/x64/basic_network.codelib'
d=open(p,'rb').read(); pe=struct.unpack_from('<I',d,0x3c)[0]
nsec=struct.unpack_from('<H',d,pe+6)[0]; opt=pe+24; osz=struct.unpack_from('<H',d,pe+20)[0]
ddoff=opt+(112 if struct.unpack_from('<H',d,opt)[0]==0x20b else 96)
secs=[]
for i in range(nsec):
    o=opt+osz+i*40; va,vs,ra,rs=struct.unpack_from('<IIII',d,o+8); secs.append((va,max(vs,rs),ra))
def off(rva):
    for va,sz,ra in secs:
        if va<=rva<va+sz: return ra+rva-va
rva,size=struct.unpack_from('<II',d,ddoff+8); at=off(rva)
while True:
    ilt,ts,fc,name,iat=struct.unpack_from('<IIIII',d,at)
    if not (ilt or ts or fc or name or iat): break
    n=off(name); print(d[n:d.index(b'\0',n)].decode()); at+=20
EOF
```

### 2.3 Нет Microsoft VC++ 2015-2022 x64 runtime

Та же codelib импортирует `MSVCP140.dll`, `VCRUNTIME140.dll`, `VCRUNTIME140_1.dll` (и
`api-ms-win-crt-*`). Без x64-рунтайма загрузка модуля не начинается — кнопки нет, лога нет.
`mp_vitals.dll` здесь не помогает: она импортирует только `KERNEL32.dll`.

### 2.4 Windows блокирует загрузку

Mark of the Web на скачанном из интернета `basic_network.codelib`/`mp_vitals.dll`,
SmartScreen, Smart App Control (Windows 11) и антивирусный карантин блокируют загрузку
неподписанного модуля **без какого-либо сообщения в игре**. Внешне это неотличимо от п. 2.2/2.3:
нет кнопки и нет лога.

### 2.5 Неправильное расположение архива

Если `MiusMultiplayer\` распакован не рядом со `starter_x64.exe` (или вложен как
`...\MiusMultiplayer\MiusMultiplayer\`), старый установщик выдавал `RC=3` и ничего не копировал.
Новый установщик ищет `starter_x64.exe` в трёх родительских папках, поэтому вложенность больше не
ломает установку.

### 2.6 Другая версия игры

Даже при загруженном модуле кнопка требует совпадения смещений `controller + 0x1258` и
`+ 0x1200` билда 3598/1 (`ui.cpp:75-79`), а `opgame/tacgame.progpack` собраны из
`programs.flatdata` этого же билда. На другом билде модуль загрузится (лог будет), но entry
не появится. Именно поэтому в логе есть маркер `ui: Direct3D device`: он появляется только когда
кнопка действительно нарисована.

## 3. Как теперь устроена диагностика

`Check_Install.bat` (через `Check_Install.ps1`) теперь только читает и печатает:

* SHA-256 файлов kit и уже установленных файлов (расхождение — не ошибка, а подсказка перезапустить `Enable`);
* **все импорты** `basic_network.codelib` и какие из них не разрешаются (game folder / System32);
* наличие `mp_vitals.dll` в корне игры и наличие `vcruntime140.dll`, `vcruntime140_1.dll`, `msvcp140.dll`;
* содержит ли `root\programs\opgame.progpack` строки `basic_network.codelib` и `i_network`;
* Mark of the Web на установленных модулях и состояние Smart App Control;
* последний `out\basic_network_opgame_*.log`: маркер загрузки `basic_network.codelib loaded`,
  маркер отрисовки `ui: Direct3D device`, строки `vitals:`;
* сколько файлов игры отличается от historical fingerprint `game_build_3598_1.sha256`
  (только информация, ничего не блокирует).

Ключевое правило чтения логов:

| Наблюдение | Вывод |
| --- | --- |
| нет `out\basic_network_opgame_*.log` после запуска игры | Windows не загрузила `basic_network.codelib` → пп. 2.2–2.4 |
| лог есть, `basic_network.codelib loaded` есть, `ui: Direct3D device` нет, `opgame.progpack` отсутствует или его SHA-256 не `eca75b74…9535` | program pack не заменён: модуль загружен, но его render call никто не вызывает → перезапустить `Enable_Multiplayer.bat` |
| лог есть, `basic_network.codelib loaded` есть, `ui: Direct3D device` нет, `opgame.progpack` правильный | модуль загружен, главное меню не распознано → п. 2.6 |
| `ui: Direct3D device` есть, кнопки нет | кнопка нарисована, но её закрывает другой overlay/разрешение окна; это уже не установка |
| `ui: no i_render object` / `ui: no Direct3D device` | модуль не смог получить device у engine (нестандартный renderer/overlay) |

Установщик теперь создаёт `<игра>\out`, чтобы отсутствие папки не маскировало отсутствие лога.

## 4. Что изменено в установщиках

* `Enable_Multiplayer.bat` (v1.0.0 и v1.0.1) больше **не содержит ни одной проверки**, влияющей на
  замену: убраны `:known_network`, `:copy_checked`, `certutil`, проход по
  `game_build_3598_1.sha256`, `if /i "%H%"`, allow-list модулей.
* Каждый файл заменяется безусловно: `attrib -r -s -h` → `del /f /q` → `copy /y`, повтор,
  затем копирование через `*.put` и `move /y` в той же папке (единственный оставшийся отказ —
  физическая невозможность записи, и тогда печатается список `NOT REPLACED`, а остальные файлы
  всё равно заменяются).
* Порядок копирования прежний и безопасный: `mp_vitals.dll` → `basic_network.codelib` → `mp_join.exe`
  → `opgame.progpack` → `tacgame.progpack` → `steam_appid.txt`.
* Backup прежних файлов стал best-effort: его сбой не прерывает установку.
* После копирования снимается Mark of the Web (`Unblock-File`) с kit'а и установленных модулей.
* Установщик пишет `state\installed.txt` — точный список замененных файлов.
* `Disable_Multiplayer.bat` удаляет **по этому списку**, без сравнения hash-ей, и восстанавливает
  сохранённые прежние файлы. Старый hash-режим остался только для установок, сделанных прежними
  архивами (у них нет списка), чтобы не удалять чужие файлы наугад.
* `Check_Install.ps1` переписан в read-only диагностику кнопки (см. §3), ничего не блокирует.
* `MiusMultiplayer_beta_FPS_fix` archive удалён вместе с `scripts/package_fps_beta.py` и
  `development/fps-beta-kit-source/`: его payload входит в v1.0.1, а второй installer с собственным
  codelib приводил к смешанным установкам — то есть к тому же «кнопки нет». В игре, где остался
  beta/withdrawn module, `Enable_Multiplayer.bat` из v1.0.1 заменяет codelib и packs безусловно.
* Legacy developer-скрипты `development/forbidden/development/kit-source/Install_Vitals.bat` и `Uninstall_Vitals.bat`
  (они не входят ни в один player archive) тоже больше не отказывают по unknown hash и не прерываются
  на проверке после copy: модуль заменяется безусловно, сохранённая копия возвращается без сверки hash-а.
* Packaging-скрипты (`scripts/package_*.py`) теперь **запрещают** возврат прежних гейтов:
  если в `Enable_Multiplayer.bat` снова появится `certutil`, `known_network`, `copy_checked`,
  `foreign_network`, `ODD`, `game_build_3598_1.sha256` или `SHA-256 verification failed`,
  сборка архива падает.

## 5. Что остаётся непреодолимым ограничением

* Другой build игры: смещения главного меню в codelib и progpack'и собраны для 3598/1. Это
  проверяется логом (`basic_network.codelib loaded` без `ui: Direct3D device`).
* Отсутствующий VC++ x64 runtime или заблокированный Windows модуль: установщик может только
  диагностировать это (`Check_Install.bat`) и снять Mark of the Web; ставить runtime и менять
  политику Smart App Control он не должен.
* Реальный two-peer прогон Steam в Linux sandbox не выполняется: см. `README.md`.

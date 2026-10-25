MiusMultiplayer v1.0.1 — Steam + FPS + Vitals unified install
===============================================================

Это один install-kit для Steam Graviteam Tactics: Mius-Front 6.00 build 3598/1.
Он объединяет все текущие runtime fixes в ОДИН устанавливаемый archive:

  * Steam-preserving FPS basic_network.codelib;
  * mp_vitals.dll с проверенными Vitals и operation-tactical policy fixes;
  * opgame/tacgame program packs, mp_join.exe и steam_appid.txt.

Не нужно отдельно ставить старый MiusMultiplayer v1.0.0, beta_FPS_fix,
mpmod1 или MiusVitals. Установщик всегда заменяет все шесть runtime files,
включая module, оставшийся от другого мода или другого выпуска.

ЧТО ИМЕННО ВНУТРИ
-----------------

basic_network.codelib SHA-256:

  7b42a69abe58deb1583d2f5320eebbecd9718ffa27ea08933613e67df1901f67

Он начинается с проверенного Steam-capable 3598/1 module и добавляет
исполняемый .fpsfix trampoline: session_sim_step не вызывается, если ctrl_tick
сообщает zero-ms simulation step. Для любого non-zero step вызывается исходный
path. Это сохраняет Steam Friends, rich presence, Join Game callback и
SteamNetworking P2P, включая MP_STEAM_PEER, SendP2PPacket, ReadP2PPacket и
JoinRequested. Это реальное модифицированное codelib, а не неизменённый stable
binary, и не public-source rebuild без Steam layer.

mp_vitals.dll, program packs и launcher собраны из текущего проверенного unified
kit. Vitals import/signature surface retained by the FPS module; in operation
tactical battle the existing absent-only MP_SYNC_STATE=0 policy remains active.

Source-only overlay `0002-ordered-operation-turns-and-network-hygiene.patch` не
входит в этот player archive: ему всё ещё нужны отдельные Windows/two-peer map
validation и matching launcher before runtime release.

УСТАНОВКА
---------
1. Полностью закройте игру на ОБОИХ ПК.
2. Распакуйте этот архив в папку, где лежит starter_x64.exe. Должно получиться:

     ...\Graviteam Tactics Mius-Front\MiusMultiplayer\Enable_Multiplayer.bat

   Распаковка на один уровень глубже тоже распознаётся: установщик ищет
   starter_x64.exe в трёх родительских папках.
3. Запустите сначала Check_Install.bat, затем Enable_Multiplayer.bat.
4. Второй игрок должен поставить этот же v1.0.1 archive.
5. Перед campaign/FPS comparison проведите Steam two-PC regression в обеих
   host directions: rich-presence host -> friend Join Game -> P2P lobby ->
   tactical session -> reconnect/exit.

ЗАМЕНА ФАЙЛОВ — БЕЗ ПРОВЕРОК
----------------------------
Enable_Multiplayer.bat больше не принимает решения по hash-ам. Для каждого
runtime file он снимает read-only, удаляет старый файл, копирует новый, при
неудаче повторяет попытку и копирует через временное имя в той же папке.
Ни historical game hash list, ни allow-list известных модулей, ни certutil, ни
проверка SHA-256 после copy не могут остановить замену или прервать установку.
Если один файл физически не записывается (игра запущена, папка read-only), это
печатается в конце списком NOT REPLACED, остальные файлы всё равно заменяются.
Повторный запуск Enable_Multiplayer.bat безопасен.

Перед заменой любой прежний файл с тем же именем один раз сохраняется в
MiusMultiplayer\state\backup. Сбой этого backup-а не блокирует установку.
После copy снимается Mark of the Web (Unblock-File), чтобы SmartScreen,
Smart App Control или application allow-list не отказывали в загрузке модуля.

ПОЧЕМУ МОЖЕТ НЕ БЫТЬ КНОПКИ MULTIPLAYER
---------------------------------------
Кнопку рисует сам basic_network.codelib внутри главного меню, поэтому она есть
только если Windows загрузила этот module и engine создал объект i_network.

  1. Module не загрузился, если отсутствует или не загружается mp_vitals.dll:
     basic_network.codelib имеет ЖЁСТКИЙ импорт mp_vitals.dll!MpVitalsInit.
     DLL должна лежать в корне игры рядом со starter_x64.exe (Windows ищет
     зависимости по папке exe, а не по bin\rel\x64).
  2. Module не загрузится без Microsoft Visual C++ 2015-2022 x64 runtime
     (vcruntime140.dll, vcruntime140_1.dll, msvcp140.dll).
  3. Загрузку могут молча заблокировать Smart App Control, SmartScreen,
     антивирус или Mark of the Web на скачанном файле.
  4. root\programs\opgame.progpack должен содержать basic_network.codelib:
     именно этот loose pack заставляет engine создать i_network в меню.
  5. Другая версия игры: кнопка и переходы в stock-экраны рассчитаны на
     Steam 6.00 build 3598/1.

Диагностика: Check_Install.bat (только чтение, ничего не меняет и не блокирует)
печатает разрешаются ли ВСЕ импорты module, есть ли mp_vitals.dll и VC++
runtime, содержит ли opgame.progpack нужную строку, есть ли Mark of the Web,
состояние Smart App Control и последний basic_network_opgame log.

Ключевой признак: модуль пишет log в <игра>\out\basic_network_opgame_<pid>.log
при загрузке. Если после запуска игры такого log-а НЕТ, значит Windows не
загрузила basic_network.codelib; если log есть, но в нём нет строки
"ui: Direct3D device", значит module загружен, но главное меню не распознано
(другая версия игры). Строка "ui: Direct3D device" подтверждает, что кнопка
действительно нарисована.

УДАЛЕНИЕ И ОТКАТ
----------------
Закройте игру и запустите MiusMultiplayer\Disable_Multiplayer.bat. Он удаляет
ровно тот список файлов, который записал этот установщик
(MiusMultiplayer\state\installed.txt), без сравнения hash-ей, и возвращает
сохранённые прежние файлы. Для возврата на старый v1.0.0 затем распакуйте и
установите его kit заново. Multiplayer operation saves в users\mp_saves не
удаляются.

ГРАНИЦА ТЕСТИРОВАНИЯ
--------------------
Этот archive проходит deterministic PE/package/native checks. Linux sandbox не
может выполнить Steam client two-peer gameplay; до live regression нельзя
выдавать Steam friends, reconnect или FPS за gameplay-certified результат.

ЛИЦЕНЗИЯ И ИСТОЧНИК
-------------------
Публичный MiusPlayer source supplied with this project is CC0; LICENSE_CC0.txt
is included. Player archive contains only runtime files and installers; the
full source workbench is in repository development/.

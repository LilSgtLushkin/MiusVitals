MiusMultiplayer v1.0.0 — единый мод для Mius-Front multiplayer
=================================================================

Это ОДИН install-kit для Steam Graviteam Tactics: Mius-Front 6.00 build 3598/1.
Он уже содержит:

  * multiplayer basic_network.codelib (MiusPlayer v20 base + проверенные MiusVitals fixes);
  * mp_vitals.dll, который загружается вместе с multiplayer module;
  * opgame/tacgame program packs, mp_join.exe и steam_appid.txt.

Не нужно отдельно ставить mpmod1 и MiusVitals. Не копируйте development/,
старые MiusVitals archives или public source в папку игры.

УСТАНОВКА
---------
1. Полностью закройте игру.
2. Распакуйте этот архив в папку, где лежит starter_x64.exe. Должен получиться:

     ...\Graviteam Tactics Mius-Front\MiusMultiplayer\Enable_Multiplayer.bat

   Распаковка на один уровень глубже тоже распознаётся: установщик ищет
   starter_x64.exe в трёх родительских папках.
3. Запустите сначала Check_Install.bat, затем Enable_Multiplayer.bat.
4. Второй игрок должен поставить этот же архив и также запустить Enable_Multiplayer.bat.

ЗАМЕНА ФАЙЛОВ — БЕЗ ПРОВЕРОК
----------------------------
Enable_Multiplayer.bat не принимает решений по hash-ам: он заменяет все шесть
runtime files всегда, независимо от того, какой модуль или program pack лежал
в игре раньше. Для каждого файла снимается read-only, старый файл удаляется,
новый копируется; при неудаче попытка повторяется и используется временное имя.
Ни historical game hash list, ни allow-list известных модулей, ни certutil, ни
проверка SHA-256 после copy не могут отменить замену или оборвать установку.

Если файл физически не записывается (игра запущена, папка read-only), установщик
печатает список NOT REPLACED в конце, но остальные файлы всё равно заменяет.
Повторный запуск безопасен. Прежние файлы с теми же именами один раз
сохраняются в MiusMultiplayer\state\backup; сбой backup-а установку не
блокирует. После copy снимается Mark of the Web (Unblock-File).

ПОЧЕМУ МОЖЕТ НЕ БЫТЬ КНОПКИ MULTIPLAYER
---------------------------------------
Кнопку рисует сам basic_network.codelib в главном меню. Она появляется только
если Windows загрузила module и engine создал объект i_network. Основные причины:

  1. Нет mp_vitals.dll рядом со starter_x64.exe: у basic_network.codelib
     жёсткий импорт mp_vitals.dll!MpVitalsInit, без него module не загружается.
  2. Нет Microsoft Visual C++ 2015-2022 x64 runtime (vcruntime140.dll,
     vcruntime140_1.dll, msvcp140.dll) — module тоже не загрузится.
  3. Smart App Control / SmartScreen / антивирус / Mark of the Web заблокировали
     загрузку скачанного module.
  4. root\programs\opgame.progpack отсутствует или не содержит
     basic_network.codelib — тогда engine не создаёт i_network.
  5. Другая версия игры (нужен Steam 6.00 build 3598/1).

Check_Install.bat только читает и печатает: разрешаются ли все импорты module,
есть ли DLL и VC++ runtime, содержит ли opgame.progpack нужную строку, есть ли
Mark of the Web, состояние Smart App Control и последний log.

Главный признак: module пишет <игра>\out\basic_network_opgame_<pid>.log при
загрузке. Нет log-а после запуска игры — Windows не загрузила
basic_network.codelib. Log есть, но нет строки "ui: Direct3D device" — module
загружен, но главное меню не распознано (другая версия игры). В обоих
basic_network_*.log должна появиться строка "vitals:".

В operation tactical battle после старта ожидается:

  vitals: tactical operation detected; full object-state replication is disabled
  (native position, shots, hits and Vitals remain)

Это возвращает предусмотренный MiusPlayer position-only mode, если MP_SYNC_STATE
не был задан явно. Transforms, shots, hits и Vitals не отключаются. Skirmish,
custom mission, opgame и explicit MP_SYNC_STATE не меняются.

УДАЛЕНИЕ
--------
Закройте игру и запустите MiusMultiplayer\Disable_Multiplayer.bat. Он удаляет
ровно тот список файлов, который записал установщик
(MiusMultiplayer\state\installed.txt), без сравнения hash-ей, возвращает
сохранённые прежние файлы и удаляет steam_appid.txt только если kit создал его
сам. Multiplayer operation saves в users\mp_saves не удаляются.

ГРАНИЦА ТЕСТИРОВАНИЯ
--------------------
Global-map synchronization в этой версии не менялась. Перед заявлением
исправления tactical FPS необходим реальный two-peer operational battle: после
object-number exchange не должно быть saved-object state traffic, а transforms,
shots и Vitals должны работать на обеих сторонах.

ЛИЦЕНЗИЯ И ИСТОЧНИК
-------------------
Публичный MiusPlayer source supplied with this project is CC0; LICENSE_CC0.txt
is included. The player archive contains only runtime files and installers;
the full source workbench is in the repository development/ directory.

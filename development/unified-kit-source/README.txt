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
2. Если раньше были mpmod1 и MiusVitals, удалите ТОЛЬКО их папки из корня игры.
   Не удаляйте вручную bin\rel\x64\basic_network.codelib или root\programs files:
   Enable_Multiplayer.bat распознаёт известные старые версии и безопасно заменяет их.
3. Распакуйте этот архив в папку, где лежит starter_x64.exe. Должен получиться:

     ...\Graviteam Tactics Mius-Front\MiusMultiplayer\Enable_Multiplayer.bat

4. Запустите сначала Check_Install.bat, затем Enable_Multiplayer.bat.
5. Второй игрок должен поставить этот же архив и также запустить Enable_Multiplayer.bat.

Установщик проверяет Steam build 3598/1, не перезаписывает неизвестный active
basic_network.codelib и копирует DLL ДО module, который её импортирует. После
копирования проверяются SHA-256 всех шести runtime files.

В обоих basic_network_*.log должна появиться строка "vitals:". В operation
tactical battle после старта ожидается:

  vitals: tactical operation detected; full object-state replication is disabled
  (native position, shots, hits and Vitals remain)

Это возвращает предусмотренный MiusPlayer position-only mode, если MP_SYNC_STATE
не был задан явно. Transforms, shots, hits и Vitals не отключаются. Skirmish,
custom mission, opgame и explicit MP_SYNC_STATE не меняются.

УДАЛЕНИЕ
--------
Закройте игру и запустите MiusMultiplayer\Disable_Multiplayer.bat. Он удаляет
только файлы точного установленного unified kit, возвращает сохранённые ранее
foreign program/utility files и удаляет steam_appid.txt только если kit создал
его сам. Multiplayer operation saves в users\mp_saves не удаляются.

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

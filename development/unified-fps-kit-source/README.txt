MiusMultiplayer v1.0.1 — Steam + FPS + Vitals unified install
===============================================================

Это один install-kit для Steam Graviteam Tactics: Mius-Front 6.00 build 3598/1.
Он объединяет все текущие runtime fixes в ОДИН устанавливаемый archive:

  * Steam-preserving FPS basic_network.codelib;
  * mp_vitals.dll с проверенными Vitals и operation-tactical policy fixes;
  * opgame/tacgame program packs, mp_join.exe и steam_appid.txt.

Не нужно отдельно ставить старый MiusMultiplayer v1.0.0, beta_FPS_fix,
mpmod1 или MiusVitals. Этот kit заменяет известный v1.0.0 module, withdrawn
source beta и текущий Steam-preserving beta без перезаписи unknown module.

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

   При распаковке поверх v1.0.0 или beta_FPS_fix замените scripts в той же
   MiusMultiplayer folder целиком.
3. Запустите сначала Check_Install.bat, затем Enable_Multiplayer.bat.
4. Второй игрок должен поставить этот же v1.0.1 archive.
5. Перед campaign/FPS comparison проведите Steam two-PC regression в обеих
   host directions: rich-presence host -> friend Join Game -> P2P lobby ->
   tactical session -> reconnect/exit.

Установщик сверяет hash payload после copy, принимает только known old modules
и сохраняет foreign program/utility files. Historical Steam game hash list
остаётся diagnostic fingerprint, а exact runtime signatures защищают hooks.

УДАЛЕНИЕ И ОТКАТ
----------------
Закройте игру и запустите MiusMultiplayer\Disable_Multiplayer.bat. Он удаляет
только exact v1.0.1 runtime files и восстанавливает сохранённые foreign
program/utility files. Для возврата на старый v1.0.0 затем распакуйте и
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

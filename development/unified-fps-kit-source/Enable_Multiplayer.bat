@echo off
rem MiusMultiplayer v1.0.1: one unconditional MiusPlayer + MiusVitals install for Steam 3598/1.
rem Every runtime file is force-replaced. No file hash, game-build fingerprint, module allow-list
rem or copy verification can stop a replacement or abort the install half-way.
setlocal EnableExtensions DisableDelayedExpansion
set "MOD=%~dp0"
set "MOD=%MOD:~0,-1%"
set "QUIET="
if /i "%~1"=="/quiet" set "QUIET=1"
set "RC=0"
set "FAILN=0"
set "FAILED="
set "NETWORK=7B42A69ABE58DEB1583D2F5320EEBBECD9718FFA27EA08933613E67DF1901F67"
set "VITALS=71A1BF3A37923BBE6C8EE84448D0B91705C2BFF7382B647F990B9D671EE224DF"
set "JOIN=6AA417CF6E1F01480C997446F2691A58AB55B90FA36E1FF511945989AADFEEAA"
set "OPGAME=ECA75B740F7B11D9E13F2E2001F1B2B133427D0860FB3770220B89B0B8909535"
set "TACGAME=01923EABDF6A9D5FA37DC70A3D76740242DBC9A7F26C0298CA3FAA8CCA87600D"
set "APPID=1A43840CA36F1401C85FF5F6E6981088927CFE09B23D155A9C4301DD13CA9610"

echo.
echo  MiusMultiplayer v1.0.1 - unified multiplayer and Vitals install
echo.
rem ---- the game folder: the nearest parent with starter_x64.exe
rem (so an archive unzipped one level too deep still installs into the right game)
set "GAME="
for %%I in ("%MOD%\..") do if exist "%%~fI\starter_x64.exe" set "GAME=%%~fI"
if defined GAME goto :have_game
for %%I in ("%MOD%\..\..") do if exist "%%~fI\starter_x64.exe" set "GAME=%%~fI"
if defined GAME goto :have_game
for %%I in ("%MOD%\..\..\..") do if exist "%%~fI\starter_x64.exe" set "GAME=%%~fI"
if not defined GAME goto :no_game
:have_game
echo  Game: %GAME%
if not exist "%MOD%\files\bin\rel\x64\basic_network.codelib" goto :incomplete
if not exist "%MOD%\files\mp_vitals.dll" goto :incomplete
if not exist "%MOD%\files\root\programs\opgame.progpack" goto :incomplete
if not exist "%MOD%\files\root\programs\tacgame.progpack" goto :incomplete
if not exist "%MOD%\files\steam_appid.txt" goto :incomplete
tasklist /fi "imagename eq starter_x64.exe" /nh 2>nul | find /i "starter_x64.exe" >nul && goto :running

if not exist "%MOD%\state" mkdir "%MOD%\state" >nul 2>nul
if not exist "%GAME%\out" mkdir "%GAME%\out" >nul 2>nul
if not exist "%MOD%\state" echo  Note: MiusMultiplayer\state cannot be created. Files are still replaced, but uninstall will have no removal record.
if not exist "%MOD%\state\steam_appid_added.txt" if not exist "%GAME%\steam_appid.txt" >"%MOD%\state\steam_appid_added.txt" echo MiusMultiplayer created steam_appid.txt.
if exist "%MOD%\state\installed.txt" attrib -r -s -h "%MOD%\state\installed.txt" >nul 2>nul
type nul >"%MOD%\state\installed.txt"

rem ---- keep whatever occupied these names before, so Disable_Multiplayer.bat can put it back.
rem Best effort only: a failed backup never blocks a replacement.
call :keep "root\programs\opgame.progpack"
call :keep "root\programs\tacgame.progpack"
call :keep "mp_join.exe"
call :keep "mp_vitals.dll"
call :keep "steam_appid.txt"
call :keep "bin\rel\x64\basic_network.codelib"

rem ---- force replacement. The DLL and the module are written before the program packs that make
rem the engine load them, so even an interrupted run never leaves a pack without its module.
call :put "%MOD%\files\mp_vitals.dll" "%GAME%\mp_vitals.dll" "mp_vitals.dll"
call :put "%MOD%\files\bin\rel\x64\basic_network.codelib" "%GAME%\bin\rel\x64\basic_network.codelib" "bin\rel\x64\basic_network.codelib"
call :put "%MOD%\files\mp_join.exe" "%GAME%\mp_join.exe" "mp_join.exe"
call :put "%MOD%\files\root\programs\opgame.progpack" "%GAME%\root\programs\opgame.progpack" "root\programs\opgame.progpack"
call :put "%MOD%\files\root\programs\tacgame.progpack" "%GAME%\root\programs\tacgame.progpack" "root\programs\tacgame.progpack"
call :put "%MOD%\files\steam_appid.txt" "%GAME%\steam_appid.txt" "steam_appid.txt"

rem ---- Windows must not hold an internet mark on the modules it is asked to load
call :unblock

if not "%FAILN%"=="0" goto :partial
>"%MOD%\state\enabled.txt" echo MiusMultiplayer v1.0.1 is enabled in this game.
echo  All files replaced:
echo    bin\rel\x64\basic_network.codelib  %NETWORK%
echo    mp_vitals.dll                     %VITALS%
echo    root\programs\opgame.progpack     %OPGAME%
echo    root\programs\tacgame.progpack    %TACGAME%
echo    mp_join.exe                       %JOIN%
echo    steam_appid.txt                   %APPID%
echo.
rem ---- basic_network.codelib has a hard import on mp_vitals.dll!MpVitalsInit. Without that DLL, or
rem without the VC++ x64 runtime, Windows does not load the module at all: the engine then never creates
rem i_network and the main menu has no MULTIPLAYER entry. Report these preconditions.
if not exist "%GAME%\mp_vitals.dll" echo  WARNING: mp_vitals.dll is missing; the multiplayer module cannot load.
if not exist "%SystemRoot%\System32\vcruntime140.dll" echo  WARNING: the Microsoft VC++ 2015-2022 x64 runtime is missing;
if not exist "%SystemRoot%\System32\vcruntime140.dll" echo           the multiplayer module cannot load without vcruntime140.dll.
echo  Start the game through Steam. The main menu gets the MULTIPLAYER entry at the top.
echo  If it does not appear, run Check_Install.bat: it reports which load precondition is missing.
echo  Both players need this same archive. Check both basic_network_*.log files for "vitals:".
echo  Back to the original game: Disable_Multiplayer.bat
goto :finish

rem ---- %1 = source in the kit, %2 = destination in the game, %3 = relative name for messages
:put
if not exist "%~1" (
    echo  MISSING IN KIT: %~3
    set "FAILED=%FAILED% %~3"
    set /a FAILN+=1
    exit /b 1
)
for %%I in ("%~2") do if not exist "%%~dpI" mkdir "%%~dpI" >nul 2>nul
call :overwrite "%~1" "%~2"
if not errorlevel 1 goto :put_ok
rem a second attempt clears a stale read-only or sharing state that has just gone away
call :overwrite "%~1" "%~2"
if not errorlevel 1 goto :put_ok
rem last resort: write a new file next to it and move that over the old name
copy /y "%~1" "%~2.put" >nul 2>nul
if not exist "%~2.put" goto :put_failed
attrib -r -s -h "%~2" >nul 2>nul
del /f /q "%~2" >nul 2>nul
move /y "%~2.put" "%~2" >nul 2>nul
:put_failed
if not exist "%~2" goto :put_error
:put_ok
>>"%MOD%\state\installed.txt" echo %~3
exit /b 0
:put_error
echo  COULD NOT WRITE %~2
set "FAILED=%FAILED% %~3"
set /a FAILN+=1
exit /b 1

:overwrite
attrib -r -s -h "%~2" >nul 2>nul
del /f /q "%~2" >nul 2>nul
copy /y "%~1" "%~2" >nul 2>nul
if errorlevel 1 exit /b 1
if not exist "%~2" exit /b 1
exit /b 0

rem ---- save an existing file of somebody else once, so Disable_Multiplayer.bat can put it back
:keep
if not exist "%GAME%\%~1" exit /b 0
if exist "%MOD%\state\backup\%~1" exit /b 0
for %%I in ("%MOD%\state\backup\%~1") do if not exist "%%~dpI" mkdir "%%~dpI" >nul 2>nul
fc /b "%MOD%\files\%~1" "%GAME%\%~1" >nul 2>nul && exit /b 0
copy /y "%GAME%\%~1" "%MOD%\state\backup\%~1" >nul 2>nul
if exist "%MOD%\state\backup\%~1" echo    saved the existing %~1 in MiusMultiplayer\state\backup
exit /b 0

rem ---- remove the Mark of the Web from the kit and the installed modules, so that SmartScreen,
rem Smart App Control or an application allow-list cannot silently refuse to load them
:unblock
where powershell >nul 2>nul
if errorlevel 1 exit /b 0
set "MP_UNBLOCK_MOD=%MOD%"
set "MP_UNBLOCK_FILES=%GAME%\mp_vitals.dll|%GAME%\bin\rel\x64\basic_network.codelib|%GAME%\root\programs\opgame.progpack|%GAME%\root\programs\tacgame.progpack|%GAME%\mp_join.exe|%GAME%\steam_appid.txt"
powershell -NoProfile -ExecutionPolicy Bypass -Command "$m=$env:MP_UNBLOCK_MOD; if($m){Get-ChildItem -LiteralPath $m -Recurse -File -ErrorAction SilentlyContinue | Unblock-File -ErrorAction SilentlyContinue}; foreach($f in ($env:MP_UNBLOCK_FILES -split '\|')){if($f){Unblock-File -LiteralPath $f -ErrorAction SilentlyContinue}}" >nul 2>nul
exit /b 0

:partial
echo.
echo  NOT REPLACED:%FAILED%
echo  These files keep their old content. Close the game, check write access to the game folder
echo  and run this file again. Nothing else was rolled back.
set "RC=1"
goto :finish
:no_game
echo  This folder is not inside the game folder. Put the whole MiusMultiplayer folder next to starter_x64.exe.
echo  (Steam: right-click the game, Manage, Browse local files.)
set "RC=3"
goto :finish
:incomplete
echo  The MiusMultiplayer folder is incomplete. Extract the whole archive again.
set "RC=1"
goto :finish
:running
echo  The game is running. Close it, then run this again.
set "RC=4"
:finish
echo.
if not defined QUIET pause
exit /b %RC%

@echo off
rem Removes only the exact MiusMultiplayer v1.0.0 files and restores any foreign files saved at install.
setlocal EnableExtensions DisableDelayedExpansion
set "MOD=%~dp0"
set "MOD=%MOD:~0,-1%"
for %%I in ("%MOD%\..") do set "GAME=%%~fI"
set "QUIET="
if /i "%~1"=="/quiet" set "QUIET=1"
set "RC=0"
set "NETWORK=485B655662BD062E16278F9D699A3097B49A08DE1948D2500DD589703A3102A3"
set "VITALS=71A1BF3A37923BBE6C8EE84448D0B91705C2BFF7382B647F990B9D671EE224DF"
set "JOIN=6AA417CF6E1F01480C997446F2691A58AB55B90FA36E1FF511945989AADFEEAA"
set "OPGAME=ECA75B740F7B11D9E13F2E2001F1B2B133427D0860FB3770220B89B0B8909535"
set "TACGAME=01923EABDF6A9D5FA37DC70A3D76740242DBC9A7F26C0298CA3FAA8CCA87600D"
set "APPID=1A43840CA36F1401C85FF5F6E6981088927CFE09B23D155A9C4301DD13CA9610"

echo.
echo  MiusMultiplayer v1.0.0 - remove unified multiplayer
if not exist "%GAME%\starter_x64.exe" goto :no_game
tasklist /fi "imagename eq starter_x64.exe" /nh 2>nul | find /i "starter_x64.exe" >nul && goto :running

set "HAD="
if exist "%MOD%\state\enabled.txt" set "HAD=1"
call :is_ours "bin\rel\x64\basic_network.codelib" "%NETWORK%" && set "HAD=1"
call :is_ours "mp_vitals.dll" "%VITALS%" && set "HAD=1"
call :is_ours "root\programs\tacgame.progpack" "%TACGAME%" && set "HAD=1"
if not defined HAD goto :nothing

call :remove_ours "root\programs\tacgame.progpack" "%TACGAME%" || goto :failed
call :remove_ours "root\programs\opgame.progpack" "%OPGAME%" || goto :failed
call :remove_ours "bin\rel\x64\basic_network.codelib" "%NETWORK%" || goto :failed
call :remove_ours "mp_vitals.dll" "%VITALS%" || goto :failed
call :remove_ours "mp_join.exe" "%JOIN%" || goto :failed
if exist "%MOD%\state\steam_appid_added.txt" (
    call :remove_ours "steam_appid.txt" "%APPID%" || goto :failed
)
if exist "%GAME%\users\mp_last_host.txt" del /f /q "%GAME%\users\mp_last_host.txt" >nul 2>nul
call :restore "root\programs\opgame.progpack" || goto :failed
call :restore "root\programs\tacgame.progpack" || goto :failed
call :restore "mp_join.exe" || goto :failed
call :restore "mp_vitals.dll" || goto :failed
if exist "%MOD%\state\steam_appid_added.txt" del /f /q "%MOD%\state\steam_appid_added.txt"
if exist "%MOD%\state\enabled.txt" del /f /q "%MOD%\state\enabled.txt"
rmdir "%GAME%\root\programs" 2>nul
rmdir "%MOD%\state\backup\root\programs" 2>nul
rmdir "%MOD%\state\backup\root" 2>nul
rmdir "%MOD%\state\backup" 2>nul
rmdir "%MOD%\state" 2>nul

echo  MiusMultiplayer was removed. The game is back to stock unless a saved earlier file was restored.
if exist "%GAME%\users\mp_saves\" echo  Multiplayer operation saves in users\mp_saves were kept.
goto :finish

:is_ours
if not exist "%GAME%\%~1" exit /b 1
set "H="
for /f "skip=1 delims=" %%X in ('certutil -hashfile "%GAME%\%~1" SHA256 2^>nul') do if not defined H set "H=%%X"
if defined H set "H=%H: =%"
if /i "%H%"=="%~2" exit /b 0
exit /b 1

:remove_ours
if not exist "%GAME%\%~1" exit /b 0
call :is_ours "%~1" "%~2" && goto :delete
for %%I in ("%GAME%\%~1") do echo    left changed file: %%~nxI
echo    (it no longer has this kit's expected SHA-256)
exit /b 0
:delete
del /f /q "%GAME%\%~1" >nul 2>nul
if exist "%GAME%\%~1" exit /b 1
exit /b 0

:restore
if not exist "%MOD%\state\backup\%~1" exit /b 0
if exist "%GAME%\%~1" (
    echo    kept saved backup for %~1 because a newer file is present
    exit /b 0
)
for %%I in ("%GAME%\%~1") do if not exist "%%~dpI" mkdir "%%~dpI" || exit /b 1
move /y "%MOD%\state\backup\%~1" "%GAME%\%~1" >nul || exit /b 1
echo    restored saved %~1
exit /b 0

:nothing
echo  No exact MiusMultiplayer files are installed in this game: nothing was changed.
goto :finish
:no_game
echo  This folder is not inside a game folder (starter_x64.exe is absent next to MiusMultiplayer).
set "RC=3"
goto :finish
:running
echo  The game is running. Close it, then run this again.
set "RC=4"
goto :finish
:failed
echo  A file could not be removed or restored. Close the game and run this again.
set "RC=1"
:finish
echo.
if not defined QUIET pause
exit /b %RC%

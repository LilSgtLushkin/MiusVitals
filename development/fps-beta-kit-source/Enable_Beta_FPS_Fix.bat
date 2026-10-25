@echo off
rem MiusMultiplayer beta_FPS_fix: Steam-preserving zero-ms simulation-step candidate for Steam 6.00 build 3598/1.
rem It force-replaces the active codelib and keeps a restorable copy of whatever was there before.
rem No hash, allow-list or copy verification can stop the replacement.
setlocal EnableExtensions DisableDelayedExpansion
set "MOD=%~dp0"
set "MOD=%MOD:~0,-1%"
set "QUIET="
if /i "%~1"=="/quiet" set "QUIET=1"
set "RC=0"
set "CANDIDATE=7B42A69ABE58DEB1583D2F5320EEBBECD9718FFA27EA08933613E67DF1901F67"

echo.
echo  MiusMultiplayer beta_FPS_fix - Steam-preserving experimental FPS candidate
echo.
set "GAME="
for %%I in ("%MOD%\..") do if exist "%%~fI\starter_x64.exe" set "GAME=%%~fI"
if defined GAME goto :have_game
for %%I in ("%MOD%\..\..") do if exist "%%~fI\starter_x64.exe" set "GAME=%%~fI"
if defined GAME goto :have_game
for %%I in ("%MOD%\..\..\..") do if exist "%%~fI\starter_x64.exe" set "GAME=%%~fI"
if not defined GAME goto :no_game
:have_game
set "ACTIVE=%GAME%\bin\rel\x64\basic_network.codelib"
set "BACKUP=%MOD%\state\backup\bin\rel\x64\basic_network.codelib"
echo  Game: %GAME%
if not exist "%MOD%\files\bin\rel\x64\basic_network.codelib" goto :incomplete
tasklist /fi "imagename eq starter_x64.exe" /nh 2>nul | find /i "starter_x64.exe" >nul && goto :running

if not exist "%MOD%\state" mkdir "%MOD%\state" >nul 2>nul
if not exist "%MOD%\state\backup\bin\rel\x64" mkdir "%MOD%\state\backup\bin\rel\x64" >nul 2>nul
rem ---- keep the module that is active now, whatever it is; a failed backup never blocks the swap
if not exist "%ACTIVE%" goto :replace
if exist "%BACKUP%" goto :replace
copy /y "%ACTIVE%" "%BACKUP%" >nul 2>nul
if exist "%BACKUP%" echo    saved the previous basic_network.codelib in beta_FPS_fix\state\backup
:replace
if not exist "%GAME%\out" mkdir "%GAME%\out" >nul 2>nul
attrib -r -s -h "%ACTIVE%" >nul 2>nul
del /f /q "%ACTIVE%" >nul 2>nul
copy /y "%MOD%\files\bin\rel\x64\basic_network.codelib" "%ACTIVE%" >nul 2>nul
if not errorlevel 1 if exist "%ACTIVE%" goto :copied
call :temp_copy
if not errorlevel 1 goto :copied
goto :failed

:copied
>"%MOD%\state\installed.txt" echo bin\rel\x64\basic_network.codelib
>"%MOD%\state\enabled.txt" echo beta_FPS_fix candidate enabled.
call :unblock
echo  Replaced basic_network.codelib with the beta candidate:
echo    %CANDIDATE%
echo.
echo  This is an experimental Steam-preserving FPS candidate. Both players must use this same beta archive.
echo  Steam friends, rich-presence Join Game, and Steam P2P code are retained from the verified base module.
echo  The previous codelib, including an unrecognised one, is saved in beta_FPS_fix\state\backup.
echo  Run Disable_Beta_FPS_Fix.bat before returning to stable MiusMultiplayer.
echo  The released Vitals sidecar import/signature surface is retained, but record FPS and both basic_network logs.
echo  Steam friends joining must be regression-tested on two PCs before any compatibility claim.
goto :finish

rem ---- write a new file next to the old name and move it over, in case only that name is locked
:temp_copy
copy /y "%MOD%\files\bin\rel\x64\basic_network.codelib" "%ACTIVE%.put" >nul 2>nul
if not exist "%ACTIVE%.put" exit /b 1
attrib -r -s -h "%ACTIVE%" >nul 2>nul
del /f /q "%ACTIVE%" >nul 2>nul
move /y "%ACTIVE%.put" "%ACTIVE%" >nul 2>nul
if not exist "%ACTIVE%" exit /b 1
exit /b 0

:unblock
where powershell >nul 2>nul
if errorlevel 1 exit /b 0
set "MP_UNBLOCK_GAME=%GAME%"
powershell -NoProfile -ExecutionPolicy Bypass -Command "$g=$env:MP_UNBLOCK_GAME; if($g){Unblock-File -LiteralPath (Join-Path $g 'bin\rel\x64\basic_network.codelib') -ErrorAction SilentlyContinue}" >nul 2>nul
exit /b 0

:no_game
echo  Put the whole MiusMultiplayer_beta_FPS_fix folder next to starter_x64.exe.
set "RC=3"
goto :finish
:incomplete
echo  The beta folder is incomplete. Extract the whole beta archive again.
set "RC=1"
goto :finish
:running
echo  The game is running. Close it, then run this again.
set "RC=4"
goto :finish
:failed
echo  COULD NOT WRITE %ACTIVE%
echo  Close the game, check write access to the game folder and run this file again.
set "RC=1"
:finish
echo.
if not defined QUIET pause
exit /b %RC%

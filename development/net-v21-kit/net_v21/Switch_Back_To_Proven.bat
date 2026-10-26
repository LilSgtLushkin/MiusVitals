@echo off
rem Put the proven module back. The archive always carries it, so this works even if the game folder
rem was reinstalled, moved, or the state folder was deleted.
setlocal EnableExtensions DisableDelayedExpansion
set "HERE=%~dp0"
set "HERE=%HERE:~0,-1%"
set "MOD=%HERE%\.."
for %%I in ("%MOD%") do set "MOD=%%~fI"

set "GAME="
for %%I in ("%MOD%\..") do if exist "%%~fI\starter_x64.exe" set "GAME=%%~fI"
if defined GAME goto :have_game
for %%I in ("%MOD%\..\..") do if exist "%%~fI\starter_x64.exe" set "GAME=%%~fI"
if defined GAME goto :have_game
for %%I in ("%MOD%\..\..\..") do if exist "%%~fI\starter_x64.exe" set "GAME=%%~fI"
if not defined GAME goto :no_game
:have_game

echo.
echo  net_v21: putting the proven module back
echo  Game: %GAME%
echo.

if not exist "%HERE%\basic_network.proven.codelib" goto :no_payload
tasklist /fi "imagename eq starter_x64.exe" /nh 2>nul | find /i "starter_x64.exe" >nul && goto :running

copy /y "%HERE%\basic_network.proven.codelib" "%GAME%\bin\rel\x64\basic_network.put" >nul
if errorlevel 1 goto :failed
move /y "%GAME%\bin\rel\x64\basic_network.put" "%GAME%\bin\rel\x64\basic_network.codelib" >nul
if errorlevel 1 goto :failed
powershell -NoProfile -Command "Unblock-File -LiteralPath '%GAME%\bin\rel\x64\basic_network.codelib'" >nul 2>nul

echo  Done: v1.0.1 is back, with the Vitals DLL and the FPS fix in it.
echo.
pause
exit /b 0

:no_game
echo  net_v21: the game folder was not found.
pause
exit /b 1

:no_payload
echo  net_v21: basic_network.proven.codelib is missing - unpack the archive again.
pause
exit /b 1

:running
echo  net_v21: the game is running. Close it and run this script again.
pause
exit /b 1

:failed
echo  net_v21: could not write into the game folder. Nothing was changed.
pause
exit /b 1

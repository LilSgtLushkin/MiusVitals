@echo off
rem Copy the logs a bug report needs into one folder next to the module, so they can be zipped and
rem sent as they are. Nothing in the game folder is moved or deleted.
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

set "STAMP=%DATE:/=-%_%TIME::=-%"
set "STAMP=%STAMP: =_%"
set "OUT=%HERE%\logs_%STAMP%"
set "OUT=%OUT:.=_%"
if not exist "%OUT%" mkdir "%OUT%" >nul 2>nul

echo.
echo  Copying logs from %GAME%\out
copy /y "%GAME%\out\net_v21_*.log" "%OUT%\" >nul 2>nul
copy /y "%GAME%\out\basic_network_*.log" "%OUT%\" >nul 2>nul
copy /y "%GAME%\out\mp_join*.log" "%OUT%\" >nul 2>nul
if exist "%HERE%\state\basic_network.before_netv21.codelib" copy /y "%HERE%\state\basic_network.before_netv21.codelib" "%OUT%\basic_network.before_netv21.codelib" >nul 2>nul

dir /b "%OUT%" >nul 2>nul
if errorlevel 1 goto :empty

echo  Done. The folder to send: %OUT%
echo  It contains the run log (net_v21_*.log), the module's own log and the module that was
echo  installed before the switch.
echo.
explorer "%OUT%"
pause
exit /b 0

:empty
echo  No logs were found in %GAME%\out - start the game with the net_v21 module once.
pause
exit /b 1

:no_game
echo  net_v21: the game folder was not found.
pause
exit /b 1

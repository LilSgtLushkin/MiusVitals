@echo off
rem Switch this game to the net_v21 module: our own build of the network module, with the Steam
rem layer and the detailed battle log this archive is made to test.
rem
rem Nothing is destroyed: the module that is in the game now is copied to net_v21\state\ first, and
rem net_v21\basic_network.proven.codelib is always there for one-click rollback.
setlocal EnableExtensions DisableDelayedExpansion
set "HERE=%~dp0"
set "HERE=%HERE:~0,-1%"
set "MOD=%HERE%\.."
for %%I in ("%MOD%") do set "MOD=%%~fI"

rem ---- the game folder: the same search the installer uses
set "GAME="
for %%I in ("%MOD%\..") do if exist "%%~fI\starter_x64.exe" set "GAME=%%~fI"
if defined GAME goto :have_game
for %%I in ("%MOD%\..\..") do if exist "%%~fI\starter_x64.exe" set "GAME=%%~fI"
if defined GAME goto :have_game
for %%I in ("%MOD%\..\..\..") do if exist "%%~fI\starter_x64.exe" set "GAME=%%~fI"
if not defined GAME goto :no_game
:have_game

echo.
echo  net_v21: switching this game to our own network module
echo  Game: %GAME%
echo.

if not exist "%HERE%\basic_network.netv21.codelib" goto :no_payload
if not exist "%GAME%\bin\rel\x64" goto :no_game_files

tasklist /fi "imagename eq starter_x64.exe" /nh 2>nul | find /i "starter_x64.exe" >nul && goto :running

if not exist "%HERE%\state" mkdir "%HERE%\state" >nul 2>nul
if not exist "%GAME%\out" mkdir "%GAME%\out" >nul 2>nul
rem keep whatever is installed right now, even if it is neither of the two modules we know
copy /y "%GAME%\bin\rel\x64\basic_network.codelib" "%HERE%\state\basic_network.before_netv21.codelib" >nul 2>nul

copy /y "%HERE%\basic_network.netv21.codelib" "%GAME%\bin\rel\x64\basic_network.put" >nul
if errorlevel 1 goto :failed
move /y "%GAME%\bin\rel\x64\basic_network.put" "%GAME%\bin\rel\x64\basic_network.codelib" >nul
if errorlevel 1 goto :failed
rem Windows must not hold an internet mark on the module it is asked to load
powershell -NoProfile -Command "Unblock-File -LiteralPath '%GAME%\bin\rel\x64\basic_network.codelib'" >nul 2>nul

echo  The game now runs the net_v21 module.
echo.
echo  What to expect:
echo    - the module writes out\net_v21_opgame.log and out\net_v21_tacgame.log;
echo    - the first lines of that file say which build it is (version, date, image hash);
echo    - every five seconds a summary line carries the traffic and battle counters.
echo.
echo  This module is a TEST build: it does not contain the v20 battle delta yet, and it does not
echo  import mp_vitals.dll (the Vitals performance fixes are not in this build).
echo  Back to the proven module: net_v21\Switch_Back_To_Proven.bat
echo  Collect the logs for a report:  net_v21\Collect_Logs.bat
echo.
pause
exit /b 0

:no_game
echo  net_v21: the game folder was not found - unzip the archive into the game folder
echo           (the folder that has starter_x64.exe) and run this script from there.
pause
exit /b 1

:no_game_files
echo  net_v21: %GAME%\bin\rel\x64 does not exist - is this the right game build?
pause
exit /b 1

:no_payload
echo  net_v21: basic_network.netv21.codelib is missing from the archive - unpack it again.
pause
exit /b 1

:running
echo  net_v21: the game is running. Close it and run this script again.
pause
exit /b 1

:failed
echo  net_v21: the module could not be replaced. Nothing else was changed.
echo           The previous module is in net_v21\state\basic_network.before_netv21.codelib
pause
exit /b 1

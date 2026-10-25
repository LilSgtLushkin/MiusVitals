@echo off
rem Takes MiusVitals out again: the saved basic_network.codelib goes back, mp_vitals.dll is deleted.
rem The restore is driven by the saved file, not by a hash of it, so it cannot fail on a check.
setlocal EnableExtensions DisableDelayedExpansion
set "KIT=%~dp0"
set "KIT=%KIT:~0,-1%"
for %%I in ("%KIT%\..") do set "GAME=%%~fI"
set "RC=0"
echo.
if not exist "%GAME%\starter_x64.exe" goto :no_game
tasklist /fi "imagename eq starter_x64.exe" /nh 2>nul | find /i "starter_x64.exe" >nul && goto :running
set "SRC="
if exist "%KIT%\state\basic_network.codelib.v20" set "SRC=%KIT%\state\basic_network.codelib.v20"
if not defined SRC if exist "%KIT%\state\basic_network.codelib.previous" set "SRC=%KIT%\state\basic_network.codelib.previous"
if not defined SRC if exist "%KIT%\..\mpmod1\files\bin\rel\x64\basic_network.codelib" set "SRC=%KIT%\..\mpmod1\files\bin\rel\x64\basic_network.codelib"
if not defined SRC goto :nosrc
for %%I in ("%GAME%\bin\rel\x64\basic_network.codelib") do if not exist "%%~dpI" mkdir "%%~dpI" >nul 2>nul
attrib -r -s -h "%GAME%\bin\rel\x64\basic_network.codelib" >nul 2>nul
del /f /q "%GAME%\bin\rel\x64\basic_network.codelib" >nul 2>nul
copy /y "%SRC%" "%GAME%\bin\rel\x64\basic_network.codelib" >nul 2>nul
if exist "%GAME%\bin\rel\x64\basic_network.codelib" goto :module_back
copy /y "%SRC%" "%GAME%\bin\rel\x64\basic_network.codelib.put" >nul 2>nul
if not exist "%GAME%\bin\rel\x64\basic_network.codelib.put" goto :failed
move /y "%GAME%\bin\rel\x64\basic_network.codelib.put" "%GAME%\bin\rel\x64\basic_network.codelib" >nul 2>nul
if not exist "%GAME%\bin\rel\x64\basic_network.codelib" goto :failed
:module_back
if exist "%GAME%\mp_vitals.dll" attrib -r -s -h "%GAME%\mp_vitals.dll" >nul 2>nul
if exist "%GAME%\mp_vitals.dll" del /f /q "%GAME%\mp_vitals.dll" >nul 2>nul
if exist "%KIT%\state\installed.txt" del /f /q "%KIT%\state\installed.txt" >nul 2>nul
echo  MiusVitals is removed; the saved basic_network.codelib is back.
goto :finish
:nosrc
echo  No saved copy of the previous basic_network.codelib was found (state\ of this folder, or mpmod1\files).
echo  Run MiusMultiplayer\Enable_Multiplayer.bat: it replaces the module and restores a consistent game.
echo  mp_vitals.dll can stay then.
set "RC=2"
goto :finish
:no_game
echo  This folder is not inside the game folder.
set "RC=3"
goto :finish
:running
echo  The game is running. Close it, then run this again.
set "RC=4"
goto :finish
:failed
echo  The saved module could not be copied back. Is the game still running?
set "RC=1"
:finish
echo.
pause
exit /b %RC%

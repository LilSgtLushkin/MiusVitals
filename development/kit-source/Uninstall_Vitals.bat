@echo off
rem Takes MiusVitals out again: the original basic_network.codelib (v20) goes back, mp_vitals.dll is deleted.
setlocal EnableExtensions DisableDelayedExpansion
set "KIT=%~dp0"
set "KIT=%KIT:~0,-1%"
for %%I in ("%KIT%\..") do set "GAME=%%~fI"
set "V20=8108BD1E9AF8080EF8B1050512B8311848F5A336ACB49A7479388733B4AE5723"
set "RC=0"
echo.
if not exist "%GAME%\starter_x64.exe" goto :no_game
tasklist /fi "imagename eq starter_x64.exe" /nh 2>nul | find /i "starter_x64.exe" >nul && goto :running
set "SRC="
if exist "%KIT%\state\basic_network.codelib.v20" set "SRC=%KIT%\state\basic_network.codelib.v20"
if not defined SRC if exist "%KIT%\..\mpmod1\files\bin\rel\x64\basic_network.codelib" set "SRC=%KIT%\..\mpmod1\files\bin\rel\x64\basic_network.codelib"
if not defined SRC goto :nosrc
set "H="
for /f "skip=1 delims=" %%X in ('certutil -hashfile "%SRC%" SHA256 2^>nul') do if not defined H set "H=%%X"
if defined H set "H=%H: =%"
if /i not "%H%"=="%V20%" goto :nosrc
copy /y "%SRC%" "%GAME%\bin\rel\x64\basic_network.codelib" >nul || goto :failed
if exist "%GAME%\mp_vitals.dll" del /f /q "%GAME%\mp_vitals.dll"
echo  MiusVitals is removed; the original v20 module is back.
goto :finish
:nosrc
echo  No copy of the original v20 basic_network.codelib was found (state\ of this folder, or mpmod1\files).
echo  Run mpmod1\Enable_Multiplayer.bat: it puts the original module back. mp_vitals.dll can stay then.
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
echo  The original could not be copied back. Is the game still running?
set "RC=1"
:finish
echo.
pause
exit /b %RC%

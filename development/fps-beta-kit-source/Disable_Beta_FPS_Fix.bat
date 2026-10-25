@echo off
rem Removes only the exact beta_FPS_fix codelib and restores the module saved before the beta install.
setlocal EnableExtensions DisableDelayedExpansion
set "MOD=%~dp0"
set "MOD=%MOD:~0,-1%"
for %%I in ("%MOD%\..") do set "GAME=%%~fI"
set "QUIET="
if /i "%~1"=="/quiet" set "QUIET=1"
set "RC=0"
set "CANDIDATE=78E2718383BC0679154118415189D1C1EE05ED0179A0A924455C22CB4AE2916E"

echo.
echo  MiusMultiplayer beta_FPS_fix - restore previous codelib
if not exist "%GAME%\starter_x64.exe" goto :no_game
tasklist /fi "imagename eq starter_x64.exe" /nh 2>nul | find /i "starter_x64.exe" >nul && goto :running
call :is_candidate || goto :nothing
if exist "%MOD%\state\backup\bin\rel\x64\basic_network.codelib" (
    copy /y "%MOD%\state\backup\bin\rel\x64\basic_network.codelib" "%GAME%\bin\rel\x64\basic_network.codelib" >nul || goto :failed
    echo  Restored the codelib present before beta_FPS_fix was enabled.
) else (
    del /f /q "%GAME%\bin\rel\x64\basic_network.codelib" >nul 2>nul
    if exist "%GAME%\bin\rel\x64\basic_network.codelib" goto :failed
    echo  Removed beta_FPS_fix codelib; no previous codelib backup existed.
)
del /f /q "%MOD%\state\enabled.txt" >nul 2>nul
rmdir "%MOD%\state\backup\bin\rel\x64" 2>nul
rmdir "%MOD%\state\backup\bin\rel" 2>nul
rmdir "%MOD%\state\backup\bin" 2>nul
rmdir "%MOD%\state\backup" 2>nul
rmdir "%MOD%\state" 2>nul
goto :finish

:is_candidate
if not exist "%GAME%\bin\rel\x64\basic_network.codelib" exit /b 1
set "H="
for /f "skip=1 delims=" %%X in ('certutil -hashfile "%GAME%\bin\rel\x64\basic_network.codelib" SHA256 2^>nul') do if not defined H set "H=%%X"
if defined H set "H=%H: =%"
if /i "%H%"=="%CANDIDATE%" exit /b 0
exit /b 1

:nothing
echo  The active codelib is not this beta candidate. Nothing was changed.
goto :finish
:no_game
echo  This folder is not inside a game folder (starter_x64.exe is absent next to beta_FPS_fix).
set "RC=3"
goto :finish
:running
echo  The game is running. Close it, then run this again.
set "RC=4"
goto :finish
:failed
echo  A file could not be removed or restored. Close the game and run this file again.
set "RC=1"
:finish
echo.
if not defined QUIET pause
exit /b %RC%

@echo off
rem Removes the beta_FPS_fix codelib from the install record and puts the saved module back.
rem Removal does not depend on a hash of the active file.
setlocal EnableExtensions DisableDelayedExpansion
set "MOD=%~dp0"
set "MOD=%MOD:~0,-1%"
set "QUIET="
if /i "%~1"=="/quiet" set "QUIET=1"
set "RC=0"
set "CANDIDATE=7B42A69ABE58DEB1583D2F5320EEBBECD9718FFA27EA08933613E67DF1901F67"
set "LEGACY_CANDIDATE=78E2718383BC0679154118415189D1C1EE05ED0179A0A924455C22CB4AE2916E"

echo.
echo  MiusMultiplayer beta_FPS_fix - restore previous codelib
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
tasklist /fi "imagename eq starter_x64.exe" /nh 2>nul | find /i "starter_x64.exe" >nul && goto :running

if exist "%MOD%\state\installed.txt" goto :manifest
if not exist "%ACTIVE%" goto :nothing
call :is_candidate
if errorlevel 1 goto :nothing
rem an older beta archive left no install record: remove its recognised candidate by name
call :drop "bin\rel\x64\basic_network.codelib"
goto :manifest

:manifest
if exist "%MOD%\state\installed.txt" for /f "usebackq delims=" %%L in ("%MOD%\state\installed.txt") do call :drop "%%L" || set "RC=1"
if not exist "%BACKUP%" goto :no_backup
if exist "%ACTIVE%" goto :kept_backup
for %%I in ("%ACTIVE%") do if not exist "%%~dpI" mkdir "%%~dpI" >nul 2>nul
move /y "%BACKUP%" "%ACTIVE%" >nul 2>nul
if exist "%ACTIVE%" echo  Restored the codelib that was present before beta_FPS_fix was enabled.
if exist "%ACTIVE%" goto :cleanup
echo  COULD NOT RESTORE %ACTIVE%
set "RC=1"
goto :cleanup
:kept_backup
echo  Kept the saved beta backup because another basic_network.codelib is in its place.
goto :cleanup
:no_backup
echo  Removed the beta_FPS_fix codelib; no previous codelib had been saved.

:cleanup
del /f /q "%MOD%\state\installed.txt" >nul 2>nul
del /f /q "%MOD%\state\enabled.txt" >nul 2>nul
rmdir "%MOD%\state\backup\bin\rel\x64" 2>nul
rmdir "%MOD%\state\backup\bin\rel" 2>nul
rmdir "%MOD%\state\backup\bin" 2>nul
rmdir "%MOD%\state\backup" 2>nul
rmdir "%MOD%\state" 2>nul
if not "%RC%"=="0" echo  A file could not be removed or restored. Close the game and run this again.
goto :finish

rem ---- %1 = a file this beta wrote, relative to the game folder
:drop
if not exist "%GAME%\%~1" exit /b 0
attrib -r -s -h "%GAME%\%~1" >nul 2>nul
del /f /q "%GAME%\%~1" >nul 2>nul
if exist "%GAME%\%~1" (
    echo    COULD NOT REMOVE %~1
    exit /b 1
)
exit /b 0

rem ---- an older beta archive left no install record: recognise its two candidates by SHA-256
:is_candidate
set "ACTIVE_HASH="
for /f "skip=1 delims=" %%X in ('certutil -hashfile "%ACTIVE%" SHA256 2^>nul') do if not defined ACTIVE_HASH set "ACTIVE_HASH=%%X"
if defined ACTIVE_HASH set "ACTIVE_HASH=%ACTIVE_HASH: =%"
if /i "%ACTIVE_HASH%"=="%CANDIDATE%" exit /b 0
if /i "%ACTIVE_HASH%"=="%LEGACY_CANDIDATE%" exit /b 0
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
:finish
echo.
if not defined QUIET pause
exit /b %RC%

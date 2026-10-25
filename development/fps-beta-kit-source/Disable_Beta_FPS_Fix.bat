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
set "ACTIVE=%GAME%\bin\rel\x64\basic_network.codelib"
set "BACKUP=%MOD%\state\backup\bin\rel\x64\basic_network.codelib"
set "BACKUP_HASH=%MOD%\state\backup\bin\rel\x64\basic_network.codelib.sha256"

echo.
echo  MiusMultiplayer beta_FPS_fix - restore previous codelib
if not exist "%GAME%\starter_x64.exe" goto :no_game
tasklist /fi "imagename eq starter_x64.exe" /nh 2>nul | find /i "starter_x64.exe" >nul && goto :running
call :is_candidate || goto :nothing
if not exist "%BACKUP%" goto :remove_candidate
call :read_backup_hash || goto :failed
call :copy_checked "%BACKUP%" "%ACTIVE%" "%EXPECTED_BACKUP_HASH%" || goto :failed
echo  Restored the exact codelib present before beta_FPS_fix was enabled.
goto :cleanup_beta_state

:remove_candidate
del /f /q "%ACTIVE%" >nul 2>nul
if exist "%ACTIVE%" goto :failed
echo  Removed beta_FPS_fix codelib; no previous codelib existed.

:cleanup_beta_state
del /f /q "%MOD%\state\enabled.txt" >nul 2>nul
del /f /q "%BACKUP_HASH%" >nul 2>nul
rmdir "%MOD%\state\backup\bin\rel\x64" 2>nul
rmdir "%MOD%\state\backup\bin\rel" 2>nul
rmdir "%MOD%\state\backup\bin" 2>nul
rmdir "%MOD%\state\backup" 2>nul
rmdir "%MOD%\state" 2>nul
goto :finish

:read_backup_hash
set "EXPECTED_BACKUP_HASH="
if not exist "%BACKUP_HASH%" (
    echo  The beta backup has no SHA-256 record and was not restored.
    exit /b 1
)
for /f "usebackq tokens=1" %%X in ("%BACKUP_HASH%") do if not defined EXPECTED_BACKUP_HASH set "EXPECTED_BACKUP_HASH=%%X"
if not defined EXPECTED_BACKUP_HASH exit /b 1
call :hash_file "%BACKUP%" ACTUAL_BACKUP_HASH || exit /b 1
if /i "%EXPECTED_BACKUP_HASH%"=="%ACTUAL_BACKUP_HASH%" exit /b 0
echo  The saved beta backup failed its SHA-256 check and was not restored.
exit /b 1

:is_candidate
if not exist "%ACTIVE%" exit /b 1
call :hash_file "%ACTIVE%" ACTIVE_HASH || exit /b 1
if /i "%ACTIVE_HASH%"=="%CANDIDATE%" exit /b 0
exit /b 1

:hash_file
set "%~2="
set "H="
for /f "skip=1 delims=" %%X in ('certutil -hashfile "%~1" SHA256 2^>nul') do if not defined H set "H=%%X"
if defined H set "H=%H: =%"
if not defined H exit /b 1
set "%~2=%H%"
exit /b 0

:copy_checked
copy /y "%~1" "%~2" >nul || exit /b 1
call :hash_file "%~2" COPIED_HASH || exit /b 1
if /i "%COPIED_HASH%"=="%~3" exit /b 0
echo  SHA-256 verification failed for %~2
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
echo  A file could not be removed, restored, or verified. Close the game and run this file again.
set "RC=1"
:finish
echo.
if not defined QUIET pause
exit /b %RC%

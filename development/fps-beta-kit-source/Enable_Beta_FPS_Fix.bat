@echo off
rem MiusMultiplayer beta_FPS_fix: Steam-preserving zero-ms simulation-step candidate for Steam 6.00 build 3598/1.
rem It starts from the verified Steam-capable codelib, adds one guarded FPS trampoline, and keeps a restorable backup.
setlocal EnableExtensions DisableDelayedExpansion
set "MOD=%~dp0"
set "MOD=%MOD:~0,-1%"
for %%I in ("%MOD%\..") do set "GAME=%%~fI"
set "QUIET="
if /i "%~1"=="/quiet" set "QUIET=1"
set "RC=0"
set "CANDIDATE=7B42A69ABE58DEB1583D2F5320EEBBECD9718FFA27EA08933613E67DF1901F67"
set "LEGACY_CANDIDATE=78E2718383BC0679154118415189D1C1EE05ED0179A0A924455C22CB4AE2916E"
set "ACTIVE=%GAME%\bin\rel\x64\basic_network.codelib"
set "BACKUP=%MOD%\state\backup\bin\rel\x64\basic_network.codelib"
set "BACKUP_HASH=%MOD%\state\backup\bin\rel\x64\basic_network.codelib.sha256"

echo.
echo  MiusMultiplayer beta_FPS_fix - Steam-preserving experimental FPS candidate
echo.
if not exist "%GAME%\starter_x64.exe" goto :no_game
if not exist "%MOD%\files\bin\rel\x64\basic_network.codelib" goto :incomplete
tasklist /fi "imagename eq starter_x64.exe" /nh 2>nul | find /i "starter_x64.exe" >nul && goto :running
if not exist "%MOD%\state\backup\bin\rel\x64\" mkdir "%MOD%\state\backup\bin\rel\x64" || goto :failed
call :backup_previous || goto :failed
if not exist "%GAME%\bin\rel\x64\" mkdir "%GAME%\bin\rel\x64" || goto :failed
call :copy_checked "%MOD%\files\bin\rel\x64\basic_network.codelib" "%ACTIVE%" "%CANDIDATE%" || (
    call :restore_backup_after_failed_install
    goto :failed
)
>"%MOD%\state\enabled.txt" echo beta_FPS_fix candidate enabled.
echo  Installed and SHA-256 verified:
echo    basic_network.codelib %CANDIDATE%
echo.
echo  This is an experimental Steam-preserving FPS candidate. Both players must use this same beta archive.
  Steam friends, rich-presence Join Game, and Steam P2P code are retained from the verified base module.
echo  The previous codelib, including an unrecognised one, is backed up and can be restored.
echo  Run Disable_Beta_FPS_Fix.bat before returning to stable MiusMultiplayer.
echo  The released Vitals sidecar import/signature surface is retained, but record FPS and both basic_network logs.
  Steam friends joining must be regression-tested on two PCs before any compatibility claim.
goto :finish

:backup_previous
if not exist "%ACTIVE%" exit /b 0
call :is_candidate && exit /b 0
if not exist "%BACKUP%" goto :create_backup
call :read_backup_hash || exit /b 1
call :hash_file "%ACTIVE%" ACTIVE_HASH || exit /b 1
if /i "%ACTIVE_HASH%"=="%EXPECTED_BACKUP_HASH%" exit /b 0
echo  A stale beta backup was found while a different codelib is active.
echo  Nothing was changed, so no unrecognised module can be lost. Use Disable_Beta_FPS_Fix.bat or resolve the stale beta state first.
exit /b 1

:create_backup
call :hash_file "%ACTIVE%" SOURCE_HASH || exit /b 1
copy /y "%ACTIVE%" "%BACKUP%" >nul || exit /b 1
call :hash_file "%BACKUP%" BACKED_UP_HASH || exit /b 1
if /i "%SOURCE_HASH%"=="%BACKED_UP_HASH%" goto :write_backup_hash
del /f /q "%BACKUP%" >nul 2>nul
echo  Backup verification failed; the active codelib was not replaced.
exit /b 1

:write_backup_hash
>"%BACKUP_HASH%" echo %SOURCE_HASH%  basic_network.codelib
echo    saved previous basic_network.codelib in beta_FPS_fix\state\backup
exit /b 0

:restore_backup_after_failed_install
if not exist "%BACKUP%" exit /b 0
call :read_backup_hash || exit /b 1
call :copy_checked "%BACKUP%" "%ACTIVE%" "%EXPECTED_BACKUP_HASH%" || exit /b 1
echo  Restored the previous codelib after the beta install verification failed.
exit /b 0

:read_backup_hash
set "EXPECTED_BACKUP_HASH="
if not exist "%BACKUP_HASH%" exit /b 1
for /f "usebackq tokens=1" %%X in ("%BACKUP_HASH%") do if not defined EXPECTED_BACKUP_HASH set "EXPECTED_BACKUP_HASH=%%X"
if not defined EXPECTED_BACKUP_HASH exit /b 1
call :hash_file "%BACKUP%" ACTUAL_BACKUP_HASH || exit /b 1
if /i "%EXPECTED_BACKUP_HASH%"=="%ACTUAL_BACKUP_HASH%" exit /b 0
echo  The saved beta backup failed its SHA-256 check and was not used.
exit /b 1

:is_candidate
if not exist "%ACTIVE%" exit /b 1
call :hash_file "%ACTIVE%" ACTIVE_HASH || exit /b 1
if /i "%ACTIVE_HASH%"=="%CANDIDATE%" exit /b 0
if /i "%ACTIVE_HASH%"=="%LEGACY_CANDIDATE%" exit /b 0
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
echo  A file could not be copied, backed up, or verified. Close the game and run this file again.
set "RC=1"
:finish
echo.
if not defined QUIET pause
exit /b %RC%

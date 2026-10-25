@echo off
rem MiusMultiplayer beta_FPS_fix: source-built remote-transform candidate for Steam 6.00 build 3598/1.
rem This test installer changes only basic_network.codelib and keeps a restorable backup of the prior module.
setlocal EnableExtensions DisableDelayedExpansion
set "MOD=%~dp0"
set "MOD=%MOD:~0,-1%"
for %%I in ("%MOD%\..") do set "GAME=%%~fI"
set "QUIET="
if /i "%~1"=="/quiet" set "QUIET=1"
set "RC=0"
set "CANDIDATE=78E2718383BC0679154118415189D1C1EE05ED0179A0A924455C22CB4AE2916E"
set "V20=8108BD1E9AF8080EF8B1050512B8311848F5A336ACB49A7479388733B4AE5723"
set "STABLE=485B655662BD062E16278F9D699A3097B49A08DE1948D2500DD589703A3102A3"
set "STABLE_PREVIOUS=4C91874C4B62DB424ECCA36586121BCB3B4CC7849CC900285E68DE5151EDDC36"
set "STABLE_NOSTART=E4317E630A21198A55E0E86BE4AA29C71FCADA6065824C858ACD0E2BDEF8ADF9"

echo.
echo  MiusMultiplayer beta_FPS_fix - experimental FPS candidate
echo.
if not exist "%GAME%\starter_x64.exe" goto :no_game
if not exist "%MOD%\files\bin\rel\x64\basic_network.codelib" goto :incomplete
tasklist /fi "imagename eq starter_x64.exe" /nh 2>nul | find /i "starter_x64.exe" >nul && goto :running
call :known_network
if errorlevel 1 goto :foreign_network
if not exist "%MOD%\state\backup\bin\rel\x64\" mkdir "%MOD%\state\backup\bin\rel\x64" || goto :failed
call :backup_previous || goto :failed
if not exist "%GAME%\bin\rel\x64\" mkdir "%GAME%\bin\rel\x64" || goto :failed
call :copy_checked "%MOD%\files\bin\rel\x64\basic_network.codelib" "%GAME%\bin\rel\x64\basic_network.codelib" "%CANDIDATE%" || goto :failed
>"%MOD%\state\enabled.txt" echo beta_FPS_fix candidate enabled.
echo  Installed and SHA-256 verified:
echo    basic_network.codelib %CANDIDATE%
echo.
echo  This is an experimental FPS-only source candidate. Both players must use this same beta archive.
echo  It replaces the installed codelib automatically. Run Disable_Beta_FPS_Fix.bat before returning to stable MiusMultiplayer.
echo  Vitals sidecar hooks are intentionally not claimed for this source-built candidate; record FPS and both basic_network logs.
goto :finish

:known_network
if not exist "%GAME%\bin\rel\x64\basic_network.codelib" exit /b 0
set "H="
for /f "skip=1 delims=" %%X in ('certutil -hashfile "%GAME%\bin\rel\x64\basic_network.codelib" SHA256 2^>nul') do if not defined H set "H=%%X"
if defined H set "H=%H: =%"
for %%X in ("%CANDIDATE%" "%V20%" "%STABLE%" "%STABLE_PREVIOUS%" "%STABLE_NOSTART%") do if /i "%H%"==%%~X exit /b 0
echo  Existing basic_network.codelib is unknown: %H%
echo  Nothing was changed: this beta will not overwrite an unknown module.
exit /b 1

:backup_previous
if not exist "%GAME%\bin\rel\x64\basic_network.codelib" exit /b 0
call :is_candidate && exit /b 0
if exist "%MOD%\state\backup\bin\rel\x64\basic_network.codelib" exit /b 0
copy /y "%GAME%\bin\rel\x64\basic_network.codelib" "%MOD%\state\backup\bin\rel\x64\basic_network.codelib" >nul || exit /b 1
echo    saved previous basic_network.codelib in beta_FPS_fix\state\backup
exit /b 0

:is_candidate
if not exist "%GAME%\bin\rel\x64\basic_network.codelib" exit /b 1
set "H="
for /f "skip=1 delims=" %%X in ('certutil -hashfile "%GAME%\bin\rel\x64\basic_network.codelib" SHA256 2^>nul') do if not defined H set "H=%%X"
if defined H set "H=%H: =%"
if /i "%H%"=="%CANDIDATE%" exit /b 0
exit /b 1

:copy_checked
copy /y "%~1" "%~2" >nul || exit /b 1
set "H="
for /f "skip=1 delims=" %%X in ('certutil -hashfile "%~2" SHA256 2^>nul') do if not defined H set "H=%%X"
if defined H set "H=%H: =%"
if /i "%H%"=="%~3" exit /b 0
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
:foreign_network
set "RC=2"
goto :finish
:failed
echo  A file could not be copied or verified. Close the game and run this file again.
set "RC=1"
:finish
echo.
if not defined QUIET pause
exit /b %RC%

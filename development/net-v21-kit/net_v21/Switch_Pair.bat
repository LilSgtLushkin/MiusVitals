@echo off
rem Switch the module and launcher as one recoverable operation. A process crash can still leave
rem a mixed pair: pending.txt and the two verified backups deliberately remain for manual recovery.
setlocal EnableExtensions DisableDelayedExpansion
set "HERE=%~dp0"
set "HERE=%HERE:~0,-1%"
for %%I in ("%HERE%\..") do set "MOD=%%~fI"
set "GAME="
for %%I in ("%MOD%\..") do if exist "%%~fI\starter_x64.exe" set "GAME=%%~fI"
if defined GAME goto :found_game
for %%I in ("%MOD%\..\..") do if exist "%%~fI\starter_x64.exe" set "GAME=%%~fI"
if defined GAME goto :found_game
for %%I in ("%MOD%\..\..\..") do if exist "%%~fI\starter_x64.exe" set "GAME=%%~fI"
if not defined GAME goto :no_game
:found_game
if /i "%~1"=="to" goto :to_candidate
if /i "%~1"=="back" goto :to_proven
echo  net_v21: invalid switch direction.
exit /b 2
:to_candidate
set "SRCMOD=%HERE%\basic_network.netv21.codelib"
set "SRCEXE=%HERE%\mp_join.netv21.exe"
set "DESCRIPTION=net_v21 test module and matching launcher"
goto :preflight
:to_proven
set "SRCMOD=%HERE%\basic_network.proven.codelib"
set "SRCEXE=%MOD%\files\mp_join.exe"
set "DESCRIPTION=proven v1.0.1 module and launcher"
:preflight
set "DSTMOD=%GAME%\bin\rel\x64\basic_network.codelib"
set "DSTEXE=%GAME%\mp_join.exe"
set "STATE=%MOD%\state\net_v21_switch"
set "MARK=%STATE%\pending.txt"
set "BAKMOD=%STATE%\previous.basic_network.codelib"
set "BAKEXE=%STATE%\previous.mp_join.exe"
echo.
echo  net_v21: switching to %DESCRIPTION%
echo  Game: %GAME%
if not exist "%SRCMOD%" goto :missing_payload
if not exist "%SRCEXE%" goto :missing_payload
if not exist "%DSTMOD%" goto :not_installed
if not exist "%DSTEXE%" goto :not_installed
tasklist /fi "imagename eq starter_x64.exe" /nh 2>nul | find /i "starter_x64.exe" >nul && goto :running
if not exist "%STATE%" mkdir "%STATE%" >nul 2>nul
if not exist "%STATE%" goto :failed_before
if exist "%MARK%" goto :unresolved
rem Back up both currently installed files, not only the package's proven files.
copy /b /y "%DSTMOD%" "%BAKMOD%" >nul || goto :failed_before
fc /b "%DSTMOD%" "%BAKMOD%" >nul || goto :failed_before
copy /b /y "%DSTEXE%" "%BAKEXE%" >nul || goto :failed_before
fc /b "%DSTEXE%" "%BAKEXE%" >nul || goto :failed_before
>"%MARK%" echo Interrupted switch. Do not launch the game. Backups are in this folder.
if errorlevel 1 goto :failed_before
rem Staging is on the same volume as each destination; every move and byte comparison is checked.
copy /b /y "%SRCMOD%" "%DSTMOD%.mp_v21.put" >nul || goto :rollback
fc /b "%SRCMOD%" "%DSTMOD%.mp_v21.put" >nul || goto :rollback
copy /b /y "%SRCEXE%" "%DSTEXE%.mp_v21.put" >nul || goto :rollback
fc /b "%SRCEXE%" "%DSTEXE%.mp_v21.put" >nul || goto :rollback
move /y "%DSTMOD%.mp_v21.put" "%DSTMOD%" >nul || goto :rollback
move /y "%DSTEXE%.mp_v21.put" "%DSTEXE%" >nul || goto :rollback
fc /b "%SRCMOD%" "%DSTMOD%" >nul || goto :rollback
fc /b "%SRCEXE%" "%DSTEXE%" >nul || goto :rollback
del /q "%MARK%" >nul 2>nul
if exist "%MARK%" goto :unresolved
set "MP_SWITCH_MODULE=%DSTMOD%"
set "MP_SWITCH_LAUNCHER=%DSTEXE%"
powershell -NoProfile -Command "Unblock-File -LiteralPath $env:MP_SWITCH_MODULE,$env:MP_SWITCH_LAUNCHER -ErrorAction SilentlyContinue" >nul 2>nul
echo  Done. Both files were replaced and compared byte for byte.
exit /b 0

:rollback
echo  net_v21: replacement failed; restoring both verified backups.
set "ROLLFAIL=0"
call :restore "%BAKMOD%" "%DSTMOD%" || set "ROLLFAIL=1"
call :restore "%BAKEXE%" "%DSTEXE%" || set "ROLLFAIL=1"
del /q "%DSTMOD%.mp_v21.put" "%DSTEXE%.mp_v21.put" >nul 2>nul
if "%ROLLFAIL%"=="1" goto :unresolved
del /q "%MARK%" >nul 2>nul
if exist "%MARK%" goto :unresolved
echo  Both previous files have been restored. Close the game and check write access.
exit /b 1

:restore
copy /b /y "%~1" "%~2.mp_v21.restore" >nul || exit /b 1
fc /b "%~1" "%~2.mp_v21.restore" >nul || exit /b 1
move /y "%~2.mp_v21.restore" "%~2" >nul || exit /b 1
fc /b "%~1" "%~2" >nul || exit /b 1
exit /b 0

:no_game
echo  net_v21: the game folder was not found.
exit /b 1
:missing_payload
echo  net_v21: a module or launcher is missing from the archive. Unpack it again.
exit /b 1
:not_installed
echo  net_v21: installed files are missing. Run Enable_Multiplayer.bat first.
exit /b 1
:running
echo  net_v21: the game is running. Close it and try again.
exit /b 1
:failed_before
echo  net_v21: no files were replaced. Cannot save and verify both backups.
exit /b 1
:unresolved
echo  net_v21: an earlier switch is unresolved. DO NOT START THE GAME.
echo  Restore both previous files manually from "%STATE%", then remove pending.txt.
exit /b 1

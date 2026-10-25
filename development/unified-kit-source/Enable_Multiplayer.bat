@echo off
rem MiusMultiplayer v1.0.0: one checked MiusPlayer + MiusVitals install for Steam 3598.
rem It installs the multiplayer module, program packs and Vitals DLL together; no separate mpmod1 or MiusVitals install is needed.
setlocal EnableExtensions DisableDelayedExpansion
set "MOD=%~dp0"
set "MOD=%MOD:~0,-1%"
for %%I in ("%MOD%\..") do set "GAME=%%~fI"
set "QUIET="
if /i "%~1"=="/quiet" set "QUIET=1"
set "RC=0"
set "V20=8108BD1E9AF8080EF8B1050512B8311848F5A336ACB49A7479388733B4AE5723"
set "NETWORK=485B655662BD062E16278F9D699A3097B49A08DE1948D2500DD589703A3102A3"
set "NETWORK_PREVIOUS=4C91874C4B62DB424ECCA36586121BCB3B4CC7849CC900285E68DE5151EDDC36"
set "NETWORK_OLDER=7CAF36CDDABF7EB388A3C6C393B05733E051955D63D32EF13531CE88004DCFBA"
set "NETWORK_NOSTART=E4317E630A21198A55E0E86BE4AA29C71FCADA6065824C858ACD0E2BDEF8ADF9"
set "NETWORK_NOSTART_PREVIOUS=F0766BEC48696869D59D4A654A04F20BD7BAF9F73ADB6FBF139E7DA31DFC1F4F"
set "NETWORK_NOSTART_OLDER=2FB049B9B465DE2E05BD9BBE7D5573A772319CC4D65FB20BB146E648A7CDF8EC"
set "NETWORK_NOREBASE=D2E34581B42D0573547E221873D7538B64A413267E8ED388ABEB46DD42B630D2"
set "VITALS=71A1BF3A37923BBE6C8EE84448D0B91705C2BFF7382B647F990B9D671EE224DF"
set "JOIN=6AA417CF6E1F01480C997446F2691A58AB55B90FA36E1FF511945989AADFEEAA"
set "OPGAME=ECA75B740F7B11D9E13F2E2001F1B2B133427D0860FB3770220B89B0B8909535"
set "TACGAME=01923EABDF6A9D5FA37DC70A3D76740242DBC9A7F26C0298CA3FAA8CCA87600D"
set "APPID=1A43840CA36F1401C85FF5F6E6981088927CFE09B23D155A9C4301DD13CA9610"

echo.
echo  MiusMultiplayer v1.0.0 - unified multiplayer and Vitals install
echo.
if not exist "%GAME%\starter_x64.exe" goto :no_game
if not exist "%MOD%\files\bin\rel\x64\basic_network.codelib" goto :incomplete
if not exist "%MOD%\files\mp_vitals.dll" goto :incomplete
if not exist "%MOD%\files\root\programs\opgame.progpack" goto :incomplete
if not exist "%MOD%\files\root\programs\tacgame.progpack" goto :incomplete
tasklist /fi "imagename eq starter_x64.exe" /nh 2>nul | find /i "starter_x64.exe" >nul && goto :running

set "ODD=0"
for /f "usebackq tokens=1,*" %%A in ("%MOD%\game_build_3598.sha256") do call :check_game %%A "%%B"
if not "%ODD%"=="0" goto :wrong_game

call :known_network
if errorlevel 1 goto :foreign_network
if not exist "%MOD%\state\" mkdir "%MOD%\state" || goto :failed
call :backup_if_foreign "root\programs\opgame.progpack" "%MOD%\files\root\programs\opgame.progpack" || goto :failed
call :backup_if_foreign "root\programs\tacgame.progpack" "%MOD%\files\root\programs\tacgame.progpack" || goto :failed
call :backup_if_foreign "mp_join.exe" "%MOD%\files\mp_join.exe" || goto :failed
call :backup_if_foreign "mp_vitals.dll" "%MOD%\files\mp_vitals.dll" || goto :failed

rem Copy dependencies before the module/program pack that asks the loader for them.
call :copy_checked "%MOD%\files\mp_vitals.dll" "%GAME%\mp_vitals.dll" "%VITALS%" || goto :failed
if not exist "%GAME%\bin\rel\x64\" mkdir "%GAME%\bin\rel\x64" || goto :failed
call :copy_checked "%MOD%\files\bin\rel\x64\basic_network.codelib" "%GAME%\bin\rel\x64\basic_network.codelib" "%NETWORK%" || goto :failed
call :copy_checked "%MOD%\files\mp_join.exe" "%GAME%\mp_join.exe" "%JOIN%" || goto :failed
if not exist "%GAME%\root\programs\" mkdir "%GAME%\root\programs" || goto :failed
call :copy_checked "%MOD%\files\root\programs\opgame.progpack" "%GAME%\root\programs\opgame.progpack" "%OPGAME%" || goto :failed
call :copy_checked "%MOD%\files\root\programs\tacgame.progpack" "%GAME%\root\programs\tacgame.progpack" "%TACGAME%" || goto :failed
if not exist "%GAME%\steam_appid.txt" (
    call :copy_checked "%MOD%\files\steam_appid.txt" "%GAME%\steam_appid.txt" "%APPID%" || goto :failed
    >"%MOD%\state\steam_appid_added.txt" echo MiusMultiplayer created steam_appid.txt.
)
>"%MOD%\state\enabled.txt" echo MiusMultiplayer v1.0.0 is enabled in this game.

echo  Installed and SHA-256 verified:
echo    basic_network.codelib %NETWORK%
echo    mp_vitals.dll          %VITALS%
echo    mp_join.exe            %JOIN%
echo    opgame.progpack        %OPGAME%
echo    tacgame.progpack       %TACGAME%
echo  Both players need this same archive. Check both basic_network_*.log files for "vitals:".
echo  Back to the original game: Disable_Multiplayer.bat
goto :finish

:check_game
set "H="
if not exist "%GAME%\%~2" goto :check_odd
for /f "skip=1 delims=" %%X in ('certutil -hashfile "%GAME%\%~2" SHA256 2^>nul') do if not defined H set "H=%%X"
if defined H set "H=%H: =%"
if /i "%H%"=="%~1" exit /b 0
:check_odd
set /a ODD+=1
echo    not as expected: %~2
exit /b 0

:known_network
if not exist "%GAME%\bin\rel\x64\basic_network.codelib" exit /b 0
set "H="
for /f "skip=1 delims=" %%X in ('certutil -hashfile "%GAME%\bin\rel\x64\basic_network.codelib" SHA256 2^>nul') do if not defined H set "H=%%X"
if defined H set "H=%H: =%"
for %%X in ("%V20%" "%NETWORK%" "%NETWORK_PREVIOUS%" "%NETWORK_OLDER%" "%NETWORK_NOSTART%" "%NETWORK_NOSTART_PREVIOUS%" "%NETWORK_NOSTART_OLDER%" "%NETWORK_NOREBASE%") do if /i "%H%"==%%~X exit /b 0
echo  Existing basic_network.codelib is unknown: %H%
exit /b 1

:backup_if_foreign
if not exist "%GAME%\%~1" exit /b 0
fc /b "%~2" "%GAME%\%~1" >nul 2>nul && exit /b 0
if exist "%MOD%\state\backup\%~1" exit /b 0
for %%I in ("%MOD%\state\backup\%~1") do if not exist "%%~dpI" mkdir "%%~dpI" || exit /b 1
copy /y "%GAME%\%~1" "%MOD%\state\backup\%~1" >nul || exit /b 1
echo    saved existing %~1 in MiusMultiplayer\state\backup
exit /b 0

:copy_checked
copy /y "%~1" "%~2" >nul || exit /b 1
set "H="
for /f "skip=1 delims=" %%X in ('certutil -hashfile "%~2" SHA256 2^>nul') do if not defined H set "H=%%X"
if defined H set "H=%H: =%"
if /i "%H%"=="%~3" exit /b 0
echo  SHA-256 verification failed for %~2
exit /b 1

:no_game
echo  This folder is not inside the game folder. Put the whole MiusMultiplayer folder next to starter_x64.exe.
set "RC=3"
goto :finish
:incomplete
echo  The MiusMultiplayer folder is incomplete. Extract the whole archive again.
set "RC=1"
goto :finish
:running
echo  The game is running. Close it, then run this again.
set "RC=4"
goto :finish
:wrong_game
echo  This kit requires the final Steam game version 6.00 build 3598. Nothing was changed.
set "RC=2"
goto :finish
:foreign_network
echo  Nothing was changed: do not overwrite an unknown multiplayer module.
set "RC=2"
goto :finish
:failed
echo  A file could not be copied or verified. Close the game and run this file again.
set "RC=1"
:finish
echo.
if not defined QUIET pause
exit /b %RC%

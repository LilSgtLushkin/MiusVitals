@echo off
rem MiusVitals v7.1.0: operation-map and Vitals synchronization for the Steam release (v20) of the Mius-Front multiplayer.
rem Puts two files into the game: mp_vitals.dll (game folder) and the exact checked basic_network.codelib
rem rebase for Steam 3598/1. The original is kept in state\ and Uninstall_Vitals.bat puts it back.
rem Both players must do this. Put the MiusVitals folder into the game folder (next to mpmod1).
setlocal EnableExtensions DisableDelayedExpansion
set "KIT=%~dp0"
set "KIT=%KIT:~0,-1%"
for %%I in ("%KIT%\..") do set "GAME=%%~fI"
set "V20=8108BD1E9AF8080EF8B1050512B8311848F5A336ACB49A7479388733B4AE5723"
set "PATCHED=485B655662BD062E16278F9D699A3097B49A08DE1948D2500DD589703A3102A3"
set "PATCHED_PREVIOUS=4C91874C4B62DB424ECCA36586121BCB3B4CC7849CC900285E68DE5151EDDC36"
set "PATCHED_OLDER=7CAF36CDDABF7EB388A3C6C393B05733E051955D63D32EF13531CE88004DCFBA"
set "PATCHED_NOSTART=E4317E630A21198A55E0E86BE4AA29C71FCADA6065824C858ACD0E2BDEF8ADF9"
set "PATCHED_NOSTART_PREVIOUS=F0766BEC48696869D59D4A654A04F20BD7BAF9F73ADB6FBF139E7DA31DFC1F4F"
set "PATCHED_NOSTART_OLDER=2FB049B9B465DE2E05BD9BBE7D5573A772319CC4D65FB20BB146E648A7CDF8EC"
set "PATCHED_OLD=D2E34581B42D0573547E221873D7538B64A413267E8ED388ABEB46DD42B630D2"
rem   Install_Vitals.bat norebase  puts the module without the address corrections for build 3598/1 (only if the game crashes with the normal one)
rem   Install_Vitals.bat nostart  the same, but the Start button is left as in the original (use it if the battle cannot be started with the normal one)
set "MOD=%KIT%\files\basic_network.codelib"
set "TARGET_SHA=%PATCHED%"
if /i "%~1"=="nostart" set "MOD=%KIT%\files\alt\basic_network_nostart.codelib"
if /i "%~1"=="nostart" set "TARGET_SHA=%PATCHED_NOSTART%"
if /i "%~1"=="norebase" set "MOD=%KIT%\files\alt\basic_network_v5_norebase.codelib"
if /i "%~1"=="norebase" set "TARGET_SHA=%PATCHED_OLD%"
set "RC=0"
echo.
echo  MiusVitals v7.1.0 - operation-map and Vitals synchronization
echo.
if not exist "%GAME%\starter_x64.exe" goto :no_game
if not exist "%GAME%\bin\rel\x64\basic_network.codelib" goto :no_mp
if not exist "%KIT%\files\mp_vitals.dll" goto :incomplete
if not exist "%MOD%" goto :incomplete
tasklist /fi "imagename eq starter_x64.exe" /nh 2>nul | find /i "starter_x64.exe" >nul && goto :running

set "H="
for /f "skip=1 delims=" %%X in ('certutil -hashfile "%GAME%\bin\rel\x64\basic_network.codelib" SHA256 2^>nul') do if not defined H set "H=%%X"
if defined H set "H=%H: =%"
if /i "%H%"=="%PATCHED%" goto :install
if /i "%H%"=="%PATCHED_PREVIOUS%" goto :install
if /i "%H%"=="%PATCHED_OLDER%" goto :install
if /i "%H%"=="%PATCHED_NOSTART%" goto :install
if /i "%H%"=="%PATCHED_NOSTART_PREVIOUS%" goto :install
if /i "%H%"=="%PATCHED_NOSTART_OLDER%" goto :install
if /i "%H%"=="%PATCHED_OLD%" goto :install
if /i "%H%"=="%V20%" goto :backup
goto :unknown

:backup
if not exist "%KIT%\state\" mkdir "%KIT%\state" || goto :failed
copy /y "%GAME%\bin\rel\x64\basic_network.codelib" "%KIT%\state\basic_network.codelib.v20" >nul || goto :failed

:install
rem (the DLL first, the module that asks for it last: a copy that stops half-way leaves a working game)
copy /y "%KIT%\files\mp_vitals.dll" "%GAME%\mp_vitals.dll" >nul || goto :failed
copy /y "%MOD%" "%GAME%\bin\rel\x64\basic_network.codelib" >nul || goto :failed
set "AFTER="
for /f "skip=1 delims=" %%X in ('certutil -hashfile "%GAME%\bin\rel\x64\basic_network.codelib" SHA256 2^>nul') do if not defined AFTER set "AFTER=%%X"
if defined AFTER set "AFTER=%AFTER: =%"
if /i not "%AFTER%"=="%TARGET_SHA%" goto :failed
echo  Installed and SHA-256 verified: %TARGET_SHA%
echo  Start the game as usual; the other player needs the same variant and SHA-256.
echo  In basic_network_*.log look for the lines that begin with "vitals:".
echo  Back to the original: Uninstall_Vitals.bat
goto :finish

:no_game
echo  This folder is not inside the game folder. Put the whole MiusVitals folder into the folder
echo  that contains starter_x64.exe (next to mpmod1) and run this again.
echo  It is now in: "%GAME%"
set "RC=3"
goto :finish

:no_mp
echo  The multiplayer is not enabled in this game: run mpmod1\Enable_Multiplayer.bat first.
set "RC=3"
goto :finish

:incomplete
echo  The MiusVitals folder is incomplete: copy the whole folder again.
set "RC=1"
goto :finish

:running
echo  The game is running. Close it, then run this again.
set "RC=4"
goto :finish

:unknown
echo  The installed basic_network.codelib is neither the Steam release v20 nor this patched one
echo  (SHA-256 %H%).
echo  Nothing was changed. This kit is made for the v20 module only.
set "RC=2"
goto :finish

:failed
echo  A file could not be copied. Is the game still running, or is the folder write-protected?
echo  Run Uninstall_Vitals.bat to take it out again, or this file again to complete it.
set "RC=1"
goto :finish

:finish
echo.
pause
exit /b %RC%

@echo off
rem MiusVitals v7.5.0: tactical Vitals state-policy correction for the Steam v20 Mius-Front multiplayer.
rem Puts two files into the game: mp_vitals.dll (game folder) and the checked basic_network.codelib
rem rebase for Steam 3598/1. Both are force-replaced: no hash or allow-list check can stop the copy,
rem and a failed copy of one file does not stop the other. The original module is kept in state\.
setlocal EnableExtensions DisableDelayedExpansion
set "KIT=%~dp0"
set "KIT=%KIT:~0,-1%"
for %%I in ("%KIT%\..") do set "GAME=%%~fI"
set "V20=8108BD1E9AF8080EF8B1050512B8311848F5A336ACB49A7479388733B4AE5723"
set "DLL_SHA=71A1BF3A37923BBE6C8EE84448D0B91705C2BFF7382B647F990B9D671EE224DF"
set "TARGET_SHA=485B655662BD062E16278F9D699A3097B49A08DE1948D2500DD589703A3102A3"
rem   Install_Vitals.bat norebase  puts the module without the address corrections for build 3598/1 (only if the game crashes with the normal one)
rem   Install_Vitals.bat nostart  the same, but the Start button is left as in the original (use it if the battle cannot be started with the normal one)
set "MOD=%KIT%\files\basic_network.codelib"
if /i "%~1"=="nostart" set "MOD=%KIT%\files\alt\basic_network_nostart.codelib"
if /i "%~1"=="nostart" set "TARGET_SHA=E4317E630A21198A55E0E86BE4AA29C71FCADA6065824C858ACD0E2BDEF8ADF9"
if /i "%~1"=="norebase" set "MOD=%KIT%\files\alt\basic_network_v5_norebase.codelib"
if /i "%~1"=="norebase" set "TARGET_SHA=D2E34581B42D0573547E221873D7538B64A413267E8ED388ABEB46DD42B630D2"
set "RC=0"
set "FAILN=0"
echo.
echo  MiusVitals v7.5.0 - tactical Vitals state-policy correction
echo.
if not exist "%GAME%\starter_x64.exe" goto :no_game
if not exist "%KIT%\files\mp_vitals.dll" goto :incomplete
if not exist "%MOD%" goto :incomplete
tasklist /fi "imagename eq starter_x64.exe" /nh 2>nul | find /i "starter_x64.exe" >nul && goto :running
if not exist "%GAME%\bin\rel\x64\basic_network.codelib" echo  Note: this game has no bin\rel\x64\basic_network.codelib yet; it is created now.
if not exist "%KIT%\state" mkdir "%KIT%\state" >nul 2>nul
if exist "%GAME%\bin\rel\x64\basic_network.codelib" if not exist "%KIT%\state\basic_network.codelib.previous" copy /y "%GAME%\bin\rel\x64\basic_network.codelib" "%KIT%\state\basic_network.codelib.previous" >nul 2>nul
if exist "%KIT%\state\basic_network.codelib.previous" echo    saved the previous basic_network.codelib in MiusVitals\state

rem ---- the DLL first, the module that asks for it last; each copy is retried and never hash-gated
call :put "%KIT%\files\mp_vitals.dll" "%GAME%\mp_vitals.dll" "mp_vitals.dll"
call :put "%MOD%" "%GAME%\bin\rel\x64\basic_network.codelib" "bin\rel\x64\basic_network.codelib"
if not "%FAILN%"=="0" goto :failed
>"%KIT%\state\installed.txt" (
    echo mp_vitals.dll
    echo bin\rel\x64\basic_network.codelib
)
echo  Replaced (SHA-256 values are informational only):
echo    basic_network.codelib %TARGET_SHA%
echo    mp_vitals.dll          %DLL_SHA%
echo  Start the game as usual; the other player needs this same main/nostart variant.
echo  In BOTH basic_network_*.log files look for the lines that begin with "vitals:"; an absent line means that machine did not load this kit.
if not exist "%GAME%\out" mkdir "%GAME%\out" >nul 2>nul
echo  Back to the original: Uninstall_Vitals.bat
goto :finish

:put
if not exist "%~1" (
    echo  MISSING IN KIT: %~3
    set /a FAILN+=1
    exit /b 1
)
for %%I in ("%~2") do if not exist "%%~dpI" mkdir "%%~dpI" >nul 2>nul
attrib -r -s -h "%~2" >nul 2>nul
del /f /q "%~2" >nul 2>nul
copy /y "%~1" "%~2" >nul 2>nul
if not errorlevel 1 if exist "%~2" exit /b 0
attrib -r -s -h "%~2" >nul 2>nul
copy /y "%~1" "%~2" >nul 2>nul
if not errorlevel 1 if exist "%~2" exit /b 0
copy /y "%~1" "%~2.put" >nul 2>nul
if not exist "%~2.put" goto :put_error
del /f /q "%~2" >nul 2>nul
move /y "%~2.put" "%~2" >nul 2>nul
if exist "%~2" exit /b 0
:put_error
echo  COULD NOT WRITE %~2
set /a FAILN+=1
exit /b 1

:no_game
echo  This folder is not inside the game folder. Put the whole MiusVitals folder into the folder
echo  that contains starter_x64.exe and run this again.
echo  It is now in: "%GAME%"
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
:failed
echo  A file could not be copied. Is the game still running, or is the folder write-protected?
echo  Run this file again to complete the replacement.
set "RC=1"
:finish
echo.
pause
exit /b %RC%

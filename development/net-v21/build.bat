@echo off
rem Build the network module from this tree.  Needs the Visual Studio 2022 C++ tools.
rem
rem   build.bat baseline   the published v12 sources, untouched - the M1 reference build
rem   build.bat module     the same sources plus the Steam layer - the M2 build
rem   build.bat selftest   the Steam layer's self-test, no game and no Steam needed
rem   build.bat all        all three
rem
rem Every module link writes a .map: the DLL carries no symbol table, and the map is what makes the
rem comparison against v12/v20 readable (tools/compare.py --candidate-map).
setlocal

if /i "%~1"=="baseline" goto tools
if /i "%~1"=="module" goto tools
if /i "%~1"=="selftest" goto tools
if /i "%~1"=="all" goto tools
echo usage: build.bat baseline^|module^|selftest^|all
exit /b 2

:tools
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
if defined VCToolsInstallDir goto have_tools
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
if defined VCToolsInstallDir goto have_tools
call "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
if defined VCToolsInstallDir goto have_tools
for /f "usebackq delims=" %%I in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do call "%%I\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
:have_tools
if not defined VCToolsInstallDir (
    echo build.bat: no Visual Studio C++ tools found
    exit /b 1
)
where cl
cd /d "%~dp0"

set BASE=basic_network.cpp session.cpp ui.cpp state.cpp statesync.cpp shots.cpp trench.cpp xlate.cpp
set STEAM=steam.cpp steam_module.cpp

if /i "%~1"=="baseline" goto baseline
if /i "%~1"=="module" goto module
if /i "%~1"=="selftest" goto selftest

:all
call "%~f0" baseline || exit /b 1
call "%~f0" module || exit /b 1
call "%~f0" selftest || exit /b 1
exit /b 0

:baseline
if not exist build\baseline mkdir build\baseline
cl /nologo /LD /MD /O1 /EHa /W3 /Brepro /Fo"build\baseline\\" /Fd"build\baseline\\" src\%BASE% /link user32.lib gdi32.lib ws2_32.lib ole32.lib oleaut32.lib /OUT:build\baseline\basic_network.codelib /IMPLIB:build\baseline\basic_network.lib /MAP:build\baseline\basic_network.map
exit /b %errorlevel%

:module
if not exist build\module mkdir build\module
cl /nologo /LD /MD /O1 /EHa /W3 /Brepro /Fo"build\module\\" /Fd"build\module\\" src\%BASE% src\%STEAM% /link user32.lib gdi32.lib ws2_32.lib ole32.lib oleaut32.lib /OUT:build\module\basic_network.codelib /IMPLIB:build\module\basic_network.lib /MAP:build\module\basic_network.map
exit /b %errorlevel%

:selftest
if not exist build\selftest mkdir build\selftest
cl /nologo /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" src\steam.cpp src\steam_selftest.cpp /link ws2_32.lib /OUT:build\selftest\steam_selftest.exe
exit /b %errorlevel%

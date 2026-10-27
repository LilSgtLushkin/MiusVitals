@echo off
rem Build the network module from this tree.  Needs the Visual Studio 2022 C++ tools.
rem
rem   build.bat baseline   the published v12 sources, untouched - the M1 reference build
rem   build.bat module     the same sources plus the Steam layer - the M2 build
rem   build.bat selftest   the Steam layer's and the run log's self-tests, no game and no Steam needed
rem   build.bat join       the command-line join launcher (mp_join.exe), with Steam
rem   build.bat all        all three
rem
rem Every module link writes a .map: the DLL carries no symbol table, and the map is what makes the
rem comparison against v12/v20 readable (tools/compare.py --candidate-map).
setlocal

if /i "%~1"=="baseline" goto tools
if /i "%~1"=="module" goto tools
if /i "%~1"=="selftest" goto tools
if /i "%~1"=="join" goto tools
if /i "%~1"=="all" goto tools
echo usage: build.bat baseline^|module^|selftest^|join^|all
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

rem Our tree: the published sources copied into src\ and changed where we need them.
set BASE=src\basic_network.cpp src\netlog.cpp src\hooks.cpp src\render_guard.cpp src\session.cpp src\ui.cpp src\state.cpp src\statesync.cpp src\statesync_legacy.cpp src\shots.cpp src\trench.cpp src\xlate.cpp
set STEAM=src\steam.cpp src\steam_module.cpp
rem The published sources untouched: the baseline is built from these, not from src\, so M1 keeps
rem measuring the toolchain instead of our edits. development\upstream\ is the frozen copy.
set PUBLISHED=..\upstream\miusplayer-public-43b2736\basic_network
set BASELINE=%PUBLISHED%\basic_network.cpp %PUBLISHED%\session.cpp %PUBLISHED%\ui.cpp %PUBLISHED%\state.cpp %PUBLISHED%\statesync.cpp %PUBLISHED%\shots.cpp %PUBLISHED%\trench.cpp %PUBLISHED%\xlate.cpp

if /i "%~1"=="baseline" goto baseline
if /i "%~1"=="module" goto module
if /i "%~1"=="selftest" goto selftest
if /i "%~1"=="join" goto join

:all
call "%~f0" baseline || exit /b 1
call "%~f0" module || exit /b 1
call "%~f0" selftest || exit /b 1
call "%~f0" join || exit /b 1
exit /b 0

:baseline
if not exist build\baseline mkdir build\baseline
cl /nologo /LD /MD /O1 /EHa /W3 /Brepro /Fo"build\baseline\\" /Fd"build\baseline\\" %BASELINE% /link user32.lib gdi32.lib ws2_32.lib ole32.lib oleaut32.lib /OUT:build\baseline\basic_network.codelib /IMPLIB:build\baseline\basic_network.lib /MAP:build\baseline\basic_network.map
exit /b %errorlevel%

:module
if not exist build\module mkdir build\module
cl /nologo /LD /MD /O1 /EHa /W3 /Brepro /Fo"build\module\\" /Fd"build\module\\" %BASE% %STEAM% /link user32.lib gdi32.lib ws2_32.lib ole32.lib oleaut32.lib /OUT:build\module\basic_network.codelib /IMPLIB:build\module\basic_network.lib /MAP:build\module\basic_network.map
exit /b %errorlevel%

:selftest
if not exist build\selftest mkdir build\selftest
cl /nologo /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" src\steam.cpp src\steam_logger.cpp src\steam_selftest.cpp /link ws2_32.lib /OUT:build\selftest\steam_selftest.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" src\steam_friends_test.cpp /link /OUT:build\selftest\steam_friends_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" src\netlog.cpp src\netlog_selftest.cpp /link /OUT:build\selftest\netlog_selftest.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" src\protocol_selftest.cpp /link /OUT:build\selftest\protocol_selftest.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" src\force_test.cpp /link /OUT:build\selftest\force_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" src\battle_file_test.cpp /link /OUT:build\selftest\battle_file_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" src\retry_gate_test.cpp /link /OUT:build\selftest\retry_gate_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" src\atomic_file_selftest.cpp /link /OUT:build\selftest\atomic_file_selftest.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" src\reliable_inbox_test.cpp /link /OUT:build\selftest\reliable_inbox_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" src\ceasefire_test.cpp /link /OUT:build\selftest\ceasefire_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" src\env_value_test.cpp /link /OUT:build\selftest\env_value_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" src\tactical_mode_test.cpp /link /OUT:build\selftest\tactical_mode_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" src\transform_write_test.cpp /link /OUT:build\selftest\transform_write_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" src\transform_gc_test.cpp /link /OUT:build\selftest\transform_gc_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" src\transform_send_test.cpp /link /OUT:build\selftest\transform_send_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" src\transform_inbox_test.cpp /link /OUT:build\selftest\transform_inbox_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" src\transport_policy_test.cpp /link /OUT:build\selftest\transport_policy_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" src\peer_guard_test.cpp /link /OUT:build\selftest\peer_guard_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" src\state_fragments_test.cpp /link /OUT:build\selftest\state_fragments_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /MD /O1 /EHsc /W3 /DNETV21_TEST_HOOKS /Fo"build\selftest\\" /Fd"build\selftest\\" src\statesync.cpp src\statesync_legacy.cpp src\statesync_integration_test.cpp /link /OUT:build\selftest\statesync_integration_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /MD /O1 /EHsc /W3 /DNETV21_TEST_HOOKS /Fo"build\selftest\\" /Fd"build\selftest\\" src\shots.cpp src\hooks.cpp src\shots_integration_test.cpp /link /OUT:build\selftest\shots_integration_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /MD /O1 /EHsc /W3 /DNETV21_TEST_HOOKS /Fo"build\selftest\\" /Fd"build\selftest\\" src\hooks.cpp src\hooks_selftest.cpp /link /OUT:build\selftest\hooks_selftest.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /MD /O1 /EHsc /W3 /DNETV21_TEST_HOOKS /Fo"build\selftest\\" /Fd"build\selftest\\" src\xlate.cpp src\hooks.cpp src\xlate_selftest.cpp /link /OUT:build\selftest\xlate_selftest.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /MD /O1 /EHa /W3 /DNETV21_TEST_HOOKS /Fo"build\selftest\\" /Fd"build\selftest\\" src\render_guard.cpp src\hooks.cpp src\render_guard_test.cpp /link /OUT:build\selftest\render_guard_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" src\loopback_peer.cpp /link ws2_32.lib /OUT:build\selftest\loopback_peer.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /MD /O1 /EHa /W3 /DNETV21_TEST_HOOKS /Fo"build\selftest\\" /Fd"build\selftest\\" src\session.cpp src\hooks.cpp src\steam.cpp src\steam_module.cpp src\session_loopback_peer.cpp /link ws2_32.lib ole32.lib oleaut32.lib user32.lib /OUT:build\selftest\session_loopback_peer.exe
exit /b %errorlevel%

:join
rem The launcher the same way the author builds it (/MT), plus the Steam layer, so joining by
rem "steam:<id>" works from the command line as well as from the menu.
if not exist build\join mkdir build\join
cl /nologo /MT /O1 /EHsc /W3 /Fo"build\join\\" /Fd"build\join\\" src\mp_join.cpp src\steam.cpp src\steam_logger.cpp /link ws2_32.lib /OUT:build\join\mp_join.exe
exit /b %errorlevel%

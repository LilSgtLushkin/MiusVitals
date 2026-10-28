@echo off
rem Build the network module from this tree.  Needs the Visual Studio 2022 C++ tools.
rem
rem   build.bat baseline   the published v12 sources, untouched - the M1 reference build
rem   build.bat module     the same sources plus the Steam layer - the M2 build
rem   build.bat selftest   the Steam layer's and the run log's self-tests, no game and no Steam needed
rem   build.bat render_test  only the renderer guard test (fast feedback)
rem   build.bat join       the command-line join launcher (mp_join.exe), with Steam
rem   build.bat all        all three
rem
rem Every module link writes a .map: the DLL carries no symbol table, and the map is what makes the
rem comparison against v12/v20 readable (tools/compare.py --candidate-map).
setlocal

if /i "%~1"=="baseline" goto tools
if /i "%~1"=="module" goto tools
if /i "%~1"=="selftest" goto tools
if /i "%~1"=="render_test" goto tools
if /i "%~1"=="join" goto tools
if /i "%~1"=="all" goto tools
echo usage: build.bat baseline^|module^|selftest^|render_test^|join^|all
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
set BASE=src\basic_network.cpp src\netlog.cpp src\hooks.cpp src\render_guard.cpp src\vitals.cpp src\hit_sync.cpp src\anim_sync.cpp src\session.cpp src\ui.cpp src\state.cpp src\statesync.cpp src\statesync_legacy.cpp src\shots.cpp src\trench.cpp src\xlate.cpp
set STEAM=src\steam.cpp src\steam_module.cpp
rem The published sources untouched: the baseline is built from these, not from src\, so M1 keeps
rem measuring the toolchain instead of our edits. development\upstream\ is the frozen copy.
set PUBLISHED=..\upstream\miusplayer-public-43b2736\basic_network
set BASELINE=%PUBLISHED%\basic_network.cpp %PUBLISHED%\session.cpp %PUBLISHED%\ui.cpp %PUBLISHED%\state.cpp %PUBLISHED%\statesync.cpp %PUBLISHED%\shots.cpp %PUBLISHED%\trench.cpp %PUBLISHED%\xlate.cpp

if /i "%~1"=="baseline" goto baseline
if /i "%~1"=="module" goto module
if /i "%~1"=="selftest" goto selftest
if /i "%~1"=="render_test" goto render_test
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
cl /nologo /LD /MD /O1 /EHa /W3 /Brepro /Fo"build\module\\" /Fd"build\module\\" %BASE% %STEAM% /link /Brepro user32.lib gdi32.lib ws2_32.lib ole32.lib oleaut32.lib /OUT:build\module\basic_network.codelib /IMPLIB:build\module\basic_network.lib /MAP:build\module\basic_network.map
exit /b %errorlevel%

:selftest
if not exist build\selftest mkdir build\selftest
cl /nologo /Isrc /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" src\steam.cpp src\steam_logger.cpp tests\steam_selftest.cpp /link ws2_32.lib /OUT:build\selftest\steam_selftest.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" tests\steam_friends_test.cpp /link /OUT:build\selftest\steam_friends_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" src\netlog.cpp tests\netlog_selftest.cpp /link /OUT:build\selftest\netlog_selftest.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" tests\vitals_core_test.cpp /link /OUT:build\selftest\vitals_core_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" tests\vitals_randomized_test.cpp /link /OUT:build\selftest\vitals_randomized_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" tests\hit_anim_wire_test.cpp /link /OUT:build\selftest\hit_anim_wire_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" tests\animation_channel_test.cpp /link /OUT:build\selftest\animation_channel_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" tests\pause_sync_test.cpp /link /OUT:build\selftest\pause_sync_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc /MD /O1 /EHa /W3 /DNETV21_TEST_HOOKS /Fo"build\selftest\\" /Fd"build\selftest\\" src\hit_sync.cpp src\anim_sync.cpp src\hooks.cpp tests\hit_anim_integration_test.cpp /link /OUT:build\selftest\hit_anim_integration_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc /MD /O1 /EHa /W3 /DNETV21_TEST_HOOKS /Fo"build\selftest\\" /Fd"build\selftest\\" src\vitals.cpp src\hooks.cpp tests\vitals_integration_test.cpp /link /OUT:build\selftest\vitals_integration_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" tests\protocol_selftest.cpp /link /OUT:build\selftest\protocol_selftest.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" tests\operation_turn_test.cpp /link /OUT:build\selftest\operation_turn_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" tests\opsave_checked_test.cpp /link /OUT:build\selftest\opsave_checked_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" tests\map_file_test.cpp /link /OUT:build\selftest\map_file_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" tests\map_file_windows_test.cpp /link /OUT:build\selftest\map_file_windows_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" tests\force_test.cpp /link /OUT:build\selftest\force_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" tests\battle_file_test.cpp /link /OUT:build\selftest\battle_file_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" tests\retry_gate_test.cpp /link /OUT:build\selftest\retry_gate_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" tests\atomic_file_selftest.cpp /link /OUT:build\selftest\atomic_file_selftest.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" tests\reliable_inbox_test.cpp /link /OUT:build\selftest\reliable_inbox_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" tests\ceasefire_test.cpp /link /OUT:build\selftest\ceasefire_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" tests\env_value_test.cpp /link /OUT:build\selftest\env_value_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" tests\tactical_mode_test.cpp /link /OUT:build\selftest\tactical_mode_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" tests\transform_write_test.cpp /link /OUT:build\selftest\transform_write_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" tests\transform_gc_test.cpp /link /OUT:build\selftest\transform_gc_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" tests\state_apply_delay_test.cpp /link /OUT:build\selftest\state_apply_delay_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" tests\squad_ownership_test.cpp /link /OUT:build\selftest\squad_ownership_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" tests\image_mirror_identity_test.cpp /link /OUT:build\selftest\image_mirror_identity_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" tests\transform_send_test.cpp /link /OUT:build\selftest\transform_send_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" tests\transform_inbox_test.cpp /link /OUT:build\selftest\transform_inbox_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" tests\transport_policy_test.cpp /link /OUT:build\selftest\transport_policy_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" tests\peer_guard_test.cpp /link /OUT:build\selftest\peer_guard_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" tests\state_fragments_test.cpp /link /OUT:build\selftest\state_fragments_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc /MD /O1 /EHsc /W3 /DNETV21_TEST_HOOKS /Fo"build\selftest\\" /Fd"build\selftest\\" src\statesync.cpp src\statesync_legacy.cpp tests\statesync_integration_test.cpp /link /OUT:build\selftest\statesync_integration_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc /MD /O1 /EHa /W3 /DNETV21_TEST_HOOKS /Fo"build\selftest\\" /Fd"build\selftest\\" src\state.cpp src\hooks.cpp tests\state_memory_test.cpp /link /OUT:build\selftest\state_memory_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc /MD /O1 /EHsc /W3 /DNETV21_TEST_HOOKS /Fo"build\selftest\\" /Fd"build\selftest\\" src\shots.cpp src\hooks.cpp tests\shots_integration_test.cpp /link /OUT:build\selftest\shots_integration_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /MD /O1 /EHsc /W3 /DNETV21_TEST_HOOKS /Fo"build\selftest\\" /Fd"build\selftest\\" src\shots.cpp src\hooks.cpp src\named_shots_integration_test.cpp /link /OUT:build\selftest\named_shots_integration_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc /MD /O1 /EHsc /W3 /DNETV21_TEST_HOOKS /Fo"build\selftest\\" /Fd"build\selftest\\" src\shots.cpp src\xlate.cpp src\hooks.cpp tests\shots_xlate_characterization_test.cpp /link /OUT:build\selftest\shots_xlate_characterization_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc /MD /O1 /EHsc /W3 /DNETV21_TEST_HOOKS /Fo"build\selftest\\" /Fd"build\selftest\\" src\hooks.cpp tests\hooks_selftest.cpp /link /OUT:build\selftest\hooks_selftest.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc /MD /O1 /EHsc /W3 /DNETV21_TEST_HOOKS /Fo"build\selftest\\" /Fd"build\selftest\\" src\xlate.cpp src\hooks.cpp tests\xlate_selftest.cpp /link /OUT:build\selftest\xlate_selftest.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc /MD /O1 /EHsc /W3 /DNETV21_TEST_HOOKS /Fo"build\selftest\\" /Fd"build\selftest\\" src\trench.cpp src\hooks.cpp tests\trench_thread_test.cpp /link /OUT:build\selftest\trench_thread_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc /MD /O1 /EHa /W3 /DNETV21_TEST_HOOKS /Fo"build\selftest\\" /Fd"build\selftest\\" src\render_guard.cpp src\hooks.cpp tests\render_guard_test.cpp /link /OUT:build\selftest\render_guard_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc /LD /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" tests\late_input_fixture.cpp /link user32.lib /OUT:build\selftest\late_input_fixture.dll /IMPLIB:build\selftest\late_input_fixture.lib
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" tests\late_input_test.cpp /link user32.lib /OUT:build\selftest\late_input_test.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" tests\starter_fixture.cpp /link /OUT:build\selftest\starter_fixture.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc /MD /O1 /EHsc /W3 /Fo"build\selftest\\" /Fd"build\selftest\\" tests\loopback_peer.cpp /link ws2_32.lib /OUT:build\selftest\loopback_peer.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc /MD /O1 /EHa /W3 /DNETV21_TEST_HOOKS /Fo"build\selftest\\" /Fd"build\selftest\\" src\session.cpp src\vitals.cpp src\hooks.cpp src\steam.cpp src\steam_module.cpp tests\session_loopback_peer.cpp /link ws2_32.lib ole32.lib oleaut32.lib user32.lib /OUT:build\selftest\session_loopback_peer.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc /MD /O1 /EHa /W3 /DNETV21_TEST_HOOKS /Fo"build\selftest\\" /Fd"build\selftest\\" src\session.cpp src\vitals.cpp src\hooks.cpp src\steam.cpp src\steam_module.cpp src\shots.cpp src\hit_sync.cpp src\anim_sync.cpp tests\session_event_peer.cpp /link ws2_32.lib ole32.lib oleaut32.lib user32.lib /OUT:build\selftest\session_event_peer.exe
exit /b %errorlevel%

:render_test
if not exist build\render_test mkdir build\render_test
cl /nologo /Isrc /MD /O1 /EHa /W3 /DNETV21_TEST_HOOKS /Fo"build\render_test\\" /Fd"build\render_test\\" src\render_guard.cpp src\hooks.cpp tests\render_guard_test.cpp /link /OUT:build\render_test\render_guard_test.exe
exit /b %errorlevel%

:join
rem The launcher the same way the author builds it (/MT), plus the Steam layer, so joining by
rem "steam:<id>" works from the command line as well as from the menu.
if not exist build\join mkdir build\join
cl /nologo /MT /O1 /EHsc /W3 /Brepro /Fo"build\join\\" /Fd"build\join\\" src\mp_join.cpp src\steam.cpp src\steam_logger.cpp /link /Brepro ws2_32.lib /OUT:build\join\mp_join.exe
exit /b %errorlevel%

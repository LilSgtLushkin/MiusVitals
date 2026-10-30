@echo off
rem Build the network module from this tree.  Needs the C++ tools of Visual Studio 2026 (folder 18,
rem toolset v145) or 2022 (folder 2022, v143); the newest one found wins.
rem
rem   build.bat baseline   the published v12 sources, untouched - the M1 reference build
rem   build.bat module     the same sources plus the Steam layer - the M2 build
rem   build.bat selftest   the Steam layer's and the run log's self-tests, no game and no Steam needed
rem   build.bat render_test  only the renderer guard test (fast feedback)
rem   build.bat join       the command-line join launcher (mp_join.exe), with Steam
rem   build.bat all        all three
rem
rem A second argument chooses the configuration. Release is the default and keeps the exact flags
rem the CI has always used; debug only turns optimization off, adds symbols and defines
rem NETV21_DEBUG. Debug output goes to build\<target>-debug\, so both can sit side by side.
rem
rem   build.bat module            release
rem   build.bat module debug      /Od /Zi /MD, symbols, build\module-debug\
rem   build.bat module release    the same as no second argument
rem
rem The debug build still links the release C runtime on purpose: the module is loaded by the
rem game, and the game's process has no reason to carry the debug runtime DLLs.
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
rem Look for the compiler in this order: Visual Studio 2026 (folder 18, toolset v145, cl 14.5x),
rem then Visual Studio 2022 (folder 2022, v143), then whatever vswhere finds. VS 2026 lives under
rem the version number, not the year: Microsoft Visual Studio\18\<edition>.
rem If the environment is already set up - the script runs inside an "x64 Native Tools Command
rem Prompt" - it is kept: we do not overwrite the choice with another vcvars. To force one:
rem     set MIUS_VS=C:\Program Files\Microsoft Visual Studio\18\Community
if defined VCToolsInstallDir goto have_tools
if defined MIUS_VS call "%MIUS_VS%\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
if defined VCToolsInstallDir goto have_tools
for %%Y in (18 2022 2019) do for %%E in (Community Professional Enterprise BuildTools) do if not defined VCToolsInstallDir if exist "%ProgramFiles%\Microsoft Visual Studio\%%Y\%%E\VC\Auxiliary\Build\vcvars64.bat" call "%ProgramFiles%\Microsoft Visual Studio\%%Y\%%E\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
if defined VCToolsInstallDir goto have_tools
for /f "usebackq delims=" %%I in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do call "%%I\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
:have_tools
if not defined VCToolsInstallDir (
    echo build.bat: no Visual Studio C++ tools found
    exit /b 1
)
echo build.bat: MSVC %VCToolsVersion% from %VCToolsInstallDir%
where cl
cd /d "%~dp0"

rem Configuration: release keeps the CI flags exactly; debug only adds symbols and slows down.
set CONFIG=%2
if not defined CONFIG set CONFIG=release
if /i "%CONFIG%"=="debug" goto cfg_debug
if /i "%CONFIG%"=="release" goto cfg_release
echo build.bat: configuration must be release or debug
exit /b 2
:cfg_release
set CL_MD=/MD /O1
set CL_BREPRO=/Brepro
set CL_T=/MD /O1
set LINK_DBG=
set SUFFIX=
goto cfg_done
:cfg_debug
set CL_MD=/Od /MD /Zi /DNETV21_DEBUG
set CL_BREPRO=
set CL_T=/Od /MD /Zi /DNETV21_DEBUG
set LINK_DBG=/DEBUG
set SUFFIX=-debug
goto cfg_done
:cfg_done

rem Our tree: the published sources copied into src\ and changed where we need them.
set BASE=src\basic_network.cpp src\netlog.cpp src\hooks.cpp src\render_guard.cpp src\vitals.cpp src\hit_sync.cpp src\anim_sync.cpp src\session.cpp src\ui.cpp src\state.cpp src\statesync.cpp src\statesync_legacy.cpp src\shots.cpp src\trench.cpp src\xlate.cpp
set STEAM=src\steam.cpp src\steam_module.cpp
rem The published sources untouched: the baseline is built from these, not from src\, so M1 keeps
rem measuring the toolchain instead of our edits. development\upstream\ is the frozen copy.
set PUBLISHED=..\..\upstream\miusplayer-public-43b2736\basic_network
set BASELINE=%PUBLISHED%\basic_network.cpp %PUBLISHED%\session.cpp %PUBLISHED%\ui.cpp %PUBLISHED%\state.cpp %PUBLISHED%\statesync.cpp %PUBLISHED%\shots.cpp %PUBLISHED%\trench.cpp %PUBLISHED%\xlate.cpp

if /i "%~1"=="baseline" goto baseline
if /i "%~1"=="module" goto module
if /i "%~1"=="selftest" goto selftest
if /i "%~1"=="render_test" goto render_test
if /i "%~1"=="join" goto join

:all
call "%~f0" baseline %CONFIG% || exit /b 1
call "%~f0" module %CONFIG% || exit /b 1
call "%~f0" selftest %CONFIG% || exit /b 1
call "%~f0" join %CONFIG% || exit /b 1
exit /b 0

:baseline
if not exist build\baseline%SUFFIX% mkdir build\baseline%SUFFIX%
cl /nologo /LD %CL_MD% /EHa /W3 %CL_BREPRO% /Fo"build\baseline%SUFFIX%\\" /Fd"build\baseline%SUFFIX%\\" %BASELINE% /link user32.lib gdi32.lib ws2_32.lib ole32.lib oleaut32.lib /OUT:build\baseline%SUFFIX%\basic_network.codelib /IMPLIB:build\baseline%SUFFIX%\basic_network.lib /MAP:build\baseline%SUFFIX%\basic_network.map %LINK_DBG%
exit /b %errorlevel%

:module
if not exist build\module%SUFFIX% mkdir build\module%SUFFIX%
cl /nologo /LD %CL_MD% /EHa /W3 %CL_BREPRO% /Fo"build\module%SUFFIX%\\" /Fd"build\module%SUFFIX%\\" %BASE% %STEAM% /link /Brepro %LINK_DBG% user32.lib gdi32.lib ws2_32.lib ole32.lib oleaut32.lib /OUT:build\module%SUFFIX%\basic_network.codelib /IMPLIB:build\module%SUFFIX%\basic_network.lib /MAP:build\module%SUFFIX%\basic_network.map %LINK_DBG%
exit /b %errorlevel%

:selftest
if not exist build\selftest%SUFFIX% mkdir build\selftest%SUFFIX%
cl /nologo /Isrc %CL_T% /EHsc /W3 /Fo"build\selftest%SUFFIX%\\" /Fd"build\selftest%SUFFIX%\\" src\steam.cpp src\steam_logger.cpp tests\steam_selftest.cpp /link ws2_32.lib /OUT:build\selftest%SUFFIX%\steam_selftest.exe %LINK_DBG%
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc %CL_T% /EHsc /W3 /Fo"build\selftest%SUFFIX%\\" /Fd"build\selftest%SUFFIX%\\" tests\steam_friends_test.cpp /link /OUT:build\selftest%SUFFIX%\steam_friends_test.exe %LINK_DBG%
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc %CL_T% /EHsc /W3 /Fo"build\selftest%SUFFIX%\\" /Fd"build\selftest%SUFFIX%\\" src\netlog.cpp tests\netlog_selftest.cpp /link /OUT:build\selftest%SUFFIX%\netlog_selftest.exe %LINK_DBG%
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc %CL_T% /EHsc /W3 /Fo"build\selftest%SUFFIX%\\" /Fd"build\selftest%SUFFIX%\\" tests\vitals_core_test.cpp /link /OUT:build\selftest%SUFFIX%\vitals_core_test.exe %LINK_DBG%
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc %CL_T% /EHsc /W3 /Fo"build\selftest%SUFFIX%\\" /Fd"build\selftest%SUFFIX%\\" tests\vitals_randomized_test.cpp /link /OUT:build\selftest%SUFFIX%\vitals_randomized_test.exe %LINK_DBG%
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc %CL_T% /EHsc /W3 /Fo"build\selftest%SUFFIX%\\" /Fd"build\selftest%SUFFIX%\\" tests\hit_anim_wire_test.cpp /link /OUT:build\selftest%SUFFIX%\hit_anim_wire_test.exe %LINK_DBG%
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc %CL_T% /EHsc /W3 /Fo"build\selftest%SUFFIX%\\" /Fd"build\selftest%SUFFIX%\\" tests\animation_channel_test.cpp /link /OUT:build\selftest%SUFFIX%\animation_channel_test.exe %LINK_DBG%
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc %CL_T% /EHsc /W3 /Fo"build\selftest%SUFFIX%\\" /Fd"build\selftest%SUFFIX%\\" tests\pause_sync_test.cpp /link /OUT:build\selftest%SUFFIX%\pause_sync_test.exe %LINK_DBG%
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc %CL_T% /EHa /W3 /DNETV21_TEST_HOOKS /Fo"build\selftest%SUFFIX%\\" /Fd"build\selftest%SUFFIX%\\" src\hit_sync.cpp src\anim_sync.cpp src\hooks.cpp tests\hit_anim_integration_test.cpp /link /OUT:build\selftest%SUFFIX%\hit_anim_integration_test.exe %LINK_DBG%
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc %CL_T% /EHa /W3 /DNETV21_TEST_HOOKS /Fo"build\selftest%SUFFIX%\\" /Fd"build\selftest%SUFFIX%\\" src\vitals.cpp src\hooks.cpp tests\vitals_integration_test.cpp /link /OUT:build\selftest%SUFFIX%\vitals_integration_test.exe %LINK_DBG%
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc %CL_T% /EHsc /W3 /Fo"build\selftest%SUFFIX%\\" /Fd"build\selftest%SUFFIX%\\" tests\protocol_selftest.cpp /link /OUT:build\selftest%SUFFIX%\protocol_selftest.exe %LINK_DBG%
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc %CL_T% /EHsc /W3 /Fo"build\selftest%SUFFIX%\\" /Fd"build\selftest%SUFFIX%\\" tests\operation_turn_test.cpp /link /OUT:build\selftest%SUFFIX%\operation_turn_test.exe %LINK_DBG%
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc %CL_T% /EHsc /W3 /Fo"build\selftest%SUFFIX%\\" /Fd"build\selftest%SUFFIX%\\" tests\opsave_checked_test.cpp /link /OUT:build\selftest%SUFFIX%\opsave_checked_test.exe %LINK_DBG%
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc %CL_T% /EHsc /W3 /Fo"build\selftest%SUFFIX%\\" /Fd"build\selftest%SUFFIX%\\" tests\map_file_test.cpp /link /OUT:build\selftest%SUFFIX%\map_file_test.exe %LINK_DBG%
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc %CL_T% /EHsc /W3 /Fo"build\selftest%SUFFIX%\\" /Fd"build\selftest%SUFFIX%\\" tests\map_file_windows_test.cpp /link /OUT:build\selftest%SUFFIX%\map_file_windows_test.exe %LINK_DBG%
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc %CL_T% /EHsc /W3 /Fo"build\selftest%SUFFIX%\\" /Fd"build\selftest%SUFFIX%\\" tests\force_test.cpp /link /OUT:build\selftest%SUFFIX%\force_test.exe %LINK_DBG%
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc %CL_T% /EHsc /W3 /Fo"build\selftest%SUFFIX%\\" /Fd"build\selftest%SUFFIX%\\" tests\battle_file_test.cpp /link /OUT:build\selftest%SUFFIX%\battle_file_test.exe %LINK_DBG%
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc %CL_T% /EHsc /W3 /Fo"build\selftest%SUFFIX%\\" /Fd"build\selftest%SUFFIX%\\" tests\retry_gate_test.cpp /link /OUT:build\selftest%SUFFIX%\retry_gate_test.exe %LINK_DBG%
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc %CL_T% /EHsc /W3 /Fo"build\selftest%SUFFIX%\\" /Fd"build\selftest%SUFFIX%\\" tests\atomic_file_selftest.cpp /link /OUT:build\selftest%SUFFIX%\atomic_file_selftest.exe %LINK_DBG%
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc %CL_T% /EHsc /W3 /Fo"build\selftest%SUFFIX%\\" /Fd"build\selftest%SUFFIX%\\" tests\reliable_inbox_test.cpp /link /OUT:build\selftest%SUFFIX%\reliable_inbox_test.exe %LINK_DBG%
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc %CL_T% /EHsc /W3 /Fo"build\selftest%SUFFIX%\\" /Fd"build\selftest%SUFFIX%\\" tests\ceasefire_test.cpp /link /OUT:build\selftest%SUFFIX%\ceasefire_test.exe %LINK_DBG%
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc %CL_T% /EHsc /W3 /Fo"build\selftest%SUFFIX%\\" /Fd"build\selftest%SUFFIX%\\" tests\env_value_test.cpp /link /OUT:build\selftest%SUFFIX%\env_value_test.exe %LINK_DBG%
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc %CL_T% /EHsc /W3 /Fo"build\selftest%SUFFIX%\\" /Fd"build\selftest%SUFFIX%\\" tests\tactical_mode_test.cpp /link /OUT:build\selftest%SUFFIX%\tactical_mode_test.exe %LINK_DBG%
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc %CL_T% /EHsc /W3 /Fo"build\selftest%SUFFIX%\\" /Fd"build\selftest%SUFFIX%\\" tests\transform_write_test.cpp /link /OUT:build\selftest%SUFFIX%\transform_write_test.exe %LINK_DBG%
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc %CL_T% /EHsc /W3 /Fo"build\selftest%SUFFIX%\\" /Fd"build\selftest%SUFFIX%\\" tests\transform_gc_test.cpp /link /OUT:build\selftest%SUFFIX%\transform_gc_test.exe %LINK_DBG%
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc %CL_T% /EHsc /W3 /Fo"build\selftest%SUFFIX%\\" /Fd"build\selftest%SUFFIX%\\" tests\transform_object_check_test.cpp /link /OUT:build\selftest%SUFFIX%\transform_object_check_test.exe %LINK_DBG%
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc %CL_T% /EHsc /W3 /Fo"build\selftest%SUFFIX%\\" /Fd"build\selftest%SUFFIX%\\" tests\state_apply_delay_test.cpp /link /OUT:build\selftest%SUFFIX%\state_apply_delay_test.exe %LINK_DBG%
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc %CL_T% /EHsc /W3 /Fo"build\selftest%SUFFIX%\\" /Fd"build\selftest%SUFFIX%\\" tests\squad_ownership_test.cpp /link /OUT:build\selftest%SUFFIX%\squad_ownership_test.exe %LINK_DBG%
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc %CL_T% /EHsc /W3 /Fo"build\selftest%SUFFIX%\\" /Fd"build\selftest%SUFFIX%\\" tests\image_mirror_identity_test.cpp /link /OUT:build\selftest%SUFFIX%\image_mirror_identity_test.exe %LINK_DBG%
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc %CL_T% /EHsc /W3 /Fo"build\selftest%SUFFIX%\\" /Fd"build\selftest%SUFFIX%\\" tests\transform_send_test.cpp /link /OUT:build\selftest%SUFFIX%\transform_send_test.exe %LINK_DBG%
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc %CL_T% /EHsc /W3 /Fo"build\selftest%SUFFIX%\\" /Fd"build\selftest%SUFFIX%\\" tests\transform_inbox_test.cpp /link /OUT:build\selftest%SUFFIX%\transform_inbox_test.exe %LINK_DBG%
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc %CL_T% /EHsc /W3 /Fo"build\selftest%SUFFIX%\\" /Fd"build\selftest%SUFFIX%\\" tests\transport_policy_test.cpp /link /OUT:build\selftest%SUFFIX%\transport_policy_test.exe %LINK_DBG%
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc %CL_T% /EHsc /W3 /Fo"build\selftest%SUFFIX%\\" /Fd"build\selftest%SUFFIX%\\" tests\peer_guard_test.cpp /link /OUT:build\selftest%SUFFIX%\peer_guard_test.exe %LINK_DBG%
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc %CL_T% /EHsc /W3 /Fo"build\selftest%SUFFIX%\\" /Fd"build\selftest%SUFFIX%\\" tests\state_fragments_test.cpp /link /OUT:build\selftest%SUFFIX%\state_fragments_test.exe %LINK_DBG%
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc %CL_T% /EHsc /W3 /DNETV21_TEST_HOOKS /Fo"build\selftest%SUFFIX%\\" /Fd"build\selftest%SUFFIX%\\" src\statesync.cpp src\statesync_legacy.cpp tests\statesync_integration_test.cpp /link /OUT:build\selftest%SUFFIX%\statesync_integration_test.exe %LINK_DBG%
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc %CL_T% /EHa /W3 /DNETV21_TEST_HOOKS /Fo"build\selftest%SUFFIX%\\" /Fd"build\selftest%SUFFIX%\\" src\state.cpp src\hooks.cpp tests\state_memory_test.cpp /link /OUT:build\selftest%SUFFIX%\state_memory_test.exe %LINK_DBG%
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc %CL_T% /EHsc /W3 /DNETV21_TEST_HOOKS /Fo"build\selftest%SUFFIX%\\" /Fd"build\selftest%SUFFIX%\\" src\shots.cpp src\hooks.cpp tests\shots_integration_test.cpp /link /OUT:build\selftest%SUFFIX%\shots_integration_test.exe %LINK_DBG%
if errorlevel 1 exit /b %errorlevel%
cl /nologo /MD /O1 /EHsc /W3 /DNETV21_TEST_HOOKS /Fo"build\selftest%SUFFIX%\\" /Fd"build\selftest%SUFFIX%\\" src\shots.cpp src\hooks.cpp src\named_shots_integration_test.cpp /link /OUT:build\selftest%SUFFIX%\named_shots_integration_test.exe %LINK_DBG%
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc %CL_T% /EHsc /W3 /DNETV21_TEST_HOOKS /Fo"build\selftest%SUFFIX%\\" /Fd"build\selftest%SUFFIX%\\" src\shots.cpp src\xlate.cpp src\hooks.cpp tests\shots_xlate_characterization_test.cpp /link /OUT:build\selftest%SUFFIX%\shots_xlate_characterization_test.exe %LINK_DBG%
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc %CL_T% /EHsc /W3 /DNETV21_TEST_HOOKS /Fo"build\selftest%SUFFIX%\\" /Fd"build\selftest%SUFFIX%\\" src\hooks.cpp tests\hooks_selftest.cpp /link /OUT:build\selftest%SUFFIX%\hooks_selftest.exe %LINK_DBG%
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc %CL_T% /EHsc /W3 /DNETV21_TEST_HOOKS /Fo"build\selftest%SUFFIX%\\" /Fd"build\selftest%SUFFIX%\\" src\xlate.cpp src\hooks.cpp tests\xlate_selftest.cpp /link /OUT:build\selftest%SUFFIX%\xlate_selftest.exe %LINK_DBG%
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc %CL_T% /EHsc /W3 /DNETV21_TEST_HOOKS /Fo"build\selftest%SUFFIX%\\" /Fd"build\selftest%SUFFIX%\\" src\trench.cpp src\hooks.cpp tests\trench_thread_test.cpp /link /OUT:build\selftest%SUFFIX%\trench_thread_test.exe %LINK_DBG%
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc %CL_T% /EHa /W3 /DNETV21_TEST_HOOKS /Fo"build\selftest%SUFFIX%\\" /Fd"build\selftest%SUFFIX%\\" src\render_guard.cpp src\hooks.cpp tests\render_guard_test.cpp /link /OUT:build\selftest%SUFFIX%\render_guard_test.exe %LINK_DBG%
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc /LD /MD /O1 /EHsc /W3 /Fo"build\selftest%SUFFIX%\\" /Fd"build\selftest%SUFFIX%\\" tests\late_input_fixture.cpp /link user32.lib /OUT:build\selftest%SUFFIX%\late_input_fixture.dll /IMPLIB:build\selftest%SUFFIX%\late_input_fixture.lib %LINK_DBG%
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc %CL_T% /EHsc /W3 /Fo"build\selftest%SUFFIX%\\" /Fd"build\selftest%SUFFIX%\\" tests\late_input_test.cpp /link user32.lib /OUT:build\selftest%SUFFIX%\late_input_test.exe %LINK_DBG%
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc %CL_T% /EHsc /W3 /Fo"build\selftest%SUFFIX%\\" /Fd"build\selftest%SUFFIX%\\" tests\starter_fixture.cpp /link /OUT:build\selftest%SUFFIX%\starter_fixture.exe %LINK_DBG%
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc %CL_T% /EHsc /W3 /Fo"build\selftest%SUFFIX%\\" /Fd"build\selftest%SUFFIX%\\" tests\loopback_peer.cpp /link ws2_32.lib /OUT:build\selftest%SUFFIX%\loopback_peer.exe %LINK_DBG%
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc %CL_T% /EHa /W3 /DNETV21_TEST_HOOKS /Fo"build\selftest%SUFFIX%\\" /Fd"build\selftest%SUFFIX%\\" src\session.cpp src\vitals.cpp src\hooks.cpp src\steam.cpp src\steam_module.cpp tests\session_loopback_peer.cpp /link ws2_32.lib ole32.lib oleaut32.lib user32.lib /OUT:build\selftest%SUFFIX%\session_loopback_peer.exe %LINK_DBG%
if errorlevel 1 exit /b %errorlevel%
cl /nologo /Isrc %CL_T% /EHa /W3 /DNETV21_TEST_HOOKS /Fo"build\selftest%SUFFIX%\\" /Fd"build\selftest%SUFFIX%\\" src\session.cpp src\vitals.cpp src\hooks.cpp src\steam.cpp src\steam_module.cpp src\shots.cpp src\hit_sync.cpp src\anim_sync.cpp tests\session_event_peer.cpp /link ws2_32.lib ole32.lib oleaut32.lib user32.lib /OUT:build\selftest%SUFFIX%\session_event_peer.exe %LINK_DBG%
if errorlevel 1 exit /b %errorlevel%
rem The pause/state-header slice compiles real functions out of session.cpp; it needs the same
rem compiler this script has just put on PATH, so it runs here and not from the workflow.
python tools\pause_session_slice_selftest.py
exit /b %errorlevel%

:render_test
if not exist build\render_test%SUFFIX% mkdir build\render_test%SUFFIX%
cl /nologo /Isrc %CL_MD% /EHa /W3 /DNETV21_TEST_HOOKS /Fo"build\render_test%SUFFIX%\\" /Fd"build\render_test%SUFFIX%\\" src\render_guard.cpp src\hooks.cpp tests\render_guard_test.cpp /link /OUT:build\render_test%SUFFIX%\render_guard_test.exe %LINK_DBG%
exit /b %errorlevel%

:join
rem The launcher the same way the author builds it (/MT), plus the Steam layer, so joining by
rem "steam:<id>" works from the command line as well as from the menu.
if not exist build\join%SUFFIX% mkdir build\join%SUFFIX%
cl /nologo /MT /O1 /EHsc /W3 /Brepro /Fo"build\join%SUFFIX%\\" /Fd"build\join%SUFFIX%\\" src\mp_join.cpp src\steam.cpp src\steam_logger.cpp /link /Brepro %LINK_DBG% ws2_32.lib /OUT:build\join%SUFFIX%\mp_join.exe %LINK_DBG%
exit /b %errorlevel%

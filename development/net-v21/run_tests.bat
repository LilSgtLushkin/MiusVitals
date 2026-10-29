@echo off
rem Run the self-tests that build.bat has just produced. Same list as the CI job, so a green
rem run here means a green run there.
rem
rem   run_tests.bat            build\selftest        (release)
rem   run_tests.bat debug      build\selftest-debug
rem   run_tests.bat release
setlocal enabledelayedexpansion
set CONFIG=%1
if not defined CONFIG set CONFIG=release
if /i "%CONFIG%"=="debug" goto cfg_debug
if /i "%CONFIG%"=="release" goto cfg_release
echo run_tests.bat: configuration must be release or debug
exit /b 2
:cfg_release
set SUFFIX=
goto cfg_done
:cfg_debug
set SUFFIX=-debug
goto cfg_done
:cfg_done

set DIR=build\selftest%SUFFIX%
if not exist "%DIR%" (
    echo run_tests.bat: %DIR% does not exist - run build.bat selftest %CONFIG% first
    exit /b 1
)
set PASS=0
set FAILN=0
set MISSING=0

call :run %DIR%\steam_selftest.exe
call :run %DIR%\steam_friends_test.exe
call :run %DIR%\netlog_selftest.exe
call :run %DIR%\vitals_core_test.exe
call :run %DIR%\vitals_randomized_test.exe
call :run %DIR%\hit_anim_wire_test.exe
call :run %DIR%\animation_channel_test.exe
call :run %DIR%\pause_sync_test.exe
call :run %DIR%\hit_anim_integration_test.exe
call :run %DIR%\vitals_integration_test.exe
call :run %DIR%\protocol_selftest.exe
call :run %DIR%\operation_turn_test.exe
call :run %DIR%\opsave_checked_test.exe
call :run %DIR%\map_file_test.exe
call :run %DIR%\map_file_windows_test.exe
call :run %DIR%\force_test.exe
call :run %DIR%\battle_file_test.exe
call :run %DIR%\retry_gate_test.exe
call :run %DIR%\atomic_file_selftest.exe
call :run %DIR%\reliable_inbox_test.exe
call :run %DIR%\ceasefire_test.exe
call :run %DIR%\env_value_test.exe
call :run %DIR%\tactical_mode_test.exe
call :run %DIR%\transform_write_test.exe
call :run %DIR%\transform_gc_test.exe
call :run %DIR%\transform_object_check_test.exe
call :run %DIR%\state_apply_delay_test.exe
call :run %DIR%\squad_ownership_test.exe
call :run %DIR%\image_mirror_identity_test.exe
call :run %DIR%\transform_send_test.exe
call :run %DIR%\transform_inbox_test.exe
call :run %DIR%\transport_policy_test.exe
call :run %DIR%\peer_guard_test.exe
call :run %DIR%\state_fragments_test.exe
call :run %DIR%\statesync_integration_test.exe
call :run %DIR%\state_memory_test.exe
call :run %DIR%\hooks_selftest.exe
call :run %DIR%\render_guard_test.exe
call :run %DIR%\xlate_selftest.exe
call :run %DIR%\trench_thread_test.exe
call :run %DIR%\shots_integration_test.exe
call :run %DIR%\named_shots_integration_test.exe
call :run %DIR%\shots_xlate_characterization_test.exe
call :run %DIR%\late_input_test.exe

echo.
echo passed %PASS%, failed %FAILN%, missing %MISSING%
if not "%FAILN%"=="0" exit /b 1
if not "%MISSING%"=="0" exit /b 1
exit /b 0

:run
if not exist "%1" (
    echo   MISSING %1
    set /a MISSING+=1
    exit /b 0
)
"%1" >nul 2>&1
if errorlevel 1 (
    echo   FAIL    %1
    set /a FAILN+=1
) else (
    echo   ok      %1
    set /a PASS+=1
)
exit /b 0

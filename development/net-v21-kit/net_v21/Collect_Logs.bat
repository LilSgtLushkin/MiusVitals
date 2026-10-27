@echo off
rem Gather battle and module diagnostics, plus hashes of the installed files. No binary is copied.
setlocal EnableExtensions DisableDelayedExpansion
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0Collect_Logs.ps1"
set "RESULT=%ERRORLEVEL%"
echo.
pause
exit /b %RESULT%

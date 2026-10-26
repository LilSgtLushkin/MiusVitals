@echo off
setlocal
cd /d "%~dp0"
echo GTMP_alpha - collecting one report file...
echo.
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0Collect_Logs.ps1" -Mod "%~dp0."
if errorlevel 1 (
  echo.
  echo The report could not be created. Send this window's text instead.
  pause
  exit /b 1
)
echo.
echo One file is enough:
echo   %~dp0GTMP_alpha_log.txt
echo.
pause

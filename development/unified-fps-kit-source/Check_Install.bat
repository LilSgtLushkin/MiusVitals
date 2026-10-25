@echo off
setlocal
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0Check_Install.ps1"
set "RC=%ERRORLEVEL%"
echo.
pause
exit /b %RC%

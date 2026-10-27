@echo off
rem Restore files saved before the unified 1.0.5 test installation; also recover an interrupted install.
setlocal EnableExtensions DisableDelayedExpansion
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0Apply_Multiplayer.ps1" -Action Uninstall
set "RC=%ERRORLEVEL%"
if /i not "%~1"=="/quiet" pause
exit /b %RC%

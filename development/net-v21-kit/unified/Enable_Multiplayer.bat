@echo off
rem One operation installs the complete 1.0.5 test mod, including net_v21 and its launcher.
setlocal EnableExtensions DisableDelayedExpansion
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0Apply_Multiplayer.ps1" -Action Install
set "RC=%ERRORLEVEL%"
if /i not "%~1"=="/quiet" pause
exit /b %RC%

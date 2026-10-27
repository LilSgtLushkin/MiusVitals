@echo off
setlocal EnableExtensions DisableDelayedExpansion
call "%~dp0Switch_Pair.bat" back
set "RC=%ERRORLEVEL%"
if /i not "%~1"=="/quiet" pause
exit /b %RC%

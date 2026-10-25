@echo off
rem Does not change anything: compares the game's files with build 3598/1 and writes build_report.txt
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0Check_Build.ps1"
echo.
pause

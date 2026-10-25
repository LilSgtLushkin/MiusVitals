@echo off
rem Read-only beta_FPS_fix codelib report.
setlocal EnableExtensions DisableDelayedExpansion
set "MOD=%~dp0"
set "MOD=%MOD:~0,-1%"
for %%I in ("%MOD%\..") do set "GAME=%%~fI"
set "CANDIDATE=78E2718383BC0679154118415189D1C1EE05ED0179A0A924455C22CB4AE2916E"
echo.
echo  MiusMultiplayer beta_FPS_fix candidate report
if not exist "%GAME%\starter_x64.exe" (
  echo  ERROR: put this folder next to starter_x64.exe.
  exit /b 3
)
if not exist "%GAME%\bin\rel\x64\basic_network.codelib" (
  echo  MISSING: bin\rel\x64\basic_network.codelib
  exit /b 1
)
set "H="
for /f "skip=1 delims=" %%X in ('certutil -hashfile "%GAME%\bin\rel\x64\basic_network.codelib" SHA256 2^>nul') do if not defined H set "H=%%X"
if defined H set "H=%H: =%"
echo  Active basic_network.codelib SHA-256: %H%
if /i "%H%"=="%CANDIDATE%" (
  echo  BETA FPS candidate is active.
  echo  Record FPS and both basic_network_tacgame logs, then use Disable_Beta_FPS_Fix.bat to restore the previous module.
  exit /b 0
)
echo  BETA FPS candidate is not active.
exit /b 2

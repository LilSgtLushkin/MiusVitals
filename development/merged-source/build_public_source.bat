@echo off
rem Build the pinned public MiusPlayer source for analysis/porting only.
rem Its output is NOT an installable MiusVitals codelib: see README.md.
setlocal EnableExtensions
set "HERE=%~dp0"
for %%I in ("%HERE%..\..") do set "ROOT=%%~fI"
set "PYTHON=python"

%PYTHON% "%HERE%verify_workspace.py" || exit /b 1
call "%ROOT%\development\upstream\miusplayer-public-43b2736\basic_network\build.bat"
if errorlevel 1 exit /b 1

set "CANDIDATE=%ROOT%\development\upstream\miusplayer-public-43b2736\basic_network\build\basic_network.codelib"
for /f "skip=1 delims=" %%X in ('certutil -hashfile "%CANDIDATE%" SHA256 2^>nul') do if not defined H set "H=%%X"
if defined H set "H=%H: =%"
echo.
echo  Public-source candidate SHA-256: %H%
echo  It is for source analysis only. Do NOT install it or run patch_import.py on it.
echo  A production codelib needs a new reviewed binary mapping and two-peer validation.

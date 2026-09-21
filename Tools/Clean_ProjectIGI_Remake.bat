@echo off
setlocal EnableExtensions EnableDelayedExpansion
title ProjectIGI_Remake - Clean Rebuild

cd /d "%~dp0\.."

echo ============================================================
echo   ProjectIGI_Remake - Unreal Engine Clean Rebuild
echo ============================================================
echo.
echo This script removes generated build/cache files so the
echo project can be compiled again from scratch.
echo.

tasklist /FI "IMAGENAME eq UnrealEditor.exe" | find /I "UnrealEditor.exe" >nul
if not errorlevel 1 (
    echo [ERROR] UnrealEditor.exe is still running.
    echo Close Unreal Editor and run this script again.
    pause
    exit /b 1
)

tasklist /FI "IMAGENAME eq devenv.exe" | find /I "devenv.exe" >nul
if not errorlevel 1 (
    echo [WARNING] Visual Studio is still running.
    choice /C YN /N /M "Continue anyway? [Y/N]: "
    if errorlevel 2 exit /b 1
)

choice /C YN /N /M "Start clean rebuild preparation? [Y/N]: "
if errorlevel 2 exit /b 0

echo.
echo [1/8] Removing project Binaries...
if exist "Binaries" rmdir /S /Q "Binaries"

echo [2/8] Removing project Intermediate...
if exist "Intermediate" rmdir /S /Q "Intermediate"

echo [3/8] Removing DerivedDataCache...
if exist "DerivedDataCache" rmdir /S /Q "DerivedDataCache"

echo [4/8] Removing Visual Studio cache...
if exist ".vs" rmdir /S /Q ".vs"

echo [5/8] Removing generated solution/project files...
for %%F in (*.sln *.suo *.VC.db *.VC.opendb) do (
    if exist "%%F" del /F /Q "%%F"
)

echo [6/8] Removing plugin Binaries / Intermediate...
if exist "Plugins" (
    for /D %%P in ("Plugins\*") do (
        if exist "%%~fP\Binaries" rmdir /S /Q "%%~fP\Binaries"
        if exist "%%~fP\Intermediate" rmdir /S /Q "%%~fP\Intermediate"
    )
)

echo [7/8] Cleaning common build artifacts...
for /R %%F in (*.pdb *.ilk *.exp *.lib.response *.obj.response) do (
    if exist "%%F" del /F /Q "%%F" >nul 2>&1
)

echo [8/8] Optional Saved cleanup...
choice /C YN /N /M "Delete Saved folder too? [Y/N]: "
if errorlevel 2 goto SkipSaved
if exist "Saved" rmdir /S /Q "Saved"

:SkipSaved
echo.
echo ============================================================
echo   CLEAN COMPLETE
echo ============================================================
echo.
echo Next:
echo   1. Generate Visual Studio project files
echo   2. Build Development Editor / Win64
echo   3. Launch Unreal Engine 5.8
echo.
pause
endlocal

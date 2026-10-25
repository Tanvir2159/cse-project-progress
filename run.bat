@echo off
cd /d "%~dp0"
if not exist gamehub.exe (
    call build.bat
    if errorlevel 1 exit /b 1
) else (
    gamehub.exe %*
)


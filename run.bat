@echo off
cd /d "%~dp0"
if not exist gamehub.exe (
    echo Building gamehub.exe...
    gcc -Wall -Wextra -std=c99 -Iinclude -I"include/raylib-6.0_win64_mingw-w64/include" -L"include/raylib-6.0_win64_mingw-w64/lib" -o gamehub.exe src/main.c src/util.c src/g_candy.c -lraylib -lopengl32 -lgdi32 -lwinmm
    if errorlevel 1 (
        echo Build failed.
        pause
        exit /b 1
    )
)
gamehub.exe %*

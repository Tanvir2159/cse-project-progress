@echo off
cd /d "%~dp0"
set RL=include\raylib-6.0_win64_mingw-w64
if not exist "%RL%\include\raylib.h" (
  echo.
  echo  Put raylib WIN64 here:
  echo    %CD%\%RL%\include\raylib.h
  echo    %CD%\%RL%\lib\libraylib.a
  echo.
  pause
  exit /b 1
)
echo Compiling GAME HUB...
gcc -Wall -Wextra -std=c99 -Iinclude -I"%RL%\include" -L"%RL%\lib" -o gamehub.exe ^
  src/main.c src/util.c src/gfx.c src/auth.c src/leaderboard.c src/rating.c ^
  src/g_snake.c src/g_candy.c src/g_golf.c src/g_parkour.c ^
  -lraylib -lopengl32 -lgdi32 -lwinmm
if errorlevel 1 (
  echo BUILD FAILED
  pause
  exit /b 1
)
echo BUILD OK
echo Run: gamehub.exe
gamehub.exe

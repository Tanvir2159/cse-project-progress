@echo off
gcc -Wall -Wextra -std=c99 -Iinclude -I"include/raylib-6.0_win64_mingw-w64/include" -L"include/raylib-6.0_win64_mingw-w64/lib" -o gamehub.exe src/main.c src/util.c src/g_candy.c -lraylib -lopengl32 -lgdi32 -lwinmm
if %errorlevel%==0 (
  echo build ok
  gamehub.exe
) else (
  echo build failed
  pause
)

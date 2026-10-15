GAME HUB — progress build (for demo)

WHAT WORKS
  login / register
  shell: help, ls, status, clear/neofetch, exit
  candy (graphics + terminal mode if display_mode set in file)
  rating after candy

STUB
  settings  — lists planned options, not editable yet

NOT IN THIS BUILD
  golf, snake, ludo

BUILD
  make project
  make run

BUILD (manual Windows + raylib win64)
  gcc -Wall -Wextra -std=c99 -Iinclude -I"include/raylib-6.0_win64_mingw-w64/include" -L"include/raylib-6.0_win64_mingw-w64/lib" -o gamehub.exe src/main.c src/util.c src/g_candy.c -lraylib -lopengl32 -lgdi32 -lwinmm
  .\gamehub.exe

DEMO FLOW
  1. register or login
  2. help / ls / neofetch
  3. candy
  4. settings (show stub)
  5. exit

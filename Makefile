# GAME HUB — CSE 1102
# make project   OR   mingw32-make project

CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99 -Iinclude
SRC     = src/main.c src/util.c src/gfx.c src/auth.c src/leaderboard.c src/rating.c \
          src/g_snake.c src/g_candy.c src/g_golf.c src/g_parkour.c

ifeq ($(OS),Windows_NT)
  RAY     = include/raylib-6.0_win64_mingw-w64
  CFLAGS += -I"$(RAY)/include"
  LDFLAGS = -L"$(RAY)/lib" -lraylib -lopengl32 -lgdi32 -lwinmm
  TARGET  = gamehub.exe
else
  UNAME_S := $(shell uname -s)
  ifeq ($(UNAME_S),Darwin)
    CFLAGS  += -I/opt/homebrew/include -I/usr/local/include
    LDFLAGS  = -L/opt/homebrew/lib -L/usr/local/lib -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
  else
    LDFLAGS = -lraylib -lm -lpthread -ldl
  endif
  TARGET = gamehub
endif

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC) include/hub.h
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

clean:
	rm -f gamehub gamehub.exe

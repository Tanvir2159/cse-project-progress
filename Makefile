# GAME HUB — progress build
# Sir runs:  make project
#
# Other targets:
#   make          same as make project (build only)
#   make run      build then run
#   make clean    remove executable
#   make LINUX=1  build on Linux with system raylib

CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99 -Iinclude
SRC     = src/main.c src/util.c src/g_candy.c

ifeq ($(LINUX),1)
  LDFLAGS = -lraylib -lm -lpthread -ldl
  TARGET  = gamehub
else
  RAYINC  = include/raylib-6.0_win64_mingw-w64/include
  RAYLIB  = include/raylib-6.0_win64_mingw-w64/lib
  CFLAGS += -I"$(RAYINC)"
  LDFLAGS = -L"$(RAYLIB)" -lraylib -lopengl32 -lgdi32 -lwinmm
  TARGET  = gamehub.exe
endif

.PHONY: all project run clean

all: project

project: $(TARGET)
	@echo Build complete: $(TARGET)

$(TARGET): $(SRC) include/hub.h
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

run: project
	./$(TARGET)

clean:
	rm -f gamehub gamehub.exe

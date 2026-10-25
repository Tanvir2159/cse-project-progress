# GAME HUB

C99 terminal shell (`hub-sh`) featuring raylib mini-games, user authentication, persistent scoring, leaderboards, and rating systems.

## Mini-Games

- `snake` — Classic grid snake (supports 1P / 2P modes)
- `candy` — Match-3 puzzle game
- `golf` — 3-hole mini-golf
- `parkour` — Endless runner obstacle course

## Shell Commands

| Command | Alias | Description |
|---|---|---|
| `help` | `man` | List available commands |
| `neofetch` | `fetch` | Display GameHub system card and player profile |
| `ls` | — | List game rooms and data storage files |
| `status` | — | Display player win counts and personal best scores |
| `leaderboard` | `lb` | View global top scores |
| `ratings` | `avg` | Display average community ratings for each game |
| `settings` | — | Configure session theme and candy color count |
| `passwd` | — | Update account password |
| `union` | — | Demonstrate C union memory layout (viva topic) |
| `clear` | `cls` | Clear terminal and redraw banner |
| `exit` | `quit` | Save progress and log out |
| `snake`, `candy`, `golf`, `parkour` | `1`, `2`, `3`, `4` | Launch corresponding raylib game window |

## Build & Run

### macOS / Linux

**Prerequisites:**
- macOS: `brew install raylib`
- Debian/Ubuntu: `sudo apt install libraylib-dev`

**Run:**
```bash
./run
# or
make run
```

### Windows

**Prerequisites:**
- MinGW-w64 GCC (raylib 6.0 headers and libraries are bundled under `include/raylib-6.0_win64_mingw-w64/`).

**Command Line:**
```cmd
run
:: or
build.bat
:: or
mingw32-make run
```

**Code::Blocks:**
1. Open `GAMEHUB.cbp` in Code::Blocks.
2. Build and run via **Build → Build and run** (`F9`).
3. Ensure execution working directory is `.` (**Project → Properties → Build targets → Execution working dir: .**) so that `data/` files load correctly.

## Project Structure

```
├── data/                                 # Persistent storage (users, scores, leaderboard, ratings)
├── include/
│   ├── hub.h                             # Core types, definitions, and function prototypes
│   └── raylib-6.0_win64_mingw-w64/       # Bundled raylib distribution for Windows MinGW
├── src/
│   ├── auth.c                            # User authentication and password management
│   ├── g_candy.c                         # Match-3 candy game
│   ├── g_golf.c                          # Mini-golf game
│   ├── g_parkour.c                       # Parkour runner game
│   ├── g_snake.c                         # Snake game
│   ├── gfx.c                             # Raylib drawing helpers and player picker
│   ├── leaderboard.c                     # Leaderboard persistence and overlay
│   ├── main.c                            # Terminal shell entry point and command dispatcher
│   ├── rating.c                          # Post-game rating prompt and calculation
│   └── util.c                            # Score loading, session settings, and banner
├── GAMEHUB.cbp                           # Code::Blocks project configuration
├── Makefile                              # Cross-platform build configuration
├── build.bat                             # Windows MinGW build script
├── run                                   # macOS/Linux launcher
└── run.bat                               # Windows launcher
```

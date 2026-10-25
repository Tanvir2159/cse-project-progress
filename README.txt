GAME HUB — simplified games (still matches report structure)

GAMES
  snake    grid, eat food, 1 player
  candy    match-3, no burst particles
  golf     3 holes only (not 8)
  parkour  jump runner, simple obstacles

SHELL (unchanged)
  login → name@hub:~$ → type snake|candy|golf|parkour
  ratings after each game, leaderboard, passwd, union

BUILD
  Put raylib under include/raylib-6.0_win64_mingw-w64/
  mingw32-make project
  OR build.bat
  OR open GAMEHUB.cbp in Code::Blocks

WHY SIMPLER GAMES
  Same multi-file design, auth, scores, ratings.
  Game files are short so you can read and explain every line.

#ifndef HUB_H
#define HUB_H

#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <math.h>

#define SW              1000
#define SH              650
#define MAX_NAME        32
#define MAX_PASS        32
#define USERS_FILE      "data/users.txt"
#define RATINGS_FILE    "data/ratings.txt"
#define SETTINGS_FILE   "data/settings.txt"

#ifdef _WIN32
  #define CLEAR_CMD "cls"
#else
  #define CLEAR_CMD "clear"
#endif

/* ANSI (long names — avoid clash with raylib / Windows) */
#define ANSI_RST  "\033[0m"
#define ANSI_B    "\033[1m"
#define ANSI_C    "\033[1;36m"
#define ANSI_Y    "\033[1;33m"
#define ANSI_G    "\033[1;32m"
#define ANSI_M    "\033[1;35m"
#define ANSI_RD   "\033[1;31m"
#define ANSI_BL   "\033[1;34m"
#define ANSI_DIM  "\033[2m"
#define ANSI_BG_BLK "\033[40m"
#define ANSI_BG_RED "\033[41m"
#define ANSI_BG_GRN "\033[42m"
#define ANSI_BG_YEL "\033[43m"
#define ANSI_BG_BLU "\033[44m"
#define ANSI_BG_MAG "\033[45m"
#define ANSI_BG_CYN "\033[46m"
#define ANSI_BG_WHT "\033[47m"
#define ANSI_FG_BLK "\033[30m"
#define ANSI_FG_WHT "\033[97m"

typedef struct {
    char name[MAX_NAME];
    char pass[MAX_PASS];
} User;

/* progress build: fields kept for later; settings menu is a stub */
typedef struct {
    int display_mode;   /* 0 graphics, 1 terminal */
    int difficulty;     /* 1 easy 2 normal 3 hard */
    int theme;          /* 0 cyan 1 magenta 2 green */
    int two_player;
    int candy_colors;
    int candy_rows;
    int candy_cols;
    int candy_moves;
} Settings;

typedef struct {
    User     user;
    Settings settings;
} Session;

void enable_console(void);
void clear_screen(void);
int  login_or_register(User *u);
void load_settings(Settings *s);
void save_settings(const Settings *s);
void open_settings_menu(Settings *s);
void ask_and_save_rating(const char *username, const char *game);
float average_rating(const char *game);
void show_status(const Session *s);
void print_banner(const Session *s);
void print_help(void);
void print_ls(void);
const char *diff_name(int d);
const char *mode_name(int m);
const char *theme_name(int t);
const char *accent(const Settings *s);
void term_title_candy(void);

void game_candy(Session *s);

#endif

#include "hub.h"

#ifdef _WIN32
  #define WIN32_LEAN_AND_MEAN
  #define NOGDI
  #define NOUSER
  #include <windows.h>
#endif

void enable_console(void) {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (h != INVALID_HANDLE_VALUE && GetConsoleMode(h, &mode))
        SetConsoleMode(h, mode | 0x0004);
#endif
}

void clear_screen(void) { system(CLEAR_CMD); }

static void trim(char *s) { s[strcspn(s, "\r\n")] = '\0'; }

const char *diff_name(int d) {
    if (d <= 1) return "easy";
    if (d == 2) return "normal";
    return "hard";
}
const char *mode_name(int m) { return m ? "terminal" : "graphics"; }
const char *theme_name(int t) {
    if (t == 1) return "magenta";
    if (t == 2) return "green";
    return "cyan";
}
const char *accent(const Settings *s) {
    if (s->theme == 1) return ANSI_M;
    if (s->theme == 2) return ANSI_G;
    return ANSI_C;
}

void term_title_candy(void) {
    printf("\n");
    printf(ANSI_M ANSI_B);
    printf("   _____          _   _ ________     __\n");
    printf("  / ____|   /\\   | \\ | |  ____\\ \\   / /\n");
    printf(" | |       /  \\  |  \\| | |__   \\ \\_/ / \n");
    printf(" | |      / /\\ \\ | . ` |  __|   \\   /  \n");
    printf(" | |____ / ____ \\| |\\  | |____   | |   \n");
    printf("  \\_____/_/    \\_\\_| \\_|______|  |_|   \n");
    printf(ANSI_RST "\n");
}

int login_or_register(User *u) {
    char name[MAX_NAME], pass[MAX_PASS];
    int choice;
    printf("\n  " ANSI_C ANSI_B "GAME HUB" ANSI_RST "  login\n");
    printf("  1) login\n  2) register\n  choice: ");
    if (scanf("%d", &choice) != 1) return 0;
    while (getchar() != '\n') {}
    printf("  username: ");
    if (!fgets(name, sizeof name, stdin)) return 0;
    trim(name);
    printf("  password: ");
    if (!fgets(pass, sizeof pass, stdin)) return 0;
    trim(pass);
    if (!name[0] || !pass[0]) {
        printf("  " ANSI_RD "empty name or password" ANSI_RST "\n");
        return 0;
    }
    if (choice == 2) {
        FILE *fp = fopen(USERS_FILE, "r");
        if (fp) {
            char n[MAX_NAME], p[MAX_PASS];
            while (fscanf(fp, "%31s %31s", n, p) == 2) {
                if (strcmp(n, name) == 0) {
                    fclose(fp);
                    printf("  " ANSI_RD "name already taken" ANSI_RST "\n");
                    return 0;
                }
            }
            fclose(fp);
        }
        fp = fopen(USERS_FILE, "a");
        if (!fp) return 0;
        fprintf(fp, "%s %s\n", name, pass);
        fclose(fp);
        strncpy(u->name, name, MAX_NAME - 1);
        strncpy(u->pass, pass, MAX_PASS - 1);
        printf("  " ANSI_G "registered. welcome %s" ANSI_RST "\n", u->name);
        return 1;
    }
    FILE *fp = fopen(USERS_FILE, "r");
    if (!fp) {
        printf("  " ANSI_RD "no users yet — choose register" ANSI_RST "\n");
        return 0;
    }
    char n[MAX_NAME], p[MAX_PASS];
    while (fscanf(fp, "%31s %31s", n, p) == 2) {
        if (strcmp(n, name) == 0 && strcmp(p, pass) == 0) {
            fclose(fp);
            strncpy(u->name, name, MAX_NAME - 1);
            strncpy(u->pass, pass, MAX_PASS - 1);
            printf("  " ANSI_G "welcome back %s" ANSI_RST "\n", u->name);
            return 1;
        }
    }
    fclose(fp);
    printf("  " ANSI_RD "wrong username or password" ANSI_RST "\n");
    return 0;
}

void load_settings(Settings *s) {
    s->display_mode = 0;
    s->difficulty = 2;
    s->theme = 0;
    s->two_player = 0;
    s->candy_colors = 5;
    s->candy_rows = 8;
    s->candy_cols = 8;
    s->candy_moves = 22;
    FILE *fp = fopen(SETTINGS_FILE, "r");
    if (!fp) return;
    fscanf(fp, "%d %d %d %d %d %d %d %d",
           &s->display_mode, &s->difficulty, &s->theme, &s->two_player,
           &s->candy_colors, &s->candy_rows, &s->candy_cols, &s->candy_moves);
    fclose(fp);
}

void save_settings(const Settings *s) {
    FILE *fp = fopen(SETTINGS_FILE, "w");
    if (!fp) return;
    fprintf(fp, "%d %d %d %d %d %d %d %d\n",
            s->display_mode, s->difficulty, s->theme, s->two_player,
            s->candy_colors, s->candy_rows, s->candy_cols, s->candy_moves);
    fclose(fp);
}

/* progress build: settings command exists, full options not wired yet */
void open_settings_menu(Settings *s) {
    (void)s;
    printf("\n  " ANSI_Y ANSI_B "settings" ANSI_RST "  (progress build)\n");
    printf("  planned options:\n");
    printf("    - display mode   (graphics / terminal)\n");
    printf("    - difficulty     (easy / normal / hard)\n");
    printf("    - theme          (cyan / magenta / green)\n");
    printf("    - multiplayer    (shared keyboard)\n");
    printf("    - candy board size and colours\n");
    printf("  " ANSI_DIM "not connected yet — next milestone" ANSI_RST "\n\n");
}

void ask_and_save_rating(const char *username, const char *game) {
    int r = 0;
    printf("\n  rate " ANSI_Y "%s" ANSI_RST " (1-5), or 0 to skip: ", game);
    if (scanf("%d", &r) != 1) { while (getchar() != '\n'); return; }
    while (getchar() != '\n') {}
    if (r < 1 || r > 5) { printf("  skipped\n"); return; }
    FILE *fp = fopen(RATINGS_FILE, "a");
    if (!fp) return;
    fprintf(fp, "%s %s %d\n", username, game, r);
    fclose(fp);
    printf("  " ANSI_G "saved" ANSI_RST ". average for %s is now %.2f\n",
           game, average_rating(game));
}

float average_rating(const char *game) {
    FILE *fp = fopen(RATINGS_FILE, "r");
    if (!fp) return 0.0f;
    char u[MAX_NAME], g[32];
    int r, sum = 0, n = 0;
    while (fscanf(fp, "%31s %31s %d", u, g, &r) == 3)
        if (strcmp(g, game) == 0) { sum += r; n++; }
    fclose(fp);
    return n ? (float)sum / (float)n : 0.0f;
}

void show_status(const Session *s) {
    const char *A = accent(&s->settings);
    printf("\n  %sstatus%s\n", A, ANSI_RST);
    printf("  user         %s\n", s->user.name);
    printf("  display      %s\n", mode_name(s->settings.display_mode));
    printf("  difficulty   %s\n", diff_name(s->settings.difficulty));
    printf("  theme        %s\n", theme_name(s->settings.theme));
    printf("  candy avg    %.2f\n", average_rating("candy"));
    printf("  milestone    login + shell + candy\n\n");
}

void print_banner(const Session *s) {
    time_t tnow = time(NULL);
    char *ts = ctime(&tnow);
    ts[strlen(ts) - 1] = '\0';
    const char *A = accent(&s->settings);
    clear_screen();
    printf("\n");
    printf("%s%s", A, ANSI_B);
    printf("   _____          __  __ ______\n");
    printf("  / ____|   /\\   |  \\/  |  ____|\n");
    printf(" | |  __   /  \\  | \\  / | |__   \n");
    printf(" | | |_ | / /\\ \\ | |\\/| |  __|  \n");
    printf(" | |__| |/ ____ \\| |  | | |____ \n");
    printf("  \\_____/_/    \\_\\_|  |_|______|\n");
    printf("%s", ANSI_RST);
    printf("%s%s           H U B%s%s  ·  progress build%s\n",
           ANSI_Y, ANSI_B, ANSI_RST, ANSI_DIM, ANSI_RST);
    printf("%s  ==================================================%s\n", ANSI_DIM, ANSI_RST);
    printf("  %suser%s      %s%s%s\n", ANSI_Y, ANSI_RST, ANSI_G, s->user.name, ANSI_RST);
    printf("  %sshell%s     hub-sh\n", ANSI_Y, ANSI_RST);
    printf("  %skernel%s    C99  structs / files / macros\n", ANSI_Y, ANSI_RST);
    printf("  %stime%s      %s\n", ANSI_Y, ANSI_RST, ts);
    printf("  %sgames%s     candy  (more planned)\n", ANSI_Y, ANSI_RST);
    printf("  %stheme%s     %s%s%s\n", ANSI_Y, ANSI_RST, A, theme_name(s->settings.theme), ANSI_RST);
    printf("%s  ==================================================%s\n", ANSI_DIM, ANSI_RST);
    printf("  type %shelp%s · %sls%s · %ssettings%s · %sstatus%s · %scandy%s · %sexit%s\n\n",
           ANSI_Y, ANSI_RST, ANSI_Y, ANSI_RST, ANSI_Y, ANSI_RST,
           ANSI_Y, ANSI_RST, ANSI_Y, ANSI_RST, ANSI_Y, ANSI_RST);
}

void print_help(void) {
    printf("\n  %s%scommands%s\n", ANSI_C, ANSI_B, ANSI_RST);
    printf("    %shelp%s       this list\n", ANSI_Y, ANSI_RST);
    printf("    %sls%s         list games and data\n", ANSI_Y, ANSI_RST);
    printf("    %sstatus%s     user and progress\n", ANSI_Y, ANSI_RST);
    printf("    %ssettings%s   planned options (stub)\n", ANSI_Y, ANSI_RST);
    printf("    %sclear%s      redraw banner\n", ANSI_Y, ANSI_RST);
    printf("    %sexit%s       quit\n", ANSI_Y, ANSI_RST);
    printf("\n  %s%sgames%s\n", ANSI_C, ANSI_B, ANSI_RST);
    printf("    %scandy%s    match-3 board (working)\n", ANSI_G, ANSI_RST);
    printf("    " ANSI_DIM "golf / snake / ludo  — next milestones" ANSI_RST "\n\n");
}

void print_ls(void) {
    printf("\n  %sgames/%s\n", ANSI_C, ANSI_RST);
    printf("    candy\n");
    printf("    " ANSI_DIM "golf   snake   ludo   (planned)" ANSI_RST "\n");
    printf("  %sdata/%s\n", ANSI_C, ANSI_RST);
    printf("    users.txt    ratings.txt    settings.txt\n\n");
}

#include "hub.h"

static void status(const Player *p) {
    const char *names[] = { "snake", "candy", "golf", "parkour" };
    printf("\n  player: %s\n", p->name);
    for (int i = 0; i < G_COUNT; i++)
        printf("    %-12s wins %d\n", names[i], p->wins[i]);
    printf("    best snake %d  candy %d  golf %d  parkour %d\n\n",
           p->best_snake, p->best_candy, p->best_golf, p->best_parkour);
}

static void run_game(void (*fn)(Player *), Player *p, GameId id) {
    const char *names[] = { "snake", "candy", "golf", "parkour" };
    printf("  opening graphics window...\n");
    fflush(stdout);
    fn(p);
    p->wins[id]++;
    score_save(p);
    /* after the window closes, optional 1–5 rating */
    rating_ask_and_save(p->name, names[id]);
    printf("  back to hub-sh.\n");
}

int main(void) {
    enable_console();
    srand((unsigned)time(NULL));

    char name[MAX_NAME];
    if (!auth_login(name, MAX_NAME)) return 0;

    Player p;
    score_load(&p, name);
    shell_banner(&p);

    /* demo union for viva topic */
    IntBytes u;
    u.as_int = 0x41424344;

    char line[128];
    while (1) {
        printf("\033[1;32m%s@hub\033[0m:\033[1;36m~\033[0m$ ", p.name);
        fflush(stdout);
        if (!fgets(line, sizeof line, stdin)) break;
        line[strcspn(line, "\n")] = '\0';
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (!*cmd) continue;

        if (!strcmp(cmd, "help") || !strcmp(cmd, "man")) shell_help();
        else if (!strcmp(cmd, "neofetch") || !strcmp(cmd, "fetch")) shell_banner(&p);
        else if (!strcmp(cmd, "ls")) {
            printf("  rooms/  snake candy golf parkour\n");
            printf("  data/   scores.txt  users.txt  leaderboard.txt  ratings.txt\n");
        } else if (!strcmp(cmd, "status")) status(&p);
        else if (!strcmp(cmd, "leaderboard") || !strcmp(cmd, "lb")) lb_print_all();
        else if (!strcmp(cmd, "ratings") || !strcmp(cmd, "avg")) rating_print_averages();
        else if (!strcmp(cmd, "passwd")) auth_change_password(p.name);
        else if (!strcmp(cmd, "settings")) shell_settings(&p);
        else if (!strcmp(cmd, "clear") || !strcmp(cmd, "cls")) shell_banner(&p);
        else if (!strcmp(cmd, "union")) {
            printf("  union IntBytes demo: as_int=%d bytes='%c%c%c%c'\n",
                   u.as_int, u.as_bytes[0], u.as_bytes[1],
                   u.as_bytes[2], u.as_bytes[3]);
        } else if (!strcmp(cmd, "exit") || !strcmp(cmd, "quit")) {
            score_save(&p);
            printf("  logout. bye %s\n", p.name);
            break;
        } else if (!strcmp(cmd, "snake") || !strcmp(cmd, "1"))
            run_game(game_snake, &p, G_SNAKE);
        else if (!strcmp(cmd, "candy") || !strcmp(cmd, "2"))
            run_game(game_candy, &p, G_CANDY);
        else if (!strcmp(cmd, "golf") || !strcmp(cmd, "3"))
            run_game(game_golf, &p, G_GOLF);
        else if (!strcmp(cmd, "parkour") || !strcmp(cmd, "4"))
            run_game(game_parkour, &p, G_PARKOUR);
        else
            printf("  hub-sh: \033[1;31m%s\033[0m: not found — try help\n", cmd);
    }
    return 0;
}

#include "hub.h"

int main(void) {
    enable_console();
    srand((unsigned)time(NULL));

    Session s;
    memset(&s, 0, sizeof s);
    load_settings(&s.settings);

    if (!login_or_register(&s.user)) {
        printf("  login failed. bye\n");
        return 1;
    }
    print_banner(&s);

    char line[128];
    for (;;) {
        const char *A = accent(&s.settings);
        printf("%s%s%s@%shub%s> ", ANSI_G, s.user.name, ANSI_RST, A, ANSI_RST);
        fflush(stdout);
        if (!fgets(line, sizeof line, stdin)) break;
        line[strcspn(line, "\r\n")] = '\0';
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (!*cmd) continue;

        if (!strcmp(cmd, "help") || !strcmp(cmd, "man")) print_help();
        else if (!strcmp(cmd, "ls")) print_ls();
        else if (!strcmp(cmd, "settings")) open_settings_menu(&s.settings);
        else if (!strcmp(cmd, "status")) show_status(&s);
        else if (!strcmp(cmd, "clear") || !strcmp(cmd, "neofetch") || !strcmp(cmd, "fetch"))
            print_banner(&s);
        else if (!strcmp(cmd, "exit") || !strcmp(cmd, "quit")) {
            save_settings(&s.settings);
            printf("  bye %s\n", s.user.name);
            break;
        } else if (!strcmp(cmd, "candy")) {
            game_candy(&s);
            ask_and_save_rating(s.user.name, "candy");
        } else if (!strcmp(cmd, "golf") || !strcmp(cmd, "snake") || !strcmp(cmd, "ludo")) {
            printf("  " ANSI_Y "%s" ANSI_RST " not in this progress build yet\n", cmd);
        } else {
            printf("  %sunknown:%s %s  (try help)\n", ANSI_RD, ANSI_RST, cmd);
        }
    }
    return 0;
}

#include "hub.h"

/* ratings.txt lines:  username  game_name  stars(1-5) */

void rating_ask_and_save(const char *user, const char *game) {
    char line[64];
    int stars = 0;

    printf("  rate %s from 1 to 5 (or Enter to skip): ", game);
    fflush(stdout);
    if (!fgets(line, sizeof line, stdin)) return;
    if (sscanf(line, "%d", &stars) != 1) return;
    if (stars < 1 || stars > 5) {
        printf("  skipped (need 1–5).\n");
        return;
    }

    /* ensure data/ exists */
#ifdef _WIN32
    system("if not exist data mkdir data");
#else
    system("mkdir -p data");
#endif

    FILE *fp = fopen(RATING_FILE, "a");
    if (!fp) {
        printf("  could not save rating.\n");
        return;
    }
    fprintf(fp, "%s %s %d\n", user, game, stars);
    fclose(fp);
    printf("  saved: %s → %d/5\n", game, stars);
}

float rating_average(const char *game) {
    FILE *fp = fopen(RATING_FILE, "r");
    if (!fp) return -1.0f;

    char user[MAX_NAME], gname[32];
    int stars, sum = 0, count = 0;
    while (fscanf(fp, "%31s %31s %d", user, gname, &stars) == 3) {
        if (strcmp(gname, game) == 0 && stars >= 1 && stars <= 5) {
            sum += stars;
            count++;
        }
    }
    fclose(fp);
    if (count == 0) return -1.0f;
    return (float)sum / (float)count;
}

void rating_print_averages(void) {
    const char *games[] = { "snake", "candy", "golf", "parkour" };
    printf("\n  average ratings (1–5):\n");
    for (int i = 0; i < 4; i++) {
        float a = rating_average(games[i]);
        if (a < 0)
            printf("    %-10s  no ratings yet\n", games[i]);
        else
            printf("    %-10s  %.2f / 5\n", games[i], a);
    }
    printf("\n");
}

#include "hub.h"

#define MAXN 10

static Color palette[] = {
    (Color){ 230, 55, 70, 255 },
    (Color){ 50, 190, 80, 255 },
    (Color){ 50, 110, 230, 255 },
    (Color){ 245, 200, 40, 255 },
    (Color){ 160, 70, 210, 255 },
    (Color){ 240, 130, 40, 255 }
};

static const char *sym = "RGBYPO";

static void draw_candy(int x, int y, int size, int kind, int selected) {
    int cx = x + size / 2, cy = y + size / 2, r = size / 2 - 6;
    Color c = palette[kind % 6];
    DrawCircle(cx, cy, (float)r + 2, (Color){ 0, 0, 0, 50 });
    DrawCircle(cx, cy, (float)r, c);
    DrawCircle(cx - r / 3, cy - r / 3, (float)r / 3.2f, (Color){ 255, 255, 255, 140 });
    if (selected)
        DrawCircleLines(cx, cy, (float)r + 4, WHITE);
}

static void candy_terminal(Session *s) {
    int kinds = s->settings.candy_colors;
    int rows = s->settings.candy_rows;
    int cols = s->settings.candy_cols;
    if (rows > 8) rows = 8;
    if (cols > 8) cols = 8;
    if (kinds < 3) kinds = 3;
    if (kinds > 6) kinds = 6;
    int moves = s->settings.candy_moves;
    if (s->settings.difficulty == 1) moves += 6;
    if (s->settings.difficulty == 3) moves -= 6;
    if (moves < 8) moves = 8;
    int g[MAXN][MAXN], score = 0;
    for (int r = 0; r < rows; r++)
        for (int c = 0; c < cols; c++)
            g[r][c] = rand() % kinds;

    clear_screen();
    term_title_candy();
    printf("  board %dx%d  colours %d  difficulty %s\n",
           rows, cols, kinds, diff_name(s->settings.difficulty));
    printf("  swap: " ANSI_C "r1 c1 r2 c2" ANSI_RST "   e.g. " ANSI_Y "0 0 0 1" ANSI_RST "   quit: q\n");

    while (moves > 0) {
        printf("\n  " ANSI_BG_MAG ANSI_FG_WHT " score %d   moves %d " ANSI_RST "\n    ", score, moves);
        for (int c = 0; c < cols; c++) printf(" %d", c);
        printf("\n");
        for (int r = 0; r < rows; r++) {
            printf("  %d ", r);
            for (int c = 0; c < cols; c++) {
                int k = g[r][c];
                const char *bg =
                    k == 0 ? ANSI_BG_RED :
                    k == 1 ? ANSI_BG_GRN :
                    k == 2 ? ANSI_BG_BLU :
                    k == 3 ? ANSI_BG_YEL :
                    k == 4 ? ANSI_BG_MAG : ANSI_BG_CYN;
                printf("%s%s[]%s", bg, ANSI_FG_BLK, ANSI_RST);
            }
            printf("\n");
        }
        printf("  > ");
        char line[64];
        if (!fgets(line, sizeof line, stdin)) break;
        if (line[0] == 'q' || line[0] == 'Q') break;
        int r1, c1, r2, c2;
        if (sscanf(line, "%d %d %d %d", &r1, &c1, &r2, &c2) != 4) {
            printf("  " ANSI_RD "need four numbers" ANSI_RST "\n");
            continue;
        }
        if (r1 < 0 || r1 >= rows || r2 < 0 || r2 >= rows ||
            c1 < 0 || c1 >= cols || c2 < 0 || c2 >= cols) continue;
        if (abs(r1 - r2) + abs(c1 - c2) != 1) {
            printf("  " ANSI_RD "must be adjacent" ANSI_RST "\n");
            continue;
        }
        int t = g[r1][c1]; g[r1][c1] = g[r2][c2]; g[r2][c2] = t;
        int cleared = 0;
        for (int rr = 0; rr < rows; rr++)
            for (int cc = 0; cc < cols - 2; cc++)
                if (g[rr][cc] == g[rr][cc + 1] && g[rr][cc] == g[rr][cc + 2] && g[rr][cc] >= 0) {
                    g[rr][cc] = g[rr][cc + 1] = g[rr][cc + 2] = -1;
                    cleared += 3;
                }
        for (int cc = 0; cc < cols; cc++)
            for (int rr = 0; rr < rows - 2; rr++)
                if (g[rr][cc] == g[rr + 1][cc] && g[rr][cc] == g[rr + 2][cc] && g[rr][cc] >= 0) {
                    g[rr][cc] = g[rr + 1][cc] = g[rr + 2][cc] = -1;
                    cleared += 3;
                }
        if (!cleared) {
            t = g[r1][c1]; g[r1][c1] = g[r2][c2]; g[r2][c2] = t;
            printf("  " ANSI_Y "no match" ANSI_RST "\n");
            continue;
        }
        score += cleared * 10;
        moves--;
        for (int cc = 0; cc < cols; cc++) {
            int w = rows - 1;
            for (int rr = rows - 1; rr >= 0; rr--)
                if (g[rr][cc] >= 0) {
                    g[w][cc] = g[rr][cc];
                    if (w != rr) g[rr][cc] = -1;
                    w--;
                }
            for (int rr = w; rr >= 0; rr--)
                g[rr][cc] = rand() % kinds;
        }
    }
    printf("\n  " ANSI_BG_YEL ANSI_FG_BLK ANSI_B " DONE " ANSI_RST "  score %d\n\n", score);
}

void game_candy(Session *s) {
    if (s->settings.display_mode == 1) { candy_terminal(s); return; }

    int kinds = s->settings.candy_colors;
    int rows = s->settings.candy_rows;
    int cols = s->settings.candy_cols;
    if (kinds < 3) kinds = 3;
    if (kinds > 6) kinds = 6;
    if (rows < 6) rows = 6;
    if (rows > 10) rows = 10;
    if (cols < 6) cols = 6;
    if (cols > 10) cols = 10;

    int moves = s->settings.candy_moves;
    if (s->settings.difficulty == 1) moves += 6;
    if (s->settings.difficulty == 3) { moves -= 6; if (kinds < 6) kinds++; }
    if (moves < 8) moves = 8;

    int g[MAXN][MAXN];
    for (int r = 0; r < rows; r++)
        for (int c = 0; c < cols; c++)
            g[r][c] = GetRandomValue(0, kinds - 1);

    int score = 0, sel = -1;
    float cell = (rows >= 9 || cols >= 9) ? 50.0f : 58.0f;
    float ox = (SW - cols * cell) / 2.0f, oy = 90;

    InitWindow(SW, SH, "Candy");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_ESCAPE)) break;

        if (moves > 0 && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            int c = (int)((GetMouseX() - ox) / cell);
            int r = (int)((GetMouseY() - oy) / cell);
            if (r >= 0 && r < rows && c >= 0 && c < cols) {
                int id = r * cols + c;
                if (sel < 0) sel = id;
                else {
                    int r1 = sel / cols, c1 = sel % cols;
                    if (abs(r1 - r) + abs(c1 - c) == 1) {
                        int t = g[r1][c1]; g[r1][c1] = g[r][c]; g[r][c] = t;
                        int cleared = 0;
                        for (int rr = 0; rr < rows; rr++)
                            for (int cc = 0; cc < cols - 2; cc++)
                                if (g[rr][cc] == g[rr][cc + 1] && g[rr][cc] == g[rr][cc + 2] && g[rr][cc] >= 0) {
                                    g[rr][cc] = g[rr][cc + 1] = g[rr][cc + 2] = -1;
                                    cleared += 3;
                                }
                        for (int cc = 0; cc < cols; cc++)
                            for (int rr = 0; rr < rows - 2; rr++)
                                if (g[rr][cc] == g[rr + 1][cc] && g[rr][cc] == g[rr + 2][cc] && g[rr][cc] >= 0) {
                                    g[rr][cc] = g[rr + 1][cc] = g[rr + 2][cc] = -1;
                                    cleared += 3;
                                }
                        if (!cleared) {
                            t = g[r1][c1]; g[r1][c1] = g[r][c]; g[r][c] = t;
                        } else {
                            score += cleared * 10 * s->settings.difficulty;
                            moves--;
                            for (int cc = 0; cc < cols; cc++) {
                                int w = rows - 1;
                                for (int rr = rows - 1; rr >= 0; rr--)
                                    if (g[rr][cc] >= 0) {
                                        g[w][cc] = g[rr][cc];
                                        if (w != rr) g[rr][cc] = -1;
                                        w--;
                                    }
                                for (int rr = w; rr >= 0; rr--)
                                    g[rr][cc] = GetRandomValue(0, kinds - 1);
                            }
                        }
                    }
                    sel = -1;
                }
            }
        }

        BeginDrawing();
        ClearBackground((Color){ 28, 14, 42, 255 });
        DrawRectangle(0, 0, SW, 64, (Color){ 18, 8, 28, 255 });
        DrawText(TextFormat("CANDY   score %d   moves %d   %s   board %dx%d",
                            score, moves, diff_name(s->settings.difficulty), rows, cols),
                 16, 14, 22, RAYWHITE);
        DrawText("click two adjacent candies   ESC menu", 16, 40, 16, (Color){ 200, 180, 220, 255 });

        DrawRectangleRounded((Rectangle){ ox - 10, oy - 10, cols * cell + 20, rows * cell + 20 },
                             0.04f, 8, (Color){ 50, 28, 70, 255 });
        for (int r = 0; r < rows; r++)
            for (int c = 0; c < cols; c++) {
                int x = (int)(ox + c * cell), y = (int)(oy + r * cell);
                DrawRectangle(x + 3, y + 3, (int)cell - 6, (int)cell - 6, (Color){ 40, 22, 58, 255 });
                if (g[r][c] >= 0)
                    draw_candy(x, y, (int)cell, g[r][c], sel == r * cols + c);
            }
        if (moves <= 0) {
            DrawRectangle(0, SH / 2 - 36, SW, 72, (Color){ 0, 0, 0, 170 });
            DrawText("OUT OF MOVES  —  ESC to rate", SW / 2 - 180, SH / 2 - 12, 26, GOLD);
        }
        EndDrawing();
    }
    CloseWindow();
}

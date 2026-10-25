#include "hub.h"
/* Candy — match-3, colour count from settings */

#define N 8
#define MAXK 6

static Color COLS[MAXK] = { RED, GREEN, BLUE, GOLD, PURPLE, ORANGE };

static int clear_matches(int g[N][N]) {
    int mark[N][N] = {0}, n = 0;
    for (int r = 0; r < N; r++)
        for (int c = 0; c < N - 2; c++)
            if (g[r][c] >= 0 && g[r][c] == g[r][c+1] && g[r][c] == g[r][c+2])
                mark[r][c] = mark[r][c+1] = mark[r][c+2] = 1;
    for (int c = 0; c < N; c++)
        for (int r = 0; r < N - 2; r++)
            if (g[r][c] >= 0 && g[r][c] == g[r+1][c] && g[r][c] == g[r+2][c])
                mark[r][c] = mark[r+1][c] = mark[r+2][c] = 1;
    for (int r = 0; r < N; r++)
        for (int c = 0; c < N; c++)
            if (mark[r][c]) { g[r][c] = -1; n++; }
    return n;
}

static void gravity(int g[N][N], int kinds) {
    for (int c = 0; c < N; c++) {
        int w = N - 1;
        for (int r = N - 1; r >= 0; r--)
            if (g[r][c] >= 0) { g[w][c] = g[r][c]; if (w != r) g[r][c] = -1; w--; }
        for (int r = w; r >= 0; r--) g[r][c] = GetRandomValue(0, kinds - 1);
    }
}

void game_candy(Player *p) {
    int kinds = p->candy_colors;
    if (kinds < 3) kinds = 3;
    if (kinds > 6) kinds = 6;

    int board[N][N], score = 0, moves = 15, sel = -1;
    for (int r = 0; r < N; r++)
        for (int c = 0; c < N; c++)
            board[r][c] = GetRandomValue(0, kinds - 1);

    float cell = 52, ox = 280, oy = 90;
    gfx_open("Candy");

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_ESCAPE)) break;
        if (IsKeyPressed(KEY_R)) {
            for (int r = 0; r < N; r++)
                for (int c = 0; c < N; c++)
                    board[r][c] = GetRandomValue(0, kinds - 1);
            score = 0; moves = 15; sel = -1;
        }

        if (moves > 0 && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            int c = (int)((GetMouseX() - ox) / cell);
            int r = (int)((GetMouseY() - oy) / cell);
            if (r >= 0 && r < N && c >= 0 && c < N) {
                int id = r * N + c;
                if (sel < 0) sel = id;
                else {
                    int r1 = sel / N, c1 = sel % N;
                    if (abs(r1 - r) + abs(c1 - c) == 1) {
                        int t = board[r1][c1];
                        board[r1][c1] = board[r][c];
                        board[r][c] = t;
                        int n = clear_matches(board);
                        if (!n) {
                            t = board[r1][c1];
                            board[r1][c1] = board[r][c];
                            board[r][c] = t;
                        } else {
                            score += n * 10;
                            moves--;
                            gravity(board, kinds);
                        }
                    }
                    sel = -1;
                }
            }
        }

        BeginDrawing();
        gfx_background((Color){40, 15, 55, 255}, (Color){20, 8, 35, 255});
        gfx_topbar("CANDY", "click neighbours | R | ESC");
        DrawText(TextFormat("Score %d  Moves %d  Colours %d  Best %d",
                            score, moves, kinds, p->best_candy), 20, 70, 20, RAYWHITE);

        for (int r = 0; r < N; r++)
            for (int c = 0; c < N; c++) {
                float x = ox + c * cell, y = oy + r * cell;
                DrawRectangle(x + 2, y + 2, (int)cell - 4, (int)cell - 4, (Color){50, 25, 70, 255});
                if (board[r][c] >= 0)
                    DrawCircle(x + cell/2, y + cell/2, cell * 0.35f, COLS[board[r][c] % kinds]);
                if (sel == r * N + c)
                    DrawCircleLines(x + cell/2, y + cell/2, cell * 0.4f, WHITE);
            }

        if (moves <= 0)
            DrawText("OUT OF MOVES — ESC", SW/2 - 120, SH - 40, 22, GOLD);
        EndDrawing();
    }
    CloseWindow();
    if (score > p->best_candy) p->best_candy = score;
    lb_submit("candy", p->name, score, 0);
}

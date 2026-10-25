#include "hub.h"
/* Snake — 1 or 2 players (simple) */

#define COLS 22
#define ROWS 14
#define CELL 28
#define MAXB (COLS * ROWS)

typedef struct {
    Vector2 body[MAXB];
    int len;
    Vector2 dir;
    int score;
    int dead;
    Color color;
} Sn;

static void reset_snake(Sn *s, float x, float y, Vector2 d, Color c) {
    s->len = 3;
    s->body[0] = (Vector2){x, y};
    s->body[1] = (Vector2){x - d.x, y};
    s->body[2] = (Vector2){x - 2 * d.x, y};
    s->dir = d;
    s->score = 0;
    s->dead = 0;
    s->color = c;
}

static void step_snake(Sn *s) {
    if (s->dead) return;
    for (int i = s->len; i > 0; i--) s->body[i] = s->body[i - 1];
    s->body[0].x += s->dir.x;
    s->body[0].y += s->dir.y;
    if (s->body[0].x < 0 || s->body[0].x >= COLS ||
        s->body[0].y < 0 || s->body[0].y >= ROWS)
        s->dead = 1;
    for (int i = 1; i < s->len; i++)
        if (s->body[i].x == s->body[0].x && s->body[i].y == s->body[0].y)
            s->dead = 1;
}

void game_snake(Player *p) {
    Sn a, b;
    reset_snake(&a, 6, 7, (Vector2){1, 0}, (Color){100, 255, 140, 255});
    reset_snake(&b, 16, 7, (Vector2){-1, 0}, (Color){100, 180, 255, 255});

    Vector2 food = {11, 4};
    float timer = 0, step = 0.13f;
    float ox = (SW - COLS * CELL) / 2.0f, oy = 85;

    gfx_open("Snake");
    int players = gfx_pick_players("SNAKE", "arrows", "WASD");
    if (players < 1) players = 1;

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_ESCAPE)) break;
        if (IsKeyPressed(KEY_R)) {
            reset_snake(&a, 6, 7, (Vector2){1, 0}, a.color);
            reset_snake(&b, 16, 7, (Vector2){-1, 0}, b.color);
            food = (Vector2){11, 4};
            timer = 0;
        }

        /* P1 arrows */
        if (!a.dead) {
            if (IsKeyPressed(KEY_RIGHT) && a.dir.x == 0) a.dir = (Vector2){1, 0};
            if (IsKeyPressed(KEY_LEFT)  && a.dir.x == 0) a.dir = (Vector2){-1, 0};
            if (IsKeyPressed(KEY_UP)    && a.dir.y == 0) a.dir = (Vector2){0, -1};
            if (IsKeyPressed(KEY_DOWN)  && a.dir.y == 0) a.dir = (Vector2){0, 1};
        }
        /* P2 WASD */
        if (players == 2 && !b.dead) {
            if (IsKeyPressed(KEY_D) && b.dir.x == 0) b.dir = (Vector2){1, 0};
            if (IsKeyPressed(KEY_A) && b.dir.x == 0) b.dir = (Vector2){-1, 0};
            if (IsKeyPressed(KEY_W) && b.dir.y == 0) b.dir = (Vector2){0, -1};
            if (IsKeyPressed(KEY_S) && b.dir.y == 0) b.dir = (Vector2){0, 1};
        }

        timer += GetFrameTime();
        if (timer >= step) {
            timer = 0;
            step_snake(&a);
            if (players == 2) step_snake(&b);

            if (!a.dead && a.body[0].x == food.x && a.body[0].y == food.y) {
                a.len++; a.score += 10;
                food.x = (float)GetRandomValue(0, COLS - 1);
                food.y = (float)GetRandomValue(0, ROWS - 1);
            }
            if (players == 2 && !b.dead && b.body[0].x == food.x && b.body[0].y == food.y) {
                b.len++; b.score += 10;
                food.x = (float)GetRandomValue(0, COLS - 1);
                food.y = (float)GetRandomValue(0, ROWS - 1);
            }
        }

        BeginDrawing();
        gfx_background((Color){20, 25, 45, 255}, (Color){10, 12, 25, 255});
        gfx_topbar("SNAKE", players == 2 ? "P1 arrows  P2 WASD | R | ESC" : "arrows | R | ESC");
        if (players == 1)
            DrawText(TextFormat("Score %d   Best %d", a.score, p->best_snake), 20, 70, 22, RAYWHITE);
        else
            DrawText(TextFormat("P1 %d   P2 %d   Best %d", a.score, b.score, p->best_snake),
                     20, 70, 22, RAYWHITE);

        DrawRectangleLinesEx((Rectangle){ox - 2, oy - 2, COLS * CELL + 4, ROWS * CELL + 4}, 2, GOLD);
        for (int r = 0; r < ROWS; r++)
            for (int c = 0; c < COLS; c++)
                DrawRectangle((int)(ox + c * CELL), (int)(oy + r * CELL), CELL, CELL,
                              ((r + c) % 2) ? (Color){25, 35, 55, 255} : (Color){30, 40, 65, 255});

        DrawCircle(ox + food.x * CELL + CELL / 2, oy + food.y * CELL + CELL / 2,
                   CELL * 0.35f, RED);

        for (int i = 0; i < a.len; i++)
            DrawRectangle((int)(ox + a.body[i].x * CELL) + 2, (int)(oy + a.body[i].y * CELL) + 2,
                          CELL - 4, CELL - 4, a.dead ? GRAY : a.color);
        if (players == 2)
            for (int i = 0; i < b.len; i++)
                DrawRectangle((int)(ox + b.body[i].x * CELL) + 2, (int)(oy + b.body[i].y * CELL) + 2,
                              CELL - 4, CELL - 4, b.dead ? GRAY : b.color);

        if (a.dead && (players == 1 || b.dead))
            DrawText("DEAD — press R", SW / 2 - 90, SH - 40, 24, RED);
        EndDrawing();
    }
    CloseWindow();
    int best = a.score;
    if (players == 2 && b.score > best) best = b.score;
    if (best > p->best_snake) p->best_snake = best;
    lb_submit("snake", p->name, best, 0);
}

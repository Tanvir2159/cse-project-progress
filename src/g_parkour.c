#include "hub.h"
/* Parkour — simple side runner, jump only */

#define GROUND_Y  520.0f
#define PLAYER_W  36.0f
#define PLAYER_H  50.0f
#define GRAVITY   1600.0f
#define JUMP      620.0f
#define RUN_SPEED 280.0f

typedef struct {
    float x, y, w, h;
} Obs;

void game_parkour(Player *p) {
    float px = 120, py = GROUND_Y - PLAYER_H;
    float vy = 0;
    int on_ground = 1, score = 0, dead = 0;
    float scroll = 0, spawn_timer = 0;

    Obs obs[8];
    int nobs = 0;

    gfx_open("Parkour");

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_ESCAPE)) break;
        if (IsKeyPressed(KEY_R)) {
            px = 120; py = GROUND_Y - PLAYER_H; vy = 0;
            on_ground = 1; score = 0; dead = 0;
            scroll = 0; spawn_timer = 0; nobs = 0;
        }

        float dt = GetFrameTime();
        if (!dead) {
            /* jump */
            if (on_ground && (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))) {
                vy = -JUMP;
                on_ground = 0;
            }
            vy += GRAVITY * dt;
            py += vy * dt;
            if (py >= GROUND_Y - PLAYER_H) {
                py = GROUND_Y - PLAYER_H;
                vy = 0;
                on_ground = 1;
            }

            scroll += RUN_SPEED * dt;
            score = (int)(scroll / 10);

            /* spawn obstacles */
            spawn_timer += dt;
            if (spawn_timer > 1.2f && nobs < 8) {
                spawn_timer = 0;
                obs[nobs].w = (float)GetRandomValue(30, 50);
                obs[nobs].h = (float)GetRandomValue(40, 80);
                obs[nobs].x = SW + 20;
                obs[nobs].y = GROUND_Y - obs[nobs].h;
                nobs++;
            }

            /* move obstacles left */
            for (int i = 0; i < nobs; i++) {
                obs[i].x -= RUN_SPEED * dt;
            }
            /* remove off-screen */
            int w = 0;
            for (int i = 0; i < nobs; i++)
                if (obs[i].x + obs[i].w > 0)
                    obs[w++] = obs[i];
            nobs = w;

            /* collision */
            Rectangle player = {px, py, PLAYER_W, PLAYER_H};
            for (int i = 0; i < nobs; i++) {
                Rectangle o = {obs[i].x, obs[i].y, obs[i].w, obs[i].h};
                if (CheckCollisionRecs(player, o)) {
                    dead = 1;
                    break;
                }
            }
        }

        BeginDrawing();
        gfx_background((Color){30, 50, 90, 255}, (Color){15, 25, 50, 255});
        gfx_topbar("PARKOUR", "SPACE / UP jump | R restart | ESC hub");
        DrawText(TextFormat("Score %d   Best %d", score, p->best_parkour), 20, 70, 22, RAYWHITE);

        /* ground */
        DrawRectangle(0, (int)GROUND_Y, SW, SH - (int)GROUND_Y, (Color){60, 50, 40, 255});

        /* player */
        DrawRectangle((int)px, (int)py, (int)PLAYER_W, (int)PLAYER_H,
                      dead ? GRAY : (Color){80, 200, 120, 255});

        /* obstacles */
        for (int i = 0; i < nobs; i++)
            DrawRectangle((int)obs[i].x, (int)obs[i].y, (int)obs[i].w, (int)obs[i].h, ORANGE);

        if (dead)
            DrawText("HIT — press R", SW/2 - 80, SH/2, 28, RED);
        EndDrawing();
    }
    CloseWindow();
    if (score > p->best_parkour) p->best_parkour = score;
    lb_submit("parkour", p->name, score, 0);
}

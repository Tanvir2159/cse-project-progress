#include "hub.h"
/* Golf — 3 simple holes only */

#define BR 12.0f
#define HR 14.0f
#define LEVELS 3

typedef struct {
    Vector2 start, hole;
    const char *name;
    Rectangle wall;
} Level;

static Level make_level(int id) {
    Level L;
    L.start = (Vector2){120, SH / 2.0f};
    L.hole  = (Vector2){850, SH / 2.0f};
    L.wall  = (Rectangle){0, 0, 0, 0};
    if (id == 0) {
        L.name = "1  Open";
    } else if (id == 1) {
        L.name = "2  Wall";
        L.wall = (Rectangle){450, 150, 30, 350};
    } else {
        L.name = "3  High hole";
        L.hole = (Vector2){820, 150};
        L.wall = (Rectangle){400, 280, 30, 300};
    }
    return L;
}

void game_golf(Player *p) {
    int level = 0, strokes = 0, total = 0, aiming = 0;
    Level L = make_level(0);
    Vector2 ball = L.start, vel = {0, 0}, aim0 = {0, 0};

    gfx_open("Mini Golf");

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_ESCAPE)) break;
        if (IsKeyPressed(KEY_R)) {
            ball = L.start;
            vel = (Vector2){0, 0};
            strokes = 0;
        }

        float speed = sqrtf(vel.x * vel.x + vel.y * vel.y);
        int moving = speed > 3.0f;

        if (!moving) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 m = GetMousePosition();
                float dx = m.x - ball.x, dy = m.y - ball.y;
                if (dx * dx + dy * dy < 50 * 50) {
                    aiming = 1;
                    aim0 = m;
                }
            }
            if (aiming && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                Vector2 m = GetMousePosition();
                vel.x = (aim0.x - m.x) * 3.2f;
                vel.y = (aim0.y - m.y) * 3.2f;
                strokes++;
                total++;
                aiming = 0;
            }
            if (!IsMouseButtonDown(MOUSE_LEFT_BUTTON)) aiming = 0;
        } else {
            float dt = GetFrameTime();
            ball.x += vel.x * dt;
            ball.y += vel.y * dt;
            vel.x *= 0.985f;
            vel.y *= 0.985f;

            if (ball.x < BR) { ball.x = BR; vel.x = fabsf(vel.x) * 0.7f; }
            if (ball.x > SW - BR) { ball.x = SW - BR; vel.x = -fabsf(vel.x) * 0.7f; }
            if (ball.y < 70 + BR) { ball.y = 70 + BR; vel.y = fabsf(vel.y) * 0.7f; }
            if (ball.y > SH - BR) { ball.y = SH - BR; vel.y = -fabsf(vel.y) * 0.7f; }

            if (L.wall.width > 0) {
                Rectangle w = L.wall;
                if (ball.x + BR > w.x && ball.x - BR < w.x + w.width &&
                    ball.y + BR > w.y && ball.y - BR < w.y + w.height) {
                    /* simple bounce: reverse main axis */
                    if (ball.x < w.x + w.width / 2) { ball.x = w.x - BR; vel.x = -fabsf(vel.x) * 0.75f; }
                    else { ball.x = w.x + w.width + BR; vel.x = fabsf(vel.x) * 0.75f; }
                }
            }

            float hx = ball.x - L.hole.x, hy = ball.y - L.hole.y;
            if (hx * hx + hy * hy < HR * HR && speed < 100) {
                level++;
                if (level >= LEVELS) {
                    if (p->best_golf == 0 || total < p->best_golf)
                        p->best_golf = total;
                    lb_submit("golf", p->name, total, 1);
                    while (!WindowShouldClose() && !IsKeyPressed(KEY_ESCAPE)) {
                        BeginDrawing();
                        ClearBackground((Color){20, 80, 40, 255});
                        DrawText(TextFormat("DONE — %d strokes", total), 320, 280, 28, WHITE);
                        DrawText("ESC hub", 420, 330, 20, LIGHTGRAY);
                        EndDrawing();
                    }
                    CloseWindow();
                    return;
                }
                L = make_level(level);
                ball = L.start;
                vel = (Vector2){0, 0};
                strokes = 0;
            }
        }

        BeginDrawing();
        ClearBackground((Color){40, 140, 60, 255});
        gfx_topbar(L.name, "drag to aim | R reset | ESC hub");
        DrawText(TextFormat("Strokes %d   Total %d   Hole %d/%d",
                            strokes, total, level + 1, LEVELS), 20, 70, 20, RAYWHITE);

        if (L.wall.width > 0)
            DrawRectangleRec(L.wall, (Color){180, 160, 70, 255});

        DrawCircle(L.hole.x, L.hole.y, HR, BLACK);
        DrawLineEx((Vector2){L.hole.x, L.hole.y},
                   (Vector2){L.hole.x, L.hole.y - 36}, 3, WHITE);
        DrawTriangle(
            (Vector2){L.hole.x, L.hole.y - 36},
            (Vector2){L.hole.x, L.hole.y - 24},
            (Vector2){L.hole.x + 16, L.hole.y - 30}, RED);

        if (aiming) {
            Vector2 m = GetMousePosition();
            DrawLineEx(ball, (Vector2){ball.x + (ball.x - m.x), ball.y + (ball.y - m.y)},
                       3, GOLD);
        }

        DrawCircle(ball.x, ball.y, BR, RAYWHITE);
        EndDrawing();
    }
    CloseWindow();
    if (total > 0 && (p->best_golf == 0 || total < p->best_golf))
        p->best_golf = total;
}

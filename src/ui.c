#include "ui.h"

#include "food.h"

#include <math.h>
#include <stdio.h>

static const Color C_BG_TOP = {17, 22, 36, 255};
static const Color C_BG_BOTTOM = {40, 47, 67, 255};
static const Color C_PANEL = {30, 36, 53, 236};
static const Color C_LINE = {106, 125, 160, 80};
static const Color C_TEXT = {239, 244, 255, 255};
static const Color C_MUTED = {165, 176, 197, 255};
static const Color C_ACCENT = {100, 232, 151, 255};
static const Color C_BLUE = {72, 178, 255, 255};
static const Color C_ORANGE = {255, 184, 76, 255};
static const Color C_MAGENTA = {255, 92, 166, 255};
static const Color C_YELLOW = {255, 224, 108, 255};
static const Color C_DANGER = {255, 105, 105, 255};
static const Color C_BOARD_A = {35, 53, 58, 255};
static const Color C_BOARD_B = {30, 46, 53, 255};

static void DrawBackground(void);
static void DrawHeader(void);
static void DrawBoardFrame(void);
static void DrawBoardGrid(void);
static void DrawSnake(const Game *game);
static void DrawSnakeEyes(Cell head, Direction direction);
static void DrawSidePanel(const Game *game);
static void DrawMenu(const Game *game);
static void DrawStateOverlay(const Game *game);
static void DrawButton(const UIButton *button);
static void DrawRoundedPanel(Rectangle rect, float roundness, Color color, Color border);
static void DrawTextFit(const char *text, Rectangle rect, int fontSize, Color color, bool centered);
static void DrawInfoTile(const char *label, const char *value, Rectangle rect, Color accent);
static void DrawPill(const char *text, Rectangle rect, Color accent);
static void DrawTitleSnakeMark(Vector2 origin, float time);
static Vector2 BoardCellCenter(Cell cell);
static Color ButtonBaseColor(ButtonId id);
static Color MixColor(Color a, Color b, float t);
static Color WithAlpha(Color color, float alpha);
static void FormatTime(float seconds, char *buffer, int bufferSize);
static float SpeedRatio(const Game *game);

void UIDrawGame(const Game *game) {
    ClearBackground(C_BG_BOTTOM);
    DrawBackground();
    DrawHeader();
    DrawBoardFrame();
    DrawBoardGrid();
    FoodDraw(game);
    DrawSnake(game);
    DrawSidePanel(game);

    if (game->state == STATE_MENU) {
        DrawMenu(game);
    } else if (game->state == STATE_PAUSED || game->state == STATE_GAME_OVER) {
        DrawStateOverlay(game);
    }
}

static void DrawBackground(void) {
    float time = (float)GetTime();
    float drift = fmodf(time * 14.0f, 64.0f);

    DrawRectangleGradientV(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, C_BG_TOP, C_BG_BOTTOM);

    for (int x = -64; x < WINDOW_WIDTH + 64; x += 32) {
        DrawLine(x + (int)drift, 0, x + (int)drift, WINDOW_HEIGHT, (Color){255, 255, 255, 10});
    }
    for (int y = -64; y < WINDOW_HEIGHT + 64; y += 32) {
        DrawLine(0, y + (int)(drift * 0.55f), WINDOW_WIDTH, y + (int)(drift * 0.55f), (Color){255, 255, 255, 9});
    }
    for (int i = -WINDOW_HEIGHT; i < WINDOW_WIDTH; i += 48) {
        DrawLineEx((Vector2){(float)i + drift * 0.35f, 0.0f},
                   (Vector2){(float)(i + WINDOW_HEIGHT) + drift * 0.35f, (float)WINDOW_HEIGHT}, 1.0f,
                   (Color){120, 230, 190, 8});
    }

    DrawRectangleGradientV(0, 0, WINDOW_WIDTH, 170, (Color){89, 158, 255, 38}, (Color){89, 158, 255, 0});
}

static void DrawHeader(void) {
    Rectangle header = {48, 30, 864, 102};
    float time = (float)GetTime();

    DrawRoundedPanel(header, 0.18f, C_PANEL, C_LINE);

    DrawTitleSnakeMark((Vector2){76.0f, 65.0f}, time);
    DrawText("SNAKE", 126, 50, 36, C_TEXT);
    DrawText("NEON RUN", 128, 86, 18, C_ACCENT);
    DrawText("SOLO ARCADE", 304, 55, 24, C_MUTED);
    DrawText("Bright board. Fast turns. One clean run.", 304, 88, 17, (Color){198, 209, 229, 255});

    DrawPill("SOLO", (Rectangle){720, 52, 64, 28}, C_BLUE);
    DrawPill("SCORE", (Rectangle){794, 52, 78, 28}, C_ACCENT);
    DrawPill("BEST RUN", (Rectangle){720, 88, 152, 26}, C_ORANGE);
}

static void DrawBoardFrame(void) {
    Rectangle shadow = {BOARD_X + 8, BOARD_Y + 12, BOARD_W, BOARD_H};
    Rectangle board = {BOARD_X, BOARD_Y, BOARD_W, BOARD_H};

    DrawRectangleRounded(shadow, 0.04f, 12, (Color){0, 0, 0, 82});
    for (int i = 0; i < 5; i++) {
        float pad = 18.0f + (float)i * 5.0f;
        float alpha = 34.0f - (float)i * 5.0f;
        DrawRectangleRoundedLinesEx((Rectangle){BOARD_X - pad, BOARD_Y - pad, BOARD_W + pad * 2.0f, BOARD_H + pad * 2.0f},
                                    0.055f, 16, 2.0f, (Color){92, 210, 176, (unsigned char)alpha});
    }
    DrawRectangleRounded((Rectangle){BOARD_X - 12, BOARD_Y - 12, BOARD_W + 24, BOARD_H + 24}, 0.05f, 14,
                         (Color){16, 23, 34, 236});
    DrawRectangleRoundedLinesEx((Rectangle){BOARD_X - 12, BOARD_Y - 12, BOARD_W + 24, BOARD_H + 24}, 0.05f, 14, 2.0f,
                                (Color){122, 151, 184, 112});
    DrawRectangleRounded(board, 0.03f, 10, C_BOARD_A);
}

static void DrawBoardGrid(void) {
    for (int y = 0; y < BOARD_ROWS; y++) {
        for (int x = 0; x < BOARD_COLS; x++) {
            Color cellColor = ((x + y) % 2 == 0) ? C_BOARD_A : C_BOARD_B;
            DrawRectangle(BOARD_X + x * CELL_SIZE, BOARD_Y + y * CELL_SIZE, CELL_SIZE, CELL_SIZE, cellColor);
        }
    }

    for (int x = 0; x <= BOARD_COLS; x++) {
        int px = BOARD_X + x * CELL_SIZE;
        DrawLine(px, BOARD_Y, px, BOARD_Y + BOARD_H, (Color){255, 255, 255, x % 5 == 0 ? 18 : 8});
    }
    for (int y = 0; y <= BOARD_ROWS; y++) {
        int py = BOARD_Y + y * CELL_SIZE;
        DrawLine(BOARD_X, py, BOARD_X + BOARD_W, py, (Color){255, 255, 255, y % 4 == 0 ? 16 : 7});
    }

    DrawRectangleGradientV(BOARD_X, BOARD_Y, BOARD_W, 68, (Color){255, 255, 255, 18}, (Color){255, 255, 255, 0});
    DrawRectangleRoundedLinesEx((Rectangle){BOARD_X + 1, BOARD_Y + 1, BOARD_W - 2, BOARD_H - 2}, 0.03f, 10, 2.0f,
                                (Color){185, 239, 219, 58});
}

static void DrawSnake(const Game *game) {
    const Snake *snake = &game->snake;
    float flash = game->eatFeedback / 0.24f;
    if (flash < 0.0f) flash = 0.0f;
    if (flash > 1.0f) flash = 1.0f;

    Vector2 headCenter = BoardCellCenter(snake->body[0]);
    DrawCircleGradient(headCenter, 28.0f + flash * 18.0f, (Color){104, 255, 177, (unsigned char)(42.0f + flash * 60.0f)},
                       (Color){104, 255, 177, 0});

    for (int i = snake->length - 1; i >= 0; i--) {
        Vector2 center = BoardCellCenter(snake->body[i]);
        float depth = snake->length > 1 ? (float)i / (float)(snake->length - 1) : 0.0f;
        float radius = 7.3f + (1.0f - depth) * 1.6f;
        if (i == 0) {
            radius = 9.3f;
        }
        DrawCircleV((Vector2){center.x + 2.0f, center.y + 3.0f}, radius, (Color){0, 0, 0, 58});
    }

    for (int i = snake->length - 2; i >= 0; i--) {
        Vector2 a = BoardCellCenter(snake->body[i]);
        Vector2 b = BoardCellCenter(snake->body[i + 1]);
        float depth = snake->length > 1 ? (float)i / (float)(snake->length - 1) : 0.0f;
        Color linkColor = MixColor((Color){86, 224, 139, 255}, (Color){49, 188, 197, 255}, depth);
        linkColor = MixColor(linkColor, C_YELLOW, flash * (0.22f + 0.16f * (1.0f - depth)));

        if (fabsf(a.x - b.x) < 0.1f) {
            float y = a.y < b.y ? a.y : b.y;
            Rectangle link = {a.x - 7.1f, y, 14.2f, fabsf(a.y - b.y)};
            DrawRectangleRounded((Rectangle){link.x + 2.0f, link.y + 3.0f, link.width, link.height}, 0.5f, 8,
                                 (Color){0, 0, 0, 42});
            DrawRectangleRounded(link, 0.5f, 8, linkColor);
        } else if (fabsf(a.y - b.y) < 0.1f) {
            float x = a.x < b.x ? a.x : b.x;
            Rectangle link = {x, a.y - 7.1f, fabsf(a.x - b.x), 14.2f};
            DrawRectangleRounded((Rectangle){link.x + 2.0f, link.y + 3.0f, link.width, link.height}, 0.5f, 8,
                                 (Color){0, 0, 0, 42});
            DrawRectangleRounded(link, 0.5f, 8, linkColor);
        }
    }

    for (int i = snake->length - 1; i >= 0; i--) {
        Vector2 center = BoardCellCenter(snake->body[i]);
        float depth = snake->length > 1 ? (float)i / (float)(snake->length - 1) : 0.0f;
        float radius = 7.3f + (1.0f - depth) * 1.6f;
        Color base = MixColor((Color){89, 231, 140, 255}, (Color){49, 190, 202, 255}, depth);

        if (i == 0) {
            radius = 9.3f;
            base = MixColor(C_ACCENT, C_YELLOW, flash * 0.55f);
        } else {
            base = MixColor(base, C_YELLOW, flash * 0.24f * (1.0f - depth));
        }

        DrawCircleV(center, radius, base);
        DrawCircleV((Vector2){center.x - radius * 0.28f, center.y - radius * 0.32f}, radius * 0.28f,
                    (Color){229, 255, 238, 148});
    }

    DrawSnakeEyes(snake->body[0], snake->direction);
}

static void DrawSnakeEyes(Cell head, Direction direction) {
    float cx = BOARD_X + (float)head.x * CELL_SIZE + CELL_SIZE * 0.5f;
    float cy = BOARD_Y + (float)head.y * CELL_SIZE + CELL_SIZE * 0.5f;
    Vector2 forward = {0.0f, 0.0f};
    Vector2 side = {0.0f, 0.0f};

    if (direction == DIR_UP) {
        forward.y = -4.2f;
        side.x = 4.5f;
    } else if (direction == DIR_DOWN) {
        forward.y = 4.2f;
        side.x = 4.5f;
    } else if (direction == DIR_LEFT) {
        forward.x = -4.2f;
        side.y = 4.5f;
    } else {
        forward.x = 4.2f;
        side.y = 4.5f;
    }

    Vector2 eyeA = {cx + forward.x + side.x, cy + forward.y + side.y};
    Vector2 eyeB = {cx + forward.x - side.x, cy + forward.y - side.y};

    DrawCircleV(eyeA, 2.9f, RAYWHITE);
    DrawCircleV(eyeB, 2.9f, RAYWHITE);
    DrawCircleV((Vector2){eyeA.x + forward.x * 0.16f, eyeA.y + forward.y * 0.16f}, 1.25f,
                (Color){23, 29, 42, 255});
    DrawCircleV((Vector2){eyeB.x + forward.x * 0.16f, eyeB.y + forward.y * 0.16f}, 1.25f,
                (Color){23, 29, 42, 255});
}

static void DrawSidePanel(const Game *game) {
    Rectangle panel = {PANEL_X, PANEL_Y, PANEL_W, PANEL_H};
    char scoreText[64];
    char bestText[64];
    char timeText[32];
    char levelText[32];
    char speedText[32];

    DrawRoundedPanel(panel, 0.08f, C_PANEL, C_LINE);

    DrawText("RUN STATUS", PANEL_X + 24, PANEL_Y + 20, 20, C_TEXT);
    DrawPill(GameStateText(game->state), (Rectangle){PANEL_X + 24, PANEL_Y + 48, 88, 28}, C_ACCENT);
    DrawPill(GameDifficultyText(game->difficulty), (Rectangle){PANEL_X + 120, PANEL_Y + 48, 86, 28}, C_ORANGE);

    snprintf(scoreText, sizeof(scoreText), "%d", game->score);
    DrawText("Score", PANEL_X + 24, PANEL_Y + 88, 17, C_MUTED);
    int scoreSize = 38 + (int)(game->scorePulse * 30.0f);
    DrawTextFit(scoreText, (Rectangle){PANEL_X + 22, PANEL_Y + 104 - (scoreSize - 38) / 2, 186, 54}, scoreSize, C_TEXT, false);

    snprintf(bestText, sizeof(bestText), "%d", game->highScore);
    FormatTime(game->runTime, timeText, sizeof(timeText));
    snprintf(levelText, sizeof(levelText), "%d", game->level);
    snprintf(speedText, sizeof(speedText), "%.1f/s", 1.0f / game->moveInterval);

    DrawInfoTile("BEST", bestText, (Rectangle){PANEL_X + 24, PANEL_Y + 164, 86, 58}, C_ORANGE);
    DrawInfoTile("TIME", timeText, (Rectangle){PANEL_X + 120, PANEL_Y + 164, 86, 58}, C_BLUE);
    DrawInfoTile("LEVEL", levelText, (Rectangle){PANEL_X + 24, PANEL_Y + 232, 86, 54}, C_ACCENT);
    DrawInfoTile("SPEED", speedText, (Rectangle){PANEL_X + 120, PANEL_Y + 232, 86, 54}, C_MAGENTA);

    DrawText("Pace", PANEL_X + 24, PANEL_Y + 294, 15, C_MUTED);
    DrawRectangleRounded((Rectangle){PANEL_X + 68, PANEL_Y + 299, 138, 8}, 0.5f, 8, (Color){255, 255, 255, 34});
    DrawRectangleRounded((Rectangle){PANEL_X + 68, PANEL_Y + 299, 138.0f * SpeedRatio(game), 8}, 0.5f, 8, C_BLUE);

    for (int i = 0; i < SIDE_BUTTON_COUNT; i++) {
        DrawButton(&game->sideButtons[i]);
    }
}

static void DrawMenu(const Game *game) {
    float alpha = game->overlayAlpha;
    Rectangle board = {BOARD_X, BOARD_Y, BOARD_W, BOARD_H};
    Rectangle modal = {BOARD_X + 120, BOARD_Y + 28, BOARD_W - 240, 426};
    char bestText[64];
    char modeText[80];

    snprintf(bestText, sizeof(bestText), "Best %d", game->highScore);
    snprintf(modeText, sizeof(modeText), "%s  /  %s", GameDifficultyText(game->difficulty), game->soundEnabled ? "Sound On" : "Sound Off");

    DrawRectangleRec(board, WithAlpha((Color){9, 13, 20, 255}, 128.0f * alpha));
    DrawRoundedPanel(modal, 0.075f, WithAlpha((Color){25, 31, 48, 255}, 244.0f * alpha), WithAlpha(C_LINE, 255.0f * alpha));
    DrawRectangleRoundedLinesEx((Rectangle){modal.x + 8, modal.y + 8, modal.width - 16, modal.height - 16}, 0.07f, 12, 1.2f,
                                WithAlpha((Color){116, 232, 178, 255}, 82.0f * alpha));

    DrawTitleSnakeMark((Vector2){modal.x + 34.0f, modal.y + 58.0f}, (float)GetTime());
    DrawTextFit("SNAKE RUN", (Rectangle){modal.x + 84, modal.y + 28, modal.width - 108, 46}, 34, WithAlpha(C_TEXT, 255.0f * alpha), true);
    DrawTextFit("Solo neon arena", (Rectangle){modal.x, modal.y + 80, modal.width, 26}, 18, WithAlpha(C_MUTED, 255.0f * alpha), true);

    DrawPill(bestText, (Rectangle){modal.x + 54, modal.y + 124, 112, 30}, C_ORANGE);
    DrawPill(modeText, (Rectangle){modal.x + 176, modal.y + 124, 132, 30}, C_BLUE);

    for (int i = 0; i < MENU_BUTTON_COUNT; i++) {
        DrawButton(&game->menuButtons[i]);
    }
}

static void DrawStateOverlay(const Game *game) {
    float alpha = game->overlayAlpha;
    Rectangle board = {BOARD_X, BOARD_Y, BOARD_W, BOARD_H};
    Rectangle modal = {BOARD_X + 92, BOARD_Y + 72 + (1.0f - alpha) * 12.0f, BOARD_W - 184, 318};
    const char *title = game->state == STATE_GAME_OVER ? "GAME OVER" : "PAUSED";
    const char *message = game->state == STATE_GAME_OVER ? "Final result" : "Run suspended";
    char scoreText[64];
    char bestText[64];
    char timeText[32];
    char modeText[40];

    DrawRectangleRec(board, WithAlpha((Color){9, 13, 20, 255}, 122.0f * alpha));
    DrawRoundedPanel(modal, 0.075f, WithAlpha((Color){27, 33, 49, 255}, 240.0f * alpha), WithAlpha(C_LINE, 255.0f * alpha));
    DrawTextFit(title, (Rectangle){modal.x, modal.y + 24, modal.width, 46}, 38, WithAlpha(C_TEXT, 255.0f * alpha), true);
    DrawTextFit(message, (Rectangle){modal.x, modal.y + 76, modal.width, 28}, 20, WithAlpha(C_MUTED, 255.0f * alpha), true);

    snprintf(scoreText, sizeof(scoreText), "%d", game->score);
    snprintf(bestText, sizeof(bestText), "%d", game->highScore);
    FormatTime(game->runTime, timeText, sizeof(timeText));
    snprintf(modeText, sizeof(modeText), "%s", GameDifficultyText(game->difficulty));

    DrawInfoTile(game->state == STATE_GAME_OVER ? "FINAL" : "SCORE", scoreText,
                 (Rectangle){modal.x + 34, modal.y + 122, 160, 58}, C_ACCENT);
    DrawInfoTile("BEST", bestText, (Rectangle){modal.x + 222, modal.y + 122, 160, 58}, C_ORANGE);
    DrawInfoTile("TIME", timeText, (Rectangle){modal.x + 34, modal.y + 190, 160, 58}, C_BLUE);
    DrawInfoTile("MODE", modeText, (Rectangle){modal.x + 222, modal.y + 190, 160, 58}, C_MAGENTA);

    for (int i = 0; i < OVERLAY_BUTTON_COUNT; i++) {
        DrawButton(&game->overlayButtons[i]);
    }
}

static void DrawButton(const UIButton *button) {
    Color base = ButtonBaseColor(button->id);
    Color fill = MixColor(base, MixColor(base, RAYWHITE, 0.34f), button->hoverT);
    Color border = MixColor((Color){255, 255, 255, 72}, (Color){255, 255, 255, 190}, button->hoverT);
    Color textColor = (button->id == BUTTON_QUIT) ? C_TEXT : (Color){12, 30, 30, 255};
    Rectangle rect = button->bounds;
    float lift = 1.5f * button->hoverT - 2.0f * button->pressT;

    rect.y -= lift;
    rect.x -= button->hoverT * 1.5f;
    rect.width += button->hoverT * 3.0f;

    DrawRectangleRounded((Rectangle){rect.x, rect.y + 4.0f + button->pressT * 2.0f, rect.width, rect.height}, 0.22f, 10,
                         (Color){0, 0, 0, 60});
    DrawRectangleRounded(rect, 0.22f, 10, fill);
    DrawRectangleRounded((Rectangle){rect.x + 3.0f, rect.y + 3.0f, rect.width - 6.0f, rect.height * 0.42f}, 0.22f, 10,
                         (Color){255, 255, 255, (unsigned char)(28.0f + 40.0f * button->hoverT)});
    DrawRectangleRoundedLinesEx(rect, 0.22f, 10, 2.0f, border);
    DrawTextFit(button->label, rect, rect.height < 38.0f ? 17 : 19, textColor, true);
}

static void DrawRoundedPanel(Rectangle rect, float roundness, Color color, Color border) {
    DrawRectangleRounded((Rectangle){rect.x + 4, rect.y + 7, rect.width, rect.height}, roundness, 12, (Color){0, 0, 0, color.a / 5});
    DrawRectangleRounded(rect, roundness, 12, color);
    DrawRectangleRoundedLinesEx(rect, roundness, 12, 1.5f, border);
}

static void DrawTextFit(const char *text, Rectangle rect, int fontSize, Color color, bool centered) {
    int size = fontSize;
    int width = MeasureText(text, size);

    while (width > (int)(rect.width - 16.0f) && size > 12) {
        size--;
        width = MeasureText(text, size);
    }

    int x = centered ? (int)(rect.x + (rect.width - width) * 0.5f) : (int)rect.x;
    int y = (int)(rect.y + (rect.height - size) * 0.5f);
    DrawText(text, x, y, size, color);
}

static void DrawInfoTile(const char *label, const char *value, Rectangle rect, Color accent) {
    DrawRectangleRounded(rect, 0.12f, 10, (Color){18, 25, 38, 176});
    DrawRectangleRoundedLinesEx(rect, 0.12f, 10, 1.2f, WithAlpha(accent, 92.0f));
    DrawTextFit(label, (Rectangle){rect.x + 8, rect.y + 6, rect.width - 16, 18}, 13, C_MUTED, false);
    DrawTextFit(value, (Rectangle){rect.x + 8, rect.y + 25, rect.width - 16, rect.height - 30}, 22, accent, false);
}

static void DrawPill(const char *text, Rectangle rect, Color accent) {
    DrawRectangleRounded(rect, 0.5f, 12, WithAlpha(accent, 38.0f));
    DrawRectangleRoundedLinesEx(rect, 0.5f, 12, 1.2f, WithAlpha(accent, 112.0f));
    DrawTextFit(text, rect, 15, MixColor(C_TEXT, accent, 0.25f), true);
}

static void DrawTitleSnakeMark(Vector2 origin, float time) {
    for (int i = 0; i < 5; i++) {
        float t = (float)i / 4.0f;
        Vector2 p = {
            origin.x + cosf(time * 1.7f + t * PI) * 4.0f + t * 11.0f,
            origin.y + sinf(time * 1.7f + t * PI) * 7.0f
        };
        float radius = 7.8f - t * 1.2f;
        Color color = MixColor(C_ACCENT, C_BLUE, t * 0.55f);
        DrawCircleV((Vector2){p.x + 2.0f, p.y + 3.0f}, radius, (Color){0, 0, 0, 54});
        DrawCircleV(p, radius, color);
        DrawCircleV((Vector2){p.x - radius * 0.28f, p.y - radius * 0.28f}, radius * 0.26f,
                    (Color){236, 255, 244, 150});
    }
}

static Vector2 BoardCellCenter(Cell cell) {
    return (Vector2){
        BOARD_X + (float)cell.x * CELL_SIZE + CELL_SIZE * 0.5f,
        BOARD_Y + (float)cell.y * CELL_SIZE + CELL_SIZE * 0.5f
    };
}

static Color ButtonBaseColor(ButtonId id) {
    switch (id) {
        case BUTTON_START:
        case BUTTON_PAUSE:
            return C_ACCENT;
        case BUTTON_DIFFICULTY:
            return C_BLUE;
        case BUTTON_SOUND:
            return C_MAGENTA;
        case BUTTON_RESET_BEST:
            return C_YELLOW;
        case BUTTON_RESTART:
            return C_ORANGE;
        case BUTTON_MENU:
            return (Color){154, 176, 210, 255};
        case BUTTON_QUIT:
            return C_DANGER;
    }
    return C_ACCENT;
}

static Color MixColor(Color a, Color b, float t) {
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    return (Color){
        (unsigned char)((float)a.r + ((float)b.r - (float)a.r) * t),
        (unsigned char)((float)a.g + ((float)b.g - (float)a.g) * t),
        (unsigned char)((float)a.b + ((float)b.b - (float)a.b) * t),
        (unsigned char)((float)a.a + ((float)b.a - (float)a.a) * t)
    };
}

static Color WithAlpha(Color color, float alpha) {
    if (alpha < 0.0f) alpha = 0.0f;
    if (alpha > 255.0f) alpha = 255.0f;
    color.a = (unsigned char)alpha;
    return color;
}

static void FormatTime(float seconds, char *buffer, int bufferSize) {
    int total = (int)seconds;
    int minutes = total / 60;
    int secs = total % 60;
    snprintf(buffer, (size_t)bufferSize, "%02d:%02d", minutes, secs);
}

static float SpeedRatio(const Game *game) {
    float denominator = game->baseMoveInterval - game->minMoveInterval;
    if (denominator <= 0.0f) {
        return 1.0f;
    }

    float ratio = 1.0f - (game->moveInterval - game->minMoveInterval) / denominator;
    if (ratio < 0.0f) return 0.0f;
    if (ratio > 1.0f) return 1.0f;
    return ratio;
}

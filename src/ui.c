#include "ui.h"

#include "food.h"

#include <math.h>
#include <stdio.h>

static const Color C_BG_TOP = {24, 28, 43, 255};
static const Color C_BG_BOTTOM = {44, 51, 72, 255};
static const Color C_PANEL = {32, 38, 55, 232};
static const Color C_LINE = {106, 125, 160, 80};
static const Color C_TEXT = {239, 244, 255, 255};
static const Color C_MUTED = {165, 176, 197, 255};
static const Color C_ACCENT = {100, 232, 151, 255};
static const Color C_ACCENT_DARK = {34, 166, 101, 255};
static const Color C_BLUE = {72, 178, 255, 255};
static const Color C_ORANGE = {255, 184, 76, 255};
static const Color C_BOARD_A = {35, 53, 58, 255};
static const Color C_BOARD_B = {30, 46, 53, 255};

static void UpdateButton(UIButton *button, Vector2 mouse, float dt);
static void ResetButton(UIButton *button, float dt);
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
static Color MixColor(Color a, Color b, float t);
static Color WithAlpha(Color color, float alpha);
static float Approach(float value, float target, float amount);
static float SpeedRatio(const Game *game);

void UIInit(Game *game) {
    game->menuButtons[0] = (UIButton){{BOARD_X + 180, BOARD_Y + 236, 240, 46}, "Start Game", BUTTON_START, false, false, 0.0f, 0.0f};
    game->menuButtons[1] = (UIButton){{BOARD_X + 180, BOARD_Y + 294, 240, 46}, "Difficulty", BUTTON_DIFFICULTY, false, false, 0.0f, 0.0f};
    game->menuButtons[2] = (UIButton){{BOARD_X + 180, BOARD_Y + 352, 240, 46}, "Quit", BUTTON_QUIT, false, false, 0.0f, 0.0f};

    game->sideButtons[0] = (UIButton){{PANEL_X + 24, PANEL_Y + 304, 182, 38}, "Pause", BUTTON_PAUSE, false, false, 0.0f, 0.0f};
    game->sideButtons[1] = (UIButton){{PANEL_X + 24, PANEL_Y + 350, 182, 38}, "Restart", BUTTON_RESTART, false, false, 0.0f, 0.0f};
    game->sideButtons[2] = (UIButton){{PANEL_X + 24, PANEL_Y + 396, 182, 38}, "Sound", BUTTON_SOUND, false, false, 0.0f, 0.0f};
    game->sideButtons[3] = (UIButton){{PANEL_X + 24, PANEL_Y + 442, 182, 38}, "Reset Best", BUTTON_RESET_BEST, false, false, 0.0f, 0.0f};

    game->overlayButtons[0] = (UIButton){{BOARD_X + 145, BOARD_Y + 298, 146, 44}, "Resume", BUTTON_START, false, false, 0.0f, 0.0f};
    game->overlayButtons[1] = (UIButton){{BOARD_X + 309, BOARD_Y + 298, 146, 44}, "Menu", BUTTON_MENU, false, false, 0.0f, 0.0f};
}

void UIUpdateButtons(Game *game, float dt) {
    Vector2 mouse = GetMousePosition();
    static char menuDifficulty[40];
    static char soundLabel[24];
    static char pauseLabel[24];

    snprintf(menuDifficulty, sizeof(menuDifficulty), "Difficulty: %s", GameDifficultyText(game->difficulty));
    game->menuButtons[0].label = "Start Game";
    game->menuButtons[1].label = menuDifficulty;
    game->menuButtons[2].label = "Quit";

    snprintf(soundLabel, sizeof(soundLabel), "Sound: %s", game->soundEnabled ? "On" : "Off");
    if (game->state == STATE_PAUSED) {
        snprintf(pauseLabel, sizeof(pauseLabel), "Resume");
        game->sideButtons[0].id = BUTTON_PAUSE;
    } else if (game->state == STATE_MENU || game->state == STATE_GAME_OVER) {
        snprintf(pauseLabel, sizeof(pauseLabel), "Start");
        game->sideButtons[0].id = BUTTON_START;
    } else {
        snprintf(pauseLabel, sizeof(pauseLabel), "Pause");
        game->sideButtons[0].id = BUTTON_PAUSE;
    }
    game->sideButtons[0].label = pauseLabel;
    game->sideButtons[1].label = "Restart";
    game->sideButtons[2].label = soundLabel;
    game->sideButtons[3].label = "Reset Best";

    for (int i = 0; i < 4; i++) {
        UpdateButton(&game->sideButtons[i], mouse, dt);
        if (game->sideButtons[i].hovered && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            GameActivateButton(game, game->sideButtons[i].id);
        }
    }

    if (game->state == STATE_MENU) {
        for (int i = 0; i < 3; i++) {
            UpdateButton(&game->menuButtons[i], mouse, dt);
            if (game->menuButtons[i].hovered && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                GameActivateButton(game, game->menuButtons[i].id);
            }
        }
    } else {
        for (int i = 0; i < 3; i++) {
            ResetButton(&game->menuButtons[i], dt);
        }
    }

    if (game->state == STATE_PAUSED || game->state == STATE_GAME_OVER) {
        game->overlayButtons[0].id = game->state == STATE_GAME_OVER ? BUTTON_RESTART : BUTTON_START;
        game->overlayButtons[0].label = game->state == STATE_GAME_OVER ? "New Game" : "Resume";
        game->overlayButtons[1].id = BUTTON_MENU;
        game->overlayButtons[1].label = "Menu";

        for (int i = 0; i < 2; i++) {
            UpdateButton(&game->overlayButtons[i], mouse, dt);
            if (game->overlayButtons[i].hovered && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                GameActivateButton(game, game->overlayButtons[i].id);
            }
        }
    } else {
        for (int i = 0; i < 2; i++) {
            ResetButton(&game->overlayButtons[i], dt);
        }
    }
}

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

static void UpdateButton(UIButton *button, Vector2 mouse, float dt) {
    button->hovered = CheckCollisionPointRec(mouse, button->bounds);
    button->pressed = button->hovered && IsMouseButtonDown(MOUSE_LEFT_BUTTON);
    button->hoverT = Approach(button->hoverT, button->hovered ? 1.0f : 0.0f, dt * 9.0f);
    button->pressT = Approach(button->pressT, button->pressed ? 1.0f : 0.0f, dt * 14.0f);
}

static void ResetButton(UIButton *button, float dt) {
    button->hovered = false;
    button->pressed = false;
    button->hoverT = Approach(button->hoverT, 0.0f, dt * 9.0f);
    button->pressT = Approach(button->pressT, 0.0f, dt * 14.0f);
}

static void DrawBackground(void) {
    DrawRectangleGradientV(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, C_BG_TOP, C_BG_BOTTOM);

    for (int x = 0; x < WINDOW_WIDTH; x += 32) {
        DrawLine(x, 0, x, WINDOW_HEIGHT, (Color){255, 255, 255, 13});
    }
    for (int y = 0; y < WINDOW_HEIGHT; y += 32) {
        DrawLine(0, y, WINDOW_WIDTH, y, (Color){255, 255, 255, 11});
    }
    for (int i = -WINDOW_HEIGHT; i < WINDOW_WIDTH; i += 48) {
        DrawLineEx((Vector2){(float)i, 0.0f}, (Vector2){(float)(i + WINDOW_HEIGHT), (float)WINDOW_HEIGHT}, 1.0f,
                   (Color){255, 255, 255, 8});
    }
}

static void DrawHeader(void) {
    Rectangle header = {48, 30, 864, 102};
    DrawRoundedPanel(header, 0.18f, C_PANEL, C_LINE);

    DrawText("SNAKE", 76, 52, 34, C_TEXT);
    DrawText("ARCADE", 76, 86, 18, C_ACCENT);
    DrawText("CLASSIC RUN", 242, 62, 22, C_MUTED);
    DrawText("DIFFICULTY  SOUND  SCORE CHASE", 242, 92, 18, (Color){198, 209, 229, 255});
}

static void DrawBoardFrame(void) {
    Rectangle shadow = {BOARD_X + 8, BOARD_Y + 12, BOARD_W, BOARD_H};
    Rectangle board = {BOARD_X, BOARD_Y, BOARD_W, BOARD_H};

    DrawRectangleRounded(shadow, 0.04f, 12, (Color){0, 0, 0, 64});
    DrawRectangleRounded((Rectangle){BOARD_X - 12, BOARD_Y - 12, BOARD_W + 24, BOARD_H + 24}, 0.05f, 14,
                         (Color){18, 24, 35, 230});
    DrawRectangleRoundedLinesEx((Rectangle){BOARD_X - 12, BOARD_Y - 12, BOARD_W + 24, BOARD_H + 24}, 0.05f, 14, 2.0f,
                                (Color){114, 135, 170, 90});
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
        DrawLine(px, BOARD_Y, px, BOARD_Y + BOARD_H, (Color){255, 255, 255, 12});
    }
    for (int y = 0; y <= BOARD_ROWS; y++) {
        int py = BOARD_Y + y * CELL_SIZE;
        DrawLine(BOARD_X, py, BOARD_X + BOARD_W, py, (Color){255, 255, 255, 10});
    }
}

static void DrawSnake(const Game *game) {
    const Snake *snake = &game->snake;

    for (int i = snake->length - 1; i >= 0; i--) {
        Cell part = snake->body[i];
        float x = BOARD_X + (float)part.x * CELL_SIZE;
        float y = BOARD_Y + (float)part.y * CELL_SIZE;
        float inset = (i == 0) ? 2.0f : 3.0f;
        Rectangle rect = {x + inset, y + inset, CELL_SIZE - inset * 2.0f, CELL_SIZE - inset * 2.0f};
        Color base = (i == 0) ? C_ACCENT : (i % 2 == 0 ? (Color){71, 214, 132, 255} : (Color){58, 194, 122, 255});

        DrawRectangleRounded(rect, 0.45f, 8, (Color){0, 0, 0, 45});
        rect.x -= 1.0f;
        rect.y -= 1.0f;
        DrawRectangleRounded(rect, 0.45f, 8, base);
        DrawCircle((int)(rect.x + rect.width * 0.33f), (int)(rect.y + rect.height * 0.28f), 2.0f,
                   (Color){215, 255, 229, 135});
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
    DrawRoundedPanel(panel, 0.08f, C_PANEL, C_LINE);

    DrawText("STATUS", PANEL_X + 24, PANEL_Y + 22, 20, C_TEXT);
    DrawText(GameStateText(game->state), PANEL_X + 24, PANEL_Y + 50, 18, C_ACCENT);
    DrawText(GameDifficultyText(game->difficulty), PANEL_X + 132, PANEL_Y + 50, 18, C_ORANGE);

    char text[64];
    snprintf(text, sizeof(text), "%d", game->score);
    DrawText("Score", PANEL_X + 24, PANEL_Y + 86, 17, C_MUTED);
    int scoreSize = 38 + (int)(game->scorePulse * 30.0f);
    DrawText(text, PANEL_X + 24, PANEL_Y + 108 - (scoreSize - 38) / 2, scoreSize, C_TEXT);

    snprintf(text, sizeof(text), "%d", game->highScore);
    DrawText("Best", PANEL_X + 24, PANEL_Y + 164, 17, C_MUTED);
    DrawText(text, PANEL_X + 24, PANEL_Y + 187, 28, C_ORANGE);

    snprintf(text, sizeof(text), "Level %d", game->level);
    DrawText(text, PANEL_X + 24, PANEL_Y + 235, 22, C_TEXT);
    DrawText("Speed", PANEL_X + 24, PANEL_Y + 269, 16, C_MUTED);
    DrawRectangleRounded((Rectangle){PANEL_X + 24, PANEL_Y + 292, 182, 8}, 0.5f, 8, (Color){255, 255, 255, 34});
    DrawRectangleRounded((Rectangle){PANEL_X + 24, PANEL_Y + 292, 182.0f * SpeedRatio(game), 8}, 0.5f, 8, C_BLUE);

    for (int i = 0; i < 4; i++) {
        DrawButton(&game->sideButtons[i]);
    }
}

static void DrawMenu(const Game *game) {
    float alpha = game->overlayAlpha;
    Rectangle board = {BOARD_X, BOARD_Y, BOARD_W, BOARD_H};
    Rectangle modal = {BOARD_X + 120, BOARD_Y + 62, BOARD_W - 240, 366};

    DrawRectangleRec(board, WithAlpha((Color){9, 13, 20, 255}, 112.0f * alpha));
    DrawRoundedPanel(modal, 0.08f, WithAlpha((Color){27, 33, 49, 255}, 238.0f * alpha), WithAlpha(C_LINE, 255.0f * alpha));
    DrawTextFit("SNAKE RUN", (Rectangle){modal.x, modal.y + 30, modal.width, 42}, 34, WithAlpha(C_TEXT, 255.0f * alpha), true);
    DrawTextFit("Single-player arcade mode", (Rectangle){modal.x, modal.y + 78, modal.width, 28}, 19, WithAlpha(C_MUTED, 255.0f * alpha), true);
    DrawTextFit("Arrow Keys / WASD", (Rectangle){modal.x, modal.y + 116, modal.width, 24}, 18, WithAlpha(C_ACCENT, 255.0f * alpha), true);

    for (int i = 0; i < 3; i++) {
        DrawButton(&game->menuButtons[i]);
    }
}

static void DrawStateOverlay(const Game *game) {
    float alpha = game->overlayAlpha;
    Rectangle board = {BOARD_X, BOARD_Y, BOARD_W, BOARD_H};
    Rectangle modal = {BOARD_X + 112, BOARD_Y + 98, BOARD_W - 224, 270};
    const char *title = game->state == STATE_GAME_OVER ? "GAME OVER" : "PAUSED";
    const char *message = game->state == STATE_GAME_OVER ? "Final result" : "Run suspended";
    char details[128];

    DrawRectangleRec(board, WithAlpha((Color){9, 13, 20, 255}, 122.0f * alpha));
    DrawRoundedPanel(modal, 0.08f, WithAlpha((Color){27, 33, 49, 255}, 238.0f * alpha), WithAlpha(C_LINE, 255.0f * alpha));
    DrawTextFit(title, (Rectangle){modal.x, modal.y + 28, modal.width, 46}, 38, WithAlpha(C_TEXT, 255.0f * alpha), true);
    DrawTextFit(message, (Rectangle){modal.x, modal.y + 88, modal.width, 28}, 20, WithAlpha(C_MUTED, 255.0f * alpha), true);

    snprintf(details, sizeof(details), "%s %d   Best %d", game->state == STATE_GAME_OVER ? "Final" : "Score", game->score,
             game->highScore);
    DrawTextFit(details, (Rectangle){modal.x, modal.y + 134, modal.width, 30}, 20, WithAlpha(C_ACCENT, 255.0f * alpha), true);

    for (int i = 0; i < 2; i++) {
        DrawButton(&game->overlayButtons[i]);
    }
}

static void DrawButton(const UIButton *button) {
    Color fill = MixColor(C_ACCENT_DARK, C_ACCENT, button->hoverT);
    Color border = MixColor((Color){188, 255, 212, 110}, (Color){238, 255, 244, 190}, button->hoverT);
    Rectangle rect = button->bounds;
    float lift = 1.5f * button->hoverT - 2.0f * button->pressT;

    rect.y -= lift;
    rect.x -= button->hoverT * 1.5f;
    rect.width += button->hoverT * 3.0f;

    DrawRectangleRounded((Rectangle){rect.x, rect.y + 4.0f + button->pressT * 2.0f, rect.width, rect.height}, 0.22f, 10,
                         (Color){0, 0, 0, 60});
    DrawRectangleRounded(rect, 0.22f, 10, fill);
    DrawRectangleRoundedLinesEx(rect, 0.22f, 10, 2.0f, border);
    DrawTextFit(button->label, rect, 19, (Color){11, 36, 25, 255}, true);
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

static float Approach(float value, float target, float amount) {
    if (value < target) {
        value += amount;
        if (value > target) {
            value = target;
        }
    } else if (value > target) {
        value -= amount;
        if (value < target) {
            value = target;
        }
    }
    return value;
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

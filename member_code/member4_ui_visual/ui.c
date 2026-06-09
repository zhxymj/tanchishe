#include "ui.h"

#include "food.h"

#include <math.h>
#include <stdio.h>

static const Color C_BG_TOP = {100, 178, 246, 255};
static const Color C_BG_BOTTOM = {214, 150, 244, 255};
static const Color C_PANEL = {24, 42, 82, 214};
static const Color C_SURFACE = {27, 45, 86, 230};
static const Color C_SURFACE_LIGHT = {88, 126, 206, 124};
static const Color C_LINE = {238, 244, 255, 116};
static const Color C_TEXT = {239, 244, 255, 255};
static const Color C_MUTED = {193, 215, 226, 255};
static const Color C_ACCENT = {86, 238, 154, 255};
static const Color C_BLUE = {64, 166, 245, 255};
static const Color C_ORANGE = {255, 181, 74, 255};
static const Color C_MAGENTA = {255, 93, 154, 255};
static const Color C_YELLOW = {255, 229, 98, 255};
static const Color C_DANGER = {255, 92, 103, 255};
static const Color C_BOARD_A = {109, 181, 238, 255};
static const Color C_BOARD_B = {96, 166, 228, 255};

static const char *UI_FONT_PATH = "C:/Windows/Fonts/simhei.ttf";
static const char *UI_FONT_CHARS =
    "贪吃蛇霓虹冲刺单机街机专注走位刷新最高分进行本局状态菜单运行中暂停结束"
    "普通简单困难分数时间等级速度节奏设置开始游戏难度重置纪录退出"
    "继续重开再来一局游戏结束已暂停最终成绩返回最高普通简单困难格秒模式清空"
    "大作战竞技场明亮休闲欢乐目标吞食糖果"
    "0123456789:/. ：，。";

static void DrawBackground(void);
static void DrawHeader(const Game *game);
static void DrawBoardFrame(void);
static void DrawBoardGrid(void);
static void DrawSnake(const Game *game);
static void DrawSnakeEyes(Cell head, Direction direction);
static void DrawSidePanel(const Game *game);
static void DrawMenu(const Game *game);
static void DrawStateOverlay(const Game *game);
static void DrawButton(const UIButton *button);
static void DrawRoundedPanel(Rectangle rect, float roundness, Color color, Color border);
static void DrawUIText(const char *text, int x, int y, int fontSize, Color color);
static void DrawTextFit(const char *text, Rectangle rect, int fontSize, Color color, bool centered);
static void DrawInfoTile(const char *label, const char *value, Rectangle rect, Color accent);
static void DrawPill(const char *text, Rectangle rect, Color accent);
static void DrawMiniDivider(Rectangle rect);
static void DrawCornerTicks(Rectangle rect, float size, Color color);
static void DrawTitleSnakeMark(Vector2 origin, float time);
static Vector2 BoardCellCenter(Cell cell);
static Color ButtonBaseColor(ButtonId id);
static bool ButtonIsSecondary(ButtonId id);
static Font GetUIFont(void);
static Color MixColor(Color a, Color b, float t);
static Color WithAlpha(Color color, float alpha);
static void FormatTime(float seconds, char *buffer, int bufferSize);
static float SpeedRatio(const Game *game);

void UIDrawGame(const Game *game) {
    ClearBackground(C_BG_BOTTOM);
    DrawBackground();
    DrawHeader(game);
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
    DrawRectangleGradientV(0, 0, WINDOW_WIDTH, 190, (Color){255, 255, 255, 58}, (Color){255, 255, 255, 0});
    DrawRectangleGradientV(0, WINDOW_HEIGHT - 210, WINDOW_WIDTH, 210, (Color){54, 55, 128, 0}, (Color){54, 55, 128, 78});

    for (int x = -64; x < WINDOW_WIDTH + 64; x += 32) {
        DrawLine(x + (int)drift, 0, x + (int)drift, WINDOW_HEIGHT, (Color){255, 255, 255, 18});
    }
    for (int y = -64; y < WINDOW_HEIGHT + 64; y += 32) {
        DrawLine(0, y + (int)(drift * 0.55f), WINDOW_WIDTH, y + (int)(drift * 0.55f), (Color){255, 255, 255, 15});
    }
    for (int i = -WINDOW_HEIGHT; i < WINDOW_WIDTH; i += 62) {
        DrawLineEx((Vector2){(float)i + drift * 0.35f, 0.0f},
                   (Vector2){(float)(i + WINDOW_HEIGHT) + drift * 0.35f, (float)WINDOW_HEIGHT}, 1.0f,
                   (Color){255, 255, 255, 12});
    }

    for (int y = 138; y < WINDOW_HEIGHT; y += 96) {
        DrawLineEx((Vector2){42.0f, (float)y}, (Vector2){WINDOW_WIDTH - 42.0f, (float)y + 34.0f}, 1.0f,
                   (Color){255, 255, 255, 12});
    }
}

static void DrawHeader(const Game *game) {
    Rectangle header = {48, 30, 864, 102};
    float time = (float)GetTime();
    char bestText[48];

    snprintf(bestText, sizeof(bestText), "纪录 %d", game->highScore);
    DrawRoundedPanel(header, 0.18f, C_PANEL, C_LINE);
    DrawRectangleRounded((Rectangle){header.x + 14, header.y + 14, 5, header.height - 28}, 0.6f, 8, C_ACCENT);
    DrawRectangleRounded((Rectangle){header.x + 28, header.y + 16, header.width - 56, 1.0f}, 0.5f, 4,
                         (Color){255, 255, 255, 38});
    DrawCornerTicks(header, 18.0f, (Color){176, 220, 255, 72});

    DrawTitleSnakeMark((Vector2){84.0f, 65.0f}, time);
    DrawUIText("蛇蛇大作战", 134, 48, 32, C_TEXT);
    DrawUIText("单机竞技场", 136, 86, 18, C_ACCENT);
    DrawUIText("明亮竞技场", 316, 54, 24, C_TEXT);
    DrawUIText("吞食糖果，刷新最高分。", 316, 88, 17, (Color){219, 244, 241, 255});

    DrawPill("单机", (Rectangle){714, 52, 66, 28}, C_BLUE);
    DrawPill(GameStateText(game->state), (Rectangle){790, 52, 66, 28}, C_ACCENT);
    DrawPill(bestText, (Rectangle){714, 88, 142, 26}, C_ORANGE);
}

static void DrawBoardFrame(void) {
    Rectangle shadow = {BOARD_X + 8, BOARD_Y + 12, BOARD_W, BOARD_H};
    Rectangle board = {BOARD_X, BOARD_Y, BOARD_W, BOARD_H};

    DrawRectangleRounded(shadow, 0.04f, 12, (Color){32, 46, 118, 54});
    for (int i = 0; i < 4; i++) {
        float pad = 15.0f + (float)i * 6.0f;
        float alpha = 50.0f - (float)i * 9.0f;
        DrawRectangleRoundedLinesEx((Rectangle){BOARD_X - pad, BOARD_Y - pad, BOARD_W + pad * 2.0f, BOARD_H + pad * 2.0f},
                                    0.055f, 16, 2.0f, (Color){255, 255, 255, (unsigned char)alpha});
    }
    DrawRectangleRounded((Rectangle){BOARD_X - 12, BOARD_Y - 12, BOARD_W + 24, BOARD_H + 24}, 0.05f, 14,
                         (Color){232, 238, 255, 96});
    DrawRectangleRoundedLinesEx((Rectangle){BOARD_X - 12, BOARD_Y - 12, BOARD_W + 24, BOARD_H + 24}, 0.05f, 14, 2.0f,
                                (Color){255, 255, 255, 132});
    DrawCornerTicks((Rectangle){BOARD_X - 12, BOARD_Y - 12, BOARD_W + 24, BOARD_H + 24}, 22.0f,
                    (Color){255, 255, 255, 120});
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
        DrawLine(px, BOARD_Y, px, BOARD_Y + BOARD_H, (Color){255, 255, 255, x % 5 == 0 ? 32 : 14});
    }
    for (int y = 0; y <= BOARD_ROWS; y++) {
        int py = BOARD_Y + y * CELL_SIZE;
        DrawLine(BOARD_X, py, BOARD_X + BOARD_W, py, (Color){255, 255, 255, y % 4 == 0 ? 30 : 13});
    }

    DrawRectangleGradientV(BOARD_X, BOARD_Y, BOARD_W, 86, (Color){255, 255, 255, 44}, (Color){255, 255, 255, 0});
    DrawRectangleGradientV(BOARD_X, BOARD_Y + BOARD_H - 84, BOARD_W, 84, (Color){47, 54, 130, 0}, (Color){47, 54, 130, 34});
    DrawRectangleGradientH(BOARD_X, BOARD_Y, 60, BOARD_H, (Color){255, 255, 255, 20}, (Color){255, 255, 255, 0});
    DrawRectangleGradientH(BOARD_X + BOARD_W - 60, BOARD_Y, 60, BOARD_H, (Color){255, 255, 255, 0}, (Color){255, 255, 255, 20});
    DrawRectangleRoundedLinesEx((Rectangle){BOARD_X + 1, BOARD_Y + 1, BOARD_W - 2, BOARD_H - 2}, 0.03f, 10, 2.0f,
                                (Color){255, 255, 255, 82});
}

static void DrawSnake(const Game *game) {
    const Snake *snake = &game->snake;
    float flash = game->eatFeedback / 0.24f;
    if (flash < 0.0f) flash = 0.0f;
    if (flash > 1.0f) flash = 1.0f;

    Vector2 headCenter = BoardCellCenter(snake->body[0]);
    DrawCircleGradient(headCenter, 36.0f + flash * 24.0f, (Color){255, 255, 255, (unsigned char)(52.0f + flash * 68.0f)},
                       (Color){104, 255, 177, 0});

    for (int i = snake->length - 1; i >= 0; i--) {
        Vector2 center = BoardCellCenter(snake->body[i]);
        float depth = snake->length > 1 ? (float)i / (float)(snake->length - 1) : 0.0f;
        float radius = 8.2f + (1.0f - depth) * 1.8f;
        if (i == 0) {
            radius = 10.8f;
        }
        DrawCircleV((Vector2){center.x + 2.0f, center.y + 3.0f}, radius, (Color){0, 90, 72, 54});
    }

    for (int i = snake->length - 2; i >= 0; i--) {
        Vector2 a = BoardCellCenter(snake->body[i]);
        Vector2 b = BoardCellCenter(snake->body[i + 1]);
        float depth = snake->length > 1 ? (float)i / (float)(snake->length - 1) : 0.0f;
        Color linkColor = MixColor((Color){84, 240, 146, 255}, (Color){60, 196, 238, 255}, depth);
        linkColor = MixColor(linkColor, C_YELLOW, flash * (0.22f + 0.16f * (1.0f - depth)));

        if (fabsf(a.x - b.x) < 0.1f) {
            float y = a.y < b.y ? a.y : b.y;
            Rectangle link = {a.x - 8.0f, y, 16.0f, fabsf(a.y - b.y)};
            DrawRectangleRounded((Rectangle){link.x + 2.0f, link.y + 3.0f, link.width, link.height}, 0.5f, 8,
                                 (Color){0, 90, 72, 36});
            DrawRectangleRounded(link, 0.5f, 8, linkColor);
        } else if (fabsf(a.y - b.y) < 0.1f) {
            float x = a.x < b.x ? a.x : b.x;
            Rectangle link = {x, a.y - 8.0f, fabsf(a.x - b.x), 16.0f};
            DrawRectangleRounded((Rectangle){link.x + 2.0f, link.y + 3.0f, link.width, link.height}, 0.5f, 8,
                                 (Color){0, 90, 72, 36});
            DrawRectangleRounded(link, 0.5f, 8, linkColor);
        }
    }

    for (int i = snake->length - 1; i >= 0; i--) {
        Vector2 center = BoardCellCenter(snake->body[i]);
        float depth = snake->length > 1 ? (float)i / (float)(snake->length - 1) : 0.0f;
        float radius = 8.2f + (1.0f - depth) * 1.8f;
        Color base = MixColor((Color){101, 248, 150, 255}, (Color){58, 201, 242, 255}, depth);

        if (i == 0) {
            radius = 10.8f;
            base = MixColor(C_ACCENT, C_YELLOW, flash * 0.55f);
        } else {
            base = MixColor(base, C_YELLOW, flash * 0.24f * (1.0f - depth));
        }

        DrawCircleV(center, radius, base);
        DrawCircleV((Vector2){center.x - radius * 0.28f, center.y - radius * 0.32f}, radius * 0.34f,
                    (Color){245, 255, 248, 168});
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
    DrawRectangleRounded((Rectangle){PANEL_X + 12, PANEL_Y + 12, 4, PANEL_H - 24}, 0.6f, 8, WithAlpha(C_BLUE, 118.0f));
    DrawRectangleRounded((Rectangle){PANEL_X + 22, PANEL_Y + 86, PANEL_W - 44, 1.0f}, 0.5f, 4,
                         (Color){255, 255, 255, 28});
    DrawCornerTicks(panel, 14.0f, (Color){176, 220, 255, 58});

    DrawUIText("本局状态", PANEL_X + 24, PANEL_Y + 20, 20, C_TEXT);
    DrawPill(GameStateText(game->state), (Rectangle){PANEL_X + 24, PANEL_Y + 48, 88, 28}, C_ACCENT);
    DrawPill(GameDifficultyText(game->difficulty), (Rectangle){PANEL_X + 120, PANEL_Y + 48, 86, 28}, C_ORANGE);

    snprintf(scoreText, sizeof(scoreText), "%d", game->score);
    DrawRectangleRounded((Rectangle){PANEL_X + 22, PANEL_Y + 88, 186, 68}, 0.1f, 10, (Color){18, 25, 38, 160});
    DrawRectangleRoundedLinesEx((Rectangle){PANEL_X + 22, PANEL_Y + 88, 186, 68}, 0.1f, 10, 1.2f, WithAlpha(C_ACCENT, 82.0f));
    DrawUIText("分数", PANEL_X + 34, PANEL_Y + 98, 14, C_MUTED);
    int scoreSize = 38 + (int)(game->scorePulse * 30.0f);
    DrawTextFit(scoreText, (Rectangle){PANEL_X + 32, PANEL_Y + 112 - (scoreSize - 38) / 2, 166, 42}, scoreSize, C_TEXT, false);

    snprintf(bestText, sizeof(bestText), "%d", game->highScore);
    FormatTime(game->runTime, timeText, sizeof(timeText));
    snprintf(levelText, sizeof(levelText), "%d", game->level);
    snprintf(speedText, sizeof(speedText), "%.1f格/秒", 1.0f / game->moveInterval);

    DrawInfoTile("最高", bestText, (Rectangle){PANEL_X + 24, PANEL_Y + 164, 86, 58}, C_ORANGE);
    DrawInfoTile("时间", timeText, (Rectangle){PANEL_X + 120, PANEL_Y + 164, 86, 58}, C_BLUE);
    DrawInfoTile("等级", levelText, (Rectangle){PANEL_X + 24, PANEL_Y + 232, 86, 54}, C_ACCENT);
    DrawInfoTile("速度", speedText, (Rectangle){PANEL_X + 120, PANEL_Y + 232, 86, 54}, C_MAGENTA);

    DrawMiniDivider((Rectangle){PANEL_X + 24, PANEL_Y + 286, 182, 1});
    DrawUIText("节奏", PANEL_X + 24, PANEL_Y + 294, 15, C_MUTED);
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
    char difficultyText[40];

    snprintf(bestText, sizeof(bestText), "最高 %d", game->highScore);
    snprintf(difficultyText, sizeof(difficultyText), "%s", GameDifficultyText(game->difficulty));

    DrawRectangleRec(board, WithAlpha((Color){20, 30, 82, 255}, 86.0f * alpha));
    DrawRoundedPanel(modal, 0.075f, WithAlpha(C_SURFACE, 244.0f * alpha), WithAlpha(C_LINE, 255.0f * alpha));
    DrawRectangleRoundedLinesEx((Rectangle){modal.x + 8, modal.y + 8, modal.width - 16, modal.height - 16}, 0.07f, 12, 1.2f,
                                WithAlpha((Color){116, 232, 178, 255}, 82.0f * alpha));
    DrawCornerTicks(modal, 18.0f, WithAlpha(C_ACCENT, 128.0f * alpha));
    DrawRectangleRounded((Rectangle){modal.x + 30, modal.y + 20, modal.width - 60, 1.0f}, 0.5f, 4,
                         WithAlpha((Color){255, 255, 255, 255}, 34.0f * alpha));

    DrawTitleSnakeMark((Vector2){modal.x + 34.0f, modal.y + 58.0f}, (float)GetTime());
    DrawTextFit("蛇蛇大作战", (Rectangle){modal.x + 84, modal.y + 28, modal.width - 108, 46}, 32, WithAlpha(C_TEXT, 255.0f * alpha), true);
    DrawTextFit("明亮单机竞技场", (Rectangle){modal.x, modal.y + 80, modal.width, 26}, 18, WithAlpha(C_MUTED, 255.0f * alpha), true);

    DrawPill(bestText, (Rectangle){modal.x + 30, modal.y + 120, 104, 30}, C_ORANGE);
    DrawPill(difficultyText, (Rectangle){modal.x + 146, modal.y + 120, 92, 30}, C_BLUE);
    DrawPill("单机", (Rectangle){modal.x + 250, modal.y + 120, 80, 30}, C_MAGENTA);

    DrawRectangleRounded((Rectangle){modal.x + 24, modal.y + 214, modal.width - 48, 138}, 0.05f, 8, WithAlpha(C_SURFACE_LIGHT, 150.0f * alpha));
    DrawRectangleRoundedLinesEx((Rectangle){modal.x + 24, modal.y + 214, modal.width - 48, 138}, 0.05f, 8, 1.0f,
                                WithAlpha(C_LINE, 130.0f * alpha));
    DrawUIText("设置", (int)(modal.x + 42), (int)(modal.y + 220), 13, WithAlpha(C_MUTED, 255.0f * alpha));
    DrawMiniDivider((Rectangle){modal.x + 42, modal.y + 238, modal.width - 84, 1});

    for (int i = 0; i < MENU_BUTTON_COUNT; i++) {
        DrawButton(&game->menuButtons[i]);
    }
}

static void DrawStateOverlay(const Game *game) {
    float alpha = game->overlayAlpha;
    Rectangle board = {BOARD_X, BOARD_Y, BOARD_W, BOARD_H};
    Rectangle modal = {BOARD_X + 92, BOARD_Y + 72 + (1.0f - alpha) * 12.0f, BOARD_W - 184, 318};
    const char *title = game->state == STATE_GAME_OVER ? "游戏结束" : "已暂停";
    const char *message = game->state == STATE_GAME_OVER ? "最终成绩" : "本局暂停";
    char scoreText[64];
    char bestText[64];
    char timeText[32];
    char modeText[40];

    DrawRectangleRec(board, WithAlpha((Color){20, 30, 82, 255}, 100.0f * alpha));
    DrawRoundedPanel(modal, 0.075f, WithAlpha(C_SURFACE, 242.0f * alpha), WithAlpha(C_LINE, 255.0f * alpha));
    DrawRectangleRounded((Rectangle){modal.x + 16, modal.y + 18, 5, modal.height - 36}, 0.6f, 8,
                         WithAlpha(game->state == STATE_GAME_OVER ? C_DANGER : C_BLUE, 170.0f * alpha));
    DrawCornerTicks(modal, 18.0f, WithAlpha(game->state == STATE_GAME_OVER ? C_DANGER : C_BLUE, 128.0f * alpha));
    DrawTextFit(title, (Rectangle){modal.x, modal.y + 24, modal.width, 46}, 38, WithAlpha(C_TEXT, 255.0f * alpha), true);
    DrawTextFit(message, (Rectangle){modal.x, modal.y + 76, modal.width, 28}, 20, WithAlpha(C_MUTED, 255.0f * alpha), true);
    DrawMiniDivider((Rectangle){modal.x + 46, modal.y + 112, modal.width - 92, 1});

    snprintf(scoreText, sizeof(scoreText), "%d", game->score);
    snprintf(bestText, sizeof(bestText), "%d", game->highScore);
    FormatTime(game->runTime, timeText, sizeof(timeText));
    snprintf(modeText, sizeof(modeText), "%s", GameDifficultyText(game->difficulty));

    DrawInfoTile(game->state == STATE_GAME_OVER ? "最终" : "分数", scoreText,
                 (Rectangle){modal.x + 34, modal.y + 122, 160, 58}, C_ACCENT);
    DrawInfoTile("最高", bestText, (Rectangle){modal.x + 222, modal.y + 122, 160, 58}, C_ORANGE);
    DrawInfoTile("时间", timeText, (Rectangle){modal.x + 34, modal.y + 190, 160, 58}, C_BLUE);
    DrawInfoTile("难度", modeText, (Rectangle){modal.x + 222, modal.y + 190, 160, 58}, C_MAGENTA);

    for (int i = 0; i < OVERLAY_BUTTON_COUNT; i++) {
        DrawButton(&game->overlayButtons[i]);
    }
}

static void DrawButton(const UIButton *button) {
    Color base = ButtonBaseColor(button->id);
    bool secondary = ButtonIsSecondary(button->id);
    Color fill = secondary ? MixColor(WithAlpha((Color){238, 255, 248, 255}, 72.0f), WithAlpha(base, 152.0f), button->hoverT)
                           : MixColor(base, MixColor(base, RAYWHITE, 0.32f), button->hoverT);
    Color border = secondary ? MixColor(WithAlpha((Color){255, 255, 255, 255}, 82.0f), WithAlpha(base, 216.0f), button->hoverT)
                             : MixColor((Color){255, 255, 255, 72}, (Color){255, 255, 255, 190}, button->hoverT);
    Color textColor = secondary ? MixColor(C_TEXT, (Color){255, 255, 255, 255}, button->hoverT * 0.35f) : (Color){12, 30, 30, 255};
    Rectangle rect = button->bounds;
    float lift = 1.5f * button->hoverT - 2.0f * button->pressT;

    rect.y -= lift;
    rect.x -= button->hoverT * 1.5f;
    rect.width += button->hoverT * 3.0f;

    DrawRectangleRounded((Rectangle){rect.x, rect.y + 4.0f + button->pressT * 2.0f, rect.width, rect.height}, 0.18f, 10,
                         (Color){31, 42, 116, secondary ? 42 : 62});
    if (!secondary) {
        DrawRectangleRounded((Rectangle){rect.x - 2.0f, rect.y - 2.0f, rect.width + 4.0f, rect.height + 4.0f}, 0.2f, 10,
                             WithAlpha(base, 34.0f + button->hoverT * 42.0f));
    }
    DrawRectangleRounded(rect, 0.18f, 10, fill);
    DrawRectangleRounded((Rectangle){rect.x + 3.0f, rect.y + 3.0f, rect.width - 6.0f, rect.height * 0.34f}, 0.18f, 10,
                         (Color){255, 255, 255, (unsigned char)(secondary ? 10.0f + 18.0f * button->hoverT : 26.0f + 36.0f * button->hoverT)});
    DrawRectangleRoundedLinesEx(rect, 0.18f, 10, secondary ? 1.4f : 2.0f, border);
    DrawTextFit(button->label, rect, rect.height < 38.0f ? 17 : 19, textColor, true);
}

static void DrawRoundedPanel(Rectangle rect, float roundness, Color color, Color border) {
    DrawRectangleRounded((Rectangle){rect.x + 4, rect.y + 7, rect.width, rect.height}, roundness, 12, (Color){0, 0, 0, color.a / 5});
    DrawRectangleRounded(rect, roundness, 12, color);
    DrawRectangleRoundedLinesEx(rect, roundness, 12, 1.5f, border);
}

static void DrawUIText(const char *text, int x, int y, int fontSize, Color color) {
    DrawTextEx(GetUIFont(), text, (Vector2){(float)x, (float)y}, (float)fontSize, 1.0f, color);
}

static void DrawTextFit(const char *text, Rectangle rect, int fontSize, Color color, bool centered) {
    int size = fontSize;
    Vector2 measure = MeasureTextEx(GetUIFont(), text, (float)size, 1.0f);

    while (measure.x > rect.width - 16.0f && size > 12) {
        size--;
        measure = MeasureTextEx(GetUIFont(), text, (float)size, 1.0f);
    }

    int x = centered ? (int)(rect.x + (rect.width - measure.x) * 0.5f) : (int)rect.x;
    int y = (int)(rect.y + (rect.height - measure.y) * 0.5f);
    DrawTextEx(GetUIFont(), text, (Vector2){(float)x, (float)y}, (float)size, 1.0f, color);
}

static void DrawInfoTile(const char *label, const char *value, Rectangle rect, Color accent) {
    DrawRectangleRounded((Rectangle){rect.x, rect.y + 2.0f, rect.width, rect.height}, 0.12f, 10, (Color){0, 0, 0, 42});
    DrawRectangleRounded(rect, 0.12f, 10, (Color){18, 25, 38, 188});
    DrawRectangleRounded((Rectangle){rect.x + 1.0f, rect.y + 1.0f, 4.0f, rect.height - 2.0f}, 0.6f, 8,
                         WithAlpha(accent, 170.0f));
    DrawRectangleRoundedLinesEx(rect, 0.12f, 10, 1.2f, WithAlpha(accent, 92.0f));
    DrawTextFit(label, (Rectangle){rect.x + 12, rect.y + 6, rect.width - 18, 18}, 13, C_MUTED, false);
    DrawTextFit(value, (Rectangle){rect.x + 12, rect.y + 25, rect.width - 18, rect.height - 30}, 22, accent, false);
}

static void DrawPill(const char *text, Rectangle rect, Color accent) {
    DrawRectangleRounded(rect, 0.5f, 12, (Color){16, 22, 35, 178});
    DrawCircleV((Vector2){rect.x + 15.0f, rect.y + rect.height * 0.5f}, 4.0f, WithAlpha(accent, 210.0f));
    DrawRectangleRoundedLinesEx(rect, 0.5f, 12, 1.2f, WithAlpha(accent, 112.0f));
    DrawTextFit(text, (Rectangle){rect.x + 18.0f, rect.y, rect.width - 22.0f, rect.height}, 15,
                MixColor(C_TEXT, accent, 0.18f), true);
}

static void DrawMiniDivider(Rectangle rect) {
    DrawRectangleGradientH((int)rect.x, (int)rect.y, (int)rect.width, (int)rect.height,
                           (Color){255, 255, 255, 0}, (Color){255, 255, 255, 42});
    DrawRectangleGradientH((int)(rect.x + rect.width * 0.5f), (int)rect.y, (int)(rect.width * 0.5f), (int)rect.height,
                           (Color){255, 255, 255, 42}, (Color){255, 255, 255, 0});
}

static void DrawCornerTicks(Rectangle rect, float size, Color color) {
    float x0 = rect.x + 10.0f;
    float y0 = rect.y + 10.0f;
    float x1 = rect.x + rect.width - 10.0f;
    float y1 = rect.y + rect.height - 10.0f;

    DrawLineEx((Vector2){x0, y0}, (Vector2){x0 + size, y0}, 1.5f, color);
    DrawLineEx((Vector2){x0, y0}, (Vector2){x0, y0 + size}, 1.5f, color);
    DrawLineEx((Vector2){x1, y0}, (Vector2){x1 - size, y0}, 1.5f, color);
    DrawLineEx((Vector2){x1, y0}, (Vector2){x1, y0 + size}, 1.5f, color);
    DrawLineEx((Vector2){x0, y1}, (Vector2){x0 + size, y1}, 1.5f, color);
    DrawLineEx((Vector2){x0, y1}, (Vector2){x0, y1 - size}, 1.5f, color);
    DrawLineEx((Vector2){x1, y1}, (Vector2){x1 - size, y1}, 1.5f, color);
    DrawLineEx((Vector2){x1, y1}, (Vector2){x1, y1 - size}, 1.5f, color);
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

static bool ButtonIsSecondary(ButtonId id) {
    return id == BUTTON_DIFFICULTY || id == BUTTON_RESET_BEST ||
           id == BUTTON_RESTART || id == BUTTON_MENU || id == BUTTON_QUIT;
}

static Font GetUIFont(void) {
    static Font font = {0};
    static bool loaded = false;

    if (!loaded) {
        int codepointCount = 0;
        int *codepoints = LoadCodepoints(UI_FONT_CHARS, &codepointCount);
        font = LoadFontEx(UI_FONT_PATH, 48, codepoints, codepointCount);
        UnloadCodepoints(codepoints);

        if (font.texture.id == 0) {
            font = GetFontDefault();
        } else {
            SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
        }

        loaded = true;
    }

    return font;
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

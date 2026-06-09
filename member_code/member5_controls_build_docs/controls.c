#include "controls.h"

#include <stdio.h>

static void UpdateButton(UIButton *button, Vector2 mouse, float dt);
static void ResetButton(UIButton *button, float dt);
static float Approach(float value, float target, float amount);

void ControlsInit(Game *game) {
    game->menuButtons[0] = (UIButton){{BOARD_X + 150, BOARD_Y + 196, 300, 48}, "开始游戏", BUTTON_START, false, false, 0.0f, 0.0f};
    game->menuButtons[1] = (UIButton){{BOARD_X + 150, BOARD_Y + 272, 144, 42}, "难度", BUTTON_DIFFICULTY, false, false, 0.0f, 0.0f};
    game->menuButtons[2] = (UIButton){{BOARD_X + 306, BOARD_Y + 272, 144, 42}, "重置纪录", BUTTON_RESET_BEST, false, false, 0.0f, 0.0f};
    game->menuButtons[3] = (UIButton){{BOARD_X + 150, BOARD_Y + 326, 300, 42}, "退出", BUTTON_QUIT, false, false, 0.0f, 0.0f};

    game->sideButtons[0] = (UIButton){{PANEL_X + 24, PANEL_Y + 315, 182, 34}, "暂停", BUTTON_PAUSE, false, false, 0.0f, 0.0f};
    game->sideButtons[1] = (UIButton){{PANEL_X + 24, PANEL_Y + 358, 182, 34}, "重开", BUTTON_RESTART, false, false, 0.0f, 0.0f};
    game->sideButtons[2] = (UIButton){{PANEL_X + 24, PANEL_Y + 401, 182, 34}, "重置纪录", BUTTON_RESET_BEST, false, false, 0.0f, 0.0f};

    game->overlayButtons[0] = (UIButton){{BOARD_X + 145, BOARD_Y + 326, 146, 42}, "继续", BUTTON_START, false, false, 0.0f, 0.0f};
    game->overlayButtons[1] = (UIButton){{BOARD_X + 309, BOARD_Y + 326, 146, 42}, "菜单", BUTTON_MENU, false, false, 0.0f, 0.0f};
}

void ControlsUpdate(Game *game, float dt) {
    Vector2 mouse = GetMousePosition();
    static char menuDifficulty[64];
    static char pauseLabel[32];

    snprintf(menuDifficulty, sizeof(menuDifficulty), "难度：%s", GameDifficultyText(game->difficulty));
    game->menuButtons[0].label = "开始游戏";
    game->menuButtons[1].label = menuDifficulty;
    game->menuButtons[2].label = "重置纪录";
    game->menuButtons[3].label = "退出";

    if (game->state == STATE_PAUSED) {
        snprintf(pauseLabel, sizeof(pauseLabel), "继续");
        game->sideButtons[0].id = BUTTON_PAUSE;
    } else if (game->state == STATE_MENU || game->state == STATE_GAME_OVER) {
        snprintf(pauseLabel, sizeof(pauseLabel), "开始");
        game->sideButtons[0].id = BUTTON_START;
    } else {
        snprintf(pauseLabel, sizeof(pauseLabel), "暂停");
        game->sideButtons[0].id = BUTTON_PAUSE;
    }
    game->sideButtons[0].label = pauseLabel;
    game->sideButtons[1].label = "重开";
    game->sideButtons[2].label = "重置纪录";

    for (int i = 0; i < SIDE_BUTTON_COUNT; i++) {
        UpdateButton(&game->sideButtons[i], mouse, dt);
        if (game->sideButtons[i].hovered && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            GameActivateButton(game, game->sideButtons[i].id);
        }
    }

    if (game->state == STATE_MENU) {
        for (int i = 0; i < MENU_BUTTON_COUNT; i++) {
            UpdateButton(&game->menuButtons[i], mouse, dt);
            if (game->menuButtons[i].hovered && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                GameActivateButton(game, game->menuButtons[i].id);
            }
        }
    } else {
        for (int i = 0; i < MENU_BUTTON_COUNT; i++) {
            ResetButton(&game->menuButtons[i], dt);
        }
    }

    if (game->state == STATE_PAUSED || game->state == STATE_GAME_OVER) {
        game->overlayButtons[0].id = game->state == STATE_GAME_OVER ? BUTTON_RESTART : BUTTON_START;
        game->overlayButtons[0].label = game->state == STATE_GAME_OVER ? "再来一局" : "继续";
        game->overlayButtons[1].id = BUTTON_MENU;
        game->overlayButtons[1].label = "菜单";

        for (int i = 0; i < OVERLAY_BUTTON_COUNT; i++) {
            UpdateButton(&game->overlayButtons[i], mouse, dt);
            if (game->overlayButtons[i].hovered && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                GameActivateButton(game, game->overlayButtons[i].id);
            }
        }
    } else {
        for (int i = 0; i < OVERLAY_BUTTON_COUNT; i++) {
            ResetButton(&game->overlayButtons[i], dt);
        }
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

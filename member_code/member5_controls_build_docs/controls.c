#include "controls.h"

#include <stdio.h>

static void UpdateButton(UIButton *button, Vector2 mouse, float dt);
static void ResetButton(UIButton *button, float dt);
static float Approach(float value, float target, float amount);

void ControlsInit(Game *game) {
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

void ControlsUpdate(Game *game, float dt) {
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

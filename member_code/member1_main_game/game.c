#include "game.h"

#include "food.h"
#include "snake.h"

#include <stdio.h>
#include <string.h>

static void ApplyDifficulty(Game *game);
static void ResetRound(Game *game, GameState state);
static void MoveSnake(Game *game);
static float Approach(float value, float target, float amount);

void GameInit(Game *game) {
    memset(game, 0, sizeof(*game));
    game->difficulty = DIFFICULTY_NORMAL;
    game->highScore = GameLoadHighScore();
    ApplyDifficulty(game);
    ResetRound(game, STATE_MENU);
    game->overlayAlpha = 1.0f;
}

void GameStart(Game *game) {
    if (game->state == STATE_MENU || game->state == STATE_GAME_OVER) {
        ResetRound(game, STATE_RUNNING);
    } else if (game->state == STATE_PAUSED) {
        game->state = STATE_RUNNING;
    }
}

void GameRestart(Game *game) {
    ResetRound(game, STATE_RUNNING);
}

void GameReturnToMenu(Game *game) {
    ResetRound(game, STATE_MENU);
}

void GameTogglePause(Game *game) {
    if (game->state == STATE_RUNNING) {
        game->state = STATE_PAUSED;
    } else if (game->state == STATE_PAUSED) {
        game->state = STATE_RUNNING;
    } else if (game->state == STATE_MENU) {
        GameStart(game);
    }
}

void GameNextDifficulty(Game *game) {
    game->difficulty = (Difficulty)(((int)game->difficulty + 1) % 3);
    ApplyDifficulty(game);

    if (game->state == STATE_MENU || game->state == STATE_GAME_OVER) {
        ResetRound(game, game->state);
    }
}

void GameResetHighScore(Game *game) {
    game->highScore = 0;
    GameSaveHighScore(0);
}

void GameActivateButton(Game *game, ButtonId id) {
    switch (id) {
        case BUTTON_START:
            GameStart(game);
            break;
        case BUTTON_DIFFICULTY:
            GameNextDifficulty(game);
            break;
        case BUTTON_QUIT:
            game->shouldQuit = true;
            break;
        case BUTTON_PAUSE:
            GameTogglePause(game);
            break;
        case BUTTON_RESTART:
            GameRestart(game);
            break;
        case BUTTON_RESET_BEST:
            GameResetHighScore(game);
            break;
        case BUTTON_MENU:
            GameReturnToMenu(game);
            break;
    }
}

void GameHandleKeyboard(Game *game) {
    if (game->state == STATE_RUNNING || game->state == STATE_MENU) {
        bool moved = false;
        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) moved = SnakeQueueDirection(&game->snake, DIR_UP);
        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) moved = SnakeQueueDirection(&game->snake, DIR_DOWN);
        if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) moved = SnakeQueueDirection(&game->snake, DIR_LEFT);
        if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) moved = SnakeQueueDirection(&game->snake, DIR_RIGHT);
        if (moved && game->state == STATE_MENU) {
            game->state = STATE_RUNNING;
        }
    }

    if (IsKeyPressed(KEY_ENTER)) {
        if (game->state == STATE_MENU || game->state == STATE_GAME_OVER) {
            GameStart(game);
        } else if (game->state == STATE_PAUSED) {
            game->state = STATE_RUNNING;
        }
    }

    if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_P)) {
        if (game->state == STATE_GAME_OVER) {
            GameRestart(game);
        } else {
            GameTogglePause(game);
        }
    }

    if (IsKeyPressed(KEY_R)) {
        GameRestart(game);
    }

    if (IsKeyPressed(KEY_TAB)) {
        if (game->state == STATE_MENU || game->state == STATE_GAME_OVER) {
            GameNextDifficulty(game);
        }
    }

    if (IsKeyPressed(KEY_M)) {
        GameReturnToMenu(game);
    }
}

void GameUpdate(Game *game, float dt) {
    float overlayTarget = (game->state == STATE_RUNNING) ? 0.0f : 1.0f;
    game->overlayAlpha = Approach(game->overlayAlpha, overlayTarget, dt * 3.8f);

    if (game->eatFeedback > 0.0f) {
        game->eatFeedback -= dt;
        if (game->eatFeedback < 0.0f) {
            game->eatFeedback = 0.0f;
        }
    }

    if (game->eatRingTimer > 0.0f) {
        game->eatRingTimer -= dt;
        if (game->eatRingTimer < 0.0f) {
            game->eatRingTimer = 0.0f;
        }
    }

    if (game->scorePulse > 0.0f) {
        game->scorePulse -= dt;
        if (game->scorePulse < 0.0f) {
            game->scorePulse = 0.0f;
        }
    }

    if (game->state == STATE_GAME_OVER) {
        game->gameOverTimer += dt;
    }

    FoodUpdate(game, dt);

    if (game->state != STATE_RUNNING) {
        return;
    }

    game->runTime += dt;
    game->moveTimer += dt;
    while (game->moveTimer >= game->moveInterval) {
        game->moveTimer -= game->moveInterval;
        MoveSnake(game);
        if (game->state != STATE_RUNNING) {
            break;
        }
    }
}

int GameLoadHighScore(void) {
    FILE *file = fopen(HIGH_SCORE_FILE, "r");
    int score = 0;

    if (file == NULL) {
        return 0;
    }

    if (fscanf(file, "%d", &score) != 1 || score < 0) {
        score = 0;
    }

    fclose(file);
    return score;
}

void GameSaveHighScore(int score) {
    FILE *file = fopen(HIGH_SCORE_FILE, "w");

    if (file == NULL) {
        return;
    }

    fprintf(file, "%d\n", score);
    fclose(file);
}

const char *GameStateText(GameState state) {
    switch (state) {
        case STATE_MENU: return "菜单";
        case STATE_RUNNING: return "运行中";
        case STATE_PAUSED: return "已暂停";
        case STATE_GAME_OVER: return "结束";
    }
    return "";
}

const char *GameDifficultyText(Difficulty difficulty) {
    switch (difficulty) {
        case DIFFICULTY_EASY: return "简单";
        case DIFFICULTY_NORMAL: return "普通";
        case DIFFICULTY_HARD: return "困难";
    }
    return "";
}

static void ApplyDifficulty(Game *game) {
    switch (game->difficulty) {
        case DIFFICULTY_EASY:
            game->baseMoveInterval = 0.170f;
            game->minMoveInterval = 0.078f;
            game->speedStep = 0.006f;
            break;
        case DIFFICULTY_NORMAL:
            game->baseMoveInterval = 0.145f;
            game->minMoveInterval = 0.058f;
            game->speedStep = 0.008f;
            break;
        case DIFFICULTY_HARD:
            game->baseMoveInterval = 0.115f;
            game->minMoveInterval = 0.045f;
            game->speedStep = 0.010f;
            break;
    }

    game->moveInterval = game->baseMoveInterval;
}

static void ResetRound(Game *game, GameState state) {
    SnakeInit(&game->snake);
    ApplyDifficulty(game);
    game->score = 0;
    game->level = 1;
    game->moveTimer = 0.0f;
    game->runTime = 0.0f;
    game->eatFeedback = 0.0f;
    game->eatRingTimer = 0.0f;
    game->scorePulse = 0.0f;
    game->gameOverTimer = 0.0f;
    game->lastEatCenter = (Vector2){BOARD_X + BOARD_W * 0.5f, BOARD_Y + BOARD_H * 0.5f};
    memset(game->particles, 0, sizeof(game->particles));
    FoodSpawn(game);
    game->state = state;
}

static void MoveSnake(Game *game) {
    Cell next = SnakeNextHead(&game->snake);
    bool ateFood = SnakeSameCell(next, game->food.position);

    if (SnakeHitsWall(next) || SnakeHitsSelf(&game->snake, next, ateFood)) {
        game->state = STATE_GAME_OVER;
        game->gameOverTimer = 0.0f;
        return;
    }

    SnakeAdvance(&game->snake, next, ateFood);

    if (ateFood) {
        FoodOnEaten(game);
        game->level = game->score / SCORE_PER_LEVEL + 1;
        game->moveInterval = game->baseMoveInterval - (float)(game->level - 1) * game->speedStep;
        if (game->moveInterval < game->minMoveInterval) {
            game->moveInterval = game->minMoveInterval;
        }

        if (game->score > game->highScore) {
            game->highScore = game->score;
            GameSaveHighScore(game->highScore);
        }

        FoodSpawn(game);
    }
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

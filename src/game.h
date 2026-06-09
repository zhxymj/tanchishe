#ifndef GAME_H
#define GAME_H

#include "raylib.h"

#include <stdbool.h>

#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 720
#define TARGET_FPS 60

#define BOARD_COLS 30
#define BOARD_ROWS 24
#define CELL_SIZE 20
#define MAX_SNAKE_LENGTH (BOARD_COLS * BOARD_ROWS)

#define BOARD_X 48
#define BOARD_Y 166
#define BOARD_W (BOARD_COLS * CELL_SIZE)
#define BOARD_H (BOARD_ROWS * CELL_SIZE)

#define PANEL_X 682
#define PANEL_Y 166
#define PANEL_W 230
#define PANEL_H 480

#define START_LENGTH 5
#define SCORE_PER_FOOD 10
#define SCORE_PER_LEVEL 50
#define HIGH_SCORE_FILE "snake_raylib_highscore.dat"

#define MAX_FOOD_PARTICLES 36
#define MENU_BUTTON_COUNT 4
#define SIDE_BUTTON_COUNT 3
#define OVERLAY_BUTTON_COUNT 2
#define EAT_RING_LIFE 0.42f

typedef enum {
    STATE_MENU,
    STATE_RUNNING,
    STATE_PAUSED,
    STATE_GAME_OVER
} GameState;

typedef enum {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} Direction;

typedef enum {
    DIFFICULTY_EASY,
    DIFFICULTY_NORMAL,
    DIFFICULTY_HARD
} Difficulty;

typedef enum {
    BUTTON_START,
    BUTTON_DIFFICULTY,
    BUTTON_QUIT,
    BUTTON_PAUSE,
    BUTTON_RESTART,
    BUTTON_RESET_BEST,
    BUTTON_MENU
} ButtonId;

typedef struct {
    int x;
    int y;
} Cell;

typedef struct {
    Cell body[MAX_SNAKE_LENGTH];
    int length;
    Direction direction;
    Direction queuedDirection;
} Snake;

typedef struct {
    Cell position;
    float pulse;
} Food;

typedef struct {
    Vector2 position;
    Vector2 velocity;
    float age;
    float life;
    float radius;
    Color color;
    bool active;
} FoodParticle;

typedef struct {
    Rectangle bounds;
    const char *label;
    ButtonId id;
    bool hovered;
    bool pressed;
    float hoverT;
    float pressT;
} UIButton;

typedef struct {
    GameState state;
    Snake snake;
    Food food;
    Difficulty difficulty;
    int score;
    int highScore;
    int level;
    float moveInterval;
    float baseMoveInterval;
    float minMoveInterval;
    float speedStep;
    float moveTimer;
    float runTime;
    float eatFeedback;
    float eatRingTimer;
    float gameOverTimer;
    float overlayAlpha;
    float scorePulse;
    Vector2 lastEatCenter;
    bool shouldQuit;
    FoodParticle particles[MAX_FOOD_PARTICLES];
    UIButton menuButtons[MENU_BUTTON_COUNT];
    UIButton sideButtons[SIDE_BUTTON_COUNT];
    UIButton overlayButtons[OVERLAY_BUTTON_COUNT];
} Game;

void GameInit(Game *game);
void GameStart(Game *game);
void GameRestart(Game *game);
void GameReturnToMenu(Game *game);
void GameTogglePause(Game *game);
void GameNextDifficulty(Game *game);
void GameResetHighScore(Game *game);
void GameActivateButton(Game *game, ButtonId id);
void GameHandleKeyboard(Game *game);
void GameUpdate(Game *game, float dt);

int GameLoadHighScore(void);
void GameSaveHighScore(int score);
const char *GameStateText(GameState state);
const char *GameDifficultyText(Difficulty difficulty);

#endif

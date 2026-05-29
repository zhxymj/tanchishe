/*
 * Snake Arcade - raylib single-player snake game
 * Build example on Windows:
 *   gcc src/main.c -std=c11 -O2 -Wall -Wextra -I"C:/raylib/raylib/src" \
 *       -L"C:/raylib/raylib/src" -lraylib -lopengl32 -lgdi32 -lwinmm -o snake_raylib.exe
 */

#include "raylib.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
#define BASE_MOVE_INTERVAL 0.145f
#define MIN_MOVE_INTERVAL 0.058f
#define SPEED_STEP 0.008f
#define SCORE_PER_FOOD 10
#define SCORE_PER_LEVEL 50
#define HIGH_SCORE_FILE "snake_raylib_highscore.dat"

typedef enum {
    STATE_START,
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
    BUTTON_START,
    BUTTON_PAUSE,
    BUTTON_RESTART
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
    Rectangle bounds;
    const char *label;
    ButtonId id;
    bool hovered;
    bool pressed;
} UIButton;

typedef struct {
    GameState state;
    Snake snake;
    Food food;
    int score;
    int highScore;
    int level;
    float moveInterval;
    float moveTimer;
    float eatFeedback;
    float gameOverTimer;
    UIButton buttons[3];
    UIButton overlayButton;
} Game;

typedef struct {
    Sound eat;
    Sound gameOver;
    bool ready;
} SoundPack;

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
static const Color C_FOOD = {255, 88, 116, 255};
static const Color C_BOARD_A = {35, 53, 58, 255};
static const Color C_BOARD_B = {30, 46, 53, 255};

static int LoadHighScore(void);
static void SaveHighScore(int score);
static Sound MakeToneSound(float frequency, float duration, float volume);
static void InitSounds(SoundPack *sounds);
static void UnloadSounds(SoundPack *sounds);
static void InitGame(Game *game);
static void ResetGame(Game *game);
static void StartOrResumeGame(Game *game);
static void TogglePause(Game *game);
static void QueueDirection(Game *game, Direction direction);
static void HandleKeyboard(Game *game);
static void HandleButtons(Game *game);
static void ActivateButton(Game *game, ButtonId id);
static void UpdateGame(Game *game, SoundPack *sounds);
static void MoveSnake(Game *game, SoundPack *sounds);
static bool SameCell(Cell a, Cell b);
static bool SnakeContains(const Snake *snake, Cell cell);
static void SpawnFood(Game *game);
static Cell NextHead(const Snake *snake);
static float CurrentMoveInterval(const Game *game);
static void UpdateDifficulty(Game *game);
static void DrawGame(const Game *game);
static void DrawBackground(void);
static void DrawHeader(void);
static void DrawBoardFrame(void);
static void DrawBoardGrid(void);
static void DrawFood(const Game *game);
static void DrawSnake(const Game *game);
static void DrawSnakeEyes(Cell head, Direction direction);
static void DrawSidePanel(const Game *game);
static void DrawButton(const UIButton *button);
static void DrawOverlay(const Game *game);
static void DrawRoundedPanel(Rectangle rect, float roundness, Color color, Color border);
static void DrawTextFit(const char *text, Rectangle rect, int fontSize, Color color, bool centered);
static const char *StateText(GameState state);
static const char *ButtonLabel(const Game *game, ButtonId id);

int main(void) {
    Game game = {0};
    SoundPack sounds = {0};

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Snake Arcade - raylib");
    InitAudioDevice();
    SetTargetFPS(TARGET_FPS);

    srand((unsigned int)time(NULL));
    InitSounds(&sounds);
    InitGame(&game);

    while (!WindowShouldClose()) {
        HandleKeyboard(&game);
        HandleButtons(&game);
        UpdateGame(&game, &sounds);

        BeginDrawing();
        DrawGame(&game);
        EndDrawing();
    }

    UnloadSounds(&sounds);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}

static int LoadHighScore(void) {
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

static void SaveHighScore(int score) {
    FILE *file = fopen(HIGH_SCORE_FILE, "w");

    if (file == NULL) {
        return;
    }

    fprintf(file, "%d\n", score);
    fclose(file);
}

static Sound MakeToneSound(float frequency, float duration, float volume) {
    const int sampleRate = 44100;
    const int sampleCount = (int)(sampleRate * duration);
    short *samples = (short *)MemAlloc((unsigned int)sampleCount * sizeof(short));

    if (samples == NULL) {
        return (Sound){0};
    }

    for (int i = 0; i < sampleCount; i++) {
        float t = (float)i / (float)sampleRate;
        float fade = 1.0f - (float)i / (float)sampleCount;
        float wave = sinf(2.0f * PI * frequency * t) * fade * volume;
        samples[i] = (short)(wave * 32767.0f);
    }

    Wave wave = {
        .frameCount = (unsigned int)sampleCount,
        .sampleRate = sampleRate,
        .sampleSize = 16,
        .channels = 1,
        .data = samples
    };
    Sound sound = LoadSoundFromWave(wave);
    UnloadWave(wave);
    return sound;
}

static void InitSounds(SoundPack *sounds) {
    sounds->ready = IsAudioDeviceReady();
    if (!sounds->ready) {
        return;
    }

    sounds->eat = MakeToneSound(720.0f, 0.08f, 0.18f);
    sounds->gameOver = MakeToneSound(170.0f, 0.28f, 0.22f);
}

static void UnloadSounds(SoundPack *sounds) {
    if (!sounds->ready) {
        return;
    }

    UnloadSound(sounds->eat);
    UnloadSound(sounds->gameOver);
}

static void InitGame(Game *game) {
    game->highScore = LoadHighScore();
    game->buttons[0] = (UIButton){{PANEL_X + 24, PANEL_Y + 318, 182, 46}, "Start", BUTTON_START, false, false};
    game->buttons[1] = (UIButton){{PANEL_X + 24, PANEL_Y + 371, 182, 46}, "Pause", BUTTON_PAUSE, false, false};
    game->buttons[2] = (UIButton){{PANEL_X + 24, PANEL_Y + 424, 182, 46}, "Restart", BUTTON_RESTART, false, false};
    game->overlayButton = (UIButton){{BOARD_X + BOARD_W * 0.5f - 92.0f, BOARD_Y + 292, 184, 46}, "Start", BUTTON_START, false, false};
    ResetGame(game);
    game->state = STATE_START;
}

static void ResetGame(Game *game) {
    const int startX = BOARD_COLS / 2;
    const int startY = BOARD_ROWS / 2;

    game->snake.length = START_LENGTH;
    game->snake.direction = DIR_RIGHT;
    game->snake.queuedDirection = DIR_RIGHT;

    for (int i = 0; i < START_LENGTH; i++) {
        game->snake.body[i] = (Cell){startX - i, startY};
    }

    game->score = 0;
    game->level = 1;
    game->moveInterval = BASE_MOVE_INTERVAL;
    game->moveTimer = 0.0f;
    game->eatFeedback = 0.0f;
    game->gameOverTimer = 0.0f;
    game->state = STATE_START;
    SpawnFood(game);
}

static void StartOrResumeGame(Game *game) {
    if (game->state == STATE_START || game->state == STATE_PAUSED) {
        game->state = STATE_RUNNING;
    } else if (game->state == STATE_GAME_OVER) {
        ResetGame(game);
        game->state = STATE_RUNNING;
    }
}

static void TogglePause(Game *game) {
    if (game->state == STATE_RUNNING) {
        game->state = STATE_PAUSED;
    } else if (game->state == STATE_PAUSED || game->state == STATE_START) {
        game->state = STATE_RUNNING;
    }
}

static void QueueDirection(Game *game, Direction direction) {
    Direction current = game->snake.direction;

    if ((current == DIR_UP && direction == DIR_DOWN) ||
        (current == DIR_DOWN && direction == DIR_UP) ||
        (current == DIR_LEFT && direction == DIR_RIGHT) ||
        (current == DIR_RIGHT && direction == DIR_LEFT)) {
        return;
    }

    game->snake.queuedDirection = direction;
    if (game->state == STATE_START) {
        game->state = STATE_RUNNING;
    }
}

static void HandleKeyboard(Game *game) {
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) QueueDirection(game, DIR_UP);
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) QueueDirection(game, DIR_DOWN);
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) QueueDirection(game, DIR_LEFT);
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) QueueDirection(game, DIR_RIGHT);

    if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_SPACE)) {
        TogglePause(game);
    }

    if (IsKeyPressed(KEY_R)) {
        ResetGame(game);
        game->state = STATE_RUNNING;
    }

    if (IsKeyPressed(KEY_ENTER) && game->state == STATE_GAME_OVER) {
        ResetGame(game);
        game->state = STATE_RUNNING;
    }
}

static void HandleButtons(Game *game) {
    Vector2 mouse = GetMousePosition();

    for (int i = 0; i < 3; i++) {
        UIButton *button = &game->buttons[i];
        button->label = ButtonLabel(game, button->id);
        button->hovered = CheckCollisionPointRec(mouse, button->bounds);
        button->pressed = button->hovered && IsMouseButtonDown(MOUSE_LEFT_BUTTON);

        if (button->hovered && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            ActivateButton(game, button->id);
        }
    }

    if (game->state == STATE_START || game->state == STATE_PAUSED || game->state == STATE_GAME_OVER) {
        UIButton *button = &game->overlayButton;
        button->id = game->state == STATE_GAME_OVER ? BUTTON_RESTART : BUTTON_START;
        button->label = ButtonLabel(game, button->id);
        button->hovered = CheckCollisionPointRec(mouse, button->bounds);
        button->pressed = button->hovered && IsMouseButtonDown(MOUSE_LEFT_BUTTON);

        if (button->hovered && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            ActivateButton(game, button->id);
        }
    } else {
        game->overlayButton.hovered = false;
        game->overlayButton.pressed = false;
    }
}

static void ActivateButton(Game *game, ButtonId id) {
    if (id == BUTTON_START) {
        StartOrResumeGame(game);
    } else if (id == BUTTON_PAUSE) {
        TogglePause(game);
    } else if (id == BUTTON_RESTART) {
        ResetGame(game);
        game->state = STATE_RUNNING;
    }
}

static void UpdateGame(Game *game, SoundPack *sounds) {
    float dt = GetFrameTime();

    if (game->eatFeedback > 0.0f) {
        game->eatFeedback -= dt;
        if (game->eatFeedback < 0.0f) game->eatFeedback = 0.0f;
    }

    if (game->state == STATE_GAME_OVER) {
        game->gameOverTimer += dt;
    }

    game->food.pulse += dt * 5.5f;

    if (game->state != STATE_RUNNING) {
        return;
    }

    game->moveTimer += dt;
    while (game->moveTimer >= CurrentMoveInterval(game)) {
        game->moveTimer -= CurrentMoveInterval(game);
        MoveSnake(game, sounds);
        if (game->state != STATE_RUNNING) {
            break;
        }
    }
}

static void MoveSnake(Game *game, SoundPack *sounds) {
    Snake *snake = &game->snake;
    Cell next = NextHead(snake);
    bool ateFood = SameCell(next, game->food.position);
    int selfCheckLength = ateFood ? snake->length : snake->length - 1;

    if (next.x < 0 || next.x >= BOARD_COLS || next.y < 0 || next.y >= BOARD_ROWS) {
        game->state = STATE_GAME_OVER;
        if (sounds->ready) PlaySound(sounds->gameOver);
        return;
    }

    for (int i = 0; i < selfCheckLength; i++) {
        if (SameCell(snake->body[i], next)) {
            game->state = STATE_GAME_OVER;
            if (sounds->ready) PlaySound(sounds->gameOver);
            return;
        }
    }

    snake->direction = snake->queuedDirection;

    if (ateFood && snake->length < MAX_SNAKE_LENGTH) {
        snake->length++;
    }

    for (int i = snake->length - 1; i > 0; i--) {
        snake->body[i] = snake->body[i - 1];
    }
    snake->body[0] = next;

    if (ateFood) {
        game->score += SCORE_PER_FOOD;
        game->eatFeedback = 0.22f;
        if (sounds->ready) PlaySound(sounds->eat);
        UpdateDifficulty(game);
        if (game->score > game->highScore) {
            game->highScore = game->score;
            SaveHighScore(game->highScore);
        }
        SpawnFood(game);
    }
}

static bool SameCell(Cell a, Cell b) {
    return a.x == b.x && a.y == b.y;
}

static bool SnakeContains(const Snake *snake, Cell cell) {
    for (int i = 0; i < snake->length; i++) {
        if (SameCell(snake->body[i], cell)) {
            return true;
        }
    }
    return false;
}

static void SpawnFood(Game *game) {
    Cell options[MAX_SNAKE_LENGTH];
    int count = 0;

    for (int y = 0; y < BOARD_ROWS; y++) {
        for (int x = 0; x < BOARD_COLS; x++) {
            Cell cell = {x, y};
            if (!SnakeContains(&game->snake, cell)) {
                options[count++] = cell;
            }
        }
    }

    if (count == 0) {
        game->state = STATE_GAME_OVER;
        return;
    }

    game->food.position = options[GetRandomValue(0, count - 1)];
    game->food.pulse = 0.0f;
}

static Cell NextHead(const Snake *snake) {
    Cell head = snake->body[0];

    switch (snake->queuedDirection) {
        case DIR_UP: head.y--; break;
        case DIR_DOWN: head.y++; break;
        case DIR_LEFT: head.x--; break;
        case DIR_RIGHT: head.x++; break;
    }

    return head;
}

static float CurrentMoveInterval(const Game *game) {
    return game->moveInterval;
}

static void UpdateDifficulty(Game *game) {
    int level = game->score / SCORE_PER_LEVEL + 1;
    game->level = level;
    game->moveInterval = BASE_MOVE_INTERVAL - (float)(level - 1) * SPEED_STEP;
    if (game->moveInterval < MIN_MOVE_INTERVAL) {
        game->moveInterval = MIN_MOVE_INTERVAL;
    }
}

static void DrawGame(const Game *game) {
    ClearBackground(C_BG_BOTTOM);
    DrawBackground();
    DrawHeader();
    DrawBoardFrame();
    DrawBoardGrid();
    DrawFood(game);
    DrawSnake(game);
    DrawSidePanel(game);

    if (game->state == STATE_START || game->state == STATE_PAUSED || game->state == STATE_GAME_OVER) {
        DrawOverlay(game);
    }
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
    DrawText("NEON GRID", 242, 92, 18, (Color){198, 209, 229, 255});
}

static void DrawBoardFrame(void) {
    Rectangle shadow = {BOARD_X + 8, BOARD_Y + 12, BOARD_W, BOARD_H};
    Rectangle board = {BOARD_X, BOARD_Y, BOARD_W, BOARD_H};

    DrawRectangleRounded(shadow, 0.04f, 12, (Color){0, 0, 0, 64});
    DrawRectangleRounded((Rectangle){BOARD_X - 12, BOARD_Y - 12, BOARD_W + 24, BOARD_H + 24}, 0.05f, 14, (Color){18, 24, 35, 230});
    DrawRectangleRoundedLinesEx((Rectangle){BOARD_X - 12, BOARD_Y - 12, BOARD_W + 24, BOARD_H + 24}, 0.05f, 14, 2.0f, (Color){114, 135, 170, 90});
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

static void DrawFood(const Game *game) {
    float pulse = 1.0f + sinf(game->food.pulse) * 0.12f + game->eatFeedback * 0.7f;
    float cx = BOARD_X + (float)game->food.position.x * CELL_SIZE + CELL_SIZE * 0.5f;
    float cy = BOARD_Y + (float)game->food.position.y * CELL_SIZE + CELL_SIZE * 0.5f;
    float radius = 6.5f * pulse;

    DrawCircleGradient((Vector2){cx, cy}, 20.0f * pulse, (Color){255, 88, 116, 86}, (Color){255, 88, 116, 0});
    DrawCircleV((Vector2){cx, cy}, radius, C_FOOD);
    DrawCircleV((Vector2){cx - radius * 0.32f, cy - radius * 0.35f}, radius * 0.28f, (Color){255, 230, 235, 225});
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
        DrawCircle((int)(rect.x + rect.width * 0.33f), (int)(rect.y + rect.height * 0.28f), 2.0f, (Color){215, 255, 229, 135});
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
    DrawCircleV((Vector2){eyeA.x + forward.x * 0.16f, eyeA.y + forward.y * 0.16f}, 1.25f, (Color){23, 29, 42, 255});
    DrawCircleV((Vector2){eyeB.x + forward.x * 0.16f, eyeB.y + forward.y * 0.16f}, 1.25f, (Color){23, 29, 42, 255});
}

static void DrawSidePanel(const Game *game) {
    Rectangle panel = {PANEL_X, PANEL_Y, PANEL_W, PANEL_H};
    DrawRoundedPanel(panel, 0.08f, C_PANEL, C_LINE);

    DrawText("STATUS", PANEL_X + 24, PANEL_Y + 24, 22, C_TEXT);
    DrawText(StateText(game->state), PANEL_X + 24, PANEL_Y + 54, 18, C_ACCENT);

    char scoreText[64];
    snprintf(scoreText, sizeof(scoreText), "%d", game->score);
    DrawText("Score", PANEL_X + 24, PANEL_Y + 96, 18, C_MUTED);
    DrawText(scoreText, PANEL_X + 24, PANEL_Y + 120, 38, C_TEXT);

    snprintf(scoreText, sizeof(scoreText), "%d", game->highScore);
    DrawText("Best", PANEL_X + 24, PANEL_Y + 170, 18, C_MUTED);
    DrawText(scoreText, PANEL_X + 24, PANEL_Y + 194, 30, C_ORANGE);

    snprintf(scoreText, sizeof(scoreText), "Level %d", game->level);
    DrawText(scoreText, PANEL_X + 24, PANEL_Y + 248, 24, C_TEXT);

    float speedRatio = 1.0f - (game->moveInterval - MIN_MOVE_INTERVAL) / (BASE_MOVE_INTERVAL - MIN_MOVE_INTERVAL);
    if (speedRatio < 0.0f) speedRatio = 0.0f;
    if (speedRatio > 1.0f) speedRatio = 1.0f;
    DrawText("Speed", PANEL_X + 24, PANEL_Y + 286, 16, C_MUTED);
    DrawRectangleRounded((Rectangle){PANEL_X + 24, PANEL_Y + 310, 182, 10}, 0.5f, 8, (Color){255, 255, 255, 34});
    DrawRectangleRounded((Rectangle){PANEL_X + 24, PANEL_Y + 310, 182.0f * speedRatio, 10}, 0.5f, 8, C_BLUE);

    for (int i = 0; i < 3; i++) {
        DrawButton(&game->buttons[i]);
    }
}

static void DrawButton(const UIButton *button) {
    Color fill = C_ACCENT_DARK;
    Color border = (Color){188, 255, 212, 120};
    Rectangle rect = button->bounds;

    if (button->hovered) fill = C_ACCENT;
    if (button->pressed) {
        rect.y += 2.0f;
        fill = (Color){77, 199, 126, 255};
    }

    DrawRectangleRounded((Rectangle){rect.x, rect.y + 4, rect.width, rect.height}, 0.2f, 10, (Color){0, 0, 0, 60});
    DrawRectangleRounded(rect, 0.2f, 10, fill);
    DrawRectangleRoundedLinesEx(rect, 0.2f, 10, 2.0f, border);
    DrawTextFit(button->label, rect, 20, (Color){11, 36, 25, 255}, true);
}

static void DrawOverlay(const Game *game) {
    Rectangle board = {BOARD_X, BOARD_Y, BOARD_W, BOARD_H};
    Rectangle modal = {BOARD_X + 112, BOARD_Y + 98, BOARD_W - 224, 270};
    const char *title = "READY";
    const char *message = "Classic mode";
    char details[128];

    if (game->state == STATE_PAUSED) {
        title = "PAUSED";
        message = "Run suspended";
    } else if (game->state == STATE_GAME_OVER) {
        title = "GAME OVER";
        message = "Final result";
    }

    DrawRectangleRec(board, (Color){9, 13, 20, 112});
    DrawRoundedPanel(modal, 0.08f, (Color){27, 33, 49, 238}, C_LINE);
    DrawTextFit(title, (Rectangle){modal.x, modal.y + 28, modal.width, 46}, 38, C_TEXT, true);
    DrawTextFit(message, (Rectangle){modal.x, modal.y + 88, modal.width, 28}, 20, C_MUTED, true);

    snprintf(details, sizeof(details), "%s %d   Best %d", game->state == STATE_GAME_OVER ? "Final" : "Score", game->score, game->highScore);
    DrawTextFit(details, (Rectangle){modal.x, modal.y + 134, modal.width, 30}, 20, C_ACCENT, true);
    DrawButton(&game->overlayButton);
}

static void DrawRoundedPanel(Rectangle rect, float roundness, Color color, Color border) {
    DrawRectangleRounded((Rectangle){rect.x + 4, rect.y + 7, rect.width, rect.height}, roundness, 12, (Color){0, 0, 0, 55});
    DrawRectangleRounded(rect, roundness, 12, color);
    DrawRectangleRoundedLinesEx(rect, roundness, 12, 1.5f, border);
}

static void DrawTextFit(const char *text, Rectangle rect, int fontSize, Color color, bool centered) {
    int width = MeasureText(text, fontSize);
    int x = centered ? (int)(rect.x + (rect.width - width) * 0.5f) : (int)rect.x;
    int y = (int)(rect.y + (rect.height - fontSize) * 0.5f);
    DrawText(text, x, y, fontSize, color);
}

static const char *StateText(GameState state) {
    switch (state) {
        case STATE_START: return "Waiting";
        case STATE_RUNNING: return "Running";
        case STATE_PAUSED: return "Paused";
        case STATE_GAME_OVER: return "Game Over";
    }
    return "";
}

static const char *ButtonLabel(const Game *game, ButtonId id) {
    if (id == BUTTON_START) {
        if (game->state == STATE_PAUSED) return "Resume";
        if (game->state == STATE_GAME_OVER) return "New Game";
        if (game->state == STATE_RUNNING) return "Playing";
        return "Start";
    }
    if (id == BUTTON_PAUSE) {
        return game->state == STATE_PAUSED ? "Resume" : "Pause";
    }
    return "Restart";
}

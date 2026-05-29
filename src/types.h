#ifndef TYPES_H
#define TYPES_H

#include "config.h"

typedef enum {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} Direction;

typedef enum {
    GAME_RUNNING,
    GAME_PAUSED,
    GAME_OVER,
    GAME_VICTORY,
    GAME_QUIT
} GameStatus;

typedef struct {
    int x;
    int y;
} Point;

typedef enum {
    FOOD_NORMAL,
    FOOD_BONUS
} FoodType;

typedef struct {
    Point body[MAX_CELLS];
    int length;
    Direction direction;
    Direction next_direction;
} Snake;

typedef struct {
    Point position;
    FoodType type;
} Food;

typedef struct {
    Snake snake;
    Food food;
    int score;
    int best_score;
    int level;
    int speed_ms;
    int boost_ticks;
    GameStatus status;
} Game;

#endif

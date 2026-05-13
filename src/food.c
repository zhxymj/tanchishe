#include "food.h"
#include "snake.h"

#include <stdlib.h>

void food_spawn(Food *food, const Snake *snake) {
    Point point;

    if (snake->length >= MAX_CELLS) {
        food->position.x = -1;
        food->position.y = -1;
        return;
    }

    do {
        point.x = rand() % BOARD_WIDTH;
        point.y = rand() % BOARD_HEIGHT;
    } while (snake_contains_point(snake, point));

    food->position = point;
}

int food_is_eaten(const Food *food, Point head) {
    return food->position.x == head.x && food->position.y == head.y;
}

void food_apply_score(Game *game) {
    int previous_level = game->level;

    game->score += POINTS_PER_FOOD;
    game->level = game->score / LEVEL_SCORE_STEP + 1;

    if (game->level > previous_level && game->speed_ms > MIN_SPEED_MS) {
        game->speed_ms -= SPEED_STEP_MS;
        if (game->speed_ms < MIN_SPEED_MS) {
            game->speed_ms = MIN_SPEED_MS;
        }
    }
}

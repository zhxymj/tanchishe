#include "food.h"
#include "snake.h"

#include <stdlib.h>

void food_spawn(Food *food, const Snake *snake) {
    int empty_count = MAX_CELLS - snake->length;
    int selected_empty;
    int current_empty = 0;

    if (empty_count <= 0) {
        food->position.x = -1;
        food->position.y = -1;
        return;
    }

    selected_empty = rand() % empty_count;

    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            Point candidate = {x, y};

            if (snake_contains_point(snake, candidate)) {
                continue;
            }

            if (current_empty == selected_empty) {
                food->position = candidate;
                return;
            }

            current_empty++;
        }
    }
}

int food_is_eaten(const Food *food, Point head) {
    return food->position.x == head.x && food->position.y == head.y;
}

void food_apply_score(Game *game) {
    int old_level = game->level;

    game->score += POINTS_PER_FOOD;
    game->level = game->score / LEVEL_SCORE_STEP + 1;

    if (game->level > old_level && game->speed_ms > MIN_SPEED_MS) {
        game->speed_ms -= SPEED_STEP_MS;
        if (game->speed_ms < MIN_SPEED_MS) {
            game->speed_ms = MIN_SPEED_MS;
        }
    }
}

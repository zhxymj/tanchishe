#include "food.h"
#include "snake.h"

void food_spawn(Food *food, const Snake *snake) {
    /*
     * TODO(Member 3): implement random food generation.
     * Required behavior:
     * 1. generate a random position inside the board
     * 2. avoid positions occupied by the snake body
     * 3. handle the board-full case
     */
    (void)snake;
    food->position.x = 1;
    food->position.y = 1;
}

int food_is_eaten(const Food *food, Point head) {
    /* TODO(Member 3): keep this check accurate after food logic is finished. */
    return food->position.x == head.x && food->position.y == head.y;
}

void food_apply_score(Game *game) {
    /*
     * TODO(Member 3): implement scoring, level, and speed rules.
     * Required behavior:
     * 1. add points after eating food
     * 2. update level
     * 3. increase difficulty by reducing speed_ms
     */
    (void)game;
}

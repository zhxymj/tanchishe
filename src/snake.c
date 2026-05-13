#include "snake.h"

static int is_opposite(Direction a, Direction b) {
    return (a == DIR_UP && b == DIR_DOWN) ||
           (a == DIR_DOWN && b == DIR_UP) ||
           (a == DIR_LEFT && b == DIR_RIGHT) ||
           (a == DIR_RIGHT && b == DIR_LEFT);
}

void snake_init(Snake *snake) {
    int start_x = BOARD_WIDTH / 2;
    int start_y = BOARD_HEIGHT / 2;

    /*
     * TODO(Member 2): initialize the full starting snake body.
     * Suggested result: length is INITIAL_SNAKE_LENGTH and the body is placed
     * horizontally near the board center.
     */
    snake->length = 1;
    snake->direction = DIR_RIGHT;
    snake->next_direction = DIR_RIGHT;
    snake->body[0].x = start_x;
    snake->body[0].y = start_y;
}

void snake_set_direction(Snake *snake, Direction direction) {
    /*
     * TODO(Member 2): handle direction update rules.
     * Required behavior:
     * 1. update next_direction when input is legal
     * 2. reject reverse movement, such as RIGHT -> LEFT
     */
    if (!is_opposite(snake->direction, direction)) {
        snake->next_direction = direction;
    }
}

Point snake_next_head(const Snake *snake) {
    Point head = snake->body[0];

    /*
     * TODO(Member 2): calculate and return the next head position according to
     * next_direction. The placeholder returns the current head so the framework
     * can compile before the movement module is finished.
     */
    return head;
}

void snake_move(Snake *snake, Point next_head, int grow) {
    /*
     * TODO(Member 2): complete snake movement and growth.
     * Required behavior:
     * 1. update direction
     * 2. move body segments from tail to head
     * 3. increase length when grow is true
     */
    (void)grow;
    snake->direction = snake->next_direction;
    snake->body[0] = next_head;
}

int snake_contains_point(const Snake *snake, Point point) {
    /* TODO(Member 2): keep this helper correct for food and collision modules. */
    return snake_contains_point_from_index(snake, point, 0);
}

int snake_contains_point_from_index(const Snake *snake, Point point, int start_index) {
    /* TODO(Member 2): search snake body from start_index. */
    for (int i = start_index; i < snake->length; i++) {
        if (snake->body[i].x == point.x && snake->body[i].y == point.y) {
            return 1;
        }
    }
    return 0;
}

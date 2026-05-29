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

    snake->length = INITIAL_SNAKE_LENGTH;
    snake->direction = DIR_RIGHT;
    snake->next_direction = DIR_RIGHT;

    for (int i = 0; i < snake->length; i++) {
        snake->body[i].x = start_x - i;
        snake->body[i].y = start_y;
    }
}

void snake_set_direction(Snake *snake, Direction direction) {
    if (!is_opposite(snake->direction, direction)) {
        snake->next_direction = direction;
    }
}

Point snake_next_head(const Snake *snake) {
    Point head = snake->body[0];

    switch (snake->next_direction) {
        case DIR_UP:
            head.y--;
            break;
        case DIR_DOWN:
            head.y++;
            break;
        case DIR_LEFT:
            head.x--;
            break;
        case DIR_RIGHT:
            head.x++;
            break;
    }

    return head;
}

void snake_move(Snake *snake, Point next_head, int grow) {
    if (grow && snake->length < MAX_CELLS) {
        snake->length++;
    }

    snake->direction = snake->next_direction;

    for (int i = snake->length - 1; i > 0; i--) {
        snake->body[i] = snake->body[i - 1];
    }

    snake->body[0] = next_head;
}

int snake_contains_point(const Snake *snake, Point point) {
    return snake_contains_point_from_index(snake, point, 0);
}

int snake_contains_point_from_index(const Snake *snake, Point point, int start_index) {
    for (int i = start_index; i < snake->length; i++) {
        if (snake->body[i].x == point.x && snake->body[i].y == point.y) {
            return 1;
        }
    }
    return 0;
}

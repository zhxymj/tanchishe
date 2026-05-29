#include "collision.h"

int collision_hits_wall(Point point) {
    return point.x < 0 ||
           point.x >= BOARD_WIDTH ||
           point.y < 0 ||
           point.y >= BOARD_HEIGHT;
}

int collision_hits_self(const Snake *snake, Point next_head, int grow) {
    int check_length = grow ? snake->length : snake->length - 1;

    for (int i = 0; i < check_length; i++) {
        if (snake->body[i].x == next_head.x && snake->body[i].y == next_head.y) {
            return 1;
        }
    }

    return 0;
}

int collision_is_victory(const Snake *snake) {
    return snake->length >= MAX_CELLS;
}

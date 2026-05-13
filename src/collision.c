#include "collision.h"

int collision_hits_wall(Point point) {
    /*
     * TODO(Member 4): implement wall collision.
     * Return 1 when point is outside the board, otherwise return 0.
     */
    (void)point;
    return 0;
}

int collision_hits_self(const Snake *snake, Point next_head, int grow) {
    /*
     * TODO(Member 4): implement self collision.
     * Hint: when grow is false, moving into the current tail position may be legal
     * because the tail will move away in the same update.
     */
    (void)snake;
    (void)next_head;
    (void)grow;
    return 0;
}

int collision_is_victory(const Snake *snake) {
    /* TODO(Member 4): return 1 when the snake fills the whole board. */
    (void)snake;
    return 0;
}

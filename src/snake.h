#ifndef SNAKE_H
#define SNAKE_H

#include "types.h"

void snake_init(Snake *snake);
void snake_set_direction(Snake *snake, Direction direction);
Point snake_next_head(const Snake *snake);
void snake_move(Snake *snake, Point next_head, int grow);
int snake_contains_point(const Snake *snake, Point point);
int snake_contains_point_from_index(const Snake *snake, Point point, int start_index);

#endif

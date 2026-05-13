#ifndef COLLISION_H
#define COLLISION_H

#include "types.h"

int collision_hits_wall(Point point);
int collision_hits_self(const Snake *snake, Point next_head, int grow);
int collision_is_victory(const Snake *snake);

#endif

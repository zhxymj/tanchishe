#ifndef FOOD_H
#define FOOD_H

#include "types.h"

void food_spawn(Food *food, const Snake *snake);
int food_is_eaten(const Food *food, Point head);
void food_apply_score(Game *game);

#endif

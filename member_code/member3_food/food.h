#ifndef FOOD_H
#define FOOD_H

#include "game.h"

void FoodSpawn(Game *game);
void FoodUpdate(Game *game, float dt);
void FoodOnEaten(Game *game);
void FoodDraw(const Game *game);

#endif

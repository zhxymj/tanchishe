#ifndef SNAKE_H
#define SNAKE_H

#include "game.h"

void SnakeInit(Snake *snake);
bool SnakeQueueDirection(Snake *snake, Direction direction);
Cell SnakeNextHead(const Snake *snake);
bool SnakeSameCell(Cell a, Cell b);
bool SnakeContains(const Snake *snake, Cell cell);
bool SnakeHitsSelf(const Snake *snake, Cell next, bool grow);
bool SnakeHitsWall(Cell cell);
void SnakeAdvance(Snake *snake, Cell next, bool grow);

#endif

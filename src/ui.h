#ifndef UI_H
#define UI_H

#include "game.h"

void UIInit(Game *game);
void UIUpdateButtons(Game *game, float dt);
void UIDrawGame(const Game *game);

#endif

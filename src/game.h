#ifndef GAME_H
#define GAME_H

#include "types.h"

void game_init(Game *game);
void game_handle_key(Game *game, int key);
void game_update(Game *game);
int game_current_delay(const Game *game);

#endif

#ifndef UI_H
#define UI_H

#include "types.h"

void ui_setup(void);
void ui_cleanup(void);
void ui_draw(const Game *game);
int ui_read_key(void);
void ui_sleep(int milliseconds);

#endif

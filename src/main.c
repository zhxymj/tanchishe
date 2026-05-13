#include "game.h"
#include "ui.h"

int main(void) {
    Game game;

    ui_setup();
    game_init(&game);

    while (game.status != GAME_QUIT) {
        int key = ui_read_key();

        if (key) {
            game_handle_key(&game, key);
        }

        game_update(&game);
        ui_draw(&game);
        ui_sleep(game.speed_ms);
    }

    ui_cleanup();
    return 0;
}

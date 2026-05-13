#include "game.h"

#include "collision.h"
#include "food.h"
#include "snake.h"

#include <ctype.h>
#include <stdlib.h>
#include <time.h>

void game_init(Game *game) {
    static int seeded = 0;

    /*
     * TODO(Member 1): keep initialization order stable while integrating
     * teammate modules.
     */
    if (!seeded) {
        srand((unsigned int)time(NULL));
        seeded = 1;
    }

    snake_init(&game->snake);
    game->score = 0;
    game->level = 1;
    game->speed_ms = INITIAL_SPEED_MS;
    game->status = GAME_RUNNING;
    food_spawn(&game->food, &game->snake);
}

void game_handle_key(Game *game, int key) {
    key = tolower(key);

    /*
     * TODO(Member 1): maintain game-state input rules.
     * Movement keys are passed to Member 2's snake direction module.
     */
    if (key == 'q') {
        game->status = GAME_QUIT;
        return;
    }

    if (key == 'r') {
        game_init(game);
        return;
    }

    if (key == 'p') {
        if (game->status == GAME_RUNNING) {
            game->status = GAME_PAUSED;
        } else if (game->status == GAME_PAUSED) {
            game->status = GAME_RUNNING;
        }
        return;
    }

    if (game->status != GAME_RUNNING) {
        return;
    }

    switch (key) {
        case 'w':
            snake_set_direction(&game->snake, DIR_UP);
            break;
        case 's':
            snake_set_direction(&game->snake, DIR_DOWN);
            break;
        case 'a':
            snake_set_direction(&game->snake, DIR_LEFT);
            break;
        case 'd':
            snake_set_direction(&game->snake, DIR_RIGHT);
            break;
    }
}

void game_update(Game *game) {
    Point next_head;
    int grow;

    /*
     * TODO(Member 1): this function is the integration point.
     * It should call snake, food, collision, and UI-independent game logic
     * in a clear order once teammate modules are completed.
     */
    if (game->status != GAME_RUNNING) {
        return;
    }

    next_head = snake_next_head(&game->snake);
    grow = food_is_eaten(&game->food, next_head);

    if (collision_hits_wall(next_head) ||
        collision_hits_self(&game->snake, next_head, grow)) {
        game->status = GAME_OVER;
        return;
    }

    snake_move(&game->snake, next_head, grow);

    if (grow) {
        food_apply_score(game);

        if (collision_is_victory(&game->snake)) {
            game->status = GAME_VICTORY;
        } else {
            food_spawn(&game->food, &game->snake);
        }
    }
}

#include "game.h"

#include "collision.h"
#include "food.h"
#include "snake.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static int load_high_score(void) {
    FILE *file = fopen(HIGH_SCORE_FILE, "r");
    int high_score = 0;

    if (file == NULL) {
        return 0;
    }

    if (fscanf(file, "%d", &high_score) != 1 || high_score < 0) {
        high_score = 0;
    }

    fclose(file);
    return high_score;
}

static void save_high_score(int high_score) {
    FILE *file = fopen(HIGH_SCORE_FILE, "w");

    if (file == NULL) {
        return;
    }

    fprintf(file, "%d\n", high_score);
    fclose(file);
}

static void update_high_score(Game *game) {
    if (game->score > game->best_score) {
        game->best_score = game->score;
        save_high_score(game->best_score);
    }
}

void game_init(Game *game) {
    static int seeded = 0;

    if (!seeded) {
        srand((unsigned int)time(NULL));
        seeded = 1;
    }

    snake_init(&game->snake);
    game->score = 0;
    game->best_score = load_high_score();
    game->level = 1;
    game->speed_ms = INITIAL_SPEED_MS;
    game->boost_ticks = 0;
    game->status = GAME_RUNNING;
    food_spawn(&game->food, &game->snake);
}

void game_handle_key(Game *game, int key) {
    key = tolower(key);

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

    if ((key == 'b' || key == ' ') && game->boost_ticks == 0) {
        if (game->snake.length > INITIAL_SNAKE_LENGTH) {
            game->snake.length--;
            game->boost_ticks = BOOST_TICKS;
        }
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

    if (game->status != GAME_RUNNING) {
        return;
    }

    next_head = snake_next_head(&game->snake);
    grow = food_is_eaten(&game->food, next_head);

    if (collision_hits_wall(next_head) ||
        collision_hits_self(&game->snake, next_head, grow)) {
        game->status = GAME_OVER;
        update_high_score(game);
        return;
    }

    snake_move(&game->snake, next_head, grow);

    if (grow) {
        food_apply_score(game);
        update_high_score(game);

        if (collision_is_victory(&game->snake)) {
            game->status = GAME_VICTORY;
            update_high_score(game);
        } else {
            food_spawn(&game->food, &game->snake);
        }
    }

    if (game->boost_ticks > 0) {
        game->boost_ticks--;
    }
}

int game_current_delay(const Game *game) {
    int delay = game->speed_ms;

    if (game->boost_ticks > 0) {
        delay -= BOOST_SPEED_DELTA_MS;
    }

    if (delay < MIN_SPEED_MS / 2) {
        delay = MIN_SPEED_MS / 2;
    }

    return delay;
}

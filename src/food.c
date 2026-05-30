#include "food.h"

#include "snake.h"

#include <math.h>

static Vector2 CellCenter(Cell cell);
static void EmitFoodParticles(Game *game, Vector2 center);

void FoodSpawn(Game *game) {
    Cell options[MAX_SNAKE_LENGTH];
    int count = 0;

    for (int y = 0; y < BOARD_ROWS; y++) {
        for (int x = 0; x < BOARD_COLS; x++) {
            Cell cell = {x, y};
            if (!SnakeContains(&game->snake, cell)) {
                options[count++] = cell;
            }
        }
    }

    if (count == 0) {
        game->state = STATE_GAME_OVER;
        return;
    }

    game->food.position = options[GetRandomValue(0, count - 1)];
    game->food.pulse = 0.0f;
}

void FoodUpdate(Game *game, float dt) {
    game->food.pulse += dt * 5.5f;

    for (int i = 0; i < MAX_FOOD_PARTICLES; i++) {
        FoodParticle *particle = &game->particles[i];
        if (!particle->active) {
            continue;
        }

        particle->age += dt;
        if (particle->age >= particle->life) {
            particle->active = false;
            continue;
        }

        particle->position.x += particle->velocity.x * dt;
        particle->position.y += particle->velocity.y * dt;
        particle->velocity.x *= 0.92f;
        particle->velocity.y *= 0.92f;
    }
}

void FoodOnEaten(Game *game) {
    game->score += SCORE_PER_FOOD;
    game->eatFeedback = 0.24f;
    game->scorePulse = 0.28f;
    EmitFoodParticles(game, CellCenter(game->food.position));
}

void FoodDraw(const Game *game) {
    for (int i = 0; i < MAX_FOOD_PARTICLES; i++) {
        const FoodParticle *particle = &game->particles[i];
        if (!particle->active) {
            continue;
        }

        float t = particle->age / particle->life;
        Color color = particle->color;
        color.a = (unsigned char)((1.0f - t) * 190.0f);
        DrawCircleV(particle->position, particle->radius * (1.0f + t * 1.4f), color);
    }

    float pulse = 1.0f + sinf(game->food.pulse) * 0.12f + game->eatFeedback * 0.7f;
    Vector2 center = CellCenter(game->food.position);
    float radius = 6.5f * pulse;

    DrawCircleGradient(center, 20.0f * pulse, (Color){255, 88, 116, 86}, (Color){255, 88, 116, 0});
    DrawCircleV(center, radius, (Color){255, 88, 116, 255});
    DrawCircleV((Vector2){center.x - radius * 0.32f, center.y - radius * 0.35f}, radius * 0.28f,
                (Color){255, 230, 235, 225});
}

static Vector2 CellCenter(Cell cell) {
    return (Vector2){
        BOARD_X + (float)cell.x * CELL_SIZE + CELL_SIZE * 0.5f,
        BOARD_Y + (float)cell.y * CELL_SIZE + CELL_SIZE * 0.5f
    };
}

static void EmitFoodParticles(Game *game, Vector2 center) {
    for (int i = 0; i < MAX_FOOD_PARTICLES; i++) {
        FoodParticle *particle = &game->particles[i];
        float angle = (float)GetRandomValue(0, 359) * DEG2RAD;
        float speed = (float)GetRandomValue(45, 125);

        particle->active = true;
        particle->position = center;
        particle->velocity = (Vector2){cosf(angle) * speed, sinf(angle) * speed};
        particle->age = 0.0f;
        particle->life = (float)GetRandomValue(26, 52) / 100.0f;
        particle->radius = (float)GetRandomValue(2, 5);
        particle->color = (i % 2 == 0) ? (Color){255, 226, 118, 255} : (Color){255, 96, 136, 255};
    }
}

#include "food.h"

#include "snake.h"

#include <math.h>

#define FOOD_PARTICLE_BURST 30

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
    game->lastEatCenter = CellCenter(game->food.position);
    game->score += SCORE_PER_FOOD;
    game->eatFeedback = 0.24f;
    game->eatRingTimer = EAT_RING_LIFE;
    game->scorePulse = 0.28f;
    EmitFoodParticles(game, game->lastEatCenter);
}

void FoodDraw(const Game *game) {
    for (int i = 0; i < MAX_FOOD_PARTICLES; i++) {
        const FoodParticle *particle = &game->particles[i];
        if (!particle->active) {
            continue;
        }

        float t = particle->age / particle->life;
        Color color = particle->color;
        color.a = (unsigned char)((1.0f - t) * 220.0f);
        DrawCircleGradient(particle->position, particle->radius * (2.2f + t * 1.4f), color, (Color){color.r, color.g, color.b, 0});
        DrawCircleV(particle->position, particle->radius * (1.0f + t * 1.4f), color);
    }

    if (game->eatRingTimer > 0.0f) {
        float progress = 1.0f - game->eatRingTimer / EAT_RING_LIFE;
        float alpha = (1.0f - progress) * 205.0f;
        float radius = 14.0f + progress * 44.0f;
        DrawCircleGradient(game->lastEatCenter, radius + 16.0f, (Color){255, 252, 158, (unsigned char)(alpha * 0.38f)},
                           (Color){255, 252, 158, 0});
        DrawRing(game->lastEatCenter, radius, radius + 4.0f, 0.0f, 360.0f, 48,
                 (Color){255, 245, 116, (unsigned char)alpha});
    }

    float pulse = 1.0f + sinf(game->food.pulse) * 0.12f + game->eatFeedback * 0.7f;
    Vector2 center = CellCenter(game->food.position);
    float radius = 8.0f * pulse;

    DrawCircleGradient(center, 34.0f * pulse, (Color){255, 74, 144, 132}, (Color){255, 74, 144, 0});
    DrawCircleGradient(center, 18.0f * pulse, (Color){255, 229, 91, 155}, (Color){255, 94, 142, 0});
    DrawCircleV(center, radius, (Color){255, 76, 139, 255});
    DrawCircleV(center, radius * 0.62f, (Color){255, 186, 84, 235});
    DrawCircleV((Vector2){center.x - radius * 0.32f, center.y - radius * 0.35f}, radius * 0.28f,
                (Color){255, 255, 238, 230});
}

static Vector2 CellCenter(Cell cell) {
    return (Vector2){
        BOARD_X + (float)cell.x * CELL_SIZE + CELL_SIZE * 0.5f,
        BOARD_Y + (float)cell.y * CELL_SIZE + CELL_SIZE * 0.5f
    };
}

static void EmitFoodParticles(Game *game, Vector2 center) {
    for (int i = 0; i < FOOD_PARTICLE_BURST; i++) {
        FoodParticle *particle = &game->particles[i % MAX_FOOD_PARTICLES];
        float angle = (float)GetRandomValue(0, 359) * DEG2RAD;
        float speed = (float)GetRandomValue(70, 165);

        particle->active = true;
        particle->position = center;
        particle->velocity = (Vector2){cosf(angle) * speed, sinf(angle) * speed};
        particle->age = 0.0f;
        particle->life = (float)GetRandomValue(30, 62) / 100.0f;
        particle->radius = (float)GetRandomValue(2, 6);
        particle->color = (i % 3 == 0) ? (Color){255, 232, 92, 255} :
                          (i % 3 == 1) ? (Color){255, 84, 148, 255} :
                                         (Color){86, 210, 255, 255};
    }
}

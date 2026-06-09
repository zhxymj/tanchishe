#include "controls.h"
#include "game.h"
#include "ui.h"

#include <stdlib.h>
#include <time.h>

int main(void) {
    Game game = {0};

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "贪吃蛇 - raylib");
    SetTargetFPS(TARGET_FPS);

    srand((unsigned int)time(NULL));
    GameInit(&game);
    ControlsInit(&game);

    while (!WindowShouldClose() && !game.shouldQuit) {
        float dt = GetFrameTime();

        GameHandleKeyboard(&game);
        ControlsUpdate(&game, dt);
        GameUpdate(&game, dt);

        BeginDrawing();
        UIDrawGame(&game);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

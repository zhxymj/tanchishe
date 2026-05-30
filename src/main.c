#include "game.h"
#include "ui.h"

#include <stdlib.h>
#include <time.h>

int main(void) {
    Game game = {0};
    SoundPack sounds = {0};

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Snake Arcade - raylib");
    InitAudioDevice();
    SetTargetFPS(TARGET_FPS);

    srand((unsigned int)time(NULL));
    SoundsInit(&sounds);
    GameInit(&game);
    UIInit(&game);

    while (!WindowShouldClose() && !game.shouldQuit) {
        float dt = GetFrameTime();

        GameHandleKeyboard(&game);
        UIUpdateButtons(&game, dt);
        GameUpdate(&game, &sounds, dt);

        BeginDrawing();
        UIDrawGame(&game);
        EndDrawing();
    }

    SoundsUnload(&sounds);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}

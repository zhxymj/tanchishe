#include "ui.h"

#include <conio.h>
#include <stdio.h>
#include <windows.h>

static void clear_screen(void) {
    COORD top_left = {0, 0};
    DWORD written;
    CONSOLE_SCREEN_BUFFER_INFO info;
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD cells;

    GetConsoleScreenBufferInfo(console, &info);
    cells = info.dwSize.X * info.dwSize.Y;
    FillConsoleOutputCharacter(console, ' ', cells, top_left, &written);
    FillConsoleOutputAttribute(console, info.wAttributes, cells, top_left, &written);
    SetConsoleCursorPosition(console, top_left);
}

void ui_setup(void) {
    /* TODO(Member 5): add console setup such as cursor hiding if needed. */
    SetConsoleOutputCP(65001);
}

void ui_cleanup(void) {
    /* TODO(Member 5): restore console settings if ui_setup changes them. */
    printf("\n");
}

void ui_draw(const Game *game) {
    /*
     * TODO(Member 5): implement board drawing, snake display, food display,
     * score display, and end screens.
     */
    clear_screen();

    printf("Snake Game Framework\n");
    printf("Score: %d | Level: %d | Speed: %dms\n",
           game->score, game->level, game->speed_ms);
    printf("Snake head: (%d, %d), length: %d\n",
           game->snake.body[0].x, game->snake.body[0].y, game->snake.length);
    printf("Food: (%d, %d)\n", game->food.position.x, game->food.position.y);
    printf("Controls: W/A/S/D move, P pause, R restart, Q quit\n");

    if (game->status == GAME_PAUSED) {
        printf("\nPaused. Press P to continue.\n");
    } else if (game->status == GAME_OVER) {
        printf("\nGame over. Press R to restart or Q to quit.\n");
    } else if (game->status == GAME_VICTORY) {
        printf("\nVictory. You filled the whole board. Press R to restart or Q to quit.\n");
    }
}

int ui_read_key(void) {
    /* TODO(Member 5): expand keyboard handling if arrow keys are required. */
    if (!_kbhit()) {
        return 0;
    }
    return _getch();
}

void ui_sleep(int milliseconds) {
    /* TODO(Member 5): keep frame delay stable. */
    Sleep((DWORD)milliseconds);
}

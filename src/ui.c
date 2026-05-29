#include "ui.h"

#include <conio.h>
#include <stdio.h>
#include <windows.h>

static CONSOLE_CURSOR_INFO original_cursor_info;
static int has_original_cursor_info = 0;

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

static int is_snake_cell(const Snake *snake, int x, int y, int *is_head) {
    for (int i = 0; i < snake->length; i++) {
        if (snake->body[i].x == x && snake->body[i].y == y) {
            *is_head = (i == 0);
            return 1;
        }
    }

    return 0;
}

void ui_setup(void) {
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursor_info;

    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    SetConsoleTitleA("Snake Game");

    if (GetConsoleCursorInfo(console, &original_cursor_info)) {
        has_original_cursor_info = 1;
        cursor_info = original_cursor_info;
        cursor_info.bVisible = FALSE;
        SetConsoleCursorInfo(console, &cursor_info);
    }
}

void ui_cleanup(void) {
    if (has_original_cursor_info) {
        SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &original_cursor_info);
    }
    printf("\n");
}

void ui_draw(const Game *game) {
    clear_screen();

    printf("Snake Game | Score: %d | Level: %d | Speed: %dms\n",
           game->score, game->level, game->speed_ms);
    printf("Controls: W/A/S/D or Arrow Keys move, P pause, R restart, Q quit\n\n");

    for (int y = -1; y <= BOARD_HEIGHT; y++) {
        for (int x = -1; x <= BOARD_WIDTH; x++) {
            int is_head = 0;

            if (y == -1 || y == BOARD_HEIGHT || x == -1 || x == BOARD_WIDTH) {
                putchar('#');
            } else if (is_snake_cell(&game->snake, x, y, &is_head)) {
                putchar(is_head ? '@' : 'o');
            } else if (game->food.position.x == x && game->food.position.y == y) {
                putchar('*');
            } else {
                putchar(' ');
            }
        }
        putchar('\n');
    }

    if (game->status == GAME_PAUSED) {
        printf("\nPaused. Press P to continue.\n");
    } else if (game->status == GAME_OVER) {
        printf("\nGame over. Press R to restart or Q to quit.\n");
    } else if (game->status == GAME_VICTORY) {
        printf("\nVictory. You filled the whole board. Press R to restart or Q to quit.\n");
    }
}

int ui_read_key(void) {
    int key;
    int arrow;

    if (!_kbhit()) {
        return 0;
    }

    key = _getch();

    if (key == 0 || key == 224) {
        arrow = _getch();
        switch (arrow) {
            case 72:
                return 'w';
            case 80:
                return 's';
            case 75:
                return 'a';
            case 77:
                return 'd';
            default:
                return 0;
        }
    }

    return key;
}

void ui_sleep(int milliseconds) {
    Sleep((DWORD)milliseconds);
}

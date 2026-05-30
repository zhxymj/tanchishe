#include "snake.h"

void SnakeInit(Snake *snake) {
    const int startX = BOARD_COLS / 2;
    const int startY = BOARD_ROWS / 2;

    snake->length = START_LENGTH;
    snake->direction = DIR_RIGHT;
    snake->queuedDirection = DIR_RIGHT;

    for (int i = 0; i < START_LENGTH; i++) {
        snake->body[i] = (Cell){startX - i, startY};
    }
}

bool SnakeQueueDirection(Snake *snake, Direction direction) {
    Direction current = snake->direction;

    if ((current == DIR_UP && direction == DIR_DOWN) ||
        (current == DIR_DOWN && direction == DIR_UP) ||
        (current == DIR_LEFT && direction == DIR_RIGHT) ||
        (current == DIR_RIGHT && direction == DIR_LEFT)) {
        return false;
    }

    snake->queuedDirection = direction;
    return true;
}

Cell SnakeNextHead(const Snake *snake) {
    Cell head = snake->body[0];

    switch (snake->queuedDirection) {
        case DIR_UP: head.y--; break;
        case DIR_DOWN: head.y++; break;
        case DIR_LEFT: head.x--; break;
        case DIR_RIGHT: head.x++; break;
    }

    return head;
}

bool SnakeSameCell(Cell a, Cell b) {
    return a.x == b.x && a.y == b.y;
}

bool SnakeContains(const Snake *snake, Cell cell) {
    for (int i = 0; i < snake->length; i++) {
        if (SnakeSameCell(snake->body[i], cell)) {
            return true;
        }
    }
    return false;
}

bool SnakeHitsSelf(const Snake *snake, Cell next, bool grow) {
    int lengthToCheck = grow ? snake->length : snake->length - 1;

    for (int i = 0; i < lengthToCheck; i++) {
        if (SnakeSameCell(snake->body[i], next)) {
            return true;
        }
    }

    return false;
}

bool SnakeHitsWall(Cell cell) {
    return cell.x < 0 || cell.x >= BOARD_COLS || cell.y < 0 || cell.y >= BOARD_ROWS;
}

void SnakeAdvance(Snake *snake, Cell next, bool grow) {
    snake->direction = snake->queuedDirection;

    if (grow && snake->length < MAX_SNAKE_LENGTH) {
        snake->length++;
    }

    for (int i = snake->length - 1; i > 0; i--) {
        snake->body[i] = snake->body[i - 1];
    }
    snake->body[0] = next;
}

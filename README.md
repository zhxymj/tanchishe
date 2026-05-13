# Snake Game

This is a C console version of Snake for a programming practice group project.

## Features

- Snake movement with `W`, `A`, `S`, `D`
- Pause and continue with `P`
- Restart with `R`
- Quit with `Q`
- Random food generation that avoids the snake body
- Score, level, and speed increase after eating food
- Wall collision, self collision, game over, and victory checks

## Project Structure

```text
snake-game/
  src/
    main.c          Program entry and main loop
    game.c/.h       Game flow, state management, restart, pause, update
    snake.c/.h      Snake body data, movement, growth, direction control
    food.c/.h       Food generation, score, level, speed adjustment
    collision.c/.h  Wall collision, self collision, victory judgment
    ui.c/.h         Console drawing, keyboard input, screen refresh
    config.h        Shared game constants
    types.h         Shared structs and enums
```

## Build

On Windows with GCC installed:

```powershell
gcc src\main.c src\game.c src\snake.c src\food.c src\collision.c src\ui.c -o snake.exe
.\snake.exe
```

## Controls

| Key | Action |
| --- | --- |
| `W` | Move up |
| `A` | Move left |
| `S` | Move down |
| `D` | Move right |
| `P` | Pause or continue |
| `R` | Restart |
| `Q` | Quit |

## Group Coding Tasks

| Member | Coding module | Main work |
| --- | --- | --- |
| Member 1 | Main program and game state | `main.c`, `game.c`, `game.h`: initialization, main loop, pause, restart, quit, game state switching, module integration |
| Member 2 | Snake movement and direction | `snake.c`, `snake.h`: snake data structure, head movement, body following, direction control, growth |
| Member 3 | Food, score, and difficulty | `food.c`, `food.h`: random food generation, avoiding snake body, score update, level and speed adjustment |
| Member 4 | Collision and result judgment | `collision.c`, `collision.h`: wall collision, self collision, game over judgment, victory judgment |
| Member 5 | Interface and keyboard interaction | `ui.c`, `ui.h`: map drawing, snake and food display, score display, keyboard input, screen refresh |

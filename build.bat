@echo off
gcc src\main.c src\game.c src\snake.c src\food.c src\collision.c src\ui.c -o snake.exe
if errorlevel 1 exit /b 1
snake.exe

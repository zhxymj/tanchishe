@echo off
if "%RAYLIB_PATH%"=="" (
  set RAYLIB_PATH=D:\raylib
)
set RAYLIB_INCLUDE=%RAYLIB_PATH%\include
set RAYLIB_LIB=%RAYLIB_PATH%\lib
if exist "%RAYLIB_PATH%\src\raylib.h" set RAYLIB_INCLUDE=%RAYLIB_PATH%\src
if exist "%RAYLIB_PATH%\src\libraylib.a" set RAYLIB_LIB=%RAYLIB_PATH%\src
gcc src\main.c src\game.c src\snake.c src\food.c src\ui.c -std=c11 -O2 -Wall -Wextra -I"%RAYLIB_INCLUDE%" -L"%RAYLIB_LIB%" -lraylib -lopengl32 -lgdi32 -lwinmm -o snake_raylib.exe
if errorlevel 1 exit /b 1
snake_raylib.exe

@echo off
if "%RAYLIB_PATH%"=="" (
  set RAYLIB_PATH=C:\raylib\raylib
)
gcc src\main.c -std=c11 -O2 -Wall -Wextra -I"%RAYLIB_PATH%\src" -L"%RAYLIB_PATH%\src" -lraylib -lopengl32 -lgdi32 -lwinmm -o snake_raylib.exe
if errorlevel 1 exit /b 1
snake_raylib.exe

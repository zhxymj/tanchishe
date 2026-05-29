cmake_minimum_required(VERSION 3.16)
project(tanchishe_raylib C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if (DEFINED ENV{RAYLIB_PATH})
    list(APPEND CMAKE_PREFIX_PATH "$ENV{RAYLIB_PATH}")
endif()

find_package(raylib 5.0 QUIET)

if (NOT raylib_FOUND)
    find_path(RAYLIB_INCLUDE_DIR raylib.h
        HINTS
            "$ENV{RAYLIB_PATH}/src"
            "$ENV{RAYLIB_PATH}/include"
    )
    find_library(RAYLIB_LIBRARY raylib
        HINTS
            "$ENV{RAYLIB_PATH}/src"
            "$ENV{RAYLIB_PATH}/lib"
    )

    if (NOT RAYLIB_INCLUDE_DIR OR NOT RAYLIB_LIBRARY)
        message(FATAL_ERROR "raylib not found. Install raylib, then rerun cmake.")
    endif()
endif()

add_executable(snake_raylib src/main.c)

if (raylib_FOUND)
    target_link_libraries(snake_raylib PRIVATE raylib)
else()
    target_include_directories(snake_raylib PRIVATE ${RAYLIB_INCLUDE_DIR})
    target_link_libraries(snake_raylib PRIVATE ${RAYLIB_LIBRARY})
endif()

if (WIN32)
    target_link_libraries(snake_raylib PRIVATE opengl32 gdi32 winmm)
elseif (UNIX AND NOT APPLE)
    target_link_libraries(snake_raylib PRIVATE m pthread dl rt X11)
endif()

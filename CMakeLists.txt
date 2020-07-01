cmake_minimum_required(VERSION 3.10)
project(Super_Haxagon)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)
include_directories(C:/msys64/opt/devkitpro/libctru/include)
include_directories(C:/msys64/opt/devkitpro/portlibs/3ds/include/ogg)
include_directories(C:/msys64/opt/devkitpro/portlibs/3ds/include/opus)

add_executable(SuperHaxagon ${OPTIONS}
        source/draw.c
        source/font.cpp
        source/levels.c
        source/logic.c
        source/main.cpp
        source/scores.c
        source/sound.c
        source/types.c
        source/util.c
        source/driver/driver_windows.cpp
        source/driver/driver_3ds.cpp
        source/driver/driver_3ds_audio.cpp)

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
        source/Main.cpp
        source/scores.c
        source/sound.c
        source/types.c
        source/util.c
        source/Driver/PlatformWin.cpp
        source/Driver/Platform3DS.cpp
        source/Driver/Audio3DS.cpp
        source/Driver/AudioWin.cpp
        source/Driver/Player3DS.cpp)

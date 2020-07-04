cmake_minimum_required(VERSION 3.10)
project(Super_Haxagon)

set(CMAKE_CXX_STANDARD 14)

add_compile_definitions(_3DS)

include_directories(include)
include_directories(include/Driver)
include_directories(C:/msys64/opt/devkitpro/libctru/include)
include_directories(C:/msys64/opt/devkitpro/portlibs/3ds/include/ogg)
include_directories(C:/msys64/opt/devkitpro/portlibs/3ds/include/opus)

add_executable(SuperHaxagon ${OPTIONS}
        source/Driver/Audio3DS.cpp
        source/Driver/AudioWin.cpp
        source/Driver/Platform3DS.cpp
        source/Driver/PlatformWin.cpp
        source/Driver/Player3DS.cpp

        source/States/Load.cpp
        source/States/Menu.cpp
        source/States/Over.cpp
        source/States/Play.cpp

        source/Factories/Level.cpp
        source/Factories/Pattern.cpp
        source/Factories/Wall.cpp

        source/Game.cpp
        source/Main.cpp
        source/Structs.cpp)

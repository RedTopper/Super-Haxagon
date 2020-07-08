# This CMake file should build SuperHaxagon on MinGW and MSVC
cmake_minimum_required(VERSION 3.10)
project(Super_Haxagon)

include_directories("$ENV{DEVKITPRO}/libctru/include")
include_directories("$ENV{DEVKITPRO}/portlibs/3ds/include/ogg")
include_directories("$ENV{DEVKITPRO}/portlibs/3ds/include/opus")
include_directories("${CMAKE_CURRENT_LIST_DIR}/libraries/SFML/include")
include_directories("${CMAKE_CURRENT_LIST_DIR}/include")

set(CMAKE_CXX_STANDARD 14)
set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_DIR "${CMAKE_CURRENT_LIST_DIR}/libraries/SFML/lib/cmake/SFML")

if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-Wall -Wextra -pedantic)
endif(CMAKE_COMPILER_IS_GNUCXX)

if(MINGW OR MSYS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows")
endif()

add_compile_definitions(SFML_STATIC)

find_package(SFML REQUIRED system window graphics audio)

link_directories(libraries/SFML/lib)

add_executable(SuperHaxagon WIN32 ${OPTIONS}
    source/DriverWin/AudioWin.cpp
    source/DriverWin/FontWin.cpp
    source/DriverWin/PlatformWin.cpp
    source/DriverWin/PlayerWin.cpp

    source/States/Load.cpp
    source/States/Menu.cpp
    source/States/Over.cpp
    source/States/Play.cpp

    source/Factories/Level.cpp
    source/Factories/Pattern.cpp
    source/Factories/Wall.cpp

    source/Core/Game.cpp
    source/Core/Main.cpp
    source/Core/Structs.cpp)

target_link_libraries(SuperHaxagon
    sfml-graphics-s sfml-window-s sfml-audio-s sfml-system-s
    winmm openal32 gdi32 opengl32 freetype flac vorbisenc vorbisfile vorbis ogg)

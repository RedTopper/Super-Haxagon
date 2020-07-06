cmake_minimum_required(VERSION 3.10)
project(Super_Haxagon)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")

# Note:
# If the platform is anything but windows, it won't compile.
# The extra platforms are simply to get code completion working
# in CLion. In the future it may be possible to build 3DS and
# Switch directly through CMake.
set(PLAT WINDOWS)

# These will probably need to be changed if compiling on a different
# machine, sorry
include_directories(C:/msys64/opt/devkitpro/libctru/include)
include_directories(C:/msys64/opt/devkitpro/portlibs/3ds/include/ogg)
include_directories(C:/msys64/opt/devkitpro/portlibs/3ds/include/opus)
include_directories(libraries/SFML/include)
include_directories(include)

if(PLAT STREQUAL "WINDOWS")
    link_directories(libraries/SFML/lib)

    add_executable(SuperHaxagon ${OPTIONS}
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

elseif(PLAT STREQUAL "3DS")
    add_compile_definitions(_3DS)

    add_executable(SuperHaxagon ${OPTIONS}
            source/Driver3DS/Audio3DS.cpp
            source/Driver3DS/Font3DS.cpp
            source/Driver3DS/Player3DS.cpp
            source/Driver3DS/Platform3DS.cpp

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
endif()
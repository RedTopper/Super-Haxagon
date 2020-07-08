cmake_minimum_required(VERSION 3.10)
project(Super_Haxagon)

# Note:
# If the platform is anything but windows, it won't compile.
# The extra platforms are simply to get code completion working
# in CLion. In the future it may be possible to build 3DS and
# Switch directly through CMake.
set(PLAT WINDOWS)

include_directories("$ENV{DEVKITPRO}/libctru/include")
include_directories("$ENV{DEVKITPRO}/portlibs/3ds/include/ogg")
include_directories("$ENV{DEVKITPRO}/portlibs/3ds/include/opus")
include_directories("${CMAKE_CURRENT_LIST_DIR}/libraries/SFML/include")
include_directories("${CMAKE_CURRENT_LIST_DIR}/include")

set(CMAKE_CXX_STANDARD 14)

if(CMAKE_COMPILER_IS_GNUCXX)
  add_definitions(-Wall -Wextra -pedantic)
endif(CMAKE_COMPILER_IS_GNUCXX)

if(PLAT STREQUAL "WINDOWS")
    #add_compile_definitions(SFML_STATIC)

    set(SFML_STATIC_LIBRARIES TRUE)
    set(SFML_DIR "${CMAKE_CURRENT_LIST_DIR}/libraries/SFML/lib/cmake/SFML")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows")

    find_package(SFML REQUIRED system window graphics network audio)

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
            sfml-graphics-d sfml-window-d sfml-audio-d sfml-system-d
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
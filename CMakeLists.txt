# This CMake file should build SuperHaxagon on MinGW and MSVC
# It will not compile the 3DS version, use the Makefile for that
cmake_minimum_required(VERSION 3.10)
project(Super_Haxagon)

set(CMAKE_CXX_STANDARD 17)

if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-Wall -Wextra -pedantic)
endif(CMAKE_COMPILER_IS_GNUCXX)

if(UNIX)
    message(STATUS "Compiling with GCC")
    set(DRIVER source/Driver/Linux/PlatformLinux.cpp)
    # Make sure you have SFML installed via your package manager
    # Usually `sudo apt install libsfml-dev` or similar (Debian/Ubuntu)
endif()

if(MINGW OR MSYS)
    message(STATUS "Compiling with MinGW")
    set(DRIVER source/Driver/Win/PlatformWin.cpp)
    set(SFML_STATIC_LIBRARIES TRUE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows")
    set(SFML_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libraries/SFML-MinGW/lib/cmake/SFML")
endif()

if(MSVC)
    message(STATUS "Compiling with MSVC")
    set(DRIVER source/Driver/Win/PlatformWin.cpp)
    set(SFML_STATIC_LIBRARIES TRUE)
    set(SFML_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libraries/SFML-MSVC/lib/cmake/SFML")
endif()

include_directories(SYSTEM "$ENV{DEVKITPRO}/libctru/include")
include_directories(SYSTEM "$ENV{DEVKITPRO}/libnx/include")
include_directories(SYSTEM "$ENV{DEVKITPRO}/portlibs/switch/include")
include_directories(SYSTEM "$ENV{DEVKITPRO}/portlibs/switch/include/freetype2")
include_directories(SYSTEM "${CMAKE_CURRENT_SOURCE_DIR}/libraries/stb")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

find_package(SFML 2 COMPONENTS system window graphics audio)

add_executable(SuperHaxagon WIN32 ${DRIVER}
    source/Driver/SFML/PlatformSFML.cpp
    source/Driver/SFML/AudioSFML.cpp
    source/Driver/SFML/FontSFML.cpp
    source/Driver/SFML/PlayerSoundSFML.cpp
    source/Driver/SFML/PlayerMusicSFML.cpp

    source/States/Load.cpp
    source/States/Menu.cpp
    source/States/Over.cpp
    source/States/Play.cpp
    source/States/Quit.cpp
    source/States/Transition.cpp
    source/States/Win.cpp

    source/Factories/Level.cpp
    source/Factories/Pattern.cpp
    source/Factories/Wall.cpp

    source/Core/Game.cpp
    source/Core/Metadata.cpp
    source/Core/Main.cpp
    source/Core/Structs.cpp)

target_link_libraries(SuperHaxagon sfml-graphics sfml-window sfml-audio sfml-system)

if(MINGW OR MSYS OR MSVC)
    # Only need to copy dll if on windows
    add_custom_command(TARGET SuperHaxagon POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SFML_DIR}/../../../bin/openal32.dll $<TARGET_FILE_DIR:SuperHaxagon>)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/openal32.dll DESTINATION .)
endif()

add_custom_command(TARGET SuperHaxagon POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/romfs $<TARGET_FILE_DIR:SuperHaxagon>/romfs)
install(TARGETS SuperHaxagon RUNTIME DESTINATION .)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/romfs DESTINATION .)

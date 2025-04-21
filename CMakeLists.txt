# SPDX-FileCopyrightText: 2025 AJ Walter
# SPDX-License-Identifier: GPL-3.0-or-later

cmake_minimum_required(VERSION 3.13)
project(SuperHaxagon VERSION 3.9.1)

set(CMAKE_CXX_STANDARD 17)

set(APP_NAME "SuperHaxagon")
set(APP_FQN "net.awalter.SuperHaxagon")
set(APP_DESCRIPTION "A Super Hexagon Clone")
set(APP_AUTHOR "RedTopper")
set(APP_PRODUCT_CODE "CTR-P-HAXA")
set(APP_UNIQUE_ID "0x099AA")

set(STB_VER ae721c50eaf761660b4f90cc590453cdb0c2acd0)

# Used in 3DS for a console on the bottom screen
# and Switch for debug output to a file
set(DEBUG_CONSOLE false)

set(SOURCES
    source/Main.cpp

    source/States/Load.cpp
    source/States/Menu.cpp
    source/States/Over.cpp
    source/States/Play.cpp
    source/States/Quit.cpp
    source/States/Title.cpp
    source/States/Transition.cpp
    source/States/Win.cpp

    source/Factories/LevelFactory.cpp
    source/Factories/PatternFactory.cpp
    source/Factories/WallFactory.cpp

    source/Objects/Level.cpp
    source/Objects/Pattern.cpp
    source/Objects/Wall.cpp

    source/Core/Camera.cpp
    source/Core/Game.cpp
    source/Core/Matrix.cpp
    source/Core/Metadata.cpp
    source/Core/Structs.cpp
    source/Core/SurfaceGame.cpp
    source/Core/SurfaceUI.cpp
)

if(DRIVER_EXAMPLE)
    # Any platform should be able to build this
    # if you force it to build with cmake -DDRIVER_EXAMPLE=TRUE
    include(driver/Example/example.cmake)
elseif(CMAKE_SYSTEM_NAME MATCHES "Nintendo3DS")
    include(driver/N3DS/3ds.cmake)
elseif(CMAKE_SYSTEM_NAME MATCHES "NintendoSwitch")
    include(driver/Switch/switch.cmake)
elseif(CMAKE_SYSTEM_NAME MATCHES "Nspire")
    include(driver/Nspire/nspire.cmake)
elseif(CMAKE_SYSTEM_NAME MATCHES "Miyoo")
    include(driver/SDL2-Miyoo/sdl2-miyoo.cmake)
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    if(DRIVER_SDL2)
        # cmake -DDRIVER_SDL2=TRUE ..
        # Driver forces windowed and says LINUX
        include(driver/SDL2-Linux/sdl2-linux.cmake)
    elseif(DRIVER_PORTMASTER)
        # cmake -DDRIVER_PORTMASTER=TRUE ..
        # Driver forces fullscreen and says PORTMASTER
        include(driver/SDL2-PortMaster/sdl2-portmaster.cmake)
    else()
        # Default to using the SFML2 driver
        include(driver/SFML-Linux/sfml-linux.cmake)
    endif()
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
    include(driver/SFML-Windows/sfml-windows.cmake)
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    include(driver/SFML-macOS/sfml-macos.cmake)
else()
    message(FATAL_ERROR "No viable platform to build for!")
endif()

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/driver/All/include")
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_BINARY_DIR}/include")
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/source")

# Create vX.X.X.txt version information
include(driver/All/cmake/version.cmake)

# Install README.md, LICENSE.md, etc
include(driver/All/cmake/documents.cmake)

configure_file(
    ${CMAKE_SOURCE_DIR}/driver/All/include/Driver/Tools/Configuration.hpp.in
    include/Driver/Tools/Configuration.hpp
)

# If you are using containers and your IDE can't see the header files in there,
# but you still want type completion, enable the -DHEADERS_FOR_IDE=TRUE CMake option.
# (for CLion this is in Build, Execution, Deployment > CMake > CMake Options)

# This will automatically clone the required headers into the build directories.
# This will NOT allow those components to link though, you'll need the full
# toolchain for that.
if(HEADERS_FOR_IDE)
    include(driver/All/cmake/libraries.cmake)
endif()

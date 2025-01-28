cmake_minimum_required(VERSION 3.18)
project(Super_Haxagon VERSION 3.5.0)

set(CMAKE_CXX_STANDARD 17)

set(APP_NAME "Super Haxagon")
set(APP_DESCRIPTION "A Super Hexagon Clone")
set(APP_AUTHOR "RedHat")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

set(SOURCES
    source/States/Load.cpp
    source/States/Menu.cpp
    source/States/Over.cpp
    source/States/Play.cpp
    source/States/Quit.cpp
    source/States/Transition.cpp
    source/States/Win.cpp

    source/Factories/LevelFactory.cpp
    source/Factories/PatternFactory.cpp
    source/Factories/WallFactory.cpp

    source/Objects/Level.cpp
    source/Objects/Pattern.cpp
    source/Objects/Wall.cpp

    source/Core/Platform.cpp
    source/Core/Game.cpp
    source/Core/Metadata.cpp
    source/Core/Main.cpp
    source/Core/Structs.cpp)

if(CMAKE_SYSTEM_NAME MATCHES "Nintendo3DS")
    include(cmake/n3ds.cmake)
elseif(CMAKE_SYSTEM_NAME MATCHES "NintendoSwitch")
    include(cmake/switch.cmake)
elseif(CMAKE_SYSTEM_NAME MATCHES "Nspire")
    include(cmake/nspire.cmake)
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    include(cmake/linux.cmake)
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
    include(cmake/windows.cmake)
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    include(cmake/macos.cmake)
else()
    message(FATAL_ERROR "No viable platform to build for!")
endif()
cmake_minimum_required(VERSION 3.18)
project(Super_Haxagon VERSION 3.6.1)

set(CMAKE_CXX_STANDARD 17)

set(APP_NAME "Super Haxagon")
set(APP_DESCRIPTION "A Super Hexagon Clone")
set(APP_AUTHOR "RedHat")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/source")

# If you are using containers and your IDE can't see the header files in there,
# but you still want type completion, just git clone the libraries in cmake/libraries.cmake
# and enable the -DHEADERS_FOR_IDE=TRUE CMake option.
# (for CLion this is in Build, Execution, Deployment > CMake > CMake Options)
# (you can also, like, remove the if() block too, but remember to put it back please!)
if(HEADERS_FOR_IDE)
    include(cmake/libraries.cmake)
endif()

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
    source/States/Transition.cpp
    source/States/Win.cpp

    source/Factories/LevelFactory.cpp
    source/Factories/PatternFactory.cpp
    source/Factories/WallFactory.cpp

    source/Objects/Level.cpp
    source/Objects/Pattern.cpp
    source/Objects/Wall.cpp

    source/Core/Game.cpp
    source/Core/Matrix.cpp
    source/Core/Metadata.cpp
    source/Core/Structs.cpp
    source/Core/SurfaceGame.cpp
    source/Core/SurfaceUI.cpp
)

configure_file(source/Core/Configuration.hpp.in Core/Configuration.hpp)
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

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
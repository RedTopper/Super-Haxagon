cmake_minimum_required(VERSION 3.13)
project(Super_Haxagon VERSION 3.8.3)

set(CMAKE_CXX_STANDARD 17)

set(APP_NAME "SuperHaxagon")
set(APP_DESCRIPTION "A Super Hexagon Clone")
set(APP_AUTHOR "RedHat")

set(FETCHCONTENT_QUIET FALSE)
set(STB_VER ae721c50eaf761660b4f90cc590453cdb0c2acd0)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/source")

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

include_directories("${CMAKE_CURRENT_BINARY_DIR}")

if(DRIVER_NULL)
    # Any platform should be able to build this
    # if you force it to build with cmake -DDRIVER_NULL=TRUE
    include(cmake/common/null.cmake)
elseif(CMAKE_SYSTEM_NAME MATCHES "Nintendo3DS")
    include(cmake/n3ds.cmake)
elseif(CMAKE_SYSTEM_NAME MATCHES "NintendoSwitch")
    include(cmake/switch.cmake)
elseif(CMAKE_SYSTEM_NAME MATCHES "Nspire")
    include(cmake/nspire.cmake)
elseif(CMAKE_SYSTEM_NAME MATCHES "Miyoo")
    include(cmake/miyoo.cmake)
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    if(DRIVER_SDL2)
        # cmake -DDRIVER_SDL2=TRUE ..
        # SDL2 configuration for desktops
        include(cmake/linux-sdl2.cmake)
    elseif(DRIVER_PORTMASTER)
        # cmake -DDRIVER_PORTMASTER=TRUE ..
        # Defaults to fullscreen and says portmaster
        include(cmake/portmaster.cmake)
    else()
        # Default to using the SFML2 driver
        include(cmake/linux.cmake)
    endif()
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
    include(cmake/windows.cmake)
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    include(cmake/macos.cmake)
else()
    message(FATAL_ERROR "No viable platform to build for!")
endif()

configure_file(source/Core/Configuration.hpp.in Core/Configuration.hpp)

# If you are using containers and your IDE can't see the header files in there,
# but you still want type completion, enable the -DHEADERS_FOR_IDE=TRUE CMake option.
# (for CLion this is in Build, Execution, Deployment > CMake > CMake Options)

# This will automatically clone the required headers into the build directories.
# This will NOT allow those components to link though, you'll need the full
# toolchain for that.
if(HEADERS_FOR_IDE)
    include(cmake/common/libraries.cmake)
endif()

cmake_minimum_required(VERSION 3.10)
project(Super_Haxagon)

set(CMAKE_CXX_STANDARD 14)

# Note:
# If the platform is anything but windows, it won't compile.
# The extra platforms are simply to get code completion working
# in CLion. In the future it may be possible to build 3DS and
# Switch directly through CMake.
set(PLAT WINDOWS)

include_directories(include)

if(PLAT STREQUAL "WINDOWS")
    find_package(SDL2 CONFIG REQUIRED)
    include_directories(${SDL2_INCLUDE_DIRS})

    add_executable(SuperHaxagon ${OPTIONS}
            source/Driver/AudioWin.cpp
            source/Driver/PlatformWin.cpp

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

    target_link_libraries(SuperHaxagon SDL2main SDL2)

elseif(PLAT STREQUAL "3DS")
    add_compile_definitions(_3DS)
    include_directories(C:/msys64/opt/devkitpro/libctru/include)
    include_directories(C:/msys64/opt/devkitpro/portlibs/3ds/include/ogg)
    include_directories(C:/msys64/opt/devkitpro/portlibs/3ds/include/opus)

    add_executable(SuperHaxagon ${OPTIONS}
            source/Driver/Audio3DS.cpp
            source/Driver/Player3DS.cpp
            source/Driver/Platform3DS.cpp

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
endif()
# cmake -DDRIVER_SDL2=TRUE

message(STATUS "Building for linux...")

set(DRIVER_PLATFORM_SDL2
    source/Driver/Linux/PlatformLinuxSDL2.cpp
    source/Driver/Common/PlatformIsFullyRandom.cpp
)

include(cmake/common/sdl2.cmake)

target_compile_options(SuperHaxagon PRIVATE -Wall -Wextra -pedantic)

# cmake -DDRIVER_SDL2=TRUE

if (PORTMASTER)
    # If you set -DPORTMASTER=TRUE from the commandline,
    # the driver will try to use exclusive fullscreen by default.
    message(STATUS "Building for portmaster...")
    set(DRIVER_PLATFORM_SDL2
        source/Driver/PortMaster/PlatformPortMaster.cpp
        source/Driver/Common/PlatformIsFullyRandom.cpp
    )
else()
    # Desktop linux will attempt to run in a window scaled
    # to the original aspect ratio of the 3DS.
    message(STATUS "Building for linux desktop...")
    set(DRIVER_PLATFORM_SDL2
        source/Driver/Linux/PlatformLinuxSDL2.cpp
        source/Driver/Common/PlatformIsFullyRandom.cpp
    )
endif()

include(cmake/common/sdl2.cmake)

target_compile_options(SuperHaxagon PRIVATE -Wall -Wextra -pedantic)

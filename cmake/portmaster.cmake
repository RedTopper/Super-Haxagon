# cmake -DDRIVER_SDL2=TRUE

# If you set -DPORTMASTER=TRUE from the commandline,
# the driver will try to use exclusive fullscreen by default.
message(STATUS "Building for portmaster...")

set(PLATFORM_NAME "PORTMASTER")

set(DRIVER_PLATFORM ${DRIVER_PLATFORM}
    source/Driver/Common/SDL2/ScreenSDL2.cpp
    source/Driver/Common/PlatformIsFullyRandom.cpp
    source/Driver/Common/PlatformSupportsFilesystem.cpp
    source/Driver/PortMaster/KeybindsPortMaster.cpp
    source/Driver/PortMaster/PlatformPortMaster.cpp
)

include(cmake/common/sdl2.cmake)

target_compile_options(SuperHaxagon PRIVATE -Wall -Wextra -pedantic)

# cmake -DDRIVER_SDL2=TRUE

# Desktop linux will attempt to run in a window scaled
# to the original aspect ratio of the 3DS.
message(STATUS "Building for linux desktop...")

set(PLATFORM_NAME "LINUX SDL2")

set(DRIVER_PLATFORM ${DRIVER_PLATFORM}
    source/Driver/Common/SDL2/ScreenSDL2.cpp
    source/Driver/Common/PlatformIsFullyRandom.cpp
    source/Driver/Common/PlatformSupportsFilesystem.cpp
    source/Driver/Common/SDL2/KeybindsSDL2.cpp
    source/Driver/Linux/PlatformLinuxSDL2.cpp
)

include(cmake/common/sdl2.cmake)

target_compile_options(SuperHaxagon PRIVATE -Wall -Wextra -pedantic)

install(TARGETS SuperHaxagon RUNTIME DESTINATION .)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/romfs DESTINATION .)

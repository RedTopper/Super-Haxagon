# cmake -DDRIVER_SDL2=TRUE

# If you set -DPORTMASTER=TRUE from the commandline,
# the driver will try to use exclusive fullscreen by default.
message(STATUS "Building for portmaster...")

set(PLATFORM_NAME "PORTMASTER")

set(DRIVER_PLATFORM ${DRIVER_PLATFORM}
    source/Driver/Common/SDL2/ScreenSDL2.cpp
    source/Driver/Common/SDL2/KeybindsSDL2.cpp
    source/Driver/Common/PlatformIsFullyRandom.cpp
    source/Driver/Common/PlatformSupportsFilesystem.cpp
    source/Driver/PortMaster/PlatformPortMaster.cpp
)

include(cmake/common/sdl2.cmake)

target_compile_options(SuperHaxagon PRIVATE -Wall -Wextra -pedantic)

# Package
install(FILES ${CMAKE_SOURCE_DIR}/media/portmaster/SuperHaxagon.sh DESTINATION .)

# Game files
install(TARGETS SuperHaxagon RUNTIME DESTINATION ./superhaxagon)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/romfs DESTINATION ./superhaxagon)

# Licenses
install(DIRECTORY ${CMAKE_SOURCE_DIR}/media/portmaster/licenses DESTINATION ./superhaxagon)
install(FILES ${CMAKE_SOURCE_DIR}/LICENSE.md DESTINATION ./superhaxagon/licenses)

# PortMaster packaging files
install(FILES
        ${CMAKE_SOURCE_DIR}/media/portmaster/port.json
        ${CMAKE_SOURCE_DIR}/media/portmaster/README.md
        ${CMAKE_SOURCE_DIR}/media/portmaster/screenshot.png
        ${CMAKE_SOURCE_DIR}/media/portmaster/gameinfo.xml
        ${CMAKE_SOURCE_DIR}/media/portmaster/cover.png
        DESTINATION ./superhaxagon
)

# SPDX-FileCopyrightText: 2025 AJ Walter
# SPDX-License-Identifier: GPL-3.0-or-later OR MIT

# cmake -DDRIVER_SDL2=TRUE

# Desktop linux will attempt to run in a window scaled
# to the original aspect ratio of the 3DS.
message(STATUS "Building for linux desktop...")

set(PLATFORM_NAME "LINUX SDL2")

set(DRIVER_PLATFORM ${DRIVER_PLATFORM}
    driver/SDL2/source/ScreenSDL2.cpp
    driver/SDL2/source/KeybindsSDL2.cpp
    driver/Common/source/PlatformIsFullyRandom.cpp
    driver/Common/source/PlatformSupportsFilesystem.cpp
    driver/Linux-SDL2/source/PlatformLinuxSDL2.cpp
)

include(driver/SDL2/sdl2.cmake)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION .)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/romfs DESTINATION .)

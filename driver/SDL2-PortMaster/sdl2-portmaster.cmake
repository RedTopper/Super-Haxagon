# SPDX-FileCopyrightText: 2025 AJ Walter
# SPDX-License-Identifier: GPL-3.0-or-later OR MIT

# cmake -DDRIVER_SDL2=TRUE

# If you set -DPORTMASTER=TRUE from the commandline,
# the driver will try to use exclusive fullscreen by default.
message(STATUS "Building for portmaster...")

set(PLATFORM_NAME "PORTMASTER")

set(DRIVER_PLATFORM ${DRIVER_PLATFORM}
    driver/All/source/PlatformIsFullyRandom.cpp
    driver/All/source/PlatformSupportsFilesystem.cpp
    driver/SDL2/source/ScreenSDL2.cpp
    driver/SDL2/source/KeybindsSDL2.cpp
    driver/SDL2-PortMaster/source/PlatformPortMaster.cpp
)

include(driver/SDL2/sdl2.cmake)

target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)

string(TOLOWER ${CMAKE_PROJECT_NAME} PROJECT_NAME_LOWER)

# Package
install(FILES ${CMAKE_CURRENT_LIST_DIR}/resources/${CMAKE_PROJECT_NAME}.sh DESTINATION .)

# Game files
install(TARGETS ${CMAKE_PROJECT_NAME} RUNTIME DESTINATION ./${PROJECT_NAME_LOWER})
install(DIRECTORY ${CMAKE_SOURCE_DIR}/romfs DESTINATION ./${PROJECT_NAME_LOWER})

# Licenses and README
set(LICENSE_PATH "./${PROJECT_NAME_LOWER}/licenses")
set(LICENSE_DRIVER "${CMAKE_SOURCE_DIR}/driver/SDL2/LICENSE.md")
set(README_PATH "./${PROJECT_NAME_LOWER}")
set(README_DRIVER "${CMAKE_CURRENT_LIST_DIR}/resources/README.md")

# PortMaster packaging files
install(FILES
    ${CMAKE_CURRENT_LIST_DIR}/resources/port.json
    ${CMAKE_CURRENT_LIST_DIR}/resources/screenshot.png
    ${CMAKE_CURRENT_LIST_DIR}/resources/gameinfo.xml
    ${CMAKE_CURRENT_LIST_DIR}/resources/cover.png
    ${CMAKE_CURRENT_LIST_DIR}/INSTALL.md
    DESTINATION ./${PROJECT_NAME_LOWER}
)

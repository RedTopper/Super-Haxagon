# SPDX-FileCopyrightText: 2025 AJ Walter
# SPDX-License-Identifier: GPL-3.0-or-later OR MIT

# cmake -DDRIVER_SDL2=TRUE

# Desktop linux will attempt to run in a window scaled
# to the original aspect ratio of the 3DS.
message(STATUS "Building for linux desktop...")

set(PLATFORM_NAME "LINUX SDL2")

set(DRIVER_PLATFORM ${DRIVER_PLATFORM}
    driver/All/source/PlatformIsFullyRandom.cpp
    driver/All/source/PlatformSupportsFilesystem.cpp
    driver/SDL2/source/ScreenSDL2.cpp
    driver/SDL2/source/KeybindsSDL2.cpp
    driver/SDL2-Linux/source/PlatformLinuxSDL2.cpp
)

include(driver/SDL2/sdl2.cmake)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION .)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/romfs DESTINATION .)

set(README_DRIVER "${CMAKE_CURRENT_LIST_DIR}/INSTALL.native.md")

# Icons and desktop entry for flatpak
include(GNUInstallDirs)
set(BASE_NAME "${CMAKE_CURRENT_LIST_DIR}/resources")
install(FILES "${BASE_NAME}/${APP_FQN}.desktop" DESTINATION "${CMAKE_INSTALL_DATADIR}/applications")
install(FILES "${BASE_NAME}/${APP_FQN}.png" DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/256x256/apps" RENAME "${APP_FQN}.png")

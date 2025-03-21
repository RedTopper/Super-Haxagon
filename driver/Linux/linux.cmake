# SPDX-FileCopyrightText: 2025 AJ Walter, see driver/SFML/README.md for MIT usage.
# SPDX-License-Identifier: GPL-3.0-or-later OR MIT

# cmake

message(STATUS "Building for linux...")

set(PLATFORM_NAME "LINUX SFML")

set(DRIVER_PLATFORM
    driver/Linux/source/PlatformLinux.cpp
    driver/Common/source/PlatformIsFullyRandom.cpp
)

include(driver/SFML/sfml.cmake)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)

# Icons and desktop entry for flatpak
include(GNUInstallDirs)
set(BASE_NAME "${CMAKE_SOURCE_DIR}/driver/Linux/resources")
install(FILES "${BASE_NAME}/${APP_FQN}.desktop" DESTINATION "${CMAKE_INSTALL_DATADIR}/applications")
install(FILES "${BASE_NAME}/${APP_FQN}.png" DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/256x256/apps" RENAME "${APP_FQN}.png")

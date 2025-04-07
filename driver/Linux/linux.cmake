# SPDX-FileCopyrightText: 2025 AJ Walter
# SPDX-License-Identifier: GPL-3.0-or-later OR MIT

# cmake

message(STATUS "Building for linux...")

set(PLATFORM_NAME "LINUX SFML")

set(DRIVER_PLATFORM
    driver/All/source/PlatformIsFullyRandom.cpp
    driver/Linux/source/PlatformLinux.cpp
)

include(driver/SFML/sfml.cmake)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)

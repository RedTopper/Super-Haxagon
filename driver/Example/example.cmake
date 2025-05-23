# SPDX-FileCopyrightText: 2025 AJ Walter
# SPDX-License-Identifier: GPL-3.0-or-later OR MIT

# cmake -DDRIVER_EXAMPLE=TRUE

message(STATUS "Building example driver...")

set(DRIVER
    driver/Example/source/MusicExample.cpp
    driver/Example/source/ScreenExample.cpp
    driver/Example/source/SoundExample.cpp
    driver/Example/source/FontExample.cpp
    driver/Example/source/PlatformExample.cpp
)

add_executable(${PROJECT_NAME} ${DRIVER} ${SOURCES})

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION .)

set(LICENSE_DRIVER "${CMAKE_CURRENT_LIST_DIR}/LICENSE.md")
set(README_DRIVER "${CMAKE_CURRENT_LIST_DIR}/README.md")

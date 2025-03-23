# SPDX-FileCopyrightText: 2025 AJ Walter
# SPDX-License-Identifier: GPL-3.0-or-later OR MIT

# cmake

message(STATUS "Using SDL2 driver...")

find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS})

set(DRIVER ${DRIVER_PLATFORM}
    driver/SDL2/source/MusicSDL2.cpp
    driver/SDL2/source/SoundSDL2.cpp
    driver/SDL2/source/FontSDL2.cpp
    driver/SDL2/source/PlatformSDL2.cpp
)

add_executable(${PROJECT_NAME} ${DRIVER} ${SOURCES})

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} SDL2_mixer SDL2_ttf)

target_include_directories(${PROJECT_NAME} PRIVATE "driver/SDL2/source")

file(RELATIVE_PATH REL_PATH ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/romfs)

if (NOT "${REL_PATH}" STREQUAL "romfs")
    # If the relative path is in another directory, create it.
    file(CREATE_LINK ${REL_PATH} ${CMAKE_BINARY_DIR}/romfs SYMBOLIC)
endif()

set(LICENSE_DRIVER "${CMAKE_CURRENT_LIST_DIR}/LICENSE.md")

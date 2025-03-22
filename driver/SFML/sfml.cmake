# SPDX-FileCopyrightText: 2025 AJ Walter, see driver/SDL2/README.md for MIT usage.
# SPDX-License-Identifier: GPL-3.0-or-later OR MIT

# cmake

cmake_minimum_required(VERSION 3.28)

message(STATUS "Using sfml driver...")

find_package(SFML 2 COMPONENTS system window graphics audio QUIET)

if (${SFML_FOUND})
    message(STATUS "SFML found on your local system!")
else()
    message(STATUS "Fetching SFML...")
    set(BUILD_SHARED_LIBS OFF)

    include(FetchContent)
    FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.x
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        EXCLUDE_FROM_ALL
        SYSTEM)

    FetchContent_MakeAvailable(SFML)
endif()

set(DRIVER ${DRIVER_PLATFORM}
    driver/SFML/source/MusicSFML.cpp
    driver/SFML/source/ScreenSFML.cpp
    driver/SFML/source/SoundSFML.cpp
    driver/SFML/source/FontSFML.cpp
    driver/SFML/source/PlatformSFML.cpp
    driver/Common/source/PlatformSupportsFilesystem.cpp
)

add_executable(${PROJECT_NAME} ${DRIVER} ${SOURCES})

target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-window sfml-audio sfml-system)

target_include_directories(${PROJECT_NAME} PRIVATE "driver/SFML/source")

file(RELATIVE_PATH REL_PATH ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/romfs)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    # Windows does not have permission (by default) to symlink
    file(COPY ${CMAKE_SOURCE_DIR}/romfs DESTINATION ${CMAKE_BINARY_DIR})
elseif (NOT "${REL_PATH}" STREQUAL "romfs")
    # If the relative path is in another directory, create it.
    file(CREATE_LINK ${REL_PATH} ${CMAKE_BINARY_DIR}/romfs SYMBOLIC)
endif()

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION .)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/romfs DESTINATION .)

set(LICENSE_DRIVER "${CMAKE_CURRENT_LIST_DIR}/LICENSE.md")


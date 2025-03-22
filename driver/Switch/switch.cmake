# SPDX-FileCopyrightText: 2020 AJ Walter, see driver/Switch/README.md for MIT usage.
# SPDX-License-Identifier: GPL-3.0-or-later OR MIT

# /opt/devkitpro/portlibs/switch/bin/aarch64-none-elf-cmake

message(STATUS "Building for switch...")

set(PLATFORM_NAME "SWITCH")

include_directories(SYSTEM "$ENV{DEVKITPRO}/portlibs/switch/include")
include_directories(SYSTEM "$ENV{DEVKITPRO}/portlibs/switch/include/freetype2")

set(DRIVER
    driver/Switch/source/MusicSwitch.cpp
    driver/Switch/source/ScreenSwitch.cpp
    driver/Switch/source/FontSwitch.cpp
    driver/Switch/source/PlatformSwitch.cpp
    driver/Switch/source/RenderTarget.cpp
    driver/SDL2/source/SoundSDL2.cpp
    driver/Common/source/PlatformSupportsFilesystem.cpp
)

add_executable(${PROJECT_NAME} ${DRIVER} ${SOURCES})

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wno-psabi)

target_link_libraries(${PROJECT_NAME} SDL2_mixer SDL2 EGL glad glapi drm_nouveau freetype png16 bz2 z vorbisidec opusfile opus ogg mpg123 modplug)

target_include_directories(${PROJECT_NAME} PRIVATE "driver/Switch/source")

nx_generate_nacp(${PROJECT_NAME}.nacp
    NAME "${APP_NAME}"
    AUTHOR "${APP_AUTHOR}"
)

nx_create_nro(${PROJECT_NAME}
    NACP ${PROJECT_NAME}.nacp
    ICON "${CMAKE_CURRENT_LIST_DIR}/resources/icon.jpg"
    ROMFS "${CMAKE_SOURCE_DIR}/romfs"
)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.nro" DESTINATION .)

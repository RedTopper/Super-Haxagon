# SPDX-FileCopyrightText: 2025 AJ Walter
# SPDX-License-Identifier: GPL-3.0-or-later OR MIT

# cmake -DCMAKE_TOOLCHAIN_FILE=driver/Nspire/cmake/TC-nspire.cmake

message(STATUS "Building for nspire...")

set(PLATFORM_NAME "NSPIRE")

set(DRIVER
    driver/Nspire/source/MusicNspire.cpp
    driver/Nspire/source/ScreenNspire.cpp
    driver/Nspire/source/FontNspire.cpp
    driver/Nspire/source/MemoryFS.cpp
    driver/Nspire/source/PlatformNspire.cpp
    driver/Nspire/source/stubs.c
    driver/Nspire/source/timer.c
    driver/Example/source/SoundExample.cpp
)

set(RESOURCES
    romfs/levels.haxagon
    romfs/bgm/callMeKatla.txt
    romfs/bgm/captainCool.txt
    romfs/bgm/commandoSteve.txt
    romfs/bgm/drFinkelfracken.txt
    romfs/bgm/esiannoyamFoEzam.txt
    romfs/bgm/jackRussel.txt
    romfs/bgm/mazeOfMayonnaise.txt
    romfs/bgm/screenSaver.txt
)

# Will create COMPILED_RESOURCES to add to target
build_memoryfs("${RESOURCES}")

add_executable(${PROJECT_NAME} ${DRIVER} ${SOURCES} ${COMPILED_RESOURCES})

create_tns(${PROJECT_NAME}
    NAME "${APP_NAME}"
    AUTHOR "${APP_AUTHOR}"
)

install(FILES "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.tns" DESTINATION .)

set(LICENSE_DRIVER "${CMAKE_CURRENT_LIST_DIR}/LICENSE.md")

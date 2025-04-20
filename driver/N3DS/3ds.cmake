# SPDX-FileCopyrightText: 2025 AJ Walter
# SPDX-License-Identifier: GPL-3.0-or-later OR MIT

# /opt/devkitpro/portlibs/3ds/bin/arm-none-eabi-cmake

message(STATUS "Building for 3DS...")

include(FetchContent)

message(STATUS "Fetching 3DS stb...")
FetchContent_Declare(stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG ${STB_VER}
    GIT_PROGRESS TRUE
    SOURCE_SUBDIR nonexistent)
FetchContent_MakeAvailable(stb)

set(PLATFORM_NAME "3DS")

# Include extra banner, icon, and rom functions
include(driver/N3DS/cmake/functions.cmake)

set(DRIVER
    driver/All/source/PlatformSupportsFilesystem.cpp
    driver/N3DS/source/Platform3DS.cpp
    driver/N3DS/source/Font3DS.cpp
    driver/N3DS/source/Music3DS.cpp
    driver/N3DS/source/Screen3DS.cpp
    driver/N3DS/source/Sound3DS.cpp
    driver/N3DS/source/Wav3DS.cpp
)

add_executable(${PROJECT_NAME} ${DRIVER} ${SOURCES})

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wno-psabi -ffast-math)

target_link_libraries(${PROJECT_NAME} citro2d citro3d)

target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC "${stb_SOURCE_DIR}")

ctr_generate_smdh(${PROJECT_NAME}.smdh
    NAME "${APP_NAME}"
    DESCRIPTION "${APP_DESCRIPTION}"
    AUTHOR "${APP_AUTHOR}"
    ICON "${CMAKE_CURRENT_LIST_DIR}/resources/icon.png"
)

ctr_create_3dsx(${PROJECT_NAME}
    ROMFS "${CMAKE_SOURCE_DIR}/romfs"
    SMDH ${PROJECT_NAME}.smdh
)

# Requires `bannertool`
generate_icn(${PROJECT_NAME}.icn
    NAME "${APP_NAME}"
    DESCRIPTION "${APP_DESCRIPTION}"
    AUTHOR "${APP_AUTHOR}"
    ICON "${CMAKE_CURRENT_LIST_DIR}/resources/icon.png"
)

# Requires `bannertool`
generate_bnr(${PROJECT_NAME}.bnr
    CGFX "${CMAKE_CURRENT_LIST_DIR}/resources/banner.cgfx"
    AUDIO "${CMAKE_CURRENT_LIST_DIR}/resources/audio.wav"
)

# Requires `makerom`
foreach(ROM_TARGET CIA;3DS)
    create_rom(${PROJECT_NAME}
        "${ROM_TARGET}"
        ICN ${PROJECT_NAME}.icn
        BNR ${PROJECT_NAME}.bnr
        TITLE "${APP_NAME}"
        PRODUCT_CODE "${APP_PRODUCT_CODE}"
        UNIQUE_ID "${APP_UNIQUE_ID}"
        ROMFS "${CMAKE_SOURCE_DIR}/romfs"
        LOGO "${CMAKE_CURRENT_LIST_DIR}/resources/logo.bcma.lz"
        RSF "${CMAKE_CURRENT_LIST_DIR}/resources/template.rsf"
    )
endforeach()

install(FILES "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.cia" "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.3dsx" DESTINATION .)

set(LICENSE_DRIVER "${CMAKE_CURRENT_LIST_DIR}/LICENSE.md")
set(README_DRIVER "${CMAKE_CURRENT_LIST_DIR}/INSTALL.md")

# /opt/devkitpro/portlibs/3ds/bin/arm-none-eabi-cmake

message(STATUS "Building for 3DS...")

include(FetchContent)
FetchContent_Declare(stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG ${STB_VER}
    GIT_PROGRESS TRUE
    SOURCE_SUBDIR nonexistent)

FetchContent_MakeAvailable(stb)

set(PLATFORM_NAME "3DS")

# Include platform extensions not in devkitpro for .cia and .3ds files
include(cmake/toolchains/Platform/Nintendo3DSExtra.cmake)

set(DRIVER
    source/Driver/3DS/Platform3DS.cpp
    source/Driver/3DS/Font3DS.cpp
    source/Driver/3DS/Music3DS.cpp
    source/Driver/3DS/Screen3DS.cpp
    source/Driver/3DS/Sound3DS.cpp
    source/Driver/3DS/Wav3DS.cpp
    source/Driver/Common/PlatformSupportsFilesystem.cpp
)

add_executable(SuperHaxagon ${DRIVER} ${SOURCES})

target_compile_options(SuperHaxagon PRIVATE -Wall -Wno-psabi -ffast-math)

target_link_libraries(SuperHaxagon citro2d citro3d)

target_include_directories(SuperHaxagon SYSTEM PUBLIC "${stb_SOURCE_DIR}")

ctr_generate_smdh(SuperHaxagon.smdh
    NAME "${APP_NAME}"
    DESCRIPTION "${APP_DESCRIPTION}"
    AUTHOR "${APP_AUTHOR}"
    ICON "${CMAKE_CURRENT_SOURCE_DIR}/media/3ds/icon-3ds.png"
)

ctr_create_3dsx(SuperHaxagon
    ROMFS "${CMAKE_CURRENT_SOURCE_DIR}/romfs"
    SMDH SuperHaxagon.smdh
)

# Requires `bannertool`
generate_icn(SuperHaxagon.icn
    NAME "${APP_NAME}"
    DESCRIPTION "${APP_DESCRIPTION}"
    AUTHOR "${APP_AUTHOR}"
    ICON "${CMAKE_CURRENT_SOURCE_DIR}/media/3ds/icon-3ds.png"
)

# Requires `bannertool`
generate_bnr(SuperHaxagon.bnr
    CGFX "${CMAKE_CURRENT_SOURCE_DIR}/media/3ds/banner.cgfx"
    AUDIO "${CMAKE_CURRENT_SOURCE_DIR}/media/3ds/audio.wav"
)

# Requires `makerom`
foreach(ROM_TARGET CIA;3DS)
    create_rom(SuperHaxagon
        "${ROM_TARGET}"
        ICN SuperHaxagon.icn
        BNR SuperHaxagon.bnr
        RSF "${CMAKE_CURRENT_SOURCE_DIR}/media/3ds/template.rsf"
        TITLE "${APP_NAME}"
        PRODUCT_CODE "CTR-P-HAXA"
        UNIQUE_ID "0x099AA"
        ROMFS "${CMAKE_CURRENT_SOURCE_DIR}/romfs"
        LOGO "${CMAKE_CURRENT_SOURCE_DIR}/media/3ds/logo.bcma.lz"
    )
endforeach()

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/SuperHaxagon.cia"
        "${CMAKE_CURRENT_BINARY_DIR}/SuperHaxagon.3dsx"
        DESTINATION .
)

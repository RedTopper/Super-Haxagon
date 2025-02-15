# /opt/devkitpro/portlibs/switch/bin/aarch64-none-elf-cmake

message(STATUS "Building for switch...")

include_directories(SYSTEM "$ENV{DEVKITPRO}/portlibs/switch/include")
include_directories(SYSTEM "$ENV{DEVKITPRO}/portlibs/switch/include/freetype2")

set(DRIVER
    source/Driver/Switch/MusicSwitch.cpp
    source/Driver/Switch/ScreenSwitch.cpp
    source/Driver/Switch/SoundSwitch.cpp
    source/Driver/Switch/FontSwitch.cpp
    source/Driver/Switch/PlatformSwitch.cpp
    source/Driver/Switch/RenderTarget.cpp
    source/Driver/Common/PlatformSupportsFilesystem.cpp
)

add_executable(SuperHaxagon ${DRIVER} ${SOURCES})

target_compile_options(SuperHaxagon PRIVATE -Wall -Wno-psabi)

target_link_libraries(SuperHaxagon SDL2_mixer SDL2 EGL glad glapi drm_nouveau freetype png16 bz2 z vorbisidec opusfile opus ogg mpg123 modplug)

nx_generate_nacp(SuperHaxagon.nacp
    NAME "${APP_NAME}"
    AUTHOR "${APP_AUTHOR}"
)

nx_create_nro(SuperHaxagon
    NACP SuperHaxagon.nacp
    ICON "${CMAKE_CURRENT_SOURCE_DIR}/media/switch/icon-switch.jpg"
    ROMFS "${CMAKE_CURRENT_SOURCE_DIR}/romfs"
)

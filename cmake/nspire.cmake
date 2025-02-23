# cmake -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/TC-nspire.cmake

message(STATUS "Building for nspire...")

set(PLATFORM_NAME "NSPIRE")

set(DRIVER
    source/Driver/Nspire/MusicNspire.cpp
    source/Driver/Nspire/ScreenNspire.cpp
    source/Driver/Common/Null/SoundNull.cpp
    source/Driver/Nspire/FontNspire.cpp
    source/Driver/Nspire/MemoryFS.cpp
    source/Driver/Nspire/PlatformNspire.cpp
    source/Driver/Nspire/stubs.c
    source/Driver/Nspire/timer.c
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

add_executable(SuperHaxagon ${DRIVER} ${SOURCES} ${COMPILED_RESOURCES})

create_tns(SuperHaxagon
    NAME "${APP_NAME}"
    AUTHOR "${APP_AUTHOR}"
)

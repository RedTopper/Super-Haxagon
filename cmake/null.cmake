# cmake

message(STATUS "Building null driver...")

set(DRIVER
        source/Driver/Common/Null/MusicNull.cpp
        source/Driver/Common/Null/ScreenNull.cpp
        source/Driver/Common/Null/SoundNull.cpp
        source/Driver/Common/Null/FontNull.cpp
        source/Driver/Common/Null/PlatformNull.cpp
)

add_executable(SuperHaxagon ${DRIVER} ${SOURCES})

# cmake

message(STATUS "Using SDL2 driver...")

find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS})

set(DRIVER ${DRIVER_PLATFORM}
    source/Driver/Common/SDL2/MusicSDL2.cpp
    source/Driver/Common/SDL2/SoundSDL2.cpp
    source/Driver/Common/SDL2/FontSDL2.cpp
    source/Driver/Common/SDL2/PlatformSDL2.cpp
)

add_executable(SuperHaxagon ${DRIVER} ${SOURCES})

target_link_libraries(SuperHaxagon ${SDL2_LIBRARIES} SDL2_mixer SDL2_ttf)

file(RELATIVE_PATH REL_PATH ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR}/romfs)

if (NOT "${REL_PATH}" STREQUAL "romfs")
    # If the relative path is in another directory, create it.
    file(CREATE_LINK ${REL_PATH} ${CMAKE_CURRENT_BINARY_DIR}/romfs SYMBOLIC)
endif()

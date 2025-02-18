# cmake

message(STATUS "Using SDL2 driver...")

find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS})

set(DRIVER ${DRIVER_PLATFORM_SDL2}
    source/Driver/Common/SDL2/MusicSDL2.cpp
    source/Driver/Common/SDL2/ScreenSDL2.cpp
    source/Driver/Common/SDL2/SoundSDL2.cpp
    source/Driver/Common/SDL2/FontSDL2.cpp
    source/Driver/Common/SDL2/PlatformSDL2.cpp
    source/Driver/Common/PlatformSupportsFilesystem.cpp
)

add_executable(SuperHaxagonSDL2 ${DRIVER} ${SOURCES})

target_link_libraries(SuperHaxagonSDL2 ${SDL2_LIBRARIES} SDL2_mixer SDL2_ttf)

add_custom_command(TARGET SuperHaxagonSDL2 POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/romfs $<TARGET_FILE_DIR:SuperHaxagonSDL2>/romfs)
install(TARGETS SuperHaxagonSDL2 RUNTIME DESTINATION .)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/romfs DESTINATION .)

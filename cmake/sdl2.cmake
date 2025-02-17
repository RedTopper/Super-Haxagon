# cmake

message(STATUS "Building SDL2 driver...")

find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS})

set(DRIVER
    source/Driver/Common/Null/MusicNull.cpp
    source/Driver/Common/SDL2/ScreenSDL2.cpp
    source/Driver/Common/Null/SoundNull.cpp
    source/Driver/Common/Null/FontNull.cpp
    source/Driver/Common/SDL2/PlatformSDL2.cpp
    source/Driver/Common/PlatformSupportsFilesystem.cpp
)

add_executable(SuperHaxagon ${DRIVER} ${SOURCES})

target_link_libraries(SuperHaxagon ${SDL2_LIBRARIES} SDL2_mixer)

add_custom_command(TARGET SuperHaxagon POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/romfs $<TARGET_FILE_DIR:SuperHaxagon>/romfs)
install(TARGETS SuperHaxagon RUNTIME DESTINATION .)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/romfs DESTINATION .)

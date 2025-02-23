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

add_custom_command(TARGET SuperHaxagon POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/romfs $<TARGET_FILE_DIR:SuperHaxagon>/romfs)
install(TARGETS SuperHaxagon RUNTIME DESTINATION .)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/romfs DESTINATION .)

# cmake

message(STATUS "Using sfml driver...")

find_package(SFML 2 COMPONENTS system window graphics audio)

include_directories(${SFML_INCLUDE_DIR})

set(DRIVER ${DRIVER_PLATFORM}
    source/Driver/Common/SFML/MusicSFML.cpp
    source/Driver/Common/SFML/ScreenSFML.cpp
    source/Driver/Common/SFML/SoundSFML.cpp
    source/Driver/Common/SFML/FontSFML.cpp
    source/Driver/Common/SFML/PlatformSFML.cpp
    source/Driver/Common/PlatformSupportsFilesystem.cpp
)

add_executable(SuperHaxagon ${DRIVER} ${SOURCES})

target_link_libraries(SuperHaxagon sfml-graphics sfml-window sfml-audio sfml-system)

add_custom_command(TARGET SuperHaxagon POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/romfs $<TARGET_FILE_DIR:SuperHaxagon>/romfs)
install(TARGETS SuperHaxagon RUNTIME DESTINATION .)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/romfs DESTINATION .)

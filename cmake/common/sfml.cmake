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

file(RELATIVE_PATH REL_PATH ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR}/romfs)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    # Windows does not have permission (by default) to symlink
    file(COPY ${CMAKE_SOURCE_DIR}/romfs DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
elseif (NOT "${REL_PATH}" STREQUAL "romfs")
    # If the relative path is in another directory, create it.
    file(CREATE_LINK ${REL_PATH} ${CMAKE_CURRENT_BINARY_DIR}/romfs SYMBOLIC)
endif()

install(TARGETS SuperHaxagon RUNTIME DESTINATION .)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/romfs DESTINATION .)

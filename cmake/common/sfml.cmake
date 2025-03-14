# cmake

cmake_minimum_required(VERSION 3.28)

message(STATUS "Using sfml driver...")

find_package(SFML 2 COMPONENTS system window graphics audio QUIET)

if (${SFML_FOUND})
    message(STATUS "SFML found on your local system!")
else()
    message(STATUS "Fetching SFML...")
    set(BUILD_SHARED_LIBS OFF)

    include(FetchContent)
    FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.x
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        EXCLUDE_FROM_ALL
        SYSTEM)

    FetchContent_MakeAvailable(SFML)
endif()

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

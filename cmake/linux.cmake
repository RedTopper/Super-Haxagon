# cmake

message(STATUS "Building for linux...")

find_package(OpenAL 1 REQUIRED)
if (NOT TARGET OpenAL)
    # Naughty naughty workaround for SFML not finding OpenAL-soft on Linux
    add_custom_target(OpenAL DEPENDS OpenAL::OpenAL)
endif()

set(DRIVER_PLATFORM source/Driver/Linux/PlatformLinux.cpp)

include(cmake/sfml.cmake)

target_compile_options(SuperHaxagon PRIVATE -Wall -Wextra -pedantic)

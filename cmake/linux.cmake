# cmake

message(STATUS "Building for linux...")

set(DRIVER_PLATFORM
    source/Driver/Linux/PlatformLinux.cpp
    source/Driver/Common/PlatformIsFullyRandom.cpp
)

find_package(OpenAL 1 REQUIRED)
if (NOT TARGET OpenAL)
    # Naughty naughty workaround for SFML not finding OpenAL-soft on Linux
    add_custom_target(OpenAL DEPENDS OpenAL::OpenAL)
endif()

include(cmake/common/sfml.cmake)

if (SFML_STATIC_LIBRARIES)
    target_compile_definitions(SuperHaxagon PRIVATE SFML_STATIC)
endif()

target_compile_options(SuperHaxagon PRIVATE -Wall -Wextra -pedantic)

# Icons and destkop entry for flatpak
include(GNUInstallDirs)
set(FQN "net.awalter.SuperHaxagon")
set(BASE_NAME "${CMAKE_CURRENT_SOURCE_DIR}/media/linux")
install(FILES "${BASE_NAME}/${FQN}.desktop" DESTINATION "${CMAKE_INSTALL_DATADIR}/applications")
install(FILES "${BASE_NAME}/icon-linux.png" DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/256x256/apps" RENAME "${FQN}.png")

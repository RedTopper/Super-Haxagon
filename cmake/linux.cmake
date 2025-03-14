# cmake

message(STATUS "Building for linux...")

set(PLATFORM_NAME "LINUX SFML")

set(DRIVER_PLATFORM
    source/Driver/Linux/PlatformLinux.cpp
    source/Driver/Common/PlatformIsFullyRandom.cpp
)

include(cmake/common/sfml.cmake)

target_compile_options(SuperHaxagon PRIVATE -Wall -Wextra -pedantic)

# Icons and destkop entry for flatpak
include(GNUInstallDirs)
set(FQN "net.awalter.SuperHaxagon")
set(BASE_NAME "${CMAKE_CURRENT_SOURCE_DIR}/media/linux")
install(FILES "${BASE_NAME}/${FQN}.desktop" DESTINATION "${CMAKE_INSTALL_DATADIR}/applications")
install(FILES "${BASE_NAME}/icon-linux.png" DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/256x256/apps" RENAME "${FQN}.png")

# cmake
message(STATUS "Building for macOS...")

set(PLATFORM_NAME "MACOS")

set(DRIVER_PLATFORM
    source/Driver/macOS/PlatformMacOS.cpp
    source/Driver/Common/PlatformIsFullyRandom.cpp
)

set(BUILD_BUNDLE TRUE)

include(cmake/functions/macos.cmake)

include(cmake/common/sfml.cmake)

set(MACOSX_BUNDLE_BUNDLE_NAME ${APP_NAME})  # Short name
set(MACOSX_BUNDLE_GUI_IDENTIFIER "net.awalter.${APP_NAME}")
set(MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
set(MACOSX_BUNDLE_LONG_VERSION_STRING ${MACOSX_BUNDLE_BUNDLE_VERSION})
set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${MACOSX_BUNDLE_BUNDLE_VERSION})

generate_icns(SuperHaxagon
    ICON "${CMAKE_CURRENT_SOURCE_DIR}/media/sources/icon-full.png"
)

set_target_properties(SuperHaxagon PROPERTIES
    MACOSX_BUNDLE TRUE
    INSTALL_RPATH "@executable_path/../Frameworks"
    BUILD_WITH_INSTALL_RPATH TRUE
)

add_custom_command(TARGET SuperHaxagon POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${SFML_SOURCE_DIR}/extlibs/libs-osx/Frameworks $<TARGET_BUNDLE_DIR:SuperHaxagon>/Contents/Frameworks
)

add_custom_command(TARGET SuperHaxagon POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/romfs $<TARGET_BUNDLE_DIR:SuperHaxagon>/Contents/Resources
)

install(TARGETS SuperHaxagon BUNDLE DESTINATION .)


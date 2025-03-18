# cmake -DBUILD_BUNDLE=FALSE

# Want to build a fancy .app that has an icon? Set -DBUILD_BUNDLE=TRUE

message(STATUS "Building for macOS...")

set(PLATFORM_NAME "MACOS")

if (NOT BUILD_BUNDLE)
    # We will use the Framework for OpenAL distributed by sfml's Git repo when building the bundle
    find_package(OpenAL PATHS /opt/homebrew/opt/openal-soft/lib/cmake REQUIRED CONFIG)
endif()

set(DRIVER_PLATFORM
    source/Driver/macOS/PlatformMacOS.cpp
    source/Driver/Common/PlatformIsFullyRandom.cpp
)

include(cmake/functions/macos.cmake)

include(cmake/common/sfml.cmake)

if (BUILD_BUNDLE)
    set(MACOSX_BUNDLE_BUNDLE_NAME ${APP_NAME})  # Short name
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "net.awalter.${APP_NAME}")
    set(MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
    set(MACOSX_BUNDLE_LONG_VERSION_STRING ${MACOSX_BUNDLE_BUNDLE_VERSION})
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${MACOSX_BUNDLE_BUNDLE_VERSION})

    generate_icns(SuperHaxagon
        ICON "${CMAKE_CURRENT_SOURCE_DIR}/media/macos/SuperHaxagon.png"
    )

    set_target_properties(SuperHaxagon PROPERTIES
        MACOSX_BUNDLE TRUE
        INSTALL_RPATH "@executable_path/../Frameworks"
        BUILD_WITH_INSTALL_RPATH TRUE
    )

    # Copy all frameworks into the app bundle
    # Note: use `rsync` here to preserve symlinks, or you will break the Frameworks.
    add_custom_command(TARGET SuperHaxagon POST_BUILD
        COMMAND rsync -a ${SFML_SOURCE_DIR}/extlibs/libs-osx/Frameworks/ $<TARGET_BUNDLE_DIR:SuperHaxagon>/Contents/Frameworks/
    )

    add_custom_command(TARGET SuperHaxagon POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/romfs $<TARGET_BUNDLE_DIR:SuperHaxagon>/Contents/Resources
    )

    install(TARGETS SuperHaxagon BUNDLE DESTINATION .)
else()
    install(FILES ${CMAKE_SOURCE_DIR}/media/macos/SuperHaxagon-README.md DESTINATION .)
endif()

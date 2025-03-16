# cmake

message(STATUS "Building for macOS...")

set(PLATFORM_NAME "MACOS")

set(DRIVER_PLATFORM source/Driver/macOS/PlatformMacOS.cpp source/Driver/Common/PlatformIsFullyRandom.cpp)

# Prevent default install rules in release
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(BUILD_BUNDLE TRUE)
endif()

include(cmake/common/sfml.cmake)

# Only make the bundle in Release mode
if (BUILD_BUNDLE)
    find_program(SIPS_EXE sips)
    find_program(ICONUTIL_EXE iconutil)

    if (SIPS_EXE AND ICONUTIL_EXE)
        message(STATUS "Building macOS icons...")

        set(ICNS_OUT "${CMAKE_CURRENT_BINARY_DIR}/SuperHaxagon.icns")
        set(ICONSET_DIR "${CMAKE_CURRENT_BINARY_DIR}/SuperHaxagon.iconset")

        foreach(size IN ITEMS 16 32 64 128 256 512)
            set(ICON_IN "${CMAKE_CURRENT_SOURCE_DIR}/media/sources/icon-full.png")
            set(ICON_OUT "${ICONSET_DIR}/icon_${size}x${size}.png")
            set(ICON_OUT2X "${ICONSET_DIR}/icon_${size}x${size}@2x.png")

            math(EXPR size2x "${size} * 2")

            add_custom_command(
                OUTPUT "${ICON_OUT}"
                COMMAND "${SIPS_EXE}" -z "${size}" "${size}" ${ICON_IN} --out "${ICON_OUT}"
                DEPENDS "${ICON_IN}"
                COMMENT "Creating icon ${size}x${size}"
                VERBATIM
            )

            add_custom_command(
                OUTPUT "${ICON_OUT2X}"
                COMMAND "${SIPS_EXE}" -z "${size2x}" "${size2x}" ${ICON_IN} --out "${ICON_OUT2X}"
                DEPENDS "${ICON_IN}"
                COMMENT "Creating icon ${size}x${size}@2x"
                VERBATIM
            )

            list(APPEND ICONS ${ICON_OUT})
            list(APPEND ICONS ${ICON_OUT2X})
        endforeach ()

        add_custom_command(
                OUTPUT "${ICNS_OUT}"
                COMMAND "${ICONUTIL_EXE}" -c icns -o "${ICNS_OUT}" "${ICONSET_DIR}"
                DEPENDS "${ICONS}"
                COMMENT "Creating icns"
                VERBATIM
        )

        target_add_dependencies(SuperHaxagon "${ICNS_OUT}")
        set(MACOSX_BUNDLE_ICON_FILE "${ICNS_OUT}")
    endif()

    set(MACOSX_BUNDLE_BUNDLE_NAME ${APP_NAME})  # Short name
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "net.awalter.${APP_NAME}")
    set(MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
    set(MACOSX_BUNDLE_LONG_VERSION_STRING ${MACOSX_BUNDLE_BUNDLE_VERSION})
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${MACOSX_BUNDLE_BUNDLE_VERSION})

    set_target_properties(SuperHaxagon PROPERTIES
        MACOSX_BUNDLE TRUE
        INSTALL_RPATH "@executable_path/../Frameworks"
        BUILD_WITH_INSTALL_RPATH TRUE
    )

    add_custom_command(TARGET SuperHaxagon POST_BUILD
        COMMAND rsync -a ${SFML_SOURCE_DIR}/extlibs/libs-osx/Frameworks/ $<TARGET_BUNDLE_DIR:SuperHaxagon>/Contents/Frameworks/
    )

    install(TARGETS SuperHaxagon BUNDLE DESTINATION ./Applications)
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/romfs BUNDLE DESTINATION ./Applications)
endif()

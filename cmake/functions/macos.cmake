find_program(SIPS_EXE sips)
find_program(ICONUTIL_EXE iconutil)

function(generate_icns target)
    cmake_parse_arguments(PARSE_ARGV 0 ICONUTIL_ICNS "" "ICON" "")

    if (NOT SIPS_EXE OR NOT ICONUTIL_EXE)
        message(WARNING "Could not find tools for making icons!")
        return()
    endif()

    if (NOT DEFINED ICONUTIL_ICNS_ICON)
        message(FATAL_ERROR "generate_icns: need to define location .png icon file")
    endif()

    set(outtarget "${target}_icns")

    set(ICONSET_DIR "${CMAKE_CURRENT_BINARY_DIR}/${target}.iconset")
    set(ICNS_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${target}.icns")

    file(MAKE_DIRECTORY ${ICONSET_DIR})

    foreach(size IN ITEMS 16 32 64 128 256 512)
        set(ICON_OUT "${ICONSET_DIR}/icon_${size}x${size}.png")
        set(ICON_OUT2X "${ICONSET_DIR}/icon_${size}x${size}@2x.png")

        math(EXPR size2x "${size} * 2")

        add_custom_command(
            OUTPUT "${ICON_OUT}"
            COMMAND "${SIPS_EXE}" -z "${size}" "${size}" ${ICONUTIL_ICNS_ICON} --out "${ICON_OUT}"
            DEPENDS "${ICONUTIL_ICNS_ICON}"
            COMMENT "Creating icon ${size}x${size}"
            VERBATIM
        )

        add_custom_command(
            OUTPUT "${ICON_OUT2X}"
            COMMAND "${SIPS_EXE}" -z "${size2x}" "${size2x}" ${ICONUTIL_ICNS_ICON} --out "${ICON_OUT2X}"
            DEPENDS "${ICONUTIL_ICNS_ICON}"
            COMMENT "Creating icon ${size}x${size}@2x"
            VERBATIM
        )

        list(APPEND ICONS ${ICON_OUT})
        list(APPEND ICONS ${ICON_OUT2X})
    endforeach ()

    add_custom_command(
            OUTPUT "${ICNS_OUTPUT}"
            COMMAND "${ICONUTIL_EXE}" -c icns -o "${ICNS_OUTPUT}" "${ICONSET_DIR}"
            DEPENDS "${ICONS}"
            COMMENT "Creating .icns"
            VERBATIM
    )

    add_custom_target(${outtarget} DEPENDS "${ICNS_OUTPUT}")

    add_dependencies(${target} ${outtarget})

    set(MACOSX_BUNDLE_ICON_FILE "${ICNS_OUTPUT}" PARENT_SCOPE)
endfunction()
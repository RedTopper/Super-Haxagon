# These 3DS functions follow a similar structure to those defined in Nintendo3DS.cmake,
# which are part of the devkitPro repo here:
# https://github.com/devkitPro/pacman-packages/blob/master/cmake/3ds/Nintendo3DS.cmake

find_program(BANNERTOOL_EXE NAMES bannertool HINTS "${DEVKITPRO}/tools/bin")
find_program(MAKEROM_EXE NAMES makerom HINTS "${DEVKITPRO}/tools/bin")

function(generate_bnr)
    cmake_parse_arguments(PARSE_ARGV 0 BANNERTOOL_BNR "" "OUTPUT;AUDIO;CGFX" "")
    if (NOT BANNERTOOL_EXE)
        message(WARNING "Could not find bannertool for .bnr: try building bannertool\n"
            "https://github.com/carstene1ns/3ds-bannertool"
        )
        return()
    endif()

    if (NOT DEFINED BANNERTOOL_BNR_OUTPUT)
        if(DEFINED BANNERTOOL_BNR_UNPARSED_ARGUMENTS)
            list(GET BANNERTOOL_BNR_UNPARSED_ARGUMENTS 0 BANNERTOOL_BNR_OUTPUT)
        else()
            message(FATAL_ERROR "generate_bnr: missing OUTPUT argument")
        endif()
    endif()

    if (NOT DEFINED BANNERTOOL_BNR_AUDIO OR NOT DEFINED BANNERTOOL_BNR_CGFX)
        message(FATAL_ERROR "generate_bnr: need to define location of .wav and .cgfx")
    endif()

    list(APPEND BANNERTOOL_BNR_ARGS "makebanner")
    list(APPEND BANNERTOOL_BNR_ARGS "-ci" "${BANNERTOOL_BNR_CGFX}")
    list(APPEND BANNERTOOL_BNR_ARGS "-a" "${BANNERTOOL_BNR_AUDIO}")
    list(APPEND BANNERTOOL_BNR_ARGS "-o" "${BANNERTOOL_BNR_OUTPUT}")

    get_filename_component(BANNERTOOL_BNR_OUTPUT "${BANNERTOOL_BNR_OUTPUT}" ABSOLUTE BASE_DIR "${CMAKE_CURRENT_BINARY_DIR}")
    add_custom_command(
            OUTPUT "${BANNERTOOL_BNR_OUTPUT}"
            COMMAND "${BANNERTOOL_EXE}" ${BANNERTOOL_BNR_ARGS}
            DEPENDS "${BANNERTOOL_BNR_CGFX}" "${BANNERTOOL_BNR_AUDIO}"
            VERBATIM
    )
endfunction()

function(generate_icn)
    cmake_parse_arguments(PARSE_ARGV 0 BANNERTOOL_ICN "" "OUTPUT;NAME;DESCRIPTION;AUTHOR;ICON" "")
    if (NOT BANNERTOOL_EXE)
        message(WARNING "Could not find bannertool for .icn: try building bannertool\n"
            "https://github.com/carstene1ns/3ds-bannertool"
        )
        return()
    endif()

    if (NOT DEFINED BANNERTOOL_ICN_OUTPUT)
        if(DEFINED BANNERTOOL_ICN_UNPARSED_ARGUMENTS)
            list(GET BANNERTOOL_ICN_UNPARSED_ARGUMENTS 0 BANNERTOOL_ICN_OUTPUT)
        else()
            message(FATAL_ERROR "generate_icn: missing OUTPUT argument")
        endif()
    endif()

    if (NOT DEFINED BANNERTOOL_ICN_ICON)
        message(FATAL_ERROR "generate_icn: need to define location .png icon file")
    endif()
    if (NOT DEFINED BANNERTOOL_ICN_NAME)
        set(BANNERTOOL_ICN_NAME "${CMAKE_PROJECT_NAME}")
    endif()
    if (NOT DEFINED BANNERTOOL_ICN_DESCRIPTION)
        set(BANNERTOOL_ICN_DESCRIPTION "Built with devkitARM & libctru")
    endif()
    if (NOT DEFINED BANNERTOOL_ICN_AUTHOR)
        set(BANNERTOOL_ICN_AUTHOR "Unspecified Author")
    endif()

    list(APPEND BANNERTOOL_ICN_ARGS "makesmdh")
    list(APPEND BANNERTOOL_ICN_ARGS "-s" "${BANNERTOOL_ICN_NAME}")
    list(APPEND BANNERTOOL_ICN_ARGS "-l" "${BANNERTOOL_ICN_NAME} - ${BANNERTOOL_ICN_DESCRIPTION}")
    list(APPEND BANNERTOOL_ICN_ARGS "-p" "${BANNERTOOL_ICN_AUTHOR}")
    list(APPEND BANNERTOOL_ICN_ARGS "-i" "${BANNERTOOL_ICN_ICON}")

    list(APPEND BANNERTOOL_ICN_ARGS
        "--flags" "visible,ratingrequired,recordusage"
        "--cero" "153"
        "--esrb" "153"
        "--usk" "153"
        "--pegigen" "153"
        "--pegiptr" "153"
        "--pegibbfc" "153"
        "--cob" "153"
        "--grb" "153"
        "--cgsrr" "153"
    )

    list(APPEND BANNERTOOL_ICN_ARGS "-o" "${BANNERTOOL_ICN_OUTPUT}")

    get_filename_component(BANNERTOOL_ICN_OUTPUT "${BANNERTOOL_ICN_OUTPUT}" ABSOLUTE BASE_DIR "${CMAKE_CURRENT_BINARY_DIR}")
    add_custom_command(
            OUTPUT "${BANNERTOOL_ICN_OUTPUT}"
            COMMAND "${BANNERTOOL_EXE}" ${BANNERTOOL_ICN_ARGS}
            DEPENDS "${BANNERTOOL_BNR_AUDIO}"
            VERBATIM
    )
endfunction()

function(create_rom target)
    cmake_parse_arguments(PARSE_ARGV 1 MAKEROM "CIA;3DS" "ICN;BNR;RSF;MAJOR;MINOR;MICRO;TITLE;PRODUCT_CODE;UNIQUE_ID;ROMFS;LOGO" "")
    if (NOT MAKEROM_EXE)
        message(WARNING "Could not find makerom: try building makerom\n"
            "https://github.com/3DSGuy/Project_CTR"
        )
        return()
    endif()

    if (MAKEROM_CIA AND MAKEROM_3DS)
        message(FATAL_ERROR "create_rom: both cia and 3ds should not be set in the same call ${MAKEROM_CIA}")
    elseif(NOT MAKEROM_CIA AND NOT MAKEROM_3DS)
        message(FATAL_ERROR "create_rom: must specify either cia or 3ds to build")
    elseif(MAKEROM_CIA)
        set(outtarget "${target}_cia")
        set(MAKEROM_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${target}.cia")
        list(APPEND MAKEROM_ARGS "-f" "cia")
        list(APPEND MAKEROM_ARGS "-DAPP_ENCRYPTED=false")
    elseif(MAKEROM_3DS)
        set(outtarget "${target}_3ds")
        set(MAKEROM_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${target}.3ds")
        list(APPEND MAKEROM_ARGS "-f" "cci")
        list(APPEND MAKEROM_ARGS "-DAPP_ENCRYPTED=true")
    endif()


    if(DEFINED MAKEROM_ICN)
        get_filename_component(MAKEROM_ICN "${MAKEROM_ICN}" ABSOLUTE BASE_DIR "${CMAKE_CURRENT_BINARY_DIR}")
    else()
        set(MAKEROM_ICN "${CMAKE_CURRENT_BINARY_DIR}/${target}.icn")
    endif()
    if(DEFINED MAKEROM_BNR)
        get_filename_component(MAKEROM_BNR "${MAKEROM_BNR}" ABSOLUTE BASE_DIR "${CMAKE_CURRENT_BINARY_DIR}")
    else()
        set(MAKEROM_BNR "${CMAKE_CURRENT_BINARY_DIR}/${target}.bnr")
    endif()
    if (NOT DEFINED MAKEROM_RSF)
        message(FATAL_ERROR "create_rom: need to define location .rsf config file")
    endif()
    if (NOT DEFINED MAKEROM_LOGO)
        message(FATAL_ERROR "create_rom: need to define location .bcma.lz splash logo file")
    endif()
    if (NOT DEFINED MAKEROM_MAJOR)
        set(MAKEROM_MAJOR "${CMAKE_PROJECT_VERSION_MAJOR}")
    endif()
    if (NOT DEFINED MAKEROM_MINOR)
        set(MAKEROM_MINOR "${CMAKE_PROJECT_VERSION_MINOR}")
    endif()
    if (NOT DEFINED MAKEROM_MICRO)
        set(MAKEROM_MICRO "${CMAKE_PROJECT_VERSION_PATCH}")
    endif()
    if (NOT DEFINED MAKEROM_TITLE)
        set(MAKEROM_TITLE "${CMAKE_PROJECT_NAME}")
    endif()
    if (NOT DEFINED MAKEROM_PRODUCT_CODE)
        message(FATAL_ERROR "create_rom: A globally unique product code (CTR-P-XXXX) must be defined")
    endif()
    if (NOT DEFINED MAKEROM_UNIQUE_ID)
        message(FATAL_ERROR "create_rom: A globally unique UID (0xXXXXX) must be defined")
    endif()

    list(APPEND MAKEROM_ARGS "-o" "${MAKEROM_OUTPUT}")

    list(APPEND MAKEROM_ARGS "-elf" "${CMAKE_CURRENT_BINARY_DIR}/${target}.elf")
    list(APPEND MAKEROM_ARGS "-rsf" "${MAKEROM_RSF}")
    list(APPEND MAKEROM_ARGS "-target" "t" "-exefslogo")
    list(APPEND MAKEROM_ARGS "-icon" "${MAKEROM_ICN}")
    list(APPEND MAKEROM_ARGS "-banner" "${MAKEROM_BNR}")
    list(APPEND MAKEROM_ARGS "-logo" "${MAKEROM_LOGO}")
    list(APPEND MAKEROM_ARGS "-major" "${MAKEROM_MAJOR}")
    list(APPEND MAKEROM_ARGS "-minor" "${MAKEROM_MINOR}")
    list(APPEND MAKEROM_ARGS "-micro" "${MAKEROM_MICRO}")
    list(APPEND MAKEROM_ARGS "-DAPP_TITLE=${MAKEROM_TITLE}")
    list(APPEND MAKEROM_ARGS "-DAPP_PRODUCT_CODE=${MAKEROM_PRODUCT_CODE}")
    list(APPEND MAKEROM_ARGS "-DAPP_UNIQUE_ID=${MAKEROM_UNIQUE_ID}")
    list(APPEND MAKEROM_ARGS "-DAPP_VERSION_MAJOR=${MAKEROM_MAJOR}")

    list(APPEND MAKEROM_ARGS
        "-DAPP_SYSTEM_MODE=64MB"
        "-DAPP_SYSTEM_MODE_EXT=Legacy"
        "-DAPP_CATEGORY=Application"
        "-DAPP_USE_ON_SD=true"
        "-DAPP_MEMORY_TYPE=Application"
        "-DAPP_CPU_SPEED=268MHz"
        "-DAPP_ENABLE_L2_CACHE=false"
    )

    if (DEFINED MAKEROM_ROMFS)
        list(APPEND MAKEROM_ARGS "-DAPP_ROMFS=${MAKEROM_ROMFS}")
    endif()

    add_custom_command(
            OUTPUT "${MAKEROM_OUTPUT}"
            COMMAND "${MAKEROM_EXE}" ${MAKEROM_ARGS}
            DEPENDS "${target}" "${MAKEROM_RSF}" "${MAKEROM_ICN}" "${MAKEROM_BNR}" "${MAKEROM_LOGO}"
            COMMENT "Building ROM for target ${outtarget}"
            VERBATIM
    )

    add_custom_target(${outtarget} ALL DEPENDS "${MAKEROM_OUTPUT}")
endfunction()

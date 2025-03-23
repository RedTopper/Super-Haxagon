# SPDX-FileCopyrightText: 2025 AJ Walter
# SPDX-License-Identifier: GPL-3.0-or-later OR MIT

set(CMAKE_BUILD_TYPE_INIT "Release")
set(CMAKE_EXECUTABLE_SUFFIX .elf)
set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "Shared libs not available")
set_property(GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS FALSE)

set(NSPIRE_ARCH_SETTINGS "-mcpu=arm926ej-s -marm")
set(NSPIRE_COMMON_FLAGS
        "-D_nspire -DOLD_SCREEN_API -O2 -Wall -W -Wno-narrowing \
        -ffast-math -fno-math-errno -fomit-frame-pointer -flto -fgcse-sm -fgcse-las \
        -funsafe-loop-optimizations -fno-fat-lto-objects -frename-registers -fprefetch-loop-arrays"
)
set(NSPIRE_LINKER_FLAGS  "-Wl,--gc-sections")
set(NSPIRE_STANDARD_LIBRARIES "-lnspireio -lm -L${NDLESS}/ndless-sdk/lib ")
set(NSPIRE_STANDARD_INCLUDE_DIRECTORIES "${NDLESS}/ndless-sdk/include")

foreach(lang IN ITEMS C CXX ASM)
    set(CMAKE_${lang}_FLAGS_INIT "${NSPIRE_ARCH_SETTINGS} ${NSPIRE_COMMON_FLAGS}")
    set(CMAKE_${lang}_STANDARD_LIBRARIES "${NSPIRE_STANDARD_LIBRARIES}" CACHE STRING "" FORCE)
    set(CMAKE_${lang}_STANDARD_INCLUDE_DIRECTORIES "${NSPIRE_STANDARD_INCLUDE_DIRECTORIES}" CACHE STRING "")
endforeach()

set(CMAKE_EXE_LINKER_FLAGS_INIT "${NSPIRE_ARCH_SETTINGS} ${NSPIRE_LINKER_FLAGS}")

# Functions for the TI-Nspire

function(build_memoryfs resources)
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/romfs/bgm")
    foreach(RESOURCE_FILE ${resources})
        string(REGEX REPLACE "[./]" "_" RESOURCE_NAME ${RESOURCE_FILE})
        get_filename_component(OUTPUT_FILE "${RESOURCE_FILE}.cc" ABSOLUTE BASE_DIR "${CMAKE_CURRENT_BINARY_DIR}")
        get_filename_component(INPUT_FILE "${RESOURCE_FILE}" ABSOLUTE BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
        add_custom_command(
                OUTPUT ${OUTPUT_FILE}
                COMMAND xxd -i -n ${RESOURCE_NAME} ${INPUT_FILE} ${OUTPUT_FILE}
                COMMENT "Compiling ${RESOURCE_FILE} to binary")
        list(APPEND COMPILED_RESOURCES ${OUTPUT_FILE})
    endforeach()
    set(COMPILED_RESOURCES "${COMPILED_RESOURCES}" PARENT_SCOPE)
endfunction()

function(create_tns target)
    cmake_parse_arguments(PARSE_ARGV 1 TNS "" "NAME;VERSION;AUTHOR" "")

    set(outtarget "${target}_tns")

    if (NOT GENZEHN_EXE)
        message(FATAL_ERROR "Could not find genzehn: try building ndless-sdk")
    endif()

    if (NOT DEFINED TNS_NAME)
        set(TNS_NAME "${CMAKE_PROJECT_NAME}")
    endif()
    if (NOT DEFINED TNS_VERSION)
        set(TNS_VERSION "${CMAKE_PROJECT_VERSION_MAJOR}${CMAKE_PROJECT_VERSION_MINOR}${CMAKE_PROJECT_VERSION_PATCH}")
    endif()
    if (NOT DEFINED TNS_AUTHOR)
        set(TNS_AUTHOR "Unspecified Author")
    endif()

    set(TNS_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${target}.tns")

    list(APPEND GENZEHN_ARGS "--input" "${CMAKE_CURRENT_BINARY_DIR}/${target}.elf")
    list(APPEND GENZEHN_ARGS "--output" "${TNS_OUTPUT}")
    list(APPEND GENZEHN_ARGS "--name" "${TNS_NAME}")
    list(APPEND GENZEHN_ARGS "--version" "${TNS_VERSION}")
    list(APPEND GENZEHN_ARGS "--author" "${TNS_AUTHOR}")
    list(APPEND GENZEHN_ARGS "--240x320-support" "true")
    list(APPEND GENZEHN_ARGS "--color-support" "true")

    add_custom_command(
            OUTPUT "${TNS_OUTPUT}"
            COMMAND "${GENZEHN_EXE}" ${GENZEHN_ARGS}
            DEPENDS "${target}"
            COMMENT "Building .tns for target ${outtarget}"
            VERBATIM
    )

    add_custom_target(${outtarget} ALL DEPENDS "${TNS_OUTPUT}")
endfunction()

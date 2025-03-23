# SPDX-FileCopyrightText: 2025 AJ Walter
# SPDX-License-Identifier: GPL-3.0-or-later OR MIT

set(CMAKE_SYSTEM_NAME Nspire)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_SYSTEM_PROCESSOR arm)

if(NOT DEFINED ENV{NDLESS})
    set(NDLESS "/opt/ndless")
else()
    set(NDLESS "$ENV{NDLESS}")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")

set(TOOL_HINT ${NDLESS}/ndless-sdk/bin)
find_program(CMAKE_ASM_COMPILER nspire-gcc HINTS ${TOOL_HINT})
find_program(CMAKE_C_COMPILER   nspire-gcc HINTS ${TOOL_HINT})
find_program(CMAKE_CXX_COMPILER nspire-g++ HINTS ${TOOL_HINT})
find_program(CMAKE_LINKER       nspire-ld  HINTS ${TOOL_HINT})

find_program(GENZEHN_EXE        genzehn    HINTS ${TOOL_HINT})

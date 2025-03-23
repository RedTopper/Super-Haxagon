# SPDX-FileCopyrightText: 2025 AJ Walter
# SPDX-License-Identifier: GPL-3.0-or-later OR MIT

set(CMAKE_SYSTEM_NAME MiyooMini)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_SYSTEM_PROCESSOR arm)

if(NOT DEFINED ENV{MMIYOO})
    set(MMIYOO "/opt/miyoomini-toolchain")
else()
    set(MMIYOO "$ENV{MMIYOO}")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")

set(TRIPLE arm-linux-gnueabihf)

set(TOOL_HINT ${MMIYOO}/bin)
find_program(CMAKE_ASM_COMPILER ${TRIPLE}-as  HINTS ${TOOL_HINT})
find_program(CMAKE_C_COMPILER   ${TRIPLE}-gcc HINTS ${TOOL_HINT})
find_program(CMAKE_CXX_COMPILER ${TRIPLE}-g++ HINTS ${TOOL_HINT})
find_program(CMAKE_LINKER       ${TRIPLE}-ld  HINTS ${TOOL_HINT})

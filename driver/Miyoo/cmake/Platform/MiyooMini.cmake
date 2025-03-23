# SPDX-FileCopyrightText: 2025 AJ Walter
# SPDX-License-Identifier: GPL-3.0-or-later OR MIT

set(CMAKE_BUILD_TYPE_INIT "Release")

set(MIYOO_ARCH_SETTINGS "-marm -mtune=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard -march=armv7ve")
set(MIYOO_COMMON_FLAGS "-O3 -fPIC")
set(MIYOO_LINKER_FLAGS  "-L/opt/miyoomini-toolchain/lib")
set(MIYOO_STANDARD_LIBRARIES "-lmi_common -lmi_sys -lmi_ao -lmi_gfx -lpthread -lshmvar -ldl")
set(MIYOO_STANDARD_INCLUDE_DIRECTORIES "${MMIYOO}/usr/include")

foreach(lang IN ITEMS C CXX ASM)
    set(CMAKE_${lang}_FLAGS_INIT "${MIYOO_ARCH_SETTINGS} ${MIYOO_COMMON_FLAGS}")
    set(CMAKE_${lang}_STANDARD_LIBRARIES "${MIYOO_STANDARD_LIBRARIES}" CACHE STRING "" FORCE)
    set(CMAKE_${lang}_STANDARD_INCLUDE_DIRECTORIES "${MIYOO_STANDARD_INCLUDE_DIRECTORIES}" CACHE STRING "")
endforeach()

set(CMAKE_EXE_LINKER_FLAGS_INIT "${MIYOO_ARCH_SETTINGS} ${MIYOO_LINKER_FLAGS}")

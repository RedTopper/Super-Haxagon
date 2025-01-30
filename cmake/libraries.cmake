message(STATUS "Including extra headers for your IDE...")

# 3DS
# https://github.com/devkitPro/libctru
include_directories(SYSTEM "${CMAKE_CURRENT_SOURCE_DIR}/libraries/libctru/libctru/include")
# https://github.com/devkitPro/citro2d
include_directories(SYSTEM "${CMAKE_CURRENT_SOURCE_DIR}/libraries/citro2d/include")
# https://github.com/devkitPro/citro3d
include_directories(SYSTEM "${CMAKE_CURRENT_SOURCE_DIR}/libraries/citro3d/include")

# Switch
# https://github.com/switchbrew/libnx
include_directories(SYSTEM "${CMAKE_CURRENT_SOURCE_DIR}/libraries/libnx/nx/include")
# https://github.com/devkitPro/switch-glad
include_directories(SYSTEM "${CMAKE_CURRENT_SOURCE_DIR}/libraries/switch-glad/include")
# Freetype2, SDL2, and SDL2_mixer can be installed from your distro (they are portlibs)
include_directories(SYSTEM "/usr/include/freetype2")

# TI-Nspire
# https://github.com/ndless-nspire/Ndless
include_directories(SYSTEM "${CMAKE_CURRENT_SOURCE_DIR}/libraries/Ndless/ndless-sdk/include")

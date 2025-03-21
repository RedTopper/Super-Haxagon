# SPDX-FileCopyrightText: 2025 AJ Walter, see driver/Common/README.md for MIT usage.
# SPDX-License-Identifier: GPL-3.0-or-later OR MIT

message(STATUS "Including extra headers for your IDE...")

include(FetchContent)

# 3DS
message(STATUS "Fetching 3DS libctru...")
FetchContent_Declare(libctru
    GIT_REPOSITORY https://github.com/devkitPro/libctru.git
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    SOURCE_SUBDIR libctru/include)
FetchContent_MakeAvailable(libctru)
target_include_directories(SuperHaxagon SYSTEM PUBLIC "${libctru_SOURCE_DIR}/libctru/include")

message(STATUS "Fetching 3DS citro2d...")
FetchContent_Declare(citro2d
    GIT_REPOSITORY https://github.com/devkitPro/citro2d.git
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    SOURCE_SUBDIR include)
FetchContent_MakeAvailable(citro2d)
target_include_directories(SuperHaxagon SYSTEM PUBLIC "${citro2d_SOURCE_DIR}/include")

message(STATUS "Fetching 3DS citro3d...")
FetchContent_Declare(citro3d
    GIT_REPOSITORY https://github.com/devkitPro/citro3d.git
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    SOURCE_SUBDIR include)
FetchContent_MakeAvailable(citro3d)
target_include_directories(SuperHaxagon SYSTEM PUBLIC "${citro3d_SOURCE_DIR}/include")

message(STATUS "Fetching 3DS stb...")
FetchContent_Declare(stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG ${STB_VER}
    GIT_PROGRESS TRUE
    SOURCE_SUBDIR nonexistent)
FetchContent_MakeAvailable(stb)
target_include_directories(SuperHaxagon SYSTEM PUBLIC "${stb_SOURCE_DIR}")

# Switch
# Headers for SDL2 and SDL2_mixer can be installed from your distro (they are portlibs)
message(STATUS "Fetching Switch libnx...")
FetchContent_Declare(libnx
    GIT_REPOSITORY https://github.com/switchbrew/libnx.git
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    SOURCE_SUBDIR nx/include)
FetchContent_MakeAvailable(libnx)
target_include_directories(SuperHaxagon SYSTEM PUBLIC "${libnx_SOURCE_DIR}/nx/include")

message(STATUS "Fetching Switch switch_glad...")
FetchContent_Declare(switch_glad
    GIT_REPOSITORY https://github.com/devkitPro/switch-glad.git
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    SOURCE_SUBDIR include)
FetchContent_MakeAvailable(switch_glad)
target_include_directories(SuperHaxagon SYSTEM PUBLIC "${switch_glad_SOURCE_DIR}/include")

message(STATUS "Fetching Switch freetype2...")
FetchContent_Declare(freetype2
    GIT_REPOSITORY https://github.com/freetype/freetype.git
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    GIT_TAG VER-2-13-2
    GIT_SUBMODULES include
    SOURCE_SUBDIR include)
FetchContent_MakeAvailable(freetype2)
target_include_directories(SuperHaxagon SYSTEM PUBLIC "${freetype2_SOURCE_DIR}/include")

# TI-Nspire
message(STATUS "Fetching Nspire ndless_sdk...")
FetchContent_Declare(ndless_sdk
    GIT_REPOSITORY https://github.com/ndless-nspire/Ndless.git
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    GIT_SUBMODULES ndless-sdk/include
    SOURCE_SUBDIR ndless-sdk/include)
FetchContent_MakeAvailable(ndless_sdk)
target_include_directories(SuperHaxagon SYSTEM PUBLIC "${ndless_sdk_SOURCE_DIR}/ndless-sdk/include")

message(STATUS "Including extra headers for your IDE...")

include(FetchContent)

FetchContent_Declare(libctru
    GIT_REPOSITORY https://github.com/devkitPro/libctru.git
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    SOURCE_SUBDIR libctru/include)

FetchContent_Declare(citro2d
    GIT_REPOSITORY https://github.com/devkitPro/citro2d.git
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    SOURCE_SUBDIR include)

FetchContent_Declare(citro3d
    GIT_REPOSITORY https://github.com/devkitPro/citro3d.git
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    SOURCE_SUBDIR include)

FetchContent_Declare(stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG ${STB_VER}
    GIT_PROGRESS TRUE
    SOURCE_SUBDIR nonexistent)

FetchContent_Declare(libnx
    GIT_REPOSITORY https://github.com/switchbrew/libnx.git
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    SOURCE_SUBDIR nx/include)

FetchContent_Declare(switch_glad
    GIT_REPOSITORY https://github.com/devkitPro/switch-glad.git
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    SOURCE_SUBDIR include)

FetchContent_Declare(freetype2
    GIT_REPOSITORY https://github.com/freetype/freetype.git
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    GIT_TAG VER-2-13-2
    GIT_SUBMODULES include
    SOURCE_SUBDIR include)

FetchContent_Declare(ndless_sdk
    GIT_REPOSITORY https://github.com/ndless-nspire/Ndless.git
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    GIT_SUBMODULES ndless-sdk/include
    SOURCE_SUBDIR ndless-sdk/include)

FetchContent_MakeAvailable(libctru citro2d citro3d stb libnx switch_glad freetype2 ndless_sdk)

# 3DS
target_include_directories(SuperHaxagon SYSTEM PUBLIC "${libctru_SOURCE_DIR}/libctru/include")
target_include_directories(SuperHaxagon SYSTEM PUBLIC "${citro2d_SOURCE_DIR}/include")
target_include_directories(SuperHaxagon SYSTEM PUBLIC "${citro3d_SOURCE_DIR}/include")
target_include_directories(SuperHaxagon SYSTEM PUBLIC "${stb_SOURCE_DIR}")

# Switch
# Headers for SDL2 and SDL2_mixer can be installed from your distro (they are portlibs)
target_include_directories(SuperHaxagon SYSTEM PUBLIC "${libnx_SOURCE_DIR}/nx/include")
target_include_directories(SuperHaxagon SYSTEM PUBLIC "${switch_glad_SOURCE_DIR}/include")
target_include_directories(SuperHaxagon SYSTEM PUBLIC "${freetype2_SOURCE_DIR}/include")

# TI-Nspire
target_include_directories(SuperHaxagon SYSTEM PUBLIC "${ndless_sdk_SOURCE_DIR}/ndless-sdk/include")

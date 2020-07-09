NAME := Super Haxagon

BUILD_DIR := build
OUTPUT_DIR := output
INCLUDE_DIRS := include
SOURCE_DIRS := source/Core source/Factories source/States

VERSION_PARTS := $(subst ., ,$(shell git describe --tags --abbrev=0))
VERSION_MAJOR := $(word 1, $(VERSION_PARTS))
VERSION_MINOR := $(word 2, $(VERSION_PARTS))
VERSION_MICRO := $(word 3, $(VERSION_PARTS))

AUTHOR := RedHat
DESCRIPTION := A Super Hexagon Clone

BUILD_FLAGS_CXX := -std=gnu++14

ifndef TARGET
$(error State target platform with one of 'make TARGET:=[3DS SWITCH WIN64]')
endif

# 3DS CONFIGURATION #

ifeq ($(TARGET),3DS)
    SOURCE_DIRS += source/Driver3DS

    # Shitty workaround for library includes not being set up right
    INCLUDE_DIRS += libraries/stb
    LIBRARY_DIRS += $(DEVKITPRO)/libctru $(DEVKITPRO)/portlibs/3ds/

    $(info $$LIBRARY_DIRS is [${LIBRARY_DIRS}])
    LIBRARIES += citro2d citro3d ctru m opusfile ogg opus

    PRODUCT_CODE := CTR-P-HAXA
    UNIQUE_ID := 0x099AA
    ROMFS_DIR := romfs

    BANNER_AUDIO := resource/audio.wav
    BANNER_IMAGE := resource/banner.cgfx
    ICON := resource/icon-3ds.png
endif

# Switch CONFIGURATION #

ifeq ($(TARGET),SWITCH)
    SOURCE_DIRS += source/DriverSwitch

    LIBRARY_DIRS += $(DEVKITPRO)/libnx
    LIBRARIES += nx

    ICON := resource/icon-switch.jpg
endif

# Windows CONFIGURATION #

ifeq ($(TARGET),WIN64)
    SOURCE_DIRS += source/DriverWin

    LIBRARY_DIRS += ./libraries/SFML /mingw64 /mingw64/lib
    LIBRARIES += sfml-graphics-s sfml-window-s sfml-audio-s sfml-system-s winmm openal32 gdi32 opengl32 freetype flac vorbisenc vorbisfile vorbis ogg

    BUILD_FLAGS := -mwindows -DSFML_STATIC
endif

# INTERNAL #

include buildtools/make_base

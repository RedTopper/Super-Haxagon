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

# 3DS CONFIGURATION #

ifeq ($(TARGET),3DS)
    SOURCE_DIRS += source/Driver3DS

    # Shitty workaround for library includes not being set up right
    INCLUDE_DIRS += libraries/stb
    LIBRARY_DIRS += $(DEVKITPRO)/libctru $(DEVKITPRO)/portlibs/3ds/

    $(info $$LIBRARY_DIRS is [${LIBRARY_DIRS}])
    LIBRARIES += citro2d citro3d ctru m

    PRODUCT_CODE := CTR-P-HAXA
    UNIQUE_ID := 0x099AA
    ROMFS_DIR := romfs

    BANNER_AUDIO := media/audio.wav
    BANNER_IMAGE := media/banner.cgfx
    ICON := media/icon-3ds.png
    ICON_FLAGS := --flags visible,ratingrequired,recordusage --cero 153 --esrb 153 --usk 153 --pegigen 153 --pegiptr 153 --pegibbfc 153 --cob 153 --grb 153 --cgsrr 153
endif

# Switch CONFIGURATION #

ifeq ($(TARGET),SWITCH)
    SOURCE_DIRS += source/DriverSwitch

    LIBRARY_DIRS += $(DEVKITPRO)/libnx
    LIBRARIES += nx

    ICON := media/icon-switch.jpg
endif

# Windows CONFIGURATION #

ifeq ($(TARGET),WIN64)
    SOURCE_DIRS += source/DriverSFML

    LIBRARY_DIRS += ./libraries/MinGW/SFML /mingw64 /mingw64/lib
    LIBRARIES += sfml-graphics-s sfml-window-s sfml-audio-s sfml-system-s winmm openal32 gdi32 opengl32 freetype flac vorbisenc vorbisfile vorbis ogg

    BUILD_FLAGS := -mwindows -DSFML_STATIC
endif

# Linux CONFIGURATION #

ifeq ($(TARGET),LINUX64)
    SOURCE_DIRS += source/DriverSFML

    LIBRARIES += sfml-graphics sfml-window sfml-audio sfml-system
endif

# INTERNAL #

include buildtools/make_base

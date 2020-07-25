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

ROMFS_DIR := romfs

# 3DS CONFIGURATION #

ifeq ($(TARGET),3DS)
    SOURCE_DIRS += source/Driver/3DS

    # Shitty workaround for library includes not being set up right
    INCLUDE_DIRS += libraries/stb
    LIBRARY_DIRS += $(DEVKITPRO)/libctru $(DEVKITPRO)/portlibs/3ds/

    # As long as 
    BUILD_FLAGS += -O2 -Wno-psabi -ffast-math

    $(info $$LIBRARY_DIRS is [${LIBRARY_DIRS}])
    LIBRARIES += citro2d citro3d ctru m

    PRODUCT_CODE := CTR-P-HAXA
    UNIQUE_ID := 0x099AA

    BANNER_AUDIO := media/audio.wav
    BANNER_IMAGE := media/banner.cgfx
    ICON := media/icon-3ds.png
    ICON_FLAGS := --flags visible,ratingrequired,recordusage --cero 153 --esrb 153 --usk 153 --pegigen 153 --pegiptr 153 --pegibbfc 153 --cob 153 --grb 153 --cgsrr 153
endif

# Switch CONFIGURATION #

ifeq ($(TARGET),SWITCH)
    SOURCE_DIRS += source/Driver/Switch

    # pacman -S switch-bzip2 switch-glad switch-libdrm_nouveau switch-zlib switch-freetype
    # switch-libpng switch-mesa switch-libogg switch-libvorbisidec switch-libvorbis switch-flac libnx switch-sdl2
    # I'm sure there's more but just try to play around with pacman to get them all
    LIBRARY_DIRS += $(DEVKITPRO)/libnx $(DEVKITPRO)/portlibs/switch
    LIBRARIES += SDL2_mixer SDL2 egl glad glapi drm_nouveau freetype png16 bz2 z vorbisidec opusfile opus ogg mpg123 modplug nx

    BUILD_FLAGS += -ffunction-sections -O2 -ftls-model=local-exec -I$(DEVKITPRO)/portlibs/switch/include/FreeType2
    BUILD_FLAGS_CXX += -march=armv8-a+crc+crypto

    ICON := media/icon-switch.jpg --romfsdir=./$(ROMFS_DIR)
endif

# Windows CONFIGURATION #

ifeq ($(TARGET),WIN64)
    SOURCE_DIRS += source/Driver/SFML source/Driver/Win

    LIBRARY_DIRS += ./libraries/SFML-MinGW /mingw64 /mingw64/lib
    LIBRARIES += sfml-graphics-s sfml-window-s sfml-audio-s sfml-system-s winmm openal32 gdi32 opengl32 freetype flac vorbisenc vorbisfile vorbis ogg

    BUILD_FLAGS := -mwindows -DSFML_STATIC
endif

# Linux CONFIGURATION #

ifeq ($(TARGET),LINUX64)
    SOURCE_DIRS += source/Driver/SFML source/Driver/Linux

    LIBRARIES += sfml-graphics sfml-window sfml-audio sfml-system
endif

# INTERNAL #

include libraries/buildtools/make_base

TARGET := WIN64 3DS SWITCH

NAME := Super Haxagon

BUILD_DIR := build
OUTPUT_DIR := output
INCLUDE_DIRS := include
SOURCE_DIRS := source

VERSION_PARTS := $(subst ., ,$(shell git describe --tags --abbrev=0))
VERSION_MAJOR := $(word 1, $(VERSION_PARTS))
VERSION_MINOR := $(word 2, $(VERSION_PARTS))
VERSION_MICRO := $(word 3, $(VERSION_PARTS))

AUTHOR := RedHat
DESCRIPTION := A Super Hexagon Clone

# 3DS CONFIGURATION #

ifeq ($(TARGET),3DS)
    LIBRARY_DIRS += $(DEVKITPRO)/libctru
    $(info $$LIBRARY_DIRS is [${LIBRARY_DIRS}])
    LIBRARIES += ctru

    PRODUCT_CODE := CTR-P-HAXA
    UNIQUE_ID := 0x099AA

    BANNER_AUDIO := resource/audio.wav
    BANNER_IMAGE := resource/banner.cgfx
    ICON := resource/icon-3ds.png
endif

# Switch CONFIGURATION #

ifeq ($(TARGET),SWITCH)
    LIBRARY_DIRS += $(DEVKITPRO)/libnx
    LIBRARIES += nx

    ICON := resource/icon-switch.jpg
endif

# INTERNAL #

include buildtools/make_base

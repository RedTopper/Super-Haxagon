#!/bin/bash

set -e

BUILD_DIR="./build"
RELEASE_DIR="./build/release"
ROMFS_DIR="./romfs"

rm -rf "$RELEASE_DIR"
mkdir -p "$RELEASE_DIR"

if [[ -d "$BUILD_DIR/windows" ]]; then
    WIN_RELEASE="$RELEASE_DIR/SuperHaxagon-Windows.zip"
    zip -r "$WIN_RELEASE" -j "$BUILD_DIR/windows/SuperHaxagon.exe" "$BUILD_DIR/windows/openal32.dll" > /dev/null
    zip -r "$WIN_RELEASE" "$ROMFS_DIR"/* > /dev/null
    echo "windows created: $WIN_RELEASE"
fi

LINUX_RELEASE="$RELEASE_DIR/SuperHaxagon-Linux.tar.gz"
TEMP_TAR="$RELEASE_DIR/SuperHaxagon-Linux.tar"
tar -cf "$TEMP_TAR" -C "$BUILD_DIR/linux" SuperHaxagon
tar -rf "$TEMP_TAR" -C "$ROMFS_DIR" . --transform='s,^./,romfs/,'
gzip "$TEMP_TAR"
echo "linux created: $LINUX_RELEASE"

declare -A EXTENSIONS=(
    [3ds]="*.cia *.3dsx"
    [nspire]="*.tns"
    [switch]="*.nro"
)

for PLATFORM in "${!EXTENSIONS[@]}"; do
    if [[ -d "$BUILD_DIR/$PLATFORM" ]]; then
        for EXT in ${EXTENSIONS[$PLATFORM]}; do
            cp $BUILD_DIR/$PLATFORM/$EXT "$RELEASE_DIR"/
            echo "$PLATFORM $EXT copied."
        done
    fi
done

if [[ -f "$BUILD_DIR/net.awalter.SuperHaxagon.flatpak" ]]; then
    cp "$BUILD_DIR/net.awalter.SuperHaxagon.flatpak" "$RELEASE_DIR/"
    echo "flatpak copied."
fi

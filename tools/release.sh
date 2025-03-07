#!/bin/bash

# Packages everything into a ./GitHub folder for uploading with one drag and drop.
# Aint nobody got time for manually doing this all when you have so many storming releases.

cd ../release || (echo "release directory not found" && exit)

GAME="SuperHaxagon"
DEST="./GitHub"

mkdir -p $DEST

echo "Place the 'romfs' folder from the GitHub release here (the whole romfs folder, not the contents!) or the game will not launch." > $DEST/PLACE_ROMFS_FOLDER_HERE.txt

cp -v 3ds/*.cia $DEST/ && echo "3DS .cia OK"
cp -v 3ds/*.3dsx $DEST/ && echo "3DS .3dsx OK"
cp -v switch/*.nro $DEST/ && echo "Switch .nro OK"
cp -v nspire/*.tns $DEST/ && echo "Nspire .tns OK"
cp -v net.awalter.SuperHaxagon.flatpak $DEST/ && echo "Flatpak .flatpak OK"

(
  cd "linux-sfml" || { echo "Linux SFML directory does not exist!" && exit; }
  zip -j "../$DEST/$GAME-Linux-x86-SFML-no-romfs.zip" "$GAME" "../$DEST/PLACE_ROMFS_FOLDER_HERE.txt" && echo "Linux SFML OK"
  zip -r "../$DEST/romfs.zip" "romfs" && echo "romfs OK"
)

(
  cd "linux-sdl2" || { echo "Linux SDL2 directory does not exist!" && exit; }
  zip -j "../$DEST/$GAME-Linux-x86-SDL2-no-romfs.zip" "$GAME" "../$DEST/PLACE_ROMFS_FOLDER_HERE.txt" && echo "Linux SDL2 OK"
)

(
  cd miyoo || { echo "Miyoo Mini directory does not exist!" && exit; }
  cp "../$DEST/PLACE_ROMFS_FOLDER_HERE.txt" "Roms/PORTS/Games/$GAME/"
  zip -r "../$DEST/$GAME-MiyooMini-armhf-no-romfs.zip" "Roms" -x "Roms/PORTS/Games/$GAME/romfs/*" && echo "Miyoo Mini OK"
)

(
  cd "portmaster" || { echo "PortMaster directory does not exist!" && exit; }
  zip -r "../$DEST/$GAME-PortMaster-arm64-autoinstall.zip" ./* && echo "PortMaster OK"
)

(
  cd "windows" || { echo "Windows directory does not exist!" && exit; }
  zip -j "../$DEST/$GAME-Windows-x86-no-romfs.zip" "$GAME.exe" "openal32.dll" "../$DEST/PLACE_ROMFS_FOLDER_HERE.txt" && echo "Windows OK"
)

rm $DEST/PLACE_ROMFS_FOLDER_HERE.txt

echo "Done."

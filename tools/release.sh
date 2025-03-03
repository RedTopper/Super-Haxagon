#!/bin/bash

# Packages everything into a ./GitHub folder for uploading with one drag and drop.
# Aint nobody got time for manually doing this all when you have so many storming releases.

cd ../release

GAME="SuperHaxagon"
DEST="./GitHub"

mkdir -p $DEST

echo "Place the 'romfs' folder from the GitHub release here (the whole romfs folder, not the contents!) or the game will not launch." > $DEST/PLACE_ROMFS_FOLDER_HERE.txt

cp 3ds/*.cia $DEST/
cp 3ds/*.3dsx $DEST/
cp switch/*.nro $DEST/
cp nspire/*.tns $DEST/
cp net.awalter.SuperHaxagon.flatpak $DEST/

zip -j "$DEST/$GAME-Linux-x86-SDL2-no-romfs.zip" "linux-sdl2/$GAME" "$DEST/PLACE_ROMFS_FOLDER_HERE.txt"
zip -j "$DEST/$GAME-Linux-x86-SFML-no-romfs.zip" "linux-sfml/$GAME" "$DEST/PLACE_ROMFS_FOLDER_HERE.txt"

cd "miyoo/$GAME"
zip -r "../../$DEST/romfs.zip" "romfs"
mv romfs ../../$DEST
cd ..

cp ../$DEST/PLACE_ROMFS_FOLDER_HERE.txt "$GAME/"
zip -r "../$DEST/$GAME-Miyoo-armhf-no-romfs.zip" "$GAME"
mv ../$DEST/romfs $GAME/
cd ..

cd "portmaster"
zip -r "../$DEST/$GAME-PortMaster-arm64-autoinstall.zip" *
cd ..

if [[ -d "windows" ]]; then
    zip -j "$DEST/$GAME-Windows-x86-no-romfs.zip" "windows/$GAME.exe" "windows/openal32.dll" "$DEST/PLACE_ROMFS_FOLDER_HERE.txt"
else
    echo "Windows directory does not exist!"
fi

rm $DEST/PLACE_ROMFS_FOLDER_HERE.txt

echo "Done."

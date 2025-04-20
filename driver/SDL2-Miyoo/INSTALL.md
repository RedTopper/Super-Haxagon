# SuperHaxagon for MiyooMini (OnionOS)

SuperHaxagon is a cross-platform, open source [Super Hexagon](http://superhexagon.com/) clone.

You can obtain a free copy of the source code licensed under the GPLv3 at:

https://github.com/RedTopper/Super-Haxagon

## Install

The stock OS is not supported or tested. Please use OnionOS.

1. Download `SuperHaxagon-SDL2-MiyooMini-armhf-no-romfs.zip` and `romfs.zip`
2. Extract and merge the `Roms` folder to the root of your SD card
3. Navigate to `Roms/PORTS/Games/SuperHaxagon/` and extract the `romfs` folder from `romfs.zip`
4. On the Miyoo device, navigate to Games > Ports > ~Import ports
5. Find SuperHaxagon in the Arcade folder and launch!

The romfs folder tree should look like this:

Before:

```
└── Roms/PORTS/Games/SuperHaxagon
    ├── ...
    ├── lib
    │   ├── ...
    │   └── libSDL2-2.0.so.0
    ├── PLACE_ROMFS_FOLDER_HERE.txt
    └── SuperHaxagon
```

After:

```
└── Roms/PORTS/Games/SuperHaxagon
    ├── ...
    ├── lib
    │   ├── ...
    │   └── libSDL2-2.0.so.0
    ├── romfs
    │   ├── ...
    │   └── levels.haxagon
    └── SuperHaxagon
```

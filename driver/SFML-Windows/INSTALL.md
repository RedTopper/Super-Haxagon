# SuperHaxagon for Windows

SuperHaxagon is a cross-platform, open source [Super Hexagon](http://superhexagon.com/) clone.

You can obtain a free copy of the source code licensed under the GPLv3 at:

https://github.com/RedTopper/Super-Haxagon

## Install

_Note: SFML requires a graphics adapter with OpenGL 3.2 or higher. SuperHaxagon may be unsupported on
first generation Intel processors, Remote Desktop sessions, or VMs with no hardware acceleration.
See [#22](https://github.com/RedTopper/Super-Haxagon/issues/22) for details._

1. Download `SuperHaxagon-Windows-x86_64-no-romfs.zip` and `romfs.zip`
2. Extract the files into any empty directory
3. Place the `romfs` folder from `romfs.zip` next to the `.exe` file
4. Launch the game!
The romfs folder tree should look like this:

Before:

```
└── SuperHaxagon
    ├── ...
    ├── PLACE_ROMFS_FOLDER_HERE.txt
    └── SuperHaxagon.exe
```

After:

```
└── SuperHaxagon
    ├── ...
    ├── romfs
    │   ├── ...
    │   └── levels.haxagon
    └── SuperHaxagon.exe
```

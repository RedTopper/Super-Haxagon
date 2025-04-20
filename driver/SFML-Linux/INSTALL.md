# SuperHaxagon for Linux (SFML)

SuperHaxagon is a cross-platform, open source [Super Hexagon](http://superhexagon.com/) clone.

You can obtain a free copy of the source code licensed under the GPLv3 at:

https://github.com/RedTopper/Super-Haxagon

## Install

SFML is statically linked, so you shouldn't need it installed as a dependency!

1. Download `SuperHaxagon-LinuxSFML-<arch>-no-romfs.zip` and `romfs.zip`
2. Extract the files into any empty directory
3. Extract and place the `romfs` folder from `romfs.zip` next to the `SuperHaxagon` file
4. `./SuperHaxagon`

The romfs folder tree should look like this:

Before:

```
└── SuperHaxagon
    ├── ...
    ├── PLACE_ROMFS_FOLDER_HERE.txt
    └── SuperHaxagon
```

After:

```
└── SuperHaxagon
    ├── ...
    ├── romfs
    │   ├── ...
    │   └── levels.haxagon
    └── SuperHaxagon
```

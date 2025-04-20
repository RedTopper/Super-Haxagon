# SuperHaxagon for Linux (SDL2)

SuperHaxagon is a cross-platform, open source [Super Hexagon](http://superhexagon.com/) clone.

You can obtain a free copy of the source code licensed under the GPLv3 at:

https://github.com/RedTopper/Super-Haxagon

## Install

SDL2 comes with native controller support, but requires SDL2 installed on your system.

Please find the appropriate command for your distro to install SDL2 first!

1. Download `SuperHaxagon-SDL2-Linux-x86_64-no-romfs.zip` and `romfs.zip`
2. Install SDL2 from your distro's package manager, if needed  
   Fedora: `sudo dnf install -y SDL2 SDL2_mixer SDL2_ttf`  
   Ubuntu: `sudo apt update && sudo apt install -y libsdl2 libsdl2-mixer libsdl2-ttf`
3. Extract the files into any empty directory
4. Extract and place the `romfs` folder from `romfs.zip` next to the `SuperHaxagon` file
5. `./SuperHaxagon`

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

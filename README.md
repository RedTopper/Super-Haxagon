<p align="center"><img style="text-align:center" src="./media/rendersmall.png" alt="Banner Render" title="Banner Render"/></p>

# ![Icon](./media/icon-3ds.png "Icon") Super-Haxagon

Super Haxagon is an open source [Super Hexagon](http://superhexagon.com/) clone for the 3DS, Windows, and Linux. It runs full speed on the old 3DS, new 3DS, and in [citra-emu/citra](https://github.com/citra-emu/citra). It also has a GUI based level editor called [Haxa Editor](https://github.com/RedInquisitive/Haxa-Editor).

## Download

You can download the latest build on [the releases page](https://github.com/RedInquisitive/Super-Haxagon/releases) to the right.

## Features

 * Cross Platform
 * Widescreen support
 * High Refresh Rate support
 * Custom Levels
 * OGG decoder for BGM
 * High Score Saving
 
## Credits
 * Audio from [Open Hexagon](http://vittorioromeo.info/projects.html)
 * ...And more audio by [Kevin MacLeod](http://incompetech.com/) under [Creative Commons: By Attribution 3.0](http://creativecommons.org/licenses/by/3.0/)
 * 2D engine by fincs at [devkitPro/citro2D](https://github.com/devkitPro/citro2d)
 * 3D engine by fincs at [devkitPro/citro3D](https://github.com/devkitPro/citro3d)
 * Smea for [the CTR User library](https://github.com/devkitPro/libctru)
 * Actual font: [Bump IT UP](http://fontstruct.com/fontstructions/show/155156/bump_it_up)
 * and all of the people on the [contributors page](https://github.com/RedInquisitive/Super-Haxagon/graphs/contributors) (seriously, thanks!)

## Building

Super Haxagon can build for 3DS and Windows (MinGW + MSVC). For desktop platforms, it uses a cross platform library called SFML, so it will probably also compile on Linux and MacOS without any issues.

### 3DS Build

1. Clone this repository with `git clone https://github.com/RedInquisitive/Super-Haxagon.git --recursive`
1. Get MinGW and DevkitPro
1. Use the provided Makefile with the command `make TARGET:=3DS`
1. Install either the CIA or 3DSX on your 3DS

### PC Build (MSVC)

1. Clone this repository
1. Download SFML and place it in the libraries directory and rename to `SFML`
1. Use Visual Studio to open a folder with the CMake file
1. Switch to Release and press Play

### PC Build (MinGW)

1. Clone this repository
1. Get MinGW
1. Use the provided Makefile with the command `make TARGET:=WIN64`
1. Move the `romfs` folder into a folder called `data`, and rename `romfs` to `rom`

## Screenshots

![Title Screen](./media/testmenu_20161225_2335_00002.png "One of the many modes")
![Game Over](./media/testmenu_20161225_2348_00000.png "Game over! Go! Game over! Go!")
![Walls](./media/testmenu_20161225_2336_00003.png "The walls move twoards you!")
![Shapes](./media/testmenu_20161225_2337_00006.png "All shapes and sizes!")
![Shapes](./media/testmenu_20161225_2340_00011.png "Super Pentagon?")

## The Original

![Humble Beginnings](./media/scr_2_MERGED.png "Humble Arrow")
![Humble Beginnings](./media/scr_1_MERGED.png "The First Test")

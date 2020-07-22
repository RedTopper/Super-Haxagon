<p align="center"><img style="text-align:center" src="./media/rendersmall.png" alt="Banner Render" title="Banner Render"/></p>

# ![Icon](./media/icon-3ds.png "Icon") Super-Haxagon

Super Haxagon is an cross platform, open source [Super Hexagon](http://superhexagon.com/) clone for the 3DS, Windows, and Linux. It runs full speed on the old 3DS, new 3DS, and in [citra-emu/citra](https://github.com/citra-emu/citra). It also has a GUI based level editor called [Haxa Editor](https://github.com/RedInquisitive/Haxa-Editor).

## Download

You can download the latest build on [the releases page](https://github.com/RedInquisitive/Super-Haxagon/releases) to the right.

## Features

 * Cross Platform
 * Widescreen support
 * High Refresh Rate support
 * Custom Levels
 * OGG decoder for BGM
 * High Score Saving

## Building

Super Haxagon can build for 3DS, Switch, Windows (MinGW + MSVC), and Linux. For desktop platforms, it uses a cross platform library called SFML, so it will probably also compile on other platforms that support it.

1. Clone this repository with `git clone https://github.com/RedInquisitive/Super-Haxagon.git --recursive`

### 3DS Build

1. Get MinGW and DevkitPro and install dependencies listed in the Makefile
1. Use the provided Makefile with the command `make TARGET:=3DS`
1. Install either the CIA or 3DSX on your 3DS

### Switch Build

1. Get MinGW and DevkitPro and install dependencies listed in the Makefile
1. Use the provided Makefile with the command `make TARGET:=SWITCH`
1. Copy the .nro to `sdmc:/switch/SuperHaxagon/SuperHaxagon.nro` and launch it from the hbmenu

### PC Build

#### ... with MSVC on Windows

1. Download SFML for your MSVC version
1. Rename the SFML folder to `SFML-MSVC` and place it in libraries
1. Use Visual Studio to open a folder with the CMake file
1. Build the game
1. Press play

#### ... with MinGW/msys on Windows

1. Download SFML for your MinGW version
1. Rename the SFML folder to `SFML-MinGW` and place it in libraries
1. Use the provided Makefile with the command `make TARGET:=WIN64` OR use the CMake file
1. Copy the `romfs` folder and `SFML/bin/openal32.dll` library next to the built executable (only needed with Makefile)
1. Launch the executable

#### ... with GCC on Linux

1. Install SFML through your distro's package manager
1. Clone this repository
1. Use the CMake file or Makefile `make TARGET:=LINUX64` to build it
1. Launch the executable

## Credits

Thanks everyone for:

 * The toolchain by [devkitPro](https://github.com/devkitPro)
 * The 3DS 2D engine by fincs at [devkitPro/citro2D](https://github.com/devkitPro/citro2d)
 * The CTR User library by Smea at [devkitPro/libctru](https://github.com/devkitPro/libctru)
 * the Switch User library by Switchbrew at [switchbrew/libnx](https://github.com/switchbrew/libnx)
 * The font Bump IT UP by aaronamar at [fontstruct.com](http://fontstruct.com/fontstructions/show/155156/bump_it_up)
 * Steveice10 for the build tools at [Steveice10/buildtools](https://github.com/Steveice10/buildtools)
 * Sean T. Barrett for OGG decoding at [nothings/stb](https://github.com/nothings/stb)
 * The SFML developers for, well, [SFML](https://www.sfml-dev.org/)
 * Audio from Open Hexagon at [SuperV1234/SSVOpenHexagon](https://github.com/SuperV1234/SSVOpenHexagon)
 * ...and Kevin MacLeod for more music at [incompetech.com](http://incompetech.com/) under [Creative Commons: By Attribution 3.0](http://creativecommons.org/licenses/by/3.0/)
 * and all of the people on the [contributors page](https://github.com/RedInquisitive/Super-Haxagon/graphs/contributors) (seriously, you guys rock!)

## Screenshots

Below are a few screenshots featuring both the Windows and 3DS platform. 3DS screenshots
were taken on actual hardware.

![3D Banner](./media/screenshots/HNI_0013.JPG "Banner")

### Cross Platform

![In Game Windows](./media/screenshots/Windows_2020-07-10_014324.png "In Game Windows")
![In Game 3DS](./media/screenshots/luma-2020-07-11-06-48-44-586.png "In Game 3DS")

<details><summary>More Screenshots</summary>

![Title Screen Windows](./media/screenshots/Windows_2020-07-10_014650.png "Title Screen Windows")
![Title Screen 3DS](./media/screenshots/luma-2020-07-11-06-42-12-193.png "Title Screen 3DS")
![Death Spiral Windows](./media/screenshots/Windows_2020-07-10_014604.png "Death Spiral")
![Death Spiral 3DS](./media/screenshots/luma-2020-07-11-06-55-45-531.png "Death Spiral 3DS")

</details>

<details><summary>Proof of Concept</summary>

![Humble Beginnings](./media/screenshots/scr_2_MERGED.png "Humble Arrow")
![Humble Beginnings](./media/screenshots/scr_1_MERGED.png "The First Test")

</details>

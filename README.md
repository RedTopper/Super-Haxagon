<p align="center"><img style="text-align:center" src="./media/rendersmall.png" alt="Banner Render" title="Banner Render"/></p>

# ![Icon](./media/icon-3ds.png "Icon") Super-Haxagon

Super Haxagon is a cross platform, open source [Super Hexagon](http://superhexagon.com/) clone for the Nintendo 3DS, Nintendo Switch, Windows, and Linux. It is developed in C++ and uses no additional libraries for the core logic, making it easy (in theory) to port to new platforms. It also has a GUI based level editor called [Haxa Editor](https://github.com/RedInquisitive/Haxa-Editor).

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

Super Haxagon targets the 3DS, Switch, Windows (MinGW + MSVC), and Linux. For desktop platforms, it uses a cross platform library called SFML, so it will probably also compile on other platforms that support it.

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
 * The Switch User library by Switchbrew at [switchbrew/libnx](https://github.com/switchbrew/libnx)
 * The font Bump IT UP by aaronamar at [fontstruct.com](http://fontstruct.com/fontstructions/show/155156/bump_it_up)
 * Steveice10 for the build tools at [Steveice10/buildtools](https://github.com/Steveice10/buildtools)
 * Sean T. Barrett for OGG decoding at [nothings/stb](https://github.com/nothings/stb)
 * The SFML developers for, well, [SFML](https://www.sfml-dev.org/)
 * Audio from Open Hexagon at [SuperV1234/SSVOpenHexagon](https://github.com/SuperV1234/SSVOpenHexagon)
 * ...and Kevin MacLeod for more music at [incompetech.com](http://incompetech.com/)
 * and all of the people on the [contributors page](https://github.com/RedInquisitive/Super-Haxagon/graphs/contributors) (seriously, you guys rock!)

<details><summary>Music Attribution</summary>

Werq by Kevin MacLeod  
Link: https://incompetech.filmmusic.io/song/4616-werq  
License: http://creativecommons.org/licenses/by/4.0/  

Screen Saver by Kevin MacLeod  
Link: https://incompetech.filmmusic.io/song/5715-screen-saver  
License: http://creativecommons.org/licenses/by/4.0/  

Call Me Katla, Baby by Dunderpatrullen  
Captain Cool by Bossfight  
Commando Steve by Bossfight  
Dr. Finkelfracken's Cure by Bossfight  
Jack Russel by Bossfight  
The Maze Of Mayonnaise by Bossfight  
Link: https://github.com/SuperV1234/SSVOpenHexagonAssets  
License: ᵖˡᵉᵃˢᵉ ᵈᵒⁿ'ᵗ ˢᵘᵉ ᵐᵉ  

</details>

## Cross Platform

Below are a few screenshots featuring the various platforms that Super Haxagon compiles on. The 3DS version is packaged with a custom 3D banner to make it look official. Additionally, the gif was recorded on actual Nintendo Switch hardware.

<img width="500" src="./media/screenshots/HNI_0013.JPG"                     alt="3D Banner"            title="3D Banner"/>
<img width="500" src="./media/screenshots/Windows_2020-07-10_014324.png"    alt="In Game Windows"      title="Windows"/>
<img width="500" src="./media/screenshots/luma-2020-07-11-06-48-44-586.png" alt="In Game 3DS"          title="3DS"/>
<img width="500" src="./media/screenshots/switch.gif"                       alt="In Game Switch (GIF)" title="In Game Switch (GIF)"/>
<img width="500" src="./media/screenshots/20200727_015021.jpg"              alt="Hardware"             title="Hardware"/>

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

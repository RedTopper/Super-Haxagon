<p align="center"><img style="text-align:center" src="./media/rendersmall.png" alt="Banner Render" title="Banner Render"/></p>

<!-- Note: icon-3ds.png name kept for backwards compatability with old readmes. -->

# ![Icon](./media/icon-3ds.png "Icon") SuperHaxagon

SuperHaxagon, like the original game [Super Hexagon](http://superhexagon.com/) by Terry Cavanagh, has only one goal. 
Survive as long as possible by avoiding the falling walls in a trippy, spinny frenzy!

SuperHaxagon is a cross-platform, open source _Super Hexagon_ clone for the Nintendo 3DS, Nintendo Switch, Windows,
Linux, Portmaster, Miyoo Mini, and the [TI-Nspire](https://education.ti.com/en/products/calculators/graphing-calculators/ti-nspire-cx-cas) (yes, the calculator). It is developed in C++ and uses no 
additional libraries for the core logic, making it easy (in theory) to port to new platforms. It also has a GUI based 
level editor called [Haxa Editor](https://github.com/RedTopper/Haxa-Editor).

## Download

You can download the latest build on [the releases page](https://github.com/RedTopper/Super-Haxagon/releases) to the right.

## Install

SuperHaxagon supports many different devices. [Find your install instructions here](./INSTALL.md)!

## Features

 * Cross Platform
 * Widescreen support
 * High Refresh Rate support
 * Custom Levels
 * OGG decoder for BGM
 * High Score Saving

## Building

1. Clone this repository with `git clone https://github.com/RedTopper/Super-Haxagon.git`

### For Users With Podman or Docker (Recommended):

You can use the provided `tools/compose.yaml` file for creating sane build environments.
The following examples use `podman-compose` and `podman` on fedora, but a similar result
can likely be done with `docker` by replacing `podman-compose` with `docker compose` 
(untested).

From within the `tools` directory, run the following commands replacing `<platform>` with
the platform you want to build for:

 * 3ds
 * switch
 * nspire
 * linux
 * linux-sdl2
 * nothing
 * portmaster
 * miyoo

Create the build environment with:

`podman-compose build <platform>`

Then build SuperHaxagon with:

`podman-compose up <platform>`

If you need a shell within the build environment, run:

`podman-compose run --rm <platform> bash`

Then run the following to kick off a build from the `~/git/build/<platform>` directory:

`build.sh`

For more advanced users, feel free to modify the `compose.yaml` file to your needs.

### For Linux Users Without Containerization:

Install SFML's dependencies through your package manager. An example for Fedora is in
`tools/linux/Containerfile`. You may also want to see the instructions here:

https://www.sfml-dev.org/faq/build-use/#link-static

Then you can run the following commands:

1. `mkdir build`
2. `cd build`
3. `cmake ..`
4. `make`

Note: This will create a build targeting your current machine. If you want to build for a different
platform, you'll need to replicate what's in the Containerfiles within tools.

### For Flatpak Users:

Run `flatpak.sh` from the `tools` directory.

### For Windows Users:

1. Install Visual Studio 2022
2. Open this repository in Visual Studio and press "Play"

## Editing the Beatmaps

If you are looking to swap out the existing music tracks with your own, you may be wondering what's up with those
`.txt` files next to the tracks. These metadata files provide information to the game about when to time beats, 
spins, tilts, zooms, and any other effects while the music is running.

While it _may_ be possible to automatically generate the "beats" from the music itself at runtime, some platforms
(the TI-NSpire, mostly) don't have the spare CPU cycles to deal with decoding music, so I found the best balance
to write out the events manually.

You can open these `.txt` files in [Audacity](https://www.audacityteam.org/) as a 
[Label Track](https://manual.audacityteam.org/man/creating_and_selecting_labels.html). If they change, the
events that the game can recognize are found in the `Play::update()` function within `Play.cpp`, and are 
currently as follows:

* **S**: A large, sudden spin
* **I**: Invert the background colors
* **BL**: Large beat, when the music is really thumping
* **BS**: Small beat
* **TL**: Tilt the level by moving the main camera to the left
* **TR**: Tilt the level by moving the main camera to the right
* **Z**: A huge, diving zoom.

Yes, deleting these files is cheating.

## Credits

Thanks everyone for:

 * The original game [Super Hexagon](http://superhexagon.com/) by Terry Cavanagh
 * The toolchain by [devkitPro](https://github.com/devkitPro)
 * The 3DS 2D engine by fincs at [devkitPro/citro2D](https://github.com/devkitPro/citro2d)
 * The CTR User library by Smea at [devkitPro/libctru](https://github.com/devkitPro/libctru)
 * The Switch User library by Switchbrew at [switchbrew/libnx](https://github.com/switchbrew/libnx)
 * The font Bump IT UP by aaronamar at [fontstruct.com](http://fontstruct.com/fontstructions/show/155156/bump_it_up)
 * Sean T. Barrett for OGG decoding at [nothings/stb](https://github.com/nothings/stb)
 * The sound effects, created by [TwistBit](https://github.com/TwistBit)
 * The SFML developers for, well, [SFML](https://www.sfml-dev.org/)
 * The SDL2 developers for [SDL2](https://github.com/libsdl-org/SDL)
 * [Dunderpatrullen](https://dunderpatrullen.nu/) and [Bossfight](https://www.bossfightmusic.com/) for the music
 * Kevin MacLeod for more music at [incompetech.com](http://incompetech.com/)
 * and all the people on the [contributors page](https://github.com/RedTopper/Super-Haxagon/graphs/contributors) (seriously, you guys rock!)

Previous versions of the games are supported by these folks too!

 * Steveice10 for the build tools at [RedTopper/buildtools](https://github.com/RedTopper/buildtools) (MIRROR)
 * Steveice10 for the banner tool at [RedTopper/bannertool](https://github.com/RedTopper/bannertool) (MIRROR)
 * Sound Effects from Open Hexagon at [SuperV1234/SSVOpenHexagon](https://github.com/vittorioromeo/SSVOpenHexagon)

Music Attribution

> Bleeping Demo Kevin MacLeod (incompetech.com)
> 
> Licensed under Creative Commons: By Attribution 3.0 License
> 
> http://creativecommons.org/licenses/by/3.0/

> Screen Saver Kevin MacLeod (incompetech.com)
> 
> Licensed under Creative Commons: By Attribution 3.0 License
> 
> http://creativecommons.org/licenses/by/3.0/

> Call Me Katla, Baby by Dunderpatrullen
>
> Licensed _non-commercially_

> Captain Cool by Bossfight
> 
> Commando Steve by Bossfight
> 
> Dr. Finkelfracken's Cure by Bossfight
> 
> Jack Russel by Bossfight
> 
> The Maze Of Mayonnaise by Bossfight
> 
> From [SuperV1234/SSVOpenHexagon](https://github.com/vittorioromeo/SSVOpenHexagon)

## Cross Platform

Below are a few screenshots featuring the various platforms that SuperHaxagon compiles on. The 3DS version is packaged 
with a custom 3D banner to make it look official. Additionally, the gif was recorded on actual Nintendo Switch hardware.

<img width="500" src="./media/screenshots/HNI_0013.JPG"                     alt="3D Banner"            title="3D Banner"/>
<img width="500" src="./media/screenshots/Windows_2020-07-10_014324.png"    alt="In Game Windows"      title="Windows"/>
<img width="500" src="./media/screenshots/luma-2020-07-11-06-48-44-586.png" alt="In Game 3DS"          title="3DS"/>
<img width="500" src="./media/screenshots/switch.gif"                       alt="In Game Switch (GIF)" title="In Game Switch (GIF)"/>
<img width="500" src="./media/screenshots/nspire.png"                       alt="TI-Nspire"            title="TI-Nspire"/>
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

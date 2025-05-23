<p align="center"><img style="text-align:center" src="./media/rendersmall.png" alt="Banner"/></p>

# ![SuperHaxagon](./media/icon-3ds.png "SuperHaxagon Icon") SuperHaxagon

SuperHaxagon, like the original game [Super Hexagon](http://superhexagon.com/) by Terry Cavanagh, has only one goal.
Survive as long as possible by avoiding the falling walls in a trippy, spinny frenzy!

SuperHaxagon is a cross-platform, open source _Super Hexagon_ clone for the Nintendo 3DS, Nintendo Switch, Windows,
Linux, Portmaster, Miyoo Mini, and the [TI-Nspire](https://education.ti.com/en/products/calculators/graphing-calculators/ti-nspire-cx-cas) 
(yes, the calculator). It is developed in C++ and uses no additional libraries for the core logic, making it easy (in theory) 
to port to new platforms. It also has a GUI based level editor called [Haxa Editor](https://github.com/RedTopper/Haxa-Editor).

## Features

 * Cross-platform with modular drivers
 * Music timed camera and spin effects
 * Device independent 3D effects
 * Supports any aspect ratio
 * Supports high refresh rates
 * Create custom levels
 * Save files for scores
 * Build toolchain with cmake

## Download

The latest release can always be found here:

https://github.com/RedTopper/Super-Haxagon/releases

(You may need to click "Show all assets" to find the download for your platform)

## Install

![Platforms SuperHaxagon runs on](/media/screenshots/install-banner.jpg)

You can view the latest instructions with the links below:

 * [Nintendo 3DS](./driver/N3DS/INSTALL.md)
 * [Nintendo Switch](./driver/Switch/INSTALL.md)
 * [TI-Nspire (Calculator)](./driver/Nspire/INSTALL.md)
 * [MiyooMini (OnionOS)](./driver/SDL2-Miyoo/INSTALL.md)
 * [PortMaster (Retro Handhelds)](./driver/SDL2-PortMaster/INSTALL.md)
 * [macOS](./driver/SFML-macOS/INSTALL.bundle.md) arm64
 * [Windows](./driver/SFML-Windows/INSTALL.md) x86_64
 * [Flatpak (Steam Deck/Immutable Distro)](./driver/SDL2-Linux/INSTALL.flatpak.md) x86_64
 * [Linux (SDL2 with controller support)](./driver/SDL2-Linux/INSTALL.native.md) x86_64
 * [Linux (SFML static link)](./driver/SFML-Linux/INSTALL.md) x86_64/arm64

Offline install instructions are included in your download inside the `README.md`.

### Other platforms

It may be possible to build and run SuperHaxagon on other platforms, as it is designed to be portable.
You can likely build it by following the instructions below. I'd love to hear any success for building
for other platforms!

## Building

The first step is always to clone this repository with:

```bash
git clone https://github.com/RedTopper/Super-Haxagon.git
```

Then, [find your device in the driver directory](./driver).

 * [Example](./driver/Example/README.md) for building an example driver
 * [N3DS](./driver/N3DS/README.md) for the Nintendo 3DS
 * [Nspire](./driver/Nspire/README.md) for the TI-Nspire
 * [SDL2-Linux](./driver/SDL2-Linux/README.md) for Linux with the SDL2 backend (incl. Flatpak)
 * [SDL2-Miyoo](./driver/SDL2-Miyoo/README.md) for the MiyooMini
 * [SDL2-PortMaster](./driver/SDL2-PortMaster/README.md) for PortMaster and retro gaming handhelds
 * [SFML-Linux](./driver/SFML-Linux/README.md) for Linux with the SFML2 backend
 * [SFML-macOS](./driver/SFML-macOS/README.md) for macOS
 * [SFML-Windows](./driver/SFML-Windows/README.md) for Microsoft Windows
 * [Switch](./driver/Switch/README.md) for the Nintendo Switch

## Credits

The original game [Super Hexagon](http://superhexagon.com/) by Terry Cavanagh

The people on the [contributors page](https://github.com/RedTopper/Super-Haxagon/graphs/contributors) (seriously, you guys rock!)

### Assets

Licenses for assets provided by the following people can now be found in [the CREDITS.md file](./CREDITS.md):

 * [TwistBit](https://github.com/TwistBit)
 * [aaronamar](http://fontstruct.com/fontstructions/show/155156/bump_it_up)
 * [Kevin MacLeod](http://incompetech.com/)
 * [Dunderpatrullen](https://dunderpatrullen.nu/)
 * [Bossfight](https://www.bossfightmusic.com/)

### Libraries

Individual library credits and licenses have moved to the device driver itself.
Please check out these individual LICENSE.md files for links to the wonderful projects
that make SuperHaxagon possible!

 * [N3DS Driver](./driver/N3DS/LICENSE.md) for the Nintendo 3DS
 * [Switch Driver](./driver/Switch/LICENSE.md) for the Nintendo Switch
 * [Nspire Driver](./driver/Nspire/LICENSE.md) for the TI-Nspire
 * [SFML Driver](./driver/Switch/LICENSE.md) for Linux, macOS, and Windows
 * [SDL2 Driver](./driver/SDL2/LICENSE.md) for PortMaster and MiyooMini

## License

SuperHaxagon is now licensed under GPLv3 (or later) to ensure that any implementation across any platform will
remain open for as long as possible!

The individual drivers originally written by me are dual-licensed under MIT to ensure other projects that do not
want to use the GPL can continue use this code to port their projects to new platforms.

New drivers may elect to use this "dual licensing" scheme, or simply use GPLv3 if they want more copyleft protections.

This license does not apply to assets or iconography, which have various licenses listed out in 
[the CREDITS.md file](./CREDITS.md).

## Beatmaps

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

_(Yes, deleting these files is cheating)_

## Screenshots

Below are a few screenshots featuring the various platforms that SuperHaxagon compiles on. The 3DS version is packaged 
with a custom 3D banner to make it look official. Additionally, the gif was recorded on actual Nintendo Switch hardware.

<details><summary>Screenshots</summary>

<img width="500" src="./media/screenshots/HNI_0013.JPG"                     alt="3D Banner"            title="3D Banner"/>
<img width="500" src="./media/screenshots/Windows_2020-07-10_014324.png"    alt="In Game Windows"      title="Windows"/>
<img width="500" src="./media/screenshots/luma-2020-07-11-06-48-44-586.png" alt="In Game 3DS"          title="3DS"/>
<img width="500" src="./media/screenshots/switch.gif"                       alt="In Game Switch (GIF)" title="In Game Switch (GIF)"/>
<img width="500" src="./media/screenshots/nspire.png"                       alt="TI-Nspire"            title="TI-Nspire"/>
<img width="500" src="./media/screenshots/20200727_015021.jpg"              alt="Hardware"             title="Hardware"/>

</details>

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

<p align="center"><img width="500" src="./media/screenshots/install-banner.jpg" alt="Platforms" title="Platforms SuperHaxagon runs on"/></p>

# ![Icon](./media/icon-3ds.png "Icon") Installation

SuperHaxagon supports many different platforms and has many different ways to install. Find your platform below and 
follow the instructions to install SuperHaxagon on your device!

## Download

The first step is to download the release you need. The latest release can always be found here:

https://github.com/RedTopper/Super-Haxagon/releases

> [!IMPORTANT]  
> For any release containing `no-romfs.zip`, please ensure you _also_ download and extract the romfs folder, 
> pictured below:

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

Then, click on the installation instructions you need below for your platform!

## Windows (x86)

<details><summary>Windows Install Instructions</summary>

_Note: SFML requires a graphics adapter with OpenGL 3.2 or higher. SuperHaxagon may be unsupported on 
first generation Intel processors, Remote Desktop sessions, or VMs with no hardware acceleration. 
See [#22](https://github.com/RedTopper/Super-Haxagon/issues/22) for details._

1. Download `SuperHaxagon-Windows-x86-no-romfs.zip` and `romfs.zip`
2. Extract the files into any empty directory
3. Place the `romfs` folder from `romfs.zip` next to the `.exe` file
4. Launch the game!

</details>

## Desktop Linux (x86)

<details><summary>Linux Install Instructions</summary>

There are 3 different ways to install SuperHaxagon on Linux. Pick one that works best for you below!

### Flatpak (SteamDeck or other immutable OS)

1. Download `net.awalter.SuperHaxagon.flatpak`
2. Run `flatpak --user install net.awalter.SuperHaxagon.flatpak`
3. Find SuperHaxagon in your application launcher, or use `flatpak run net.awalter.SuperHaxagon`!

### SFML (Static Link)

SFML is statically linked, so you shouldn't need it installed as a dependency.

1. Download `SuperHaxagon-Linux-x86-SFML-no-romfs.zip` and `romfs.zip`
2. Extract the files into any empty directory
3. Extract and place the `romfs` folder from `romfs.zip` next to the `SuperHaxagon` file
4. `./SuperHaxagon`

### SDL2 (Dynamic Link)

SDL2 requires SDL2 installed on your system. Please find the appropriate command for your distro to install SDL2
first! SDL2 comes with extra controller support, but is a newer driver and is less tested.

1. Install SDL2 from your distro's package manager
2. Download `SuperHaxagon-Linux-x86-SDL2-no-romfs.zip` and `romfs.zip`
3. Extract the files into any empty directory
4. Extract and place the `romfs` folder from `romfs.zip` next to the `SuperHaxagon` file
5. `./SuperHaxagon`

</details>

## Nintendo 3DS

<details><summary>3DS Install Instructions</summary>

The Nintendo 3DS ***REQUIRES*** the DSP firmware to be dumped to hear audio! Please use the 
`DSP1` Homebrew app FIRST to hear the game audio!

### .cia

1. Download `SuperHaxagon.cia` and place it on your SD card
2. Install `SuperHaxagon.cia` using FBI or a similar installer
3. Launch SuperHaxagon from your home menu

### .3dsx

1. Download `SuperHaxagon.3dsx` and place it in `sdmc:/3ds`
2. Launch the homebrew launcher
3. Launch SuperHaxagon

If you want to change the title screen music, you can additionally place any .ogg file on your SD card at
`sdmc:/3ds/data/haxagon/title.ogg`

</details>

## Nintendo Switch

<details><summary>Switch Install Instructions</summary>

1. Download `SuperHaxagon.nro` and place it in `sdmc:/switch/SuperHaxagon`
2. Launch the homebrew launcher (either in Applet mode (Album) or Game mode (R) )
3. Launch SuperHaxagon

If you want to change the title screen music, you can additionally place any .ogg file on your SD card at
`sdmc:/switch/SuperHaxagon/title.ogg`

</details>

## Miyoo Mini (OnionOS)

<details><summary>Miyoo Mini install instructions</summary>

The stock OS is not supported or tested. Please use OnionOS.

1. Download `SuperHaxagon-MiyooMini-armhf-no-romfs.zip` and `romfs.zip`
2. Extract and merge the `Roms` folder to the root of your SD card
3. Navigate to `Roms/PORTS/Games/SuperHaxagon/` and extract the `romfs` folder from `romfs.zip`
4. On the Miyoo device, navigate to Games > Ports > ~Import ports
5. Find SuperHaxagon in the Arcade folder and launch!

</details>

## PortMaster (Retro Handhelds)

<details><summary>PortMaster install instructions</summary>

The PortMaster should work on any CFW where PortMaster is supported. Please use the "autoinstall" 
folder to install the zip manually from this repo. It is "Ready to Run" and contains all assets, 
including the romfs folder.

1. Download `SuperHaxagon-PortMaster-arm64-autoinstall.zip`
2. Place it in your PortMaster's `autoinstall` folder
   * For muOS: `/mnt/mmc/MUOS/PortMaster/autoinstall/`
   * For Knulli: `/media/SHARE/system/.local/share/PortMaster/autoinstall/`
   * For other CFWs, see the PortMaster documentation for your platform.
3. Launch the PortMaster app and watch it install
4. Refresh your game list (varies by CFW)
5. Find SuperHaxagon (likely in Ports) and launch it!

</details>

## TI-Nspire (First Gen)

<details><summary>TI Nspire install instructions</summary>

Before installing, make sure you have [ndless](https://ndless.me/) for your calculator. Newer versions of the TI-Nspire may not
be supported.

1. Download `SuperHaxagon.tns`
2. Transfer it to your calculator with the [Computer Link](https://education.ti.com/en/products/computer-software/ti-nspire-computer-link) Software
3. Launch the game!

</details>

## macOS (M or Intel)

<details><summary>macOS install instructions</summary>

Unfortunately I do not have a macOS machine regularly test SuperHaxagon with, but it should always build!

The release build is built for M1 processors (arm64). [Intel based macs will need to follow the Building instructions](./driver/macOS/README.md)

1. Download `SuperHaxagon-macOS-arm.tar`
2. Extract the files into any empty directory
3. Launch the application by right-clicking and choosing "Open"

</details>

## Other platforms

<details><summary>All other platforms</summary>

It may be possible to build and run SuperHaxagon on other platforms, as it is designed to be portable. I don't supply builds,
for Windows or Linux on ARM (excluding PortMaster), but you can likely build it by following the instructions in the
[README.md](./README.md). I'd love to hear any success for building for other platforms!

</details>

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

### [Nintendo 3DS](./driver/N3DS/INSTALL.md)

### [TI-Nspire (First Gen)](./driver/Nspire/INSTALL.md)

### [Flatpak (Steam Deck/Immutable Distro)](./driver/SDL2-Linux/INSTALL.flatpak.md) x86_64

### [Linux (SDL2 with controller support)](./driver/SDL2-Linux/INSTALL.native.md) x86_64

### [Linux (SFML static link)](./driver/SDL2-Linux/INSTALL.native.md) x86_64/arm64

### [MiyooMini (OnionOS)](./driver/SDL2-Miyoo/INSTALL.md)

### [PortMaster (Retro Handhelds)](./driver/SDL2-PortMaster/INSTALL.md)

### [macOS](./driver/SFML-macOS/INSTALL.bundle.md) arm64

### [Windows](./driver/SFML-Windows/INSTALL.md) x86_64


## Nintendo Switch

<details><summary>Switch Install Instructions</summary>

1. Download and extract `SuperHaxagon-Switch-arm64.zip`
2. Merge `switch` into your SD card, placing `SuperHaxagon.nro` in `sdmc:/switch/SuperHaxagon`
3. Launch the homebrew launcher (either in Applet mode (Album) or Game mode (R) )
4. Launch SuperHaxagon

If you want to change the title screen music, you can additionally place any .ogg file on your SD card at
`sdmc:/switch/SuperHaxagon/title.ogg`

</details>


## Other platforms

<details><summary>All other platforms</summary>

It may be possible to build and run SuperHaxagon on other platforms, as it is designed to be portable. 
You can likely build it by following the instructions in the [README.md](./README.md). I'd love to hear
any success for building for other platforms!

</details>

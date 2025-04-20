# SuperHaxagon for PortMaster

This build is intended for PortMaster. Learn more about PortMaster here:

https://portmaster.games/index.html

For additional instructions on different platforms, see the original README at:

https://github.com/RedTopper/Super-Haxagon

## Controls

The controls are shown on the title screen, but here's a copy if it's needed:

| Button            | Action       |
|-------------------|--------------| 
| A                 | Select       |
| B                 | Back         |
| DPAD L, L1, L2, X | Rotate Left  |
| DPAD R, R1, R2, Y | Rotate Right |
| Menu              | Quit Game    |

## Install

The PortMaster should work on any CFW where PortMaster is supported. Please use the "autoinstall"
folder to install the zip manually from this repo. It is "Ready to Run" and contains all assets,
including the romfs folder.

1. Download `SuperHaxagon-SDL2-PortMaster-arm64.zip`
2. Place it in your PortMaster's `autoinstall` folder
    * For muOS: `/mnt/mmc/MUOS/PortMaster/autoinstall/`
    * For Knulli: `/media/SHARE/system/.local/share/PortMaster/autoinstall/`
    * For other CFWs, see the PortMaster documentation for your platform.
3. Launch the PortMaster app and watch it install
4. Refresh your game list (varies by CFW)
5. Find SuperHaxagon (likely in Ports) and launch it!

## Building

https://portmaster.games/build-environments.html

```bash
mkdir build install
cd build
cmake -DDRIVER_PORTMASTER=TRUE -DCMAKE_INSTALL_PREFIX=../install ..
make -j4
make install
```

See the building instructions here for more details:

https://github.com/RedTopper/Super-Haxagon/blob/master/driver/SDL2-PortMaster/README.md

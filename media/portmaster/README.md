# SuperHaxagon for PortMaster

For additional instructions on different platforms, see the original README at:

https://github.com/RedTopper/Super-Haxagon

## Building

A compose.yaml file is provided in the `/tools` folder for building. The container,
`monkeyx/retro_builder:arm64`, is built for ARM64, so if your HOST platform is not ARM64,
you'll need to use `qemu-user-static`. Instructions for this are in `/tools/portmaster.sh`

If you don't want to use Docker, a well-documented toolchain for building in WSL/Linux 
chroot is described here:

https://portmaster.games/build-environments.html

From there, you need to specify the DRIVER_PORTMASTER on the cmake commandline before
building, as such:

```
$ mkdir build
$ cd build
$ cmake -DDRIVER_PORTMASTER=TRUE ../
```

If you do not specify `-DDRIVER_PORTMASTER=TRUE`, cmake will try to use SFML libraries
rather than SDL2 libraries.

## Controls

The controls are shown on the title screen, but here's a copy if it's needed:

| Button | Action |
|--|--| 
|A|Select|
|B|Back|
|DPAD L, L1, L2, X|Rotate Left|
|DPAD R, R1, R2, Y|Rotate Right|
|Menu|Quit Game|



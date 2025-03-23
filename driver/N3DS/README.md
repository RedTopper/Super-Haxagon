# N3DS Driver

This driver supports the Nintendo 3DS, both old and new, using libctru, citro2d, citro3d, and stb for audio decoding.

## Building

A toolchain based on `devkitpro/devkitarm` is provided with the `Containerfile` in this directory. 

From within the `driver` directory, run the following commands:

```bash
podman-compose up --no-build 3DS
```

If you want to build the container yourself:

```bash
podman-compose up --build 3DS
```

If you do not have Podman or Docker:

* Install https://devkitpro.org/wiki/Getting_Started
* From the repo root, run the following commands:

```bash
mkdir build
cd build
/opt/devkitpro/portlibs/3ds/bin/arm-none-eabi-cmake ..
make -j4
```

# License

This driver is available under two licenses: GPLv3 (or later) OR MIT. 

You may choose which license to use:

* If this driver is used or linked against SuperHaxagon, you must use the GPLv3 license.
* If you are using this driver for an external project (for example, to port your own code to a different platform),
  you may choose to use the MIT license instead.

All licenses for this driver are provided in LICENSE.md within this directory.

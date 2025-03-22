# Switch Driver

This driver builds for the Nintendo Switch using libnx and other devkitPro libraries.

## Building

### Container Build:

The switch builds with `devkitpro/devkita64` provided on Dockerhub.

From within the parent directory, run the following commands:

```bash
podman-compose up --no-build Switch
```

### Native Build:

* Set up devkitPro: https://switchbrew.org/wiki/Setting_up_Development_Environment
* From the repo root, run the following commands:

```bash
mkdir build
cd build
/opt/devkitpro/portlibs/switch/bin/aarch64-none-elf-cmake ..
make -j4
```

## License

This driver is available under two licenses: GPLv3 (or later) OR MIT.

You may choose which license to use given the terms below.

* If this driver is used or linked with SuperHaxagon, you must use the GPLv3 license for this code.
* If you are using this driver for an external project (for example, to port your own code to a different platform),
  you _may_ choose to use the MIT license instead.

A copy of the MIT license is provided in LICENSE.md in this directory.

A copy of the GPLv3 license is provided at the root of the SuperHaxagon project.

A copy of third party libraries to make this driver possible is provided in the `licenses` folder.

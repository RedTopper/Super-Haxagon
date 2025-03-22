# TI-Nspire Driver

This driver supports the TI-Nspire.

## Building

### Container Build:

A toolchain based on `fedora:41` is provided with the `Containerfile` in this directory.

From within the parent directory, run the following commands:

```bash
podman-compose up --no-build Nspire
```

If you want to build the container yourself. This build takes a long time,
as you'll need to recompile GCC and related tools.

```bash
podman-compose up --build Nspire
```

### Native Build:

* Set up the Ndless SDK: https://hackspire.org/index.php/C_and_assembly_development_introduction
* From the repo root, run the following commands:

```bash
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../driver/Nspire/cmake/Nspire-TC.cmake ${CMAKE_ARGS} ..
make -j4
```

## License

This driver is available under two licenses: GPLv3 (or later) OR MIT.

You may choose which license to use:

* If this driver is used or linked against SuperHaxagon, you must use the GPLv3 license.
* If you are using this driver for an external project (for example, to port your own code to a different platform),
  you may choose to use the MIT license instead.

All licenses for this driver are provided in LICENSE.md within this directory.

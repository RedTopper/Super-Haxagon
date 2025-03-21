# Example Driver

This is a template for a driver that does nothing. Zilch. Nada.

Great for usage as a starting-point for porting this game to a new system.

This driver will be built first when creating a PR as a "sanity check",
so it will always compile.

## Building

### Container Build:

A toolchain based on `fedora:41` is provided with the `Containerfile` in this directory.

From within the parent directory, run the following commands:

```bash
podman-compose up --no-build Example
```

If you want to build the container yourself:

```bash
podman-compose up --build Example
```

### Native Build:

First, install the following tools: `gcc gcc-c++ cmake git`

Then, run the following commands from the repo root:

```bash
mkdir build
cd build
cmake -DDRIVER_EXAMPLE=TRUE ..
make -j4
```

## License

This driver is available under two licenses: GPLv3 (or later) OR MIT.

You may choose which license to use:

* If this driver is used or linked against SuperHaxagon, you must use the GPLv3 license.
* If you are using this driver for an external project (for example, to port your own code to a different platform),
  you may choose to use the MIT license instead.

All licenses for this driver are provided in LICENSE.md within this directory.

### For New Drivers

You may choose when upstreaming your driver (if you choose to upstream your driver, you are certainly not obligated to do so)
to license your driver under ONLY the GPLv3 (or later) license or BOTH the GPLv3 (or later) license and MIT, like above.

Existing drivers written by me (AJ Walter) are dual-licensed to help other people port their own code to new platforms.
If you'd like your driver to have stronger copy-left protections, consider only licensing under GPLv3 (or later).

Please note, any driver that links to SuperHaxagon must be _at least_ GPLv3 (or later).

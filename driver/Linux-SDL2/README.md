# Linux-SDL2 Driver

This driver supports Linux with SDL2 as a backend.

## Building

### Container Build:

A toolchain based on `fedora:41` is provided with the `Containerfile` in this directory.

From within the parent directory, run the following commands:

```bash
podman-compose up --no-build Linux-SDL2
```

If you want to build the container yourself:

```bash
podman-compose up --build Linux-SDL2
```

### Native Build:

First, install the following tools and dependencies for your distro:

`gcc gcc-c++ cmake git SDL2-devel SDL2_mixer-devel SDL2_ttf-devel`

Then, run the following commands from the repo root:

```bash
mkdir build
cd build
cmake -DDRIVER_SDL2=TRUE ..
make -j4
```

## License

This driver follows the license scheme of the parent driver at `driver/SDL2`.

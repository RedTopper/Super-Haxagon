# Linux Driver

This driver supports Linux with SFML2 as a backend.

## Building

### Container Build:

A toolchain based on `fedora:41` is provided with the `Containerfile` in this directory.

From within the parent directory, run the following commands:

```bash
podman-compose up --no-build Linux
```

If you want to build the container yourself:

```bash
podman-compose up --build Linux
```

### Native Build:

First, install the following tools: `gcc gcc-c++ cmake git`

Then, install SFML's dependencies: https://www.sfml-dev.org/faq/build-use/#link-static

> [!NOTE]  
> You can find examples of this for Fedora in the `Containerfile`, and examples for Debian in
> `.github/workflows/build-super-haxagon.yml`

Finally, run the following commands from the repo root:

```bash
mkdir build
cd build
cmake ..
make -j4
```

## License

This driver follows the license scheme of the parent driver at `driver/SFML`.

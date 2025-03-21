# Miyoo Mini Driver

This driver supports the Miyoo Mini with the SDL2 backend.

## Building

A toolchain based on `debian:buster` is provided with the `Containerfile` in this directory.

From within the parent directory, run the following commands:

```bash
podman-compose up --no-build Miyoo
```

If you want to build the container yourself:

```bash
podman-compose up --build Miyoo
```

## lib.tar.gz

The tree for lib.tar.gz is as follows:

```
lib
├── libEGL.so
├── libGLESv2.so
├── libjson-c.so.5
├── libneon.so
├── libSDL2-2.0.so.0
├── libSDL2_image-2.0.so.0
├── libSDL2_mixer-2.0.so.0
├── libSDL2_ttf-2.0.so.0
└── libz.so.1
```

You can download the libraries from:

https://github.com/RedTopper/Super-Haxagon/releases/tag/miyoo-libs

and place them here. You'll need to uncomment 

`COPY ./miyoo/lib.tar.gz ./` 

and comment out

`RUN wget ...` 

in the Containerfile to use your local copy instead.

## License

This driver follows the license scheme of the parent driver at `driver/SDL2`.

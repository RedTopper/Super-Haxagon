# lib.tar.gz

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

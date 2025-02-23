# lib.tar.gz

Looking for the libraries to build this for the Miyoo Mini?
It's the same "lib" folder that's distributed as a part of
the miyoo mini releases! Download the release, throw the
/lib directory in a lib.tar.gz, and plce it right here.

I'm trying to avoid adding somewhat large binaries to the
repo source, and since the releases need these libraries
anyway, I figured that would be the best place to store them

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

As long as you have all of the above, it will build and link properly.
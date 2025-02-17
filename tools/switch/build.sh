#!/bin/bash

/opt/devkitpro/portlibs/switch/bin/aarch64-none-elf-cmake ../git
make -j${JOBS}

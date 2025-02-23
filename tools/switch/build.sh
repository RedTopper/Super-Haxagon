#!/bin/bash

/opt/devkitpro/portlibs/switch/bin/aarch64-none-elf-cmake ${CMAKE_ARGS} ../../
make -j${JOBS}
make install

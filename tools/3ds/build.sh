#!/bin/bash

/opt/devkitpro/portlibs/3ds/bin/arm-none-eabi-cmake ${CMAKE_ARGS} ../../
make -j"$(nproc)"
make install
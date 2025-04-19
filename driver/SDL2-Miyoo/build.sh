#!/bin/bash

/usr/bin/cmake -DCMAKE_TOOLCHAIN_FILE=../../driver/SDL2-Miyoo/cmake/MiyooMini-TC.cmake ${CMAKE_ARGS} ../../
make -j"$(nproc)"
make install

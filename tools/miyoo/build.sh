#!/bin/bash

/usr/bin/cmake -DCMAKE_TOOLCHAIN_FILE=../../cmake/toolchains/Miyoo-TC.cmake ${CMAKE_ARGS} ../../
make -j"$(nproc)"
make install

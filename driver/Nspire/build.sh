#!/bin/bash

cmake -DCMAKE_TOOLCHAIN_FILE=../../driver/Nspire/cmake/Nspire-TC.cmake ${CMAKE_ARGS} ../../
make -j"$(nproc)"
make install

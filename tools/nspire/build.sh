#!/bin/bash

cmake -DCMAKE_TOOLCHAIN_FILE=/root/git/cmake/toolchains/Nspire-TC.cmake ${CMAKE_ARGS} ../../
make -j${JOBS}
make install

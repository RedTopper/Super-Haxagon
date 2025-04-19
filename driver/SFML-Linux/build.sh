#!/bin/bash

cmake ../../ ${CMAKE_ARGS} -DCMAKE_PREFIX_PATH="/root/sfml/build"

make -j"$(nproc)"
make install

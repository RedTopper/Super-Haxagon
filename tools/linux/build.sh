#!/bin/bash

cmake ../../ ${CMAKE_ARGS} \
    -DCMAKE_PREFIX_PATH="/root/sfml/build" \
    -DSFML_DIR="/root/sfml" \
    -DSFML_INCLUDE_DIR="/root/sfml/include" \
    -DSFML_STATIC_LIBRARIES="TRUE"

make -j${JOBS}
make install

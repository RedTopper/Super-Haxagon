#!/bin/bash

cmake ../git \
    -DCMAKE_PREFIX_PATH="/haxagon/sfml/build" \
    -DSFML_DIR="/haxagon/sfml" \
    -DSFML_INCLUDE_DIR="/haxagon/sfml/include" \
    -DSFML_STATIC_LIBRARIES="TRUE"

make -j${JOBS}

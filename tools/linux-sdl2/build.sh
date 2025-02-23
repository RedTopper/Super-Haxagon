#!/bin/bash

cmake -DDRIVER_SDL2=TRUE ${CMAKE_ARGS} ../../
make -j${JOBS}
make install

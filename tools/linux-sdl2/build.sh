#!/bin/bash

cmake -DDRIVER_SDL2=TRUE ${CMAKE_ARGS} ../../
make -j"$(nproc)"
make install

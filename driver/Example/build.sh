#!/bin/bash

cmake -DDRIVER_EXAMPLE=TRUE ${CMAKE_ARGS} ../../
make -j"$(nproc)"
make install

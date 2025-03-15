#!/bin/bash

cmake -DDRIVER_NULL=TRUE ${CMAKE_ARGS} ../../
make -j"$(nproc)"
make install

#!/bin/bash

cmake -DDRIVER_PORTMASTER=TRUE ${CMAKE_ARGS} ../../
make -j${JOBS}
make install

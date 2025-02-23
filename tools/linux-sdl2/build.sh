#!/bin/bash

cmake -DDRIVER_SDL2=TRUE ../git
make -j${JOBS}

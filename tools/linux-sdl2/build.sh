#!/bin/bash

cmake ../git -DDRIVER_SDL2=TRUE
make -j${JOBS}

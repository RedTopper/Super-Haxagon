#!/bin/bash

cmake ../git -DDRIVER_SDL2=TRUE -DPORTMASTER=TRUE
make -j${JOBS}

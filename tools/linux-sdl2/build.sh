#!/bin/bash

cmake ../git -DSKIP_SFML_BUILD=TRUE

make -j${JOBS}

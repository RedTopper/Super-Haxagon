#!/bin/bash

/usr/bin/cmake -DCMAKE_TOOLCHAIN_FILE=/haxagon/git/cmake/toolchains/Miyoo-TC.cmake ../git
make -j${JOBS}
tar -xzf /root/lib.tar.gz lib

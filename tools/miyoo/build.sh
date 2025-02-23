#!/bin/bash

/usr/bin/cmake -DCMAKE_TOOLCHAIN_FILE=/root/git/cmake/toolchains/Miyoo-TC.cmake ${CMAKE_ARGS} ../../
make -j${JOBS}
make install
tar -xzf /root/lib.tar.gz --directory /root/git/release/miyoo

#!/bin/bash

cmake -DCMAKE_TOOLCHAIN_FILE=/haxagon/git/cmake/toolchains/Nspire-TC.cmake ../git
make

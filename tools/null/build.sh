#!/bin/bash

cmake -DDRIVER_NULL=TRUE ../git
make -j${JOBS}

#!/bin/bash

cmake -DDRIVER_NULL=TRUE ../../
make -j${JOBS}

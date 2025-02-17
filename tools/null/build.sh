#!/bin/bash

cmake -DBUILD_NULL_DRIVER=true ../git
make -j${JOBS}

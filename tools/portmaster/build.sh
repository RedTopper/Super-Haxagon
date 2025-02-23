#!/bin/bash

cmake -DDRIVER_PORTMASTER=TRUE ../git
make -j${JOBS}

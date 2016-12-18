#pragma once

#include "header.h"

#ifdef __cplusplus
#include "font/source/BmpFont.h"
extern "C" {
#endif

void writeFont(Color color, Point point, char* string, int large);

#ifdef __cplusplus
}
#endif

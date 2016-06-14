#pragma once

#include <3ds.h>
#include <stdio.h>

#include "triangle.h"

#ifdef __cplusplus
#include "font/source/BmpFont.h"
#include "font/source/BmpFont.cpp"
extern "C" {
#endif

void writeFont(Point p, const char* s, bool large);
void freeFonts();

#ifdef __cplusplus
}
#endif
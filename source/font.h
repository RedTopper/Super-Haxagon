#pragma once

#include <3ds.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "triangle.h"

char* g_letters[6];

u8* g_font12;
u8* g_font24;

bool readFile(u8* buffer, const char* path);
bool initFont();
void write12(Point p, const char* s);

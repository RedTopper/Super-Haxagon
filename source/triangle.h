#pragma once

#include <3ds.h>
#include <sf2d.h>
#include <limits.h>

#define SCREEN_HEIGHT 240
#define TOP_WIDTH  400
#define BOT_WIDTH  320

typedef struct
{
  long x, y;
  u32 color;
} Point;

void drawTriangle(const Point points[3]);
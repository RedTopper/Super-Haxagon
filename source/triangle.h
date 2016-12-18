#pragma once

//#include <3ds.h>
//#include <sf2d.h>
//#include <limits.h>

typedef struct {
  int x, y;
} Point;

void drawTriangle(Color color, Point points[3]);
void drawTrap(Color color, Point points[4]);
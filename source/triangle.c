#include "triangle.h"

void drawTriangle(const Point points[3]) {
	sf2d_draw_triangle(points[0].x, SCREEN_HEIGHT - 1 - points[0].y,
					   points[1].x, SCREEN_HEIGHT - 1 - points[1].y,
					   points[2].x, SCREEN_HEIGHT - 1 - points[2].y,
					   points[0].color);
}
#include "triangle.h"

void drawTriangle(Color color, Point points[3]) {
	long paint = RGBA8(color.r,color.b,color.g,0xFF);
	
	//draws a triangle on the correct axis
	sf2d_draw_triangle(
		points[0].x, SCREEN_HEIGHT - 1 - points[0].y,
		points[1].x, SCREEN_HEIGHT - 1 - points[1].y,
		points[2].x, SCREEN_HEIGHT - 1 - points[2].y,
		paint);
}

void drawTrap(Color color, Point points[4]) {
	Point triangle[3];
	triangle[0] = points[0];
	triangle[1] = points[1];
	triangle[2] = points[2];
	drawTriangle(color, triangle);
	triangle[0] = points[0];
	triangle[1] = points[2];
	triangle[2] = points[3];
	drawTriangle(color, triangle);
}
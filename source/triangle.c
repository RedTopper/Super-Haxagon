#include "triangle.h"

void scanLine(int x1, int y1, int x2, int y2) {
	int sx, sy, dx1, dy1, dx2, dy2, x, y, m, n, k, cnt;

	sx = x2 - x1;
	sy = y2 - y1;

	if (sy < 0 || (sy == 0 && sx < 0)) {
		k = x1; x1 = x2; x2 = k;
		k = y1; y1 = y2; y2 = k;
		sx = -sx;
		sy = -sy;
	}

	if (sx > 0) dx1 = 1;
	else if (sx < 0) dx1 = -1;
	else dx1 = 0;

	if (sy > 0) dy1 = 1;
	else if (sy < 0) dy1 = -1;
	else dy1 = 0;

	m = ABS(sx);
	n = ABS(sy);
	dx2 = dx1;
	dy2 = 0;

	if (m < n) {
		m = ABS(sy);
		n = ABS(sx);
		dx2 = 0;
		dy2 = dy1;
	}

	x = x1; y = y1;
	cnt = m + 1;
	k = n / 2;

	while (cnt-- > 0) {
		if ((y >= 0) && (y < SCREEN_HEIGHT)) {
			if (x < ContourX[y][0]) ContourX[y][0] = x;
			if (x > ContourX[y][1]) ContourX[y][1] = x;
		}

		k += n;
		if (k < m) {
			x += dx2;
			y += dy2;
		} else {
			k -= m;
			x += dx1;
			y += dy1;
		}
	}
}

void drawTriangle(Point p0, Point p1, Point p2) {
	int y;

	for (y = 0; y < SCREEN_HEIGHT; y++) {
		ContourX[y][0] = TOP_WIDTH; // min X
		ContourX[y][1] = 0; // max X
	}

	scanLine(p0.x, p0.y, p1.x, p1.y);
	scanLine(p1.x, p1.y, p2.x, p2.y);
	scanLine(p2.x, p2.y, p0.x, p0.y);
	
	for (int y = 0; y < SCREEN_HEIGHT; y++) {
		if (ContourX[y][1] >= ContourX[y][0]) {
			int inverseY = SCREEN_HEIGHT - 1 - y;
			sf2d_draw_line(ContourX[y][0], inverseY, ContourX[y][1], inverseY, 1, RGBA8(p0.r, p0.g, p0.b, 0xFF));
		}
	}
}
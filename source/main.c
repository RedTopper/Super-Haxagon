#include <string.h>
#include <limits.h>
#include <math.h>
#include <3ds.h>

#define SCREEN_HEIGHT 240
#define TOP_WIDTH  400
#define BOT_WIDTH  320
#define ABS(x) ((x >= 0) ? x : -x)
#define TAU 6.28318530718

typedef struct {
	int x1;
	int y1;
	int x2;
	int y2;
	int x3;
	int y3;
} Triangle;

typedef struct
{
  long x, y;
  char r;
  char g;
  char b;
} Point;

long ContourX[SCREEN_HEIGHT][2];

/*
* Code borrowed from here:
* http://stackoverflow.com/questions/11139724/i-need-a-pixel-perfect-triangle-fill-algorithm-to-avoid-aliasing-artifacts
* 
* Scans a side of a triangle setting min X and max X in ContourX[][]
* (using the Bresenham's line drawing algorithm).
* I have no idea what is happening here.
*/
void scanLine(int x1, int y1, int x2, int y2) {
	int sx, sy, dx1, dy1, dx2, dy2, x, y, m, n, k, cnt;

	sx = x2 - x1;
	sy = y2 - y1;

	if (sy < 0 || sy == 0 && sx < 0) {
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

void setPixel(u8* fb, bool top, Point p) {
	if(top) {
		if(p.x < 0 || p.y < 0 || p.x >= TOP_WIDTH || p.y >= SCREEN_HEIGHT) {
			return;
		}
	} else {
		if(p.x < 0 || p.y < 0 || p.x >= BOT_WIDTH || p.y >= SCREEN_HEIGHT) {
			return;
		}
	}
	fb[3*(p.y+p.x*SCREEN_HEIGHT)] = p.b;
	fb[3*(p.y+p.x*SCREEN_HEIGHT) + 1] = p.g;
	fb[3*(p.y+p.x*SCREEN_HEIGHT) + 2] = p.r;
}

void drawTriangle(u8* fb, bool top, Point p0, Point p1, Point p2)
{
	Point trianglePoint;
	trianglePoint.r = p0.r;
	trianglePoint.g = p0.g;
	trianglePoint.b = p0.b;
	
	int y;

	for (y = 0; y < SCREEN_HEIGHT; y++) {
		int width = 0;
		if(top) {
			width = TOP_WIDTH;
		} else {
			width = BOT_WIDTH;
		}
		ContourX[y][0] = width; // min X
		ContourX[y][1] = 0; // max X
	}

	scanLine(p0.x, p0.y, p1.x, p1.y);
	scanLine(p1.x, p1.y, p2.x, p2.y);
	scanLine(p2.x, p2.y, p0.x, p0.y);
	
	for (trianglePoint.y = 0; trianglePoint.y < SCREEN_HEIGHT; trianglePoint.y++) {
		if (ContourX[trianglePoint.y][1] >= ContourX[trianglePoint.y][0]) {
			trianglePoint.x = ContourX[trianglePoint.y][0];
			int len = 1 + ContourX[trianglePoint.y][1] - ContourX[trianglePoint.y][0];

			// Can draw a horizontal line instead of individual pixels here
			while (len-- > 0) {
				trianglePoint.x++;
				setPixel(fb, top, trianglePoint);
			}
		}
	}
}

double compTimeTaken = 0.0;

int main()
{
	gfxInitDefault();
	//gfxSet3D(true); // uncomment if using stereoscopic 3D

	double radians = 0;
	double cursor = 0;
	
	while (aptMainLoop()) {
		hidScanInput();
		u32 kDown = hidKeysDown();
		if (kDown & KEY_START) break; // break in order to return to hbmenu
		
		if (kDown & KEY_L) cursor = (cursor + TAU/240.0);
		if (kDown & KEY_R) cursor = (cursor - TAU/240.0);
		if(cursor > TAU || cursor < 0) {
			cursor = 0;
		}

		u64 start_time = svcGetSystemTick ();
		
		////RENDER TOP SCREEN
		u8* fb = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);
		memset(fb, 0, SCREEN_HEIGHT*TOP_WIDTH*3);
		
		Point center;
		Point edges[6];
		center.x = TOP_WIDTH/2;
		center.y = SCREEN_HEIGHT/2;
		
		double FULL_LEN = 50;
		double BORDER_LEN = 10;
		
		//outer color
		center.r = 0xF6;
		center.g = 0x48;
		center.b = 0x13;
		for(int concentricHexes = 0; concentricHexes < 2; concentricHexes++) {
			for(int i = 0; i <= 6; i++) {
				if(i < 6) {
					edges[i].x = (int)((concentricHexes == 0 ? (double)FULL_LEN : (double)(FULL_LEN - BORDER_LEN)) * cos(radians + (double)i * TAU/6.0) + (double)TOP_WIDTH/2.0);
					edges[i].y = (int)((concentricHexes == 0 ? (double)FULL_LEN : (double)(FULL_LEN - BORDER_LEN)) * sin(radians + (double)i * TAU/6.0) + (double)SCREEN_HEIGHT/2.0);
				}
				if(i > 0) {
					drawTriangle(fb, true, center, edges[i-1], (i==6 ? edges[0] : edges[i]));
				}
			}
			//inner color
			center.r = 0x50;
			center.g = 0x0C;
			center.b = 0x01;		
		}
		
		////RENDER BOTTOM SCREEN
		fb = gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, NULL, NULL);
		memset(fb, 0, SCREEN_HEIGHT*BOT_WIDTH*3);
		
		Point time;
		time.r = 0xFF;
		time.g = 0x00;
		time.b = 0x00;
		time.y = 0;
		for(time.x = 0; time.x < (int)((double)BOT_WIDTH * compTimeTaken); time.x++) {
			setPixel(fb, false, time);
		}
		
		u64 end_time = svcGetSystemTick ();

		gfxFlushBuffers();
		gfxSwapBuffers();
		gspWaitForVBlank();
		
		u64 end_screen = svcGetSystemTick ();
		compTimeTaken = (double)(end_time - start_time) / (double)(end_screen - start_time);
		
		radians = (radians + TAU/240.0);
		if(radians > TAU) {
			radians = 0.0;
		}
	}

	gfxExit();
	return 0;
}

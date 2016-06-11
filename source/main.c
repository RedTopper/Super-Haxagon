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

int main()
{
	//Used to calculate the lagometer.
	double compTimeTaken = 0.0;
	
	//Default color of the background.
	Point BACKGROUND_COLOR;
	BACKGROUND_COLOR.r = 0x50;
	BACKGROUND_COLOR.g = 0x0C;
	BACKGROUND_COLOR.b = 0x01;
	
	//Default color of the second background.
	Point BACKGROUND_COLOR_2;
	BACKGROUND_COLOR_2.r = 0x61;
	BACKGROUND_COLOR_2.g = 0x12;
	BACKGROUND_COLOR_2.b = 0x01;

	//Default color of the foreground.
	Point FOREGROUND_COLOR;	
	FOREGROUND_COLOR.r = 0xF6;
	FOREGROUND_COLOR.g = 0x48;
	FOREGROUND_COLOR.b = 0x13;
	
	//Inside hexagon style
	double FULL_LEN = 30.0;
	double BORDER_LEN = 5.0;
	
	//Human triangle style
	double HUMAN_WIDTH = TAU/30;
	double HUMAN_HEIGHT = 5.0;
	double HUMAN_PADDING = 5.0;
	double HUMAN_STEP = TAU/120.0;
	
	gfxInitDefault();
	//gfxSet3D(true); // uncomment if using stereoscopic 3D

	double radians = 0;
	double cursor = 0;
	
	while (aptMainLoop()) {
		hidScanInput();
		u32 kDown = hidKeysDown();
		if(kDown & KEY_START) break; // break in order to return to hbmenu
		
		u32 kHold = hidKeysHeld();
		if(kHold & KEY_L) cursor = (cursor + HUMAN_STEP);
		if(kHold & KEY_R) cursor = (cursor - HUMAN_STEP);
		if(cursor > TAU) cursor-=TAU;
		if(cursor < 0) cursor+=TAU;

		u64 start_time = svcGetSystemTick ();
		
		////RENDER TOP SCREEN
		u8* fb = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);
		for(BACKGROUND_COLOR.x = 0; BACKGROUND_COLOR.x < TOP_WIDTH; BACKGROUND_COLOR.x++) {
			for(BACKGROUND_COLOR.y = 0; BACKGROUND_COLOR.y < SCREEN_HEIGHT; BACKGROUND_COLOR.y++) {
				setPixel(fb, true, BACKGROUND_COLOR);
			}
		}
		
		//This draws the main background and hexagon.
		Point center;
		Point edges[6];
		center.x = TOP_WIDTH/2;
		center.y = SCREEN_HEIGHT/2;
		for(int concentricHexes = 0; concentricHexes < 3; concentricHexes++) {
			double len = 0.0;
			if(concentricHexes == 0) {
				//second background painted first
				center.r = BACKGROUND_COLOR_2.r;
				center.g = BACKGROUND_COLOR_2.g;
				center.b = BACKGROUND_COLOR_2.b;
				len = TOP_WIDTH / 1.5;
			}
			if(concentricHexes == 1) {
				//outer color painted after
				center.r = FOREGROUND_COLOR.r;
				center.g = FOREGROUND_COLOR.g;
				center.b = FOREGROUND_COLOR.b;
				len = FULL_LEN;
			}
			if(concentricHexes == 2) {
				//inner color painted over it all.
				center.r = BACKGROUND_COLOR.r;
				center.g = BACKGROUND_COLOR.g;
				center.b = BACKGROUND_COLOR.b;
				len = FULL_LEN - BORDER_LEN;
			}
			for(int i = 0; i <= 6; i++) {
				if(i < 6) {
					edges[i].x = (int)(len * cos(radians + (double)i * TAU/6.0) + center.x);
					edges[i].y = (int)(len * sin(radians + (double)i * TAU/6.0) + center.y);
				}
				if(i > 0) {
					if(concentricHexes == 0) {
						if(i % 2 == 0) {
							continue;
						}
					}
					drawTriangle(fb, true, center, edges[i-1], (i==6 ? edges[0] : edges[i]));
				}
			}	
		}
		
		//This draws the human cursor.
		Point humanTriangle[3];
		humanTriangle[0].r = FOREGROUND_COLOR.r;
		humanTriangle[0].g = FOREGROUND_COLOR.g;
		humanTriangle[0].b = FOREGROUND_COLOR.b;
		for(int i = 0; i < 3; i++) {
			double len = 0.0;
			double position = 0.0;
			if(i == 0) {
				len = FULL_LEN + HUMAN_PADDING + HUMAN_HEIGHT;
				position = cursor + radians;
			} else {
				len = FULL_LEN + HUMAN_PADDING;
				if(i==1) {
					position = cursor + HUMAN_WIDTH/2 + radians;
				} else {
					position = cursor - HUMAN_WIDTH/2 + radians;
				}
			}
			humanTriangle[i].x = (int)(len * cos(position) + center.x);
			humanTriangle[i].y = (int)(len * sin(position) + center.y);
		}
		drawTriangle(fb, true, humanTriangle[0], humanTriangle[1], humanTriangle[2]);
		
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
		
		////FLUSH AND CALC LAGOMETER
		u64 end_time = svcGetSystemTick ();
		gfxFlushBuffers();
		gfxSwapBuffers();
		gspWaitForVBlank();
		u64 end_screen = svcGetSystemTick ();
		compTimeTaken = (double)(end_time - start_time) / (double)(end_screen - start_time);
		
		////ROTATE
		radians = (radians + TAU/240.0);
		if(radians > TAU) {
			radians -= TAU;
		}
	}

	gfxExit();
	return 0;
}

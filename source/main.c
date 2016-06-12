#include <string.h>
#include <limits.h>
#include <math.h>
#include <3ds.h>

#include "triangle.h"

#define TAU 6.28318530718
#define FG 0
#define BG1 1
#define BG2 2

Point levelColor[2][3];

////STATIC VARS
//Inside hexagon style
const double FULL_LEN = 30.0;
const double BORDER_LEN = 5.0;

//Human triangle style
const double HUMAN_WIDTH = TAU/30;
const double HUMAN_HEIGHT = 5.0;
const double HUMAN_PADDING = 5.0;

//Hexagon Constants
const int FRAMES_PER_ONE_SIDE_ROTATION = 10; //Take rotationStep into consideration when changing this!!!

////DYNAMIC VARS
int transition; //0 = no transition // 1 = forwards // -1 = backwards
int transitionFrame;
double rotationOffset;
double rotationStep;
double humanStep;
int level;
int levelLast;
int totalLevels;


void init() {
	////DYNAMIC VARS
	transition = 0;
	transitionFrame = 0;
	rotationOffset = 0.0;
	rotationStep = TAU/60.0;
	humanStep = TAU/60.0;
	level = 0;
	levelLast = 0;
	totalLevels = sizeof(levelColor)/sizeof(levelColor[0]);
	
	////LEVELS
	//Level 0
	levelColor[0][FG].r = 0xF6;
	levelColor[0][FG].g = 0x48;
	levelColor[0][FG].b = 0x13;
	
	levelColor[0][BG1].r = 0x50;
	levelColor[0][BG1].g = 0x0C;
	levelColor[0][BG1].b = 0x01;
	
	levelColor[0][BG2].r = 0x61;
	levelColor[0][BG2].g = 0x12;
	levelColor[0][BG2].b = 0x01;
	
	//Level 1
	levelColor[1][FG].r = 0x33;
	levelColor[1][FG].g = 0xE5;
	levelColor[1][FG].b = 0x66;
	
	levelColor[1][BG1].r = 0x08;
	levelColor[1][BG1].g = 0x24;
	levelColor[1][BG1].b = 0x10;
	
	levelColor[1][BG2].r = 0x00;
	levelColor[1][BG2].g = 0x00;
	levelColor[1][BG2].b = 0x00;
}

Point tweenColor(Point original, Point new, int frame) {
	Point returnPoint;
	returnPoint.r = (char)((double)(new.r - original.r) * ((double)frame / (double)FRAMES_PER_ONE_SIDE_ROTATION) + (double)original.r);
	returnPoint.g = (char)((double)(new.g - original.g) * ((double)frame / (double)FRAMES_PER_ONE_SIDE_ROTATION) + (double)original.g);
	returnPoint.b = (char)((double)(new.b - original.b) * ((double)frame / (double)FRAMES_PER_ONE_SIDE_ROTATION) + (double)original.b);
	return returnPoint;
}

void doMainMenu() {
	double radians = (double)transitionFrame / (double)FRAMES_PER_ONE_SIDE_ROTATION * TAU/6.0;
	if(transition == -1) { //if the user is going to the left, flip the radians so the animation plays backwards.
		radians *= -1;
	}
	if(levelLast % 2 == 1) { //We need to offset the hexagon if the selection is odd. Otherwise the bg colors flip!
		radians += TAU/6.0;
	}
	
	u32 kHold = hidKeysHeld();
	if(!transition) {
		if(kHold & KEY_R) {
			level++;
			transition = 1;
		} 
		if(kHold & KEY_L) {
			level--;
			transition = -1;
		} 
	}
	if(level < 0) level = totalLevels - 1;
	if(level >= totalLevels) level = 0;
	
	////CALCULATE COLORS
	Point fg;
	Point bg1;
	Point bg2;
	if(transition && transitionFrame < FRAMES_PER_ONE_SIDE_ROTATION) {
		fg = tweenColor(levelColor[levelLast][FG], levelColor[level][FG], transitionFrame);
		bg1 = tweenColor(levelColor[levelLast][BG1], levelColor[level][BG1], transitionFrame);
		bg2 = tweenColor(levelColor[levelLast][BG2], levelColor[level][BG2], transitionFrame);
		transitionFrame++;
	} else {
		levelLast = level;
		transitionFrame = 0;
		transition = false;
	}
	if(!transition) {
		fg = levelColor[level][FG];
		bg1 = levelColor[level][BG1];
		bg2 = levelColor[level][BG2];
	} 
	
	////RENDER TOP SCREEN
	u8* fb = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);
	for(bg1.x = 0; bg1.x < TOP_WIDTH; bg1.x++) {
		for(bg1.y = 0; bg1.y < SCREEN_HEIGHT; bg1.y++) {
			setPixel(fb, true, bg1);
		}
	}
	
	//This draws the main background and hexagon.
	Point center;
	Point edges[6];
	center.x = TOP_WIDTH/2;
	center.y = SCREEN_HEIGHT/2 - 40;
	for(int concentricHexes = 0; concentricHexes < 3; concentricHexes++) {
		double len = 0.0;
		if(concentricHexes == 0) {
			//second background painted first
			center.r = bg2.r;
			center.g = bg2.g;
			center.b = bg2.b;
			len = TOP_WIDTH / 1.5;
		}
		if(concentricHexes == 1) {
			//outer color painted after
			center.r = fg.r;
			center.g = fg.g;
			center.b = fg.b;
			len = FULL_LEN;
		}
		if(concentricHexes == 2) {
			//inner color painted over it all.
			center.r = bg1.r;
			center.g = bg1.g;
			center.b = bg1.b;
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
	
	//Main menu cursor is fixed in the up position.
	double cursor = TAU/4;
	humanTriangle[0].r = fg.r;
	humanTriangle[0].g = fg.g;
	humanTriangle[0].b = fg.b;
	for(int i = 0; i < 3; i++) {
		double len = 0.0;
		double position = 0.0;
		if(i == 0) {
			len = FULL_LEN + HUMAN_PADDING + HUMAN_HEIGHT;
			position = cursor;
		} else {
			len = FULL_LEN + HUMAN_PADDING;
			if(i==1) {
				position = cursor + HUMAN_WIDTH/2;
			} else {
				position = cursor - HUMAN_WIDTH/2;
			}
		}
		humanTriangle[i].x = (int)(len * cos(position) + center.x);
		humanTriangle[i].y = (int)(len * sin(position) + center.y);
	}
	drawTriangle(fb, true, humanTriangle[0], humanTriangle[1], humanTriangle[2]);
}

int main()
{
	init();
	//Used to calculate the lagometer.
	double compTimeTaken = 0.0;
	
	gfxInitDefault();
	//gfxSet3D(true); // uncomment if using stereoscopic 3D
	
	while (aptMainLoop()) {
		hidScanInput();
		u32 kDown = hidKeysDown();
		if(kDown & KEY_START) break; // break in order to return to hbmenu
		u64 start_time = svcGetSystemTick ();
		
		////DRAW MENU
		doMainMenu();
		
		////RENDER BOTTOM SCREEN
		u8* fb = gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, NULL, NULL);
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
	}

	gfxExit();
	return 0;
}

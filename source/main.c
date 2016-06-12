#include <string.h>
#include <limits.h>
#include <math.h>
#include <3ds.h>
#include <sf2d.h>

#include "triangle.h"
#include "font.h"

#define TAU 6.28318530718
#define FG 0
#define BG1 1
#define BG2 2

Point levelColor[6][3];

////STATIC VARS
//Inside hexagon style
const double FULL_LEN = 30.0;
const double BORDER_LEN = 5.0;

//Human triangle style
const double HUMAN_WIDTH = TAU/30;
const double HUMAN_HEIGHT = 5.0;
const double HUMAN_PADDING = 5.0;

//Hexagon Constants
const int FRAMES_PER_ONE_SIDE_ROTATION = 10;

////DYNAMIC VARS. g_ means global btw.
int g_transition; //0 = no transition // 1 = forwards // -1 = backwards
int g_transitionFrame;
double g_rotationOffset;
double g_rotationStep;
double g_humanStep;
int g_level;
int g_levelLast;
int g_totallevels;
double g_fps; //Used to calculate the lagometer.

sf2d_texture* g_top;
sf2d_texture* g_bot;

void init() {
	////DYNAMIC VARS
	g_transition = 0;
	g_transitionFrame = 0;
	g_rotationOffset = 0.0;
	g_rotationStep = TAU/60.0;
	g_humanStep = TAU/60.0;
	g_level = 0;
	g_levelLast = 0;
	g_totallevels = sizeof(levelColor)/sizeof(levelColor[0]);
	g_fps = 0.0;
	
	sf2d_texfmt format = TEXFMT_RGBA8;
	sf2d_place place = SF2D_PLACE_RAM;
	g_top = sf2d_create_texture(TOP_WIDTH, SCREEN_HEIGHT, format, place);
	sf2d_texture_tile32(g_top);
	g_bot = sf2d_create_texture(BOT_WIDTH, 1, format, place); //Just for lagometer.
	sf2d_texture_tile32(g_bot);
}

void initLevels() {
	//level 0
	levelColor[0][FG].r = 0xF6;		levelColor[0][BG1].r = 0x50;		levelColor[0][BG2].r = 0x61;
	levelColor[0][FG].g = 0x48;		levelColor[0][BG1].g = 0x0C;		levelColor[0][BG2].g = 0x12;
	levelColor[0][FG].b = 0x13;		levelColor[0][BG1].b = 0x01;		levelColor[0][BG2].b = 0x01;
	
	//level 1
	levelColor[1][FG].r = 0x33;		levelColor[1][BG1].r = 0x08;		levelColor[1][BG2].r = 0x00;
	levelColor[1][FG].g = 0xE5;		levelColor[1][BG1].g = 0x24;		levelColor[1][BG2].g = 0x00;
	levelColor[1][FG].b = 0x66;		levelColor[1][BG1].b = 0x10;		levelColor[1][BG2].b = 0x00;
	
	//level 2
	levelColor[2][FG].r = 0x6D;		levelColor[2][BG1].r = 0x22;		levelColor[2][BG2].r = 0x18;
	levelColor[2][FG].g = 0x10;		levelColor[2][BG1].g = 0x00;		levelColor[2][BG2].g = 0x00;
	levelColor[2][FG].b = 0xF9;		levelColor[2][BG1].b = 0x63;		levelColor[2][BG2].b = 0x52;
	
	//level 3
	levelColor[3][FG].r = 0xF1;		levelColor[3][BG1].r = 0x63;		levelColor[3][BG2].r = 0x52;
	levelColor[3][FG].g = 0xF9;		levelColor[3][BG1].g = 0x60;		levelColor[3][BG2].g = 0x4C;
	levelColor[3][FG].b = 0x10;		levelColor[3][BG1].b = 0x01;		levelColor[3][BG2].b = 0x00;
	
	//level 4
	levelColor[4][FG].r = 0xF4;		levelColor[4][BG1].r = 0xFF;		levelColor[4][BG2].r = 0xFD;
	levelColor[4][FG].g = 0x05;		levelColor[4][BG1].g = 0xFF;		levelColor[4][BG2].g = 0xD7;
	levelColor[4][FG].b = 0x86;		levelColor[4][BG1].b = 0xFF;		levelColor[4][BG2].b = 0xEC;
	
	//level 5
	levelColor[5][FG].r = 0xFF;		levelColor[5][BG1].r = 0x00;		levelColor[5][BG2].r = 0x00;
	levelColor[5][FG].g = 0xFF;		levelColor[5][BG1].g = 0x00;		levelColor[5][BG2].g = 0x00;
	levelColor[5][FG].b = 0xFF;		levelColor[5][BG1].b = 0x00;		levelColor[5][BG2].b = 0x00;
}

void fillBuffer(sf2d_texture* fb, Point p) {
	int width = 0;
	if(sf2d_get_current_screen() == GFX_TOP) {
		width = TOP_WIDTH;
	} else {
		width = BOT_WIDTH;
	}
	for(p.x = 0; p.x < width; p.x++) {
		for(p.y = 0; p.y < SCREEN_HEIGHT; p.y++) {
			setPixel(fb, p);
		}
	}
}

Point tweenColor(Point original, Point new, int frame) {
	Point returnPoint;
	returnPoint.r = (char)((double)(new.r - original.r) * ((double)frame / (double)FRAMES_PER_ONE_SIDE_ROTATION) + (double)original.r);
	returnPoint.g = (char)((double)(new.g - original.g) * ((double)frame / (double)FRAMES_PER_ONE_SIDE_ROTATION) + (double)original.g);
	returnPoint.b = (char)((double)(new.b - original.b) * ((double)frame / (double)FRAMES_PER_ONE_SIDE_ROTATION) + (double)original.b);
	return returnPoint;
}

void doMainMenu() {
	double radians = (double)g_transitionFrame / (double)FRAMES_PER_ONE_SIDE_ROTATION * TAU/6.0;
	if(g_transition == -1) { //if the user is going to the left, flip the radians so the animation plays backwards.
		radians *= -1;
	}
	if(g_levelLast % 2 == 1) { //We need to offset the hexagon if the selection is odd. Otherwise the bg colors flip!
		radians += TAU/6.0;
	}
	
	u32 kHold = hidKeysHeld();
	if(!g_transition) {
		if(kHold & KEY_R) {
			g_level++;
			g_transition = 1;
		} 
		if(kHold & KEY_L) {
			g_level--;
			g_transition = -1;
		} 
	}
	if(g_level < 0) g_level = g_totallevels - 1;
	if(g_level >= g_totallevels) g_level = 0;
	
	////CALCULATE COLORS
	Point fg;
	Point bg1;
	Point bg2;
	if(g_transition && g_transitionFrame < FRAMES_PER_ONE_SIDE_ROTATION) {
		fg = tweenColor(levelColor[g_levelLast][FG], levelColor[g_level][FG], g_transitionFrame);
		bg1 = tweenColor(levelColor[g_levelLast][BG1], levelColor[g_level][BG1], g_transitionFrame);
		bg2 = tweenColor(levelColor[g_levelLast][BG2], levelColor[g_level][BG2], g_transitionFrame);
		g_transitionFrame++;
	} else {
		g_levelLast = g_level;
		g_transitionFrame = 0;
		g_transition = false;
	}
	if(!g_transition) {
		fg = levelColor[g_level][FG];
		bg1 = levelColor[g_level][BG1];
		bg2 = levelColor[g_level][BG2];
	} 
	sf2d_set_clear_color(RGBA8(bg1.r, bg1.g, bg1.b, 0xFF));
	
	////RENDER TOP SCREEN
	sf2d_start_frame(GFX_TOP, GFX_LEFT);
	////NOT THE RIGHT WAY TO DO THIS!!!!! HELP, I HAVE NO IDEA IF THIS COULD RUIN ANYTHING!!!!!
	memset(g_top->data, 0, g_top->pow2_w * g_top->pow2_h * 4);
	
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
				drawTriangle(g_top, center, edges[i-1], (i==6 ? edges[0] : edges[i]));
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
	drawTriangle(g_top, humanTriangle[0], humanTriangle[1], humanTriangle[2]);
	sf2d_draw_texture(g_top, 0, 0);
	Point p;
	writeFont(p,"Meme");
	sf2d_end_frame();
}

void doLagometer() {
	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
	Point time;
	time.r = 0x00;
	time.g = 0x00;
	time.b = 0x00;
	time.y = SCREEN_HEIGHT - 1;
	for(time.x = 0; time.x < BOT_WIDTH; time.x++) {
		setPixel(g_bot, time);
	}
	time.r = 0xFF;
	time.g = 0x00;
	time.b = 0x00;
	for(time.x = 0; time.x < (int)((double)BOT_WIDTH * ((double)g_fps/60.0)); time.x++) {
		setPixel(g_bot, time);
	}
	sf2d_draw_texture(g_bot, 0, SCREEN_HEIGHT - 1);
	sf2d_end_frame();
}

int main() {
	//3ds init
	gfxInitDefault();
	sf2d_init();
	sf2d_set_vblank_wait(1);
	romfsInit();
	
	//program init
	init();
	initLevels();
	
	while (aptMainLoop()) {
		hidScanInput();
		u32 kDown = hidKeysDown();
		if(kDown & KEY_START) break; // break in order to return to hbmenu
		
		////DRAW MENU
		doMainMenu();
		
		////DRAW LAGOMETER
		doLagometer();
		
		////FLUSH AND CALC LAGOMETER
		sf2d_swapbuffers();
		g_fps = sf2d_get_fps();
	}
	
	sf2d_fini();

	romfsExit();
	gfxExit();
	return 0;
}

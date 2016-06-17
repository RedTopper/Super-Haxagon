#include <string.h>
#include <limits.h>
#include <math.h>
#include <3ds.h>
#include <sf2d.h>

#include "triangle.h"
#include "font.h"
#include "levels.h"
#include "sound.h"

#define ABS(x) ((x >= 0) ? x : -x)
#define TAU 6.28318530718
#define FG 0
#define BG1 1
#define BG2 2

Point levelColor[TOTAL_LEVELS][3];

////STATIC VARS
//Inside hexagon style
const double FULL_LEN = 24.0;
const double BORDER_LEN = 4.0;

//Human triangle style
const double HUMAN_WIDTH = TAU/30;
const double HUMAN_HEIGHT = 5.0;
const double HUMAN_PADDING = 5.0;

//Hexagon Constants
const int FRAMES_PER_ONE_SIDE_ROTATION = 12;

//Overflow so you don't get glitchy lines between hexagons.
const double OVERFLOW_OFFSET = TAU/600.0;

//Screen (calculated with a physical calculator)
int TOP_SCREEN_DIAG_CENTER = 257;

////DYNAMIC VARS. g_ means global btw.
int g_renderedWalls;
int g_score;
int g_transition; //0 = no transition // 1 = forwards // -1 = backwards
int g_transitionFrame;
int g_level;
int g_levelLast;
double g_fps; //Used to calculate the lagometer.
bool g_playing;

void init() {
	////DYNAMIC VARS
	g_renderedWalls = 0;
	g_score = 0;
	g_transition = 0;
	g_transitionFrame = 0;
	g_level = 0;
	g_levelLast = 0;
	g_fps = 0.0;
	g_playing = false;
}

void initLevels() {
	//level 0
	levelColor[0][FG].color = RGBA8(0xF6, 0x48, 0x13, 0xFF);
	levelColor[0][BG1].color = RGBA8(0x50, 0x0C, 0x01, 0xFF);
	levelColor[0][BG2].color = RGBA8(0x61, 0x12, 0x01, 0xFF);
	
	//level 1
	levelColor[1][FG].color = RGBA8(0x33, 0xE5, 0x66, 0xFF);
	levelColor[1][BG1].color = RGBA8(0x08, 0x24, 0x10, 0xFF);
	levelColor[1][BG2].color = RGBA8(0x00, 0x00, 0x00, 0xFF);
	
	//level 2
	levelColor[2][FG].color = RGBA8(0x6D, 0x10, 0xF9, 0xFF);
	levelColor[2][BG1].color = RGBA8(0x22, 0x00, 0x63, 0xFF);
	levelColor[2][BG2].color = RGBA8(0x18, 0x00, 0x52, 0xFF);
	
	//level 3
	levelColor[3][FG].color = RGBA8(0xF1, 0xF9, 0x10, 0xFF);
	levelColor[3][BG1].color = RGBA8(0x63, 0x60, 0x01, 0xFF);
	levelColor[3][BG2].color = RGBA8(0x52, 0x4C, 0x00, 0xFF);
	
	//level 4
	levelColor[4][FG].color = RGBA8(0xF4, 0x05, 0x86, 0xFF);
	levelColor[4][BG1].color = RGBA8(0xFF, 0xFF, 0xFF, 0xFF);
	levelColor[4][BG2].color = RGBA8(0xFD, 0xD7, 0xEC, 0xFF);
	
	//level 5
	levelColor[5][FG].color = RGBA8(0xFF, 0xFF, 0xFF, 0xFF);
	levelColor[5][BG1].color = RGBA8(0x00, 0x00, 0x00, 0xFF);
	levelColor[5][BG2].color = RGBA8(0x00, 0x00, 0x00, 0xFF);
}

u32 tweenColor(u32 original, u32 new, int frame) {
	int red = (double)((int)RGBA8_GET_R(new) - (int)RGBA8_GET_R(original)) * ((double)frame / (double)FRAMES_PER_ONE_SIDE_ROTATION) + (double)RGBA8_GET_R(original);
	int green = (double)((int)RGBA8_GET_G(new) - (int)RGBA8_GET_G(original)) * ((double)frame / (double)FRAMES_PER_ONE_SIDE_ROTATION) + (double)RGBA8_GET_G(original);
	int blue = (double)((int)RGBA8_GET_B(new) - (int)RGBA8_GET_B(original)) * ((double)frame / (double)FRAMES_PER_ONE_SIDE_ROTATION) + (double)RGBA8_GET_B(original);
	return RGBA8(red, green, blue, 0xFF);
}

void drawWall(Point center, Point fg, int distanceFromCenter, int length, int side, double radians) {
	if(distanceFromCenter + length < FULL_LEN) return; //too close
	if(sqrt(pow((double)distanceFromCenter * cos(TAU/8.0),2) + pow((double)distanceFromCenter * sin(TAU/8.0),2)) > TOP_SCREEN_DIAG_CENTER) return; //too far
	if(distanceFromCenter < FULL_LEN - 2/*To make sure it draws correctly*/) {
		distanceFromCenter = FULL_LEN - 2; //Should never be 0!!!
	}
	Point edges[4];
	edges[0].color = fg.color;
	for(int i = 0; i < 4; i++) {
		int addLength = (i == 1 || i == 2 ? length : 0);
		double leftOrRightSide = (i == 0 || i == 1 ? side/*left*/ : side + 1/*right*/);
		double offset = (i == 0 || i == 1 ? -OVERFLOW_OFFSET/*left*/ : OVERFLOW_OFFSET/*right*/);
		edges[i].x = (int)(((double)(distanceFromCenter + addLength) * cos(radians + leftOrRightSide * TAU/6.0 + offset) + (double)(center.x)));
		edges[i].y = (int)(((double)(distanceFromCenter + addLength) * sin(radians + leftOrRightSide * TAU/6.0 + offset) + (double)(center.y)));
	}
	g_renderedWalls++;
	drawPoly(edges, 4);
}

void drawMainHexagon(Point center, Point fg, Point bg, double radians) {
	//This draws the hexagon.
	Point edges[6];
	for(int concentricHexes = 0; concentricHexes < 2; concentricHexes++) {
		double len = 0.0;
		if(concentricHexes == 0) {
			//outer color painted after
			edges[0].color = fg.color;
			len = FULL_LEN;
		}
		if(concentricHexes == 1) {
			//inner color painted over it all.
			edges[0].color = bg.color;
			len = FULL_LEN - BORDER_LEN;
		}
		for(int i = 0; i < 6; i++) {
			edges[i].x = (int)(len * cos(radians + (double)i * TAU/6.0) + center.x);
			edges[i].y = (int)(len * sin(radians + (double)i * TAU/6.0) + center.y);
		}
		drawPoly(edges, 6);
	}
}

void drawBackground(Point center, Point bg, double len, double radians) {
	//This draws the main background.
	Point edges[6];
	for(int i = 0; i < 6; i++) {
		edges[i].x = (int)(len * cos(radians + (double)i * TAU/6.0) + center.x);
		edges[i].y = (int)(len * sin(radians + (double)i * TAU/6.0) + center.y);
	}
	
	Point triangle[3];
	triangle[0] = center;
	triangle[0].color = bg.color;
	for(int i = 0; i < 6; i = i + 2) {
		triangle[1] = edges[i];
		triangle[2] = edges[i + 1];
		drawPoly(triangle, 3);
	}
}

void drawHumanCursor(Point center, Point fg, double cursor, double radians) {
	//This draws the human cursor.
	Point humanTriangle[3];
	humanTriangle[0].color = fg.color;
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
	drawPoly(humanTriangle, 3);
}

int doMainMenu() {
	double radians = (double)g_transitionFrame / (double)FRAMES_PER_ONE_SIDE_ROTATION * TAU/6.0;
	if(g_transition == -1) { //if the user is going to the left, flip the radians so the animation plays backwards.
		radians *= -1;
	}
	if(g_levelLast % 2 == 1) { //We need to offset the hexagon if the selection is odd. Otherwise the bg colors flip!
		radians += TAU/6.0;
	}
	
	u32 kDown = hidKeysDown();
	u32 kHold = hidKeysHeld();
	if(!g_transition) {
		if(kDown & KEY_A) {
			return g_level;
		} 
		if(kHold & KEY_R) {
			g_level++;
			g_transition = 1;
			audioPlay(&g_select, false);
		} 
		if(kHold & KEY_L) {
			g_level--;
			g_transition = -1;
			audioPlay(&g_select, false);
		} 
	}
	if(g_level < 0) g_level = TOTAL_LEVELS - 1;
	if(g_level >= TOTAL_LEVELS) g_level = 0;
	
	////CALCULATE COLORS
	Point fg;
	Point bg1;
	Point bg2;
	if(g_transition && g_transitionFrame < FRAMES_PER_ONE_SIDE_ROTATION) {
		fg.color = tweenColor(levelColor[g_levelLast][FG].color, levelColor[g_level][FG].color, g_transitionFrame);
		bg1.color = tweenColor(levelColor[g_levelLast][BG1].color, levelColor[g_level][BG1].color, g_transitionFrame);
		bg2.color = tweenColor(levelColor[g_levelLast][BG2].color, levelColor[g_level][BG2].color, g_transitionFrame);
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
	sf2d_set_clear_color(bg1.color);
	
	////RENDER TOP SCREEN
	sf2d_start_frame(GFX_TOP, GFX_LEFT);
	
	Point center;
	center.x = TOP_WIDTH/2;
	center.y = SCREEN_HEIGHT/2 - 40;
	
	drawBackground(center, bg2, TOP_WIDTH / 1.5, radians); //1.5 is used so the renderer doesn't go overboard. FIXME: magic number.
	drawMainHexagon(center, fg, bg1, radians);
	drawHumanCursor(center, fg, TAU/4.0, 0); //Draw cursor fixed quarter circle, no movement.

	Point p;
	p.x = 4;
	p.y = 4;
	p.color = RGBA8(0xFF,0xFF,0xFF,0xFF);
	
	Point sub;
	sub.x = 4;
	sub.y = 40;
	sub.color = RGBA8(0xFF,0xFF,0xFF,0xFF);
	
	Point sub2;
	sub2.x = 4;
	sub2.y = 56;
	sub2.color = RGBA8(0xFF,0xFF,0xFF,0xFF);
	
	Point time;
	time.x = 4;
	time.y = SCREEN_HEIGHT - 18;
	time.color = RGBA8(0xFF,0xFF,0xFF,0xFF);
	
	sf2d_draw_rectangle(0,0,TOP_WIDTH, sub2.y + 16 + 2, RGBA8(0, 0, 0, 0xFF));
	sf2d_draw_rectangle(0,time.y - 4,11/*chars*/ * 16, 16 + 8, RGBA8(0, 0, 0, 0xFF));
	switch(g_level) {
		case 0:
			writeFont(p,"HEXAGON", true);
			writeFont(sub,"DIFFICULTY: HARD", false);
			writeFont(sub2,"MODE: NORMAL", false);
			writeFont(time,"SCORE: 000000", false);
			break;
		case 1:
			writeFont(p,"HEXAGONER", true);
			writeFont(sub,"DIFFICULTY: HARDER", false);
			writeFont(sub2,"MODE: NORMAL", false);
			writeFont(time,"SCORE: 000000", false);
			break;
		case 2:
			writeFont(p,"HEXAGONEST", true);
			writeFont(sub,"DIFFICULTY: HARDEST", false);
			writeFont(sub2,"MODE: NORMAL", false);
			writeFont(time,"SCORE: 000000", false);
			break;
		case 3:
			writeFont(p,"HEXAGON", true);
			writeFont(sub,"DIFFICULTY: HARDESTER", false);
			writeFont(sub2,"MODE: HYPER", false);
			writeFont(time,"SCORE: 000000", false);
			break;
		case 4:
			writeFont(p,"HEXAGONER", true);
			writeFont(sub,"DIFFICULTY: HARDESTEST", false);
			writeFont(sub2,"MODE: HYPER", false);
			writeFont(time,"SCORE: 000000", false);
			break;
		case 5:
			writeFont(p,"HEXAGONEST", true);
			writeFont(sub,"DIFFICULTY: HARDESTESTEST", false);
			writeFont(sub2,"MODE: HYPER", false);
			writeFont(time,"SCORE: 000000", false);
			break;
	}
	sf2d_end_frame();
	return -1; //Level not selected yet.
}

bool doPlayGame() {
	double radians = g_levelData[g_level].radians;
	u32 kDown = hidKeysDown();
	u32 kHold = hidKeysHeld();
	if(kDown & KEY_B) {
		g_score = 0;
		return false;
	}
	if(kHold & KEY_L) g_levelData[g_level].cursor = (g_levelData[g_level].cursor + g_levelData[g_level].rotStepHuman);
	if(kHold & KEY_R) g_levelData[g_level].cursor = (g_levelData[g_level].cursor - g_levelData[g_level].rotStepHuman);
	if(g_levelData[g_level].cursor > TAU) g_levelData[g_level].cursor-=TAU;
	if(g_levelData[g_level].cursor < 0) g_levelData[g_level].cursor+=TAU;
		
	Point fg = levelColor[g_level][FG];
	Point bg1 = levelColor[g_level][BG1];
	Point bg2 = levelColor[g_level][BG2];
		
	sf2d_set_clear_color(bg1.color);
	
	////RENDER TOP SCREEN
	sf2d_start_frame(GFX_TOP, GFX_LEFT);
	
	Point center;
	center.x = TOP_WIDTH/2;
	center.y = SCREEN_HEIGHT/2;
	
	drawBackground(center, bg2, TOP_WIDTH / 1.5, radians);
	drawMainHexagon(center, fg, bg1, radians);
	drawHumanCursor(center, fg, g_levelData[g_level].cursor, radians); //Draw cursor fixed quarter circle, no movement.
	
	////ROTATE
	radians = (radians + g_levelData[g_level].rotStep);
	if(radians > TAU) {
		radians -= TAU;
	}
	g_levelData[g_level].radians = radians;
	sf2d_end_frame();
	g_score++;
	return true;
}

void doLagometer(int level, bool success) {
	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
	sf2d_draw_line(0, SCREEN_HEIGHT - 1, (int)((double)BOT_WIDTH * ((double)g_fps/60.0)), SCREEN_HEIGHT - 1, 1, RGBA8(0xFF,0,0,0xFF));
	if(level >= 0) {
		sf2d_draw_rectangle(0,0,TOP_WIDTH, 32 + 4, RGBA8(0, 0, 0, 0xFF));
		Point p;
		p.x = 14;
		p.y = 4;
		p.color = RGBA8(0xFF,0xFF,0xFF,0xFF);
		writeFont(p,"LOADING BGM", true);
	}
	if(g_playing) {
		Point p;
		p.color = RGBA8(0xFF,0xFF,0xFF,0xFF);
		sf2d_draw_rectangle(0,0,TOP_WIDTH, 22, RGBA8(0, 0, 0, 0xFF));
		p.x = 4;
		p.y = 4;
		writeFont(p,"POINT", false);
		p.x = 230;
		p.y = 4;
		char buffer[6+1];
		sprintf(buffer,"%06d", g_score);
		writeFont(p,buffer, false);
		sf2d_draw_rectangle(0,23,TOP_WIDTH, 4, RGBA8(0, 0xFF, 0, 0xFF));
	}
	char buffer[6+1];
	sprintf(buffer,"%06d", g_patterns.patterns[0]->numberOfWalls);
	Point p;
	p.color = RGBA8(0xFF,0xFF,0xFF,0xFF);
	sf2d_draw_rectangle(0,80,TOP_WIDTH, 22, RGBA8((success ? 0 : 0xFF), (success ? 0xFF : 0), 0, 0xFF));
	p.x = 4;
	p.y = 84;
	writeFont(p,buffer, false);
	sf2d_end_frame();
}

int main() {
	//3ds init
	gfxInitDefault();
	sf2d_init();
	sf2d_set_vblank_wait(1);
	romfsInit();
	ndspInit();
	ndspSetOutputMode(NDSP_OUTPUT_STEREO);
	
	//program init
	init();
	bool success = readPatterns();
	initLevels();
	initLevelData();
	initSounds();
	
	audioPlay(&g_hexagon, false);
	
	while (aptMainLoop()) {
		int level = -1;
		hidScanInput();
		u32 kDown = hidKeysDown();
		if(kDown & KEY_START) break;
		
		if(g_playing) {
			g_playing = doPlayGame(); ////PLAY GAME
		} else {
			level = doMainMenu(); ////DRAW MENU
		}
		
		////DRAW LAGOMETER
		doLagometer(level, success);
		
		////FLUSH AND CALC LAGOMETER
		sf2d_swapbuffers();
		g_fps = sf2d_get_fps();
		
		////START GAME IF NEEDED
		if(level >= 0) {
			playLevelBGM(level);
			g_playing = true;
		}
		g_renderedWalls = 0;
	}
	freeFonts();
	audioUnload();
	ndspExit();
	sf2d_fini();

	romfsExit();
	gfxExit();
	return 0;
}

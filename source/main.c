#include <string.h>
#include <limits.h>
#include <math.h>
#include <3ds.h>
#include <sf2d.h>
#include <time.h>

#include "triangle.h"
#include "font.h"
#include "levels.h"
#include "sound.h"

#define TAU 6.28318530718
#define FG 0
#define BG1 1
#define BG2 2
#define TOTAL_PATTERNS_AT_ONE_TIME 2

typedef enum {
	CAN_MOVE,
	CANNOT_MOVE_LEFT,
	CANNOT_MOVE_RIGHT,
	DEAD,
} MovementState;

typedef enum {
	TOO_CLOSE,
	TOO_FAR,
	RENDERED,
} RenderState;

typedef enum {
	MAIN_MENU,
	PLAYING,
	GAME_OVER,
} GameState;

typedef enum {
	FULL_RESET,
	PARTIAL_RESET,
} ResetType;

typedef struct {
	bool running;
	int distanceFromCenter;
	int distanceFromCenterLastWall; //This should also include the leingth of said wall.
	int patternNumber;
	int sideOffset;
} PatternTracker;

Point g_levelColor[TOTAL_LEVELS][3];
PatternTracker g_patternTracker[TOTAL_PATTERNS_AT_ONE_TIME];

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
const int MIN_DISTANCE_FROM_LAST_PATTERN = 40;

//Overflow so you don't get glitchy lines between hexagons.
const double OVERFLOW_OFFSET = TAU/900.0; //This is really just some arbituary number so yeah...

//Screen (calculated with a physical calculator)
const int TOP_SCREEN_DIAG_CENTER = 257;

//Game over screen
const double GAME_OVER_ACCEL_RATE = 1.1;
const double GAME_OVER_ROT_SPEED = TAU/240.0;
const int FRAMES_PER_GAME_OVER = 80;

////DYNAMIC VARS. g_ means global btw.
int g_renderedWalls;
int g_score;
int g_transition; //0 = no transition // 1 = forwards // -1 = backwards
int g_transitionFrame;
	int g_level;
	int g_levelLast;
	double g_fps; //Used to calculate the lagometer.
double g_gameOverDistance;
GameState g_gameState;

void init(ResetType reset) {
	////DYNAMIC VARS
	g_renderedWalls = 0;
	g_score = 0;
	g_transition = 0;
	g_transitionFrame = 0;
	if(reset == FULL_RESET) {
		g_level = 0;
		g_levelLast = 0;
		g_fps = 0.0;
	}
	g_gameOverDistance = 0.0;
	g_gameState = MAIN_MENU;
	for(int i = 0; i < TOTAL_PATTERNS_AT_ONE_TIME; i++) {
		g_patternTracker[i].running = false;
		g_patternTracker[i].distanceFromCenter = 0;
		g_patternTracker[i].sideOffset = 0;
	}
}

void initLevels() {
	//level 0
	g_levelColor[0][FG].color = RGBA8(0xF6, 0x48, 0x13, 0xFF);
	g_levelColor[0][BG1].color = RGBA8(0x50, 0x0C, 0x01, 0xFF);
	g_levelColor[0][BG2].color = RGBA8(0x61, 0x12, 0x01, 0xFF);
	
	//level 1
	g_levelColor[1][FG].color = RGBA8(0x33, 0xE5, 0x66, 0xFF);
	g_levelColor[1][BG1].color = RGBA8(0x08, 0x24, 0x10, 0xFF);
	g_levelColor[1][BG2].color = RGBA8(0x00, 0x00, 0x00, 0xFF);
	
	//level 2
	g_levelColor[2][FG].color = RGBA8(0x6D, 0x10, 0xF9, 0xFF);
	g_levelColor[2][BG1].color = RGBA8(0x22, 0x00, 0x63, 0xFF);
	g_levelColor[2][BG2].color = RGBA8(0x18, 0x00, 0x52, 0xFF);
	
	//level 3
	g_levelColor[3][FG].color = RGBA8(0xF1, 0xF9, 0x10, 0xFF);
	g_levelColor[3][BG1].color = RGBA8(0x63, 0x60, 0x01, 0xFF);
	g_levelColor[3][BG2].color = RGBA8(0x52, 0x4C, 0x00, 0xFF);
	
	//level 4
	g_levelColor[4][FG].color = RGBA8(0xF4, 0x05, 0x86, 0xFF);
	g_levelColor[4][BG1].color = RGBA8(0xFF, 0xFF, 0xFF, 0xFF);
	g_levelColor[4][BG2].color = RGBA8(0xFD, 0xD7, 0xEC, 0xFF);
	
	//level 5
	g_levelColor[5][FG].color = RGBA8(0xFF, 0xFF, 0xFF, 0xFF);
	g_levelColor[5][BG1].color = RGBA8(0x00, 0x00, 0x00, 0xFF);
	g_levelColor[5][BG2].color = RGBA8(0x00, 0x00, 0x00, 0xFF);
}

u32 tweenColor(u32 original, u32 new, int frame) {
	int red = (double)((int)RGBA8_GET_R(new) - (int)RGBA8_GET_R(original)) * ((double)frame / (double)FRAMES_PER_ONE_SIDE_ROTATION) + (double)RGBA8_GET_R(original);
	int green = (double)((int)RGBA8_GET_G(new) - (int)RGBA8_GET_G(original)) * ((double)frame / (double)FRAMES_PER_ONE_SIDE_ROTATION) + (double)RGBA8_GET_G(original);
	int blue = (double)((int)RGBA8_GET_B(new) - (int)RGBA8_GET_B(original)) * ((double)frame / (double)FRAMES_PER_ONE_SIDE_ROTATION) + (double)RGBA8_GET_B(original);
	return RGBA8(red, green, blue, 0xFF);
}

MovementState checkCollision(int distanceFromCenter, int length, int side) {
	//Check if we are between the wall vertically
	if(distanceFromCenter <= FULL_LEN + HUMAN_PADDING + HUMAN_HEIGHT && distanceFromCenter + length >= FULL_LEN + HUMAN_PADDING + HUMAN_HEIGHT) {
		double leftRotStep = g_levelData[g_level].cursor + g_levelData[g_level].rotStepHuman;
		double rightRotStep = g_levelData[g_level].cursor - g_levelData[g_level].rotStepHuman;
		
		//If the cursor rotates around to 0 TAU (or from 0 TAU to TAU) we need to calculate BOTH ranges on the circle to compensate
		double leftSideRads = (side + 1) * TAU/6.0; 
		double leftSideRadsAlt = leftSideRads; 
		double rightSideRads = side * TAU/6.0; 
		double rightSideRadsAlt = rightSideRads;
		
		if(leftRotStep >= TAU) leftRotStep -= TAU;
		if(leftRotStep < 0) leftRotStep += TAU;
		if(rightRotStep >= TAU) rightRotStep -= TAU;
		if(rightRotStep < 0) rightRotStep += TAU;
		
		//If the cursor wrapped and the range we need to calculate overflows beyond TAU we also need to check the other equivilent region
		//exactly one TAU ago. The same idea applies to all of the other cases that "wrap around" when they shoyld not.
		if(leftSideRads >= TAU) {
			leftSideRadsAlt -= TAU;
			rightSideRadsAlt -= TAU;
		}
		if(leftSideRads < 0) {
			leftSideRadsAlt += TAU;
			rightSideRadsAlt += TAU;
		}
		if(rightSideRads >= TAU) {
			rightSideRadsAlt -= TAU;
			leftSideRadsAlt -= TAU;
		}
		if(rightSideRads < 0) {
			rightSideRadsAlt += TAU;
			leftSideRadsAlt += TAU;
		}
		
		
		//Check if we are between the wall horizontally. 
		if((g_levelData[g_level].cursor >= rightSideRads && g_levelData[g_level].cursor <= leftSideRads) ||
		   (g_levelData[g_level].cursor >= rightSideRadsAlt && g_levelData[g_level].cursor <= leftSideRadsAlt)) {
			return DEAD;
		} else if((leftRotStep > rightSideRads && leftRotStep < leftSideRads) ||
				  (leftRotStep > rightSideRadsAlt && leftRotStep < leftSideRadsAlt))  {
			return CANNOT_MOVE_LEFT;
		} else if((rightRotStep < leftSideRads && rightRotStep > rightSideRads) ||
				  (rightRotStep < leftSideRadsAlt && rightRotStep > rightSideRadsAlt)) {
			return CANNOT_MOVE_RIGHT;
		}
	}
	return CAN_MOVE;
}

RenderState drawWall(Point center, Point fg, int distanceFromCenter, int length, int side, double radians) {
	if(distanceFromCenter + length < FULL_LEN) return TOO_CLOSE;
	if(sqrt(pow((double)distanceFromCenter * cos(TAU/12.0),2) + pow((double)distanceFromCenter * sin(TAU/12.0),2)) > TOP_SCREEN_DIAG_CENTER) return TOO_FAR; //Might be expensive to calc?
	
	if(distanceFromCenter < FULL_LEN - 2/*To make sure it draws correctly*/) {
		length -= FULL_LEN - 2 - distanceFromCenter;
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
	Point triangle[3];
	triangle[0] = edges[0];
	triangle[1] = edges[1];
	triangle[2] = edges[2];
	drawTriangle(triangle);
	triangle[0] = edges[0];
	triangle[1] = edges[2];
	triangle[2] = edges[3];
	drawTriangle(triangle);
	return RENDERED;
}

MovementState drawWalls(Point center, Point fg, double radians, int manualOffset) {
	bool shouldShift = false;
	MovementState collision = CAN_MOVE;
	for(int pattern = 0; pattern < TOTAL_PATTERNS_AT_ONE_TIME; pattern++) {
		RenderState lastRender = RENDERED;
		if(!g_patternTracker[pattern].running) {
			g_patternTracker[pattern].patternNumber = rand() % g_patterns.numberOfPatterns; //Who cares about uniformity anyway?
			//This line of code gets the last wall of the pattern we are looking at.
			Wall* wall = g_patterns.patterns[g_patternTracker[pattern].patternNumber]->
						 walls[g_patterns.patterns[g_patternTracker[pattern].patternNumber]->numberOfWalls - 1]; 
			g_patternTracker[pattern].sideOffset = rand() % 6;
			g_patternTracker[pattern].distanceFromCenter = (pattern == 0 ? TOP_SCREEN_DIAG_CENTER : g_patternTracker[pattern - 1].distanceFromCenterLastWall);
			g_patternTracker[pattern].distanceFromCenterLastWall = g_patternTracker[pattern].distanceFromCenter + wall->distanceFromCenter + wall->length + MIN_DISTANCE_FROM_LAST_PATTERN;
			g_patternTracker[pattern].running = true;
		}
		
		for(int i_wall = 0; i_wall < g_patterns.patterns[g_patternTracker[pattern].patternNumber]->numberOfWalls; i_wall++) {
			Wall* wall = g_patterns.patterns[g_patternTracker[pattern].patternNumber]->walls[i_wall];
			lastRender = drawWall(
				center,
				fg,
				g_patternTracker[pattern].distanceFromCenter + wall->distanceFromCenter,
				wall->length,
				(wall->side + g_patternTracker[pattern].sideOffset) % 6,
				radians); //draw the actual wall finally.
			if(lastRender == TOO_FAR) break;
				
			MovementState collisionCheck = checkCollision(
				g_patternTracker[pattern].distanceFromCenter + wall->distanceFromCenter, 
				wall->length,
				(wall->side + g_patternTracker[pattern].sideOffset) % 6);
			if(collision == CAN_MOVE) collision = collisionCheck; //If we can move, try and replace it with something else
			if((collision == CANNOT_MOVE_LEFT || collision == CANNOT_MOVE_RIGHT) && collisionCheck == DEAD) collision = collisionCheck;
		}
		if(lastRender == TOO_CLOSE && pattern == 0) {
			shouldShift = true;
		}
		if(!manualOffset) {
			g_patternTracker[pattern].distanceFromCenter -= g_levelData[g_level].wallSpeed;
			g_patternTracker[pattern].distanceFromCenterLastWall -= g_levelData[g_level].wallSpeed;
		} else {
			g_patternTracker[pattern].distanceFromCenter -= manualOffset;
			g_patternTracker[pattern].distanceFromCenterLastWall -= manualOffset;
		}
	}
	if(shouldShift) { //We are going to shift the other patterns forward.
		for(int shift = 1; shift < TOTAL_PATTERNS_AT_ONE_TIME; shift++) {
			g_patternTracker[shift - 1].patternNumber = g_patternTracker[shift].patternNumber;
			g_patternTracker[shift - 1].sideOffset = g_patternTracker[shift].sideOffset;
			g_patternTracker[shift - 1].distanceFromCenter = g_patternTracker[shift].distanceFromCenter;
			g_patternTracker[shift - 1].distanceFromCenterLastWall = g_patternTracker[shift].distanceFromCenterLastWall;
			g_patternTracker[shift - 1].running = g_patternTracker[shift].running;
		}
		g_patternTracker[TOTAL_PATTERNS_AT_ONE_TIME - 1].running = false;
	}
	return collision;
}

void drawMainHexagon(Point center, Point fg, Point bg, double radians) {
	//This draws the hexagon.
	Point triangle[3];
	triangle[0].x = center.x;
	triangle[0].y = center.y;
	for(int concentricHexes = 0; concentricHexes < 2; concentricHexes++) {
		double len = 0.0;
		if(concentricHexes == 0) {
			//outer color painted after
			triangle[0].color = fg.color;
			len = FULL_LEN;
		}
		if(concentricHexes == 1) {
			//inner color painted over it all.
			triangle[0].color = bg.color;
			len = FULL_LEN - BORDER_LEN;
		}
		for(int i = 0; i < 7; i++) {
			triangle[(i % 2) + 1].x = (int)(len * cos(radians + (double)i * TAU/6.0) + center.x);
			triangle[(i % 2) + 1].y = (int)(len * sin(radians + (double)i * TAU/6.0) + center.y);
			if(i != 0) {
				drawTriangle(triangle);
			}
		}
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
		drawTriangle(triangle);
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
	drawTriangle(humanTriangle);
}

int doMainMenu() {
	double radians = (double)g_transitionFrame / (double)FRAMES_PER_ONE_SIDE_ROTATION * TAU/6.0;
	if(g_transition == -1) { //if the user is going to the left, flip the radians so the animation plays backwards.
		radians *= -1;
	}
	if(g_levelLast % 2 == 1) { //We need to offset the hexagon if the selection is odd. Otherwise the bg colors flip!
		radians += TAU/6.0;
	}
	
	////CALCULATE COLORS
	Point fg;
	Point bg1;
	Point bg2;
	if(g_transition && g_transitionFrame < FRAMES_PER_ONE_SIDE_ROTATION) {
		fg.color = tweenColor(g_levelColor[g_levelLast][FG].color, g_levelColor[g_level][FG].color, g_transitionFrame);
		bg1.color = tweenColor(g_levelColor[g_levelLast][BG1].color, g_levelColor[g_level][BG1].color, g_transitionFrame);
		bg2.color = tweenColor(g_levelColor[g_levelLast][BG2].color, g_levelColor[g_level][BG2].color, g_transitionFrame);
		g_transitionFrame++;
	} else {
		g_levelLast = g_level;
		g_transitionFrame = 0;
		g_transition = false;
	}
	if(!g_transition) {
		fg = g_levelColor[g_level][FG];
		bg1 = g_levelColor[g_level][BG1];
		bg2 = g_levelColor[g_level][BG2];
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
	
	//KEYS
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
		} else if(kHold & KEY_L) {
			g_level--;
			g_transition = -1;
			audioPlay(&g_select, false);
		} 
	}
	if(g_level < 0) g_level = TOTAL_LEVELS - 1;
	if(g_level >= TOTAL_LEVELS) g_level = 0;
	return -1; //Level not selected yet.
}

GameState doPlayGame() {
	double radians = g_levelData[g_level].radians;
		
	Point fg = g_levelColor[g_level][FG];
	Point bg1 = g_levelColor[g_level][BG1];
	Point bg2 = g_levelColor[g_level][BG2];
		
	sf2d_set_clear_color(bg1.color);
	
	////RENDER TOP SCREEN
	sf2d_start_frame(GFX_TOP, GFX_LEFT);
	
	Point center;
	center.x = TOP_WIDTH/2;
	center.y = SCREEN_HEIGHT/2;
	
	drawBackground(center, bg2, TOP_WIDTH / 1.5, radians);
	MovementState collision = drawWalls(center, fg, radians, 0);
	drawMainHexagon(center, fg, bg1, radians);
	drawHumanCursor(center, fg, g_levelData[g_level].cursor, radians); //Draw cursor fixed quarter circle, no movement.
	
	if(collision == DEAD) return GAME_OVER;
	
	////ROTATE
	radians = (radians + g_levelData[g_level].rotStep);
	if(radians >= TAU) {
		radians -= TAU;
	}
	g_levelData[g_level].radians = radians;
	sf2d_end_frame();
	g_score++;
	
	//KEYS
	u32 kDown = hidKeysDown();
	u32 kHold = hidKeysHeld();
	if(kDown & KEY_B) {
		return GAME_OVER; //Theoretically can be switched to MAIN_MENU.
	}
	if(kHold & KEY_L && collision != CANNOT_MOVE_LEFT) g_levelData[g_level].cursor = (g_levelData[g_level].cursor + g_levelData[g_level].rotStepHuman);
	if(kHold & KEY_R && collision != CANNOT_MOVE_RIGHT) g_levelData[g_level].cursor = (g_levelData[g_level].cursor - g_levelData[g_level].rotStepHuman);
	if(g_levelData[g_level].cursor >= TAU) g_levelData[g_level].cursor-=TAU;
	if(g_levelData[g_level].cursor < 0) g_levelData[g_level].cursor+=TAU;
	return PLAYING;
}

GameState doGameOver() {
	double radians = g_levelData[g_level].radians;
	
	if(g_transition && g_transitionFrame < FRAMES_PER_GAME_OVER) {
		g_gameOverDistance = -pow(GAME_OVER_ACCEL_RATE, g_transitionFrame);
		g_transitionFrame++;
	} else {
		g_transitionFrame = 0;
		g_transition = false;
	}
		
	Point fg = g_levelColor[g_level][FG];
	Point bg1 = g_levelColor[g_level][BG1];
	Point bg2 = g_levelColor[g_level][BG2];
		
	sf2d_set_clear_color(bg1.color);
	
	////RENDER TOP SCREEN
	sf2d_start_frame(GFX_TOP, GFX_LEFT);
	
	Point center;
	center.x = TOP_WIDTH/2;
	center.y = SCREEN_HEIGHT/2;
	
	drawBackground(center, bg2, TOP_WIDTH / 1.5, radians);
	drawWalls(center, fg, radians, g_gameOverDistance);
	drawMainHexagon(center, fg, bg1, radians);
	drawHumanCursor(center, fg, g_levelData[g_level].cursor, radians); //Draw cursor fixed quarter circle, no movement.
	
	////ROTATE
	radians = (radians + GAME_OVER_ROT_SPEED);
	if(radians >= TAU) {
		radians -= TAU;
	}
	g_levelData[g_level].radians = radians;
	sf2d_end_frame();
	
	//KEYS
	u32 kDown = hidKeysDown();
	if(!g_transition) {
		if(kDown & KEY_B) return MAIN_MENU;
		if(kDown & KEY_A) return PLAYING;
	} 
	return GAME_OVER;
}

void doLagometer(int level) {
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
	if(g_gameState == PLAYING) {
		sf2d_draw_rectangle(0,0,TOP_WIDTH, 22, RGBA8(0, 0, 0, 0xFF));
		Point p;
		p.color = RGBA8(0xFF,0xFF,0xFF,0xFF);
		p.x = 4;
		p.y = 4;
		writeFont(p,"POINT", false);
		p.x = 230;
		p.y = 4;
		char buffer[6+1];
		int scoreInt = (int)((double)g_score/60.0);
		int decimalPart = (int)(((double)g_score/60.0 - (double)scoreInt) * 100.0);
		snprintf(buffer, 6+1, "%03d:%02d", scoreInt, decimalPart); //Emergency stack overflow prevention
		writeFont(p,buffer, false);
		sf2d_draw_rectangle(0,23,TOP_WIDTH, 4, RGBA8(0, 0xFF, 0, 0xFF));
	}
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
	init(FULL_RESET);
	initPatterns();
	initLevels();
	initLevelData();
	initSounds();
	srand(svcGetSystemTick());
	
	audioPlay(&g_hexagon, false);
	
	while (aptMainLoop()) {
		int level = -1;
		hidScanInput();
		u32 kDown = hidKeysDown();
		if(kDown & KEY_START) break;
		
		////DRAW MENU
		if(g_gameState == MAIN_MENU) {
			level = doMainMenu();
			if(level != -1) { //Ran when main menu exits.
				resetLevelData();
				init(PARTIAL_RESET);
			}
		} else
		
		////PLAY GAME
		if(g_gameState == PLAYING) {
			g_gameState = doPlayGame(); 
			if(g_gameState != PLAYING) { //Ran when playing exits (either to main or game over)
				if(g_gameState == GAME_OVER) g_transition = true;
			}
		} else
		
		////GAME OVER
		if(g_gameState == GAME_OVER) {
			g_gameState = doGameOver(); 
			if(g_gameState != GAME_OVER) { //Ran when game over exits (either to main or playing)
				resetLevelData();
				init(PARTIAL_RESET);
			}
		}
		
		////DRAW LAGOMETER
		doLagometer(level);
		
		////FLUSH AND CALC LAGOMETER
		sf2d_swapbuffers();
		g_fps = sf2d_get_fps();
		
		////START GAME IF NEEDED
		if(level >= 0) {
			playLevelBGM(level);
			g_gameState = PLAYING;
		}
		g_renderedWalls = 0;
	}
	freeFonts();
	freePatterns();
	audioUnload();
	ndspExit();
	sf2d_fini();

	romfsExit();
	gfxExit();
	return 0;
}

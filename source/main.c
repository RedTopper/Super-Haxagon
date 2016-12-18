#define TAU 6.28318530718
#define SCREEN_HEIGHT 240
#define TOP_WIDTH  400
#define BOT_WIDTH  320

#include <string.h>
#include <limits.h>
#include <math.h>
#include <3ds.h>
#include <sf2d.h>
#include <time.h>

#include "util.h"
#include "triangle.h"
#include "font.h"
#include "levels.h"
#include "sound.h"

/**
 * Draws a single moving wall based on live settings, the playing pattern, and a wall.
 */
RenderState drawMovingWall(LiveLevel live, LivePattern pattern, LiveWall wall) {
	double distance = wall.distance;
	double height = wall.height;
	
	if(distance + height < FULL_LEN) return TOO_CLOSE;
	if(distance > TOP_SCREEN_DIAG_CENTER) return TOO_FAR; //Might be expensive to calc?
	
	if(distance < FULL_LEN - 2 ) {//so the distance is never negative as it enters.
		height -= FULL_LEN - 2 - distance;
		distance = FULL_LEN - 2; //Should never be 0!!!
	}
	
	Point edges[4] = {0};
	edges[0] = calcPoint(live, OVERFLOW_OFFSET, distance, wall.side + 1, pattern.sides);
	edges[1] = calcPoint(live, OVERFLOW_OFFSET, distance + height, wall.side + 1, pattern.sides);
	edges[2] = calcPoint(live, -OVERFLOW_OFFSET, distance + height, wall.side, pattern.sides);
	edges[3] = calcPoint(live, -OVERFLOW_OFFSET, distance, wall.side, pattern.sides);
	drawTrap(interpolateColor(live.currentFG, live.nextFG, live.tweenPercent), edges);
	return RENDERED;
}

/**
 * Completely draws all walls in a live level. Returns the render value
 * of the furthest wall of the closest pattern (as to trigger a shift).
 */
RenderState drawMovingPatterns(LiveLevel live, int manualOffset) {
	
	//furthest distance of closest pattern
	double nearFurthestDistance = 0;
	RenderState nearFurthestRender = RENDERED;
	
	//for all patterns
	for(int iPattern = 0; iPattern < TOTAL_PATTERNS_AT_ONE_TIME; iPattern++) {
		LivePattern pattern = live.patterns[pattern];
		
		//draw all walls
		for(int iWall = 0; iWall < pattern.numWalls; iWall++) {
			MovingWall wall = pattern.walls[iWall];
			RenderState render = drawMovingWall(live, pattern, wall);
			
			//update information about the closest pattern
			if(wall = 0 && wall.distance + wall.height > nearFurthestDistance) {
				nearFurthestDistance = wall.distance + wall.height;
				nearFurthestRender = render;
			}
		}
	}
	return nearFurthestRender;
}

MovementState collisionMovingWall(Level settings, LiveLevel live, LivePattern pattern, LiveWall wall) {
	
	//Check if we are between the wall vertically
	if(FULL_LEN + HUMAN_PADDING + HUMAN_HEIGHT < wall.distance || 
	   FULL_LEN + HUMAN_PADDING + HUMAN_HEIGHT > wall.distance + wall.height) {
		return CAN_MOVE;
	}
	
	double leftRotStep = live.cursorPos + settings.cursorStep;
	double rightRotStep = live.cursorPos - settings.cursorStep;
	
	//If the cursor rotates around to 0 TAU (or from 0 TAU to TAU) we need to calculate BOTH ranges on the circle to compensate
	double leftSideRads = (wall.side + 1.0) * TAU/(double)(pattern.sides); 
	double leftSideRadsAlt = leftSideRads; 
	double rightSideRads = wall.side * TAU/(double)(pattern.sides); 
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
	if((live.cursorPos >= rightSideRads && live.cursorPos <= leftSideRads) ||
	   (live.cursorPos >= rightSideRadsAlt && live.cursorPos <= leftSideRadsAlt)) {
		return DEAD;
	} else if((leftRotStep > rightSideRads && leftRotStep < leftSideRads) ||
			  (leftRotStep > rightSideRadsAlt && leftRotStep < leftSideRadsAlt))  {
		return CANNOT_MOVE_LEFT;
	} else if((rightRotStep < leftSideRads && rightRotStep > rightSideRads) ||
			  (rightRotStep < leftSideRadsAlt && rightRotStep > rightSideRadsAlt)) {
		return CANNOT_MOVE_RIGHT;
	}
	return CAN_MOVE;
}

MovementState collisionMovingPatterns(Level settings, LiveLevel live) {
	MovementState collision = CAN_MOVE;
	
	//for all patterns
	for(int iPattern = 0; iPattern < TOTAL_PATTERNS_AT_ONE_TIME; iPattern++) {
		LivePattern pattern = live.patterns[pattern];
		
		//for all walls
		for(int iWall = 0; iWall < pattern.numWalls; iWall++) {
			MovingWall wall = pattern.walls[iWall];
			MovementState check = checkCollision(settings, live, pattern, wall);
			
			//update colision
			if(collision == CAN_MOVE) collision = check; //If we can move, try and replace it with something else
			if(check == DEAD)  { //If we are ever dead, return it.
				return DEAD;
			}
		}
	}
	return collision;
}

void drawMainHexagon(Color color1, Color color2, Point focus, double rotation, int sides) {
	
	//This draws the center shape.
	Point triangle[3];
	triangle[0] = focus;
	
	for(int concentricHexes = 0; concentricHexes < 2; concentricHexes++) {
		double height;
		Color color;
		if(concentricHexes == 0) {
			//outer color painted first
			color = color1;
			height = FULL_LEN;
		}
		if(concentricHexes == 1) {
			//inner color painted over it all.
			color =  color2;
			height = FULL_LEN - BORDER_LEN;
		}
		
		//draw hexagon (might not actually be hexagon!) in accordance to the closest pattern
		for(int i = 0; i < sides + 1; i++) {
			triangle[(i % 2) + 1].x = (int)(height * cos(rotation + (double)i * TAU/(double)sides) + (double)(focus.x));
			triangle[(i % 2) + 1].y = (int)(height * sin(rotation + (double)i * TAU/(double)sides) + (double)(focus.y));
			if(i != 0) {
				drawTriangle(color, triangle);
			}
		}
	}
}

//basically an overload for drawMainHexagon
void drawMainHexagonLive(LiveLevel live) {
	drawMainHexagon(
		tweenColor(live.currentFG, live.nextFG, live.tweenPercent), 
		tweenColor(live.currentBG1, live.nextBG1, live.tweenPercent),
		CENTER, live.rotation, live.patterns[0].sides)
}

void drawBackground(Color color1, Color color2, Point focus, double height, double rotation, int sides) {
	
	//solid background.
	Point position = {0,0};
	Point side = {TOP_WIDTH, SCREEN_HEIGHT};
	drawRect(color1, position, size);
	
	//This draws the main background.
	Point* edges = malloc(sizeof(point) * sides);
	check(edges, "Error drawing background!", 0x0);
	
	for(int i = 0; i < sides; i++) {
		edges[i].x = (int)(height * cos(rotation + (double)i * TAU/6.0) + focus.x);
		edges[i].y = (int)(height * sin(rotation + (double)i * TAU/6.0) + focus.y);
	}
	
	Point triangle[3];
	triangle[0] = focus;
	for(int i = 0; i < sides - 1; i = i + 2) {
		triangle[1] = edges[i];
		triangle[2] = edges[i + 1];
		drawTriangle(color2, triangle);
	}
	
	//if the sides is odd we need to "make up a color" to put in the gap.
	if(sides % 2) {
		triangle[1] = edges[sides - 2];
		triangle[2] = edges[sides - 1];
		drawTriangle(tweenColor(color1, color2, 0.5f), triangle);
	}
	
	free(edges);
}

//basically an overload for drawBackground
void drawBackgroundLive(LiveLevel live) {
	drawBackground(
		tweenColor(live.currentBG2, live.nextBG2, live.tweenPercent), 
		tweenColor(live.currentBG1, live.nextBG1, live.tweenPercent),
		CENTER, TOP_SCREEN_DIAG_CENTER, live.rotation, live.patterns[0].sides)
}

void drawHumanCursor(Color color, Point focus, double cursor, double rotation) {
	Point humanTriangle[3];
	for(int i = 0; i < 3; i++) {
		double height = 0.0;
		double position = 0.0;
		if(i == 0) {
			height = FULL_LEN + HUMAN_PADDING + HUMAN_HEIGHT;
			position = cursor + rotation;
		} else {
			height = FULL_LEN + HUMAN_PADDING;
			if(i == 1) {
				position = cursor + HUMAN_WIDTH/2 + rotation;
			} else {
				position = cursor - HUMAN_WIDTH/2 + rotation;
			}
		}
		humanTriangle[i].x = (int)(height * cos(position) + focus.x);
		humanTriangle[i].y = (int)(height * sin(position) + focus.y);
	}
	drawTriangle(humanTriangle);
}

int doMainMenu(MainMenu menu) {
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
			writeFont(time,"SCORE: 000:00", false);
			break;
		case 1:
			writeFont(p,"HEXAGONER", true);
			writeFont(sub,"DIFFICULTY: HARDER", false);
			writeFont(sub2,"MODE: NORMAL", false);
			writeFont(time,"SCORE: 000:00", false);
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
		if(kDown & KEY_A ) {
			return g_level;
		} 
		if(kHold & (KEY_R | KEY_ZR | KEY_CSTICK_RIGHT | KEY_CPAD_RIGHT | KEY_DRIGHT | KEY_X)) {
			g_level++;
			g_transition = 1;
			audioPlay(&g_select, false);
		} else if(kHold & (KEY_L | KEY_ZL | KEY_CSTICK_LEFT | KEY_CPAD_LEFT | KEY_DLEFT | KEY_Y)) {
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
	
	sf2d_set_clear_color(pulseColor(bg1, radians).color);
	
	////RENDER TOP SCREEN
	sf2d_start_frame(GFX_TOP, GFX_LEFT);
	
	Point center;
	center.x = TOP_WIDTH/2;
	center.y = SCREEN_HEIGHT/2;
	
	drawBackground(
		pulseColor(center, radians),
		pulseColor(bg2, radians),
		TOP_WIDTH / 1.5,
		radians);
	MovementState collision = drawWalls(
		pulseColor(center, radians),
		pulseColor(fg, radians),
		radians, 0);
	drawMainHexagon(
		pulseColor(center, radians),
		pulseColor(fg, radians),
		pulseColor(bg1, radians),
		radians);
	drawHumanCursor(
		pulseColor(center, radians),
		pulseColor(fg, radians),
		g_levelData[g_level].cursor,
		radians);
	
	if(collision == DEAD) return GAME_OVER;
	
	////ROTATE
	radians = (radians + g_levelData[g_level].rotStep);
	if(radians >= TAU) radians -= TAU;
	if(radians < 0) radians += TAU;
	g_levelData[g_level].radians = radians;
	sf2d_end_frame();
	g_score++;
	
	//KEYS
	u32 kDown = hidKeysDown();
	u32 kHold = hidKeysHeld();
	if(kDown & KEY_B) {
		return GAME_OVER; //Theoretically can be switched to MAIN_MENU.
	}
	if((kHold & (KEY_R | KEY_ZR | KEY_CSTICK_RIGHT | KEY_CPAD_RIGHT | KEY_DRIGHT | KEY_X)) && collision != CANNOT_MOVE_RIGHT)
		g_levelData[g_level].cursor = (g_levelData[g_level].cursor - g_levelData[g_level].rotStepHuman);
	if((kHold & (KEY_L | KEY_ZL | KEY_CSTICK_LEFT  | KEY_CPAD_LEFT  | KEY_DLEFT  | KEY_Y)) && collision != CANNOT_MOVE_LEFT)
		g_levelData[g_level].cursor = (g_levelData[g_level].cursor + g_levelData[g_level].rotStepHuman);
	
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
	if(radians >= TAU) radians -= TAU;
	if(radians < 0) radians += TAU;
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
		sf2d_draw_rectangle(0,0,BOT_WIDTH, 32 + 4, RGBA8(0, 0, 0, 0xFF));
		Point p;
		p.x = 14;
		p.y = 4;
		p.color = RGBA8(0xFF,0xFF,0xFF,0xFF);
		writeFont(p,"LOADING BGM", true);
	}
	if(g_gameState == PLAYING) {
		sf2d_draw_rectangle(0,23,BOT_WIDTH, 4, RGBA8(0, 0xFF, 0, 0xFF));
		sf2d_draw_rectangle(0,0,BOT_WIDTH, 22, RGBA8(0, 0, 0, 0xFF));
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
	}
	if(g_gameState == GAME_OVER) {
		sf2d_draw_rectangle(0,0,BOT_WIDTH, 112, RGBA8(0, 0, 0, 0xFF));
		Point p;
		p.color = RGBA8(0xFF,0xFF,0xFF,0xFF);
		p.x = 44;
		p.y = 4;
		writeFont(p,"GAME OVER", true);
		p.x = 90;
		p.y = 40;
		char buffer[12+1];
		int scoreInt = (int)((double)g_score/60.0);
		int decimalPart = (int)(((double)g_score/60.0 - (double)scoreInt) * 100.0);
		snprintf(buffer, 12+1, "TIME: %03d:%02d", scoreInt, decimalPart); //Emergency stack overflow prevention
		writeFont(p,buffer, false);
		if(!g_transition) {
			p.x = 70;
			p.y = 70;
			writeFont(p,"PRESS A TO PLAY", false);
			p.x = 70;
			p.y = 86;
			writeFont(p,"PRESS B TO QUIT", false);
		}
	}
	sf2d_end_frame();
}

int main() {
	//3ds init
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
				audioPlay(&g_begin, false);
			}
		} else
		
		////PLAY GAME
		if(g_gameState == PLAYING) {
			g_gameState = doPlayGame(); 
			if(g_gameState == GAME_OVER) {
				g_transition = true;
				audioPlay(&g_over, false);
			}
			if(g_gameState == MAIN_MENU) {
				audioStop(&g_bgm);
				audioPlay(&g_hexagon, false);
			}
		} else
		
		////GAME OVER
		if(g_gameState == GAME_OVER) {
			g_gameState = doGameOver(); 
			if(g_gameState == PLAYING) {
				resetLevelData();
				init(PARTIAL_RESET);
				audioPlay(&g_begin, false);
			}
			if(g_gameState == MAIN_MENU) {
				audioStop(&g_bgm);
				audioPlay(&g_hexagon, false);
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
	sf2d_fini();
	freePatterns();
	audioUnload();
	
	gfxExit();	
	romfsExit();	
	ndspExit();	
	return 0;
}

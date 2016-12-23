#include <3ds.h>
#include <sf2d.h>
#include <stdlib.h>
#include <stdio.h>

#include "types.h"
#include "sound.h"
#include "util.h"
#include "draw.h"
#include "font.h"
#include "logic.h"

MovementState collisionLiveWall(LiveWall wall, double cursorPos, double cursorStep, int sides) {
	
	//Check if we are between the wall vertically
	if(DEF_HEX_FULL_LEN + DEF_HUMAN_PADDING + DEF_HUMAN_HEIGHT < wall.distance || 
	   DEF_HEX_FULL_LEN + DEF_HUMAN_PADDING + DEF_HUMAN_HEIGHT > wall.distance + wall.height) {
		return CAN_MOVE;
	}
	
	double leftRotStep = cursorPos + cursorStep;
	double rightRotStep = cursorPos - cursorStep;
	
	//If the cursor wrapped and the range we need to calculate overflows beyond TAU we also need to check the other equivilent regions:
	//exactly one TAU ago and the next TAU.
	//This is particularly usefull when the cursor's next step is beyond a TAU or below zero, OR a wall resides along "the seam"
	double leftSideRads = ((double)(wall.side) + 1.0) * TAU/(double)(sides); 
	double leftSideRadsNextTau = leftSideRads + TAU;
	double leftSideRadsLastTau = leftSideRads - TAU;
	double rightSideRads = (double)(wall.side) * TAU/(double)(sides); 
	double rightSideRadsNextTau = rightSideRads + TAU;
	double rightSideRadsLastTau = rightSideRads - TAU;
	
	//Check if we are between the wall horizontally. 
	if(cursorPos >= rightSideRads && cursorPos <= leftSideRads) {
		return DEAD;
	} 
	if((leftRotStep > rightSideRads && leftRotStep < leftSideRads) ||
	   (leftRotStep > rightSideRadsNextTau && leftRotStep < leftSideRadsNextTau) ||
	   (leftRotStep > rightSideRadsLastTau && leftRotStep < leftSideRadsLastTau))  {
		return CANNOT_MOVE_LEFT;
	} 
	if((rightRotStep < leftSideRads && rightRotStep > rightSideRads) ||
	   (rightRotStep < leftSideRadsNextTau && rightRotStep > rightSideRadsNextTau) ||
	   (rightRotStep < leftSideRadsLastTau && rightRotStep > rightSideRadsLastTau)) {
		return CANNOT_MOVE_RIGHT;
	}
	return CAN_MOVE;
}

MovementState collisionLiveLevel(LiveLevel live, double cursorStep) {
	MovementState collision = CAN_MOVE;
	
	//for all patterns
	for(int iPattern = 0; iPattern < TOTAL_PATTERNS_AT_ONE_TIME; iPattern++) {
		LivePattern pattern = live.patterns[iPattern];
		
		//for all walls
		for(int iWall = 0; iWall < pattern.numWalls; iWall++) {
			LiveWall wall = pattern.walls[iWall];
			MovementState check = collisionLiveWall(wall, live.cursorPos, cursorStep, pattern.sides);
			
			//update collision
			if(collision == CAN_MOVE) collision = check; //If we can move, try and replace it with something else
			if(check == DEAD)  { //If we are ever dead, return it.
				return DEAD;
			}
		}
	}
	return collision;
}

LivePattern getLivePattern(Pattern* patterns, int numPatterns, double distance) {
	int select = rand() % numPatterns;
	Pattern pattern = patterns[select];
	LivePattern live;
	live.numWalls = pattern.numWalls;
	live.sides = pattern.sides;
	live.walls = malloc(sizeof(LiveWall) * pattern.numWalls);
	check(!(live.walls), "Failed to allocate new live patterns!", DEF_DEBUG, 0);
	for(int i = 0; i < pattern.numWalls; i++) {
		live.walls[i].distance = (double)(pattern.walls[i].distance) + distance;
		live.walls[i].height = (double)(pattern.walls[i].height);
		live.walls[i].side = pattern.walls[i].side;
	}
	return live;
}

void freeLivePattern(LivePattern pattern) {
	free(pattern.walls);
}

double getFurthestWallDistance(LivePattern pattern) {
	double maxDistance = 0;
	for(int i = 0; i < pattern.numWalls; i++) {
		double distance = pattern.walls[i].distance + pattern.walls[i].height;
		if(distance > maxDistance) maxDistance = distance;
	}
	return maxDistance;
}

GameState doMainMenu(GlobalData data, Track select, int* level) {
	MainMenu menu = {0};
	menu.level = *level;
	while(aptMainLoop()) {
		
		//LOGIC
		ButtonState press = getButton();
		if(press == QUIT) return PROGRAM_QUIT;
		if(!(menu.transitioning)) {
			switch(press) {
			case SELECT:
				*level = menu.level;
				return PLAYING;
			case DIR_RIGHT:
				menu.transitioning = true;
				menu.transitionDirection = 1;
				menu.lastLevel = menu.level;
				menu.level++;
				audioPlay(&select, ONCE);
				break;
			case DIR_LEFT:
				menu.transitioning = true;
				menu.transitionDirection = -1;
				menu.lastLevel = menu.level;
				menu.level--;
				audioPlay(&select, ONCE);
				break;
			default:;
			}
		}
		if(menu.level >=  data.numLevels) menu.level = 0;
		if(menu.level < 0) menu.level = data.numLevels - 1;
		if(menu.transitioning) menu.transitionFrame++;
		if(menu.transitionFrame >= DEF_FRAMES_PER_TRANSITION) {
			menu.transitioning = false;
			menu.transitionFrame = 0;
		}
		
		//DRAW
		sf2d_start_frame(GFX_TOP, GFX_LEFT);
		drawMainMenu(data, menu);
		sf2d_end_frame();
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		drawMainMenuBot(sf2d_get_fps());
		sf2d_end_frame();
		sf2d_swapbuffers();
	}
	return PROGRAM_QUIT;
}

GameState doPlayGame(GlobalData data, int nlevel) {
	
	//Load level settings
	Level level = data.levels[nlevel];
	
	//create live level
	LiveLevel liveLevel;
	liveLevel.cursorPos = TAU/4.0 + (level.speedCursor / 2.0);
	liveLevel.rotation = 0.0;
	liveLevel.tweenFrame = 0;
	liveLevel.indexBG1 = 0;
	liveLevel.indexBG2 = 0;
	liveLevel.indexFG = 0;
	liveLevel.nextIndexBG1 = (1 < level.numBG1 ? 1 : 0);
	liveLevel.nextIndexBG2 = (1 < level.numBG2 ? 1 : 0);
	liveLevel.nextIndexFG = (1 < level.numFG ? 1 : 0);
	
	//score of run
	int score = 0;
	
	//fetch some random starting patterns
	double distance = (double)SCREEN_TOP_DIAG_FROM_CENTER;
	for(int i = 0;  i < TOTAL_PATTERNS_AT_ONE_TIME;  i++) {
		liveLevel.patterns[i] = getLivePattern(level.patterns, level.numPatterns, distance);
		distance = getFurthestWallDistance(liveLevel.patterns[i]);
	} 
	while(aptMainLoop()) {
		
		//LOGIC
		
		//update color frame and clamp
		liveLevel.tweenFrame++;
		if(liveLevel.tweenFrame >= level.speedPulse) {
			liveLevel.tweenFrame = 0;
			liveLevel.indexBG1 = liveLevel.nextIndexBG1;
			liveLevel.indexBG2 = liveLevel.nextIndexBG2;
			liveLevel.indexFG = liveLevel.nextIndexFG;
			liveLevel.nextIndexBG1 = (liveLevel.indexBG1 + 1 < level.numBG1 ? liveLevel.indexBG1 + 1 : 0);
			liveLevel.nextIndexBG2 = (liveLevel.indexBG2 + 1 < level.numBG2 ? liveLevel.indexBG2 + 1 : 0);
			liveLevel.nextIndexFG = (liveLevel.indexFG + 1 < level.numFG ? liveLevel.indexFG + 1 : 0);
		} 
		
		//bring walls forward
		for(int i = 0;  i < TOTAL_PATTERNS_AT_ONE_TIME;  i++) {
			LivePattern pattern =  liveLevel.patterns[i];
			int numWalls = pattern.numWalls;
			for(int iWall = 0; iWall < numWalls; iWall++) {
				pattern.walls[iWall].distance -= level.speedWall;
			}
		}
		
		//rotate level
		liveLevel.rotation += level.speedRotation;
		if(liveLevel.rotation >= TAU) liveLevel.rotation -= TAU;
		if(liveLevel.rotation < 0) liveLevel.rotation  += TAU;
		
		
		//button presses
		ButtonState press = getButton();
		if(press == QUIT) return PROGRAM_QUIT;
		
		//check collision
		MovementState collision = collisionLiveLevel(liveLevel, level.speedCursor);
		if(collision == DEAD) press = BACK; //go back if dead.
		
		//handle player
		switch(press) {
		case BACK:
			//free stuff
			return GAME_OVER;
		case DIR_RIGHT:
			if(collision == CANNOT_MOVE_RIGHT) break;
			liveLevel.cursorPos -= level.speedCursor;
			break;
		case DIR_LEFT:
			if(collision == CANNOT_MOVE_LEFT) break;
			liveLevel.cursorPos += level.speedCursor;
			break;
		default:;
		}
		
		//clamp cursor position
		if(liveLevel.cursorPos >= TAU) liveLevel.cursorPos -= TAU;
		if(liveLevel.cursorPos < 0) liveLevel.cursorPos  += TAU;
		
		//update score
		score++;
		
		//DRAW
		sf2d_start_frame(GFX_TOP, GFX_LEFT);
		drawPlayGame(level, liveLevel, 0);
		
		Color white = {0xFF, 0xFF, 0xFF, 0xFF};
		Point pos = {0,0};
		char framerate[16 + 1];
		snprintf(framerate, 16 + 1, "%.6f FPS", liveLevel.cursorPos);
		writeFont(white, pos, framerate, FONT16);
		
		sf2d_end_frame();
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		drawPlayGameBot(level.name, score, sf2d_get_fps());
		sf2d_end_frame();
		sf2d_swapbuffers();
	}
	return PROGRAM_QUIT;
}
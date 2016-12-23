#include <3ds.h>
#include <sf2d.h>

#include "types.h"
#include "sound.h"
#include "util.h"
#include "draw.h"
#include "logic.h"

MovementState collisionLiveWall(LiveWall wall, double cursorPos, double cursorStep, int sides) {
	
	//Check if we are between the wall vertically
	if(DEF_HEX_FULL_LEN + DEF_HUMAN_PADDING + DEF_HUMAN_HEIGHT < wall.distance || 
	   DEF_HEX_FULL_LEN + DEF_HUMAN_PADDING + DEF_HUMAN_HEIGHT > wall.distance + wall.height) {
		return CAN_MOVE;
	}
	
	double leftRotStep = cursorPos + cursorStep;
	double rightRotStep = cursorPos - cursorStep;
	
	//If the cursor rotates around to 0 TAU (or from 0 TAU to TAU) we need to calculate BOTH ranges on the circle to compensate
	double leftSideRads = (wall.side + 1.0) * TAU/(double)(sides); 
	double leftSideRadsAlt = leftSideRads; 
	double rightSideRads = wall.side * TAU/(double)(sides); 
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
	if((cursorPos >= rightSideRads && cursorPos <= leftSideRads) ||
	   (cursorPos >= rightSideRadsAlt && cursorPos <= leftSideRadsAlt)) {
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

MovementState collisionLiveLevel(LiveLevel live, double cursorStep) {
	MovementState collision = CAN_MOVE;
	
	//for all patterns
	for(int iPattern = 0; iPattern < TOTAL_PATTERNS_AT_ONE_TIME; iPattern++) {
		LivePattern pattern = live.patterns[iPattern];
		
		//for all walls
		for(int iWall = 0; iWall < pattern.numWalls; iWall++) {
			LiveWall wall = pattern.walls[iWall];
			MovementState check = collisionLiveWall(wall, live.cursorPos, cursorStep, pattern.sides);
			
			//update colision
			if(collision == CAN_MOVE) collision = check; //If we can move, try and replace it with something else
			if(check == DEAD)  { //If we are ever dead, return it.
				return DEAD;
			}
		}
	}
	return collision;
}

GameState doMainMenu(GlobalData data, Track select) {
	MainMenu menu = {0};
	while(aptMainLoop()) {
		//LOGIC
		ButtonState press = getButton();
		if(press == QUIT) return PROGRAM_QUIT;
		if(!(menu.transitioning)) {
			if(press == DIR_RIGHT) {
				menu.transitioning = true;
				menu.transitionDirection = 1;
				menu.lastLevel = menu.level;
				menu.level++;
				audioPlay(&select, ONCE);
			}
			if(press == DIR_LEFT) {
				menu.transitioning = true;
				menu.transitionDirection = -1;
				menu.lastLevel = menu.level;
				menu.level--;
				audioPlay(&select, ONCE);
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
#include "logic.h"

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

GameState doMainMenu(GlobalData data) {
	MainMenu menu;
	
	while(aptMainLoop()) {
		//set clear color to something obvious
		sf2d_set_clear_color(RGBA8(0xFF,0x00,0xFF,0xFF));
		sf2d_start_frame(GFX_TOP, GFX_LEFT);
		
		drawMainMenu(data, menu);
		
		sf2d_end_frame();
	}
	
}
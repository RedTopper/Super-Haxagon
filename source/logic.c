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

//Change sides delay
const int FRAMES_PER_CHANGE_SIDE = 36;

//Game over screen
const int FRAMES_PER_GAME_OVER = 60;
const double GAME_OVER_ACCEL_RATE = 1.1;
const double GAME_OVER_ROT_SPEED = TAU/240.0;

//mins and maxes for flipping of the rotation
//(ex: min=60 and max=240 means level will reverse anywhere
// from 1-3.99 seconds)
const int FLIP_FRAMES_MIN = 120;
const int FLIP_FRAMES_MAX = 500;

//The change in rotational speed every time the user "levels up".
const double DIFFICULTY_MULTIPLYER = 1.1;

/** INTERNAL
 * Checks if the cursor has collided with a wall.
 */
MovementState collisionLiveWall(LiveWall wall, double cursorPos, double cursorStep, int sides) {

	//Check if we are between the wall vertically
	if(DEF_HEX_FULL_LEN + DEF_HUMAN_PADDING + DEF_HUMAN_HEIGHT < wall.distance ||
	   DEF_HEX_FULL_LEN + DEF_HUMAN_PADDING + DEF_HUMAN_HEIGHT > wall.distance + wall.height) {
		return CAN_MOVE;
	}

	double leftRotStep = cursorPos + cursorStep;
	double rightRotStep = cursorPos - cursorStep;

	//If the cursor wrapped and the range we need to calculate overflows beyond TAU we also need to check the other equivalent regions:
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

/** INTERNAL
 * Checks if the cursor has collided with the running level level.
 */
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

/** INTERNAL
 * Gets a live pattern from a list of loaded patterns
 * WARNING: This method mallocs the walls array, and needs to be freed
 * upon destruction of the live pattern.
 */
LivePattern getLivePattern(Pattern* patterns, int numPatterns, double distance) {
	int select = rand() % numPatterns;
	Pattern pattern = patterns[select];
	int offset = rand() % pattern.sides;
	LivePattern live;
	live.numWalls = pattern.numWalls;
	live.sides = pattern.sides;
	live.walls = malloc(sizeof(LiveWall) * pattern.numWalls);
	if(!(live.walls)) panic("LIVE PATTERN ERROR!", "No memory!", DEF_DEBUG, 0x0000DEAD);

	for(int i = 0; i < pattern.numWalls; i++) {

		//clamp it to the amount of sides in the pattern.
		live.walls[i].side = pattern.walls[i].side + offset;
		if(live.walls[i].side >= pattern.sides) live.walls[i].side -= pattern.sides;

		live.walls[i].distance = (double)(pattern.walls[i].distance) + distance;
		live.walls[i].height = (double)(pattern.walls[i].height);
	}
	return live;
}

/** INTERNAL
 * Frees a single live pattern.
 */
void freeLivePattern(LivePattern pattern) {
	free(pattern.walls);
}

/** INTERNAL
 * Gets the furthest fixed distance of a live level, including
 * the furthest wall's height.
 */
double getFurthestWallDistance(LivePattern pattern) {
	double maxDistance = 0;
	for(int i = 0; i < pattern.numWalls; i++) {
		double distance = pattern.walls[i].distance + pattern.walls[i].height;
		if(distance > maxDistance) maxDistance = distance;
	}
	return maxDistance;
}

//EXTERNAL
GameState doMainMenu(GlobalData data, LoadedState loaded, Track select, int* level, int showGetBGM, int showLoadLevels) {
	MainMenu menu = {0};
	menu.level = *level;
	while(aptMainLoop()) {

		//LOGIC
		ButtonState press = getButton();
		if(press == QUIT) return PROGRAM_QUIT;
		if(!(menu.transitionDirection)) {
			switch(press) {
			case BACK:
				if(showLoadLevels) return SWITCH_LOAD_LOCATION;
				break;
			case SELECT:
				*level = menu.level;
				return PLAYING;
			case DIR_RIGHT:
				menu.transitionDirection = 1;
				menu.lastLevel = menu.level;
				menu.level++;
				showGetBGM = 0;
				audioPlay(&select, ONCE);
				break;
			case DIR_LEFT:
				menu.transitionDirection = -1;
				menu.lastLevel = menu.level;
				menu.level--;
				showGetBGM = 0;
				audioPlay(&select, ONCE);
				break;
			default:;
			}
		}
		if(menu.level >=  data.numLevels) menu.level = 0;
		if(menu.level < 0) menu.level = data.numLevels - 1;
		if(menu.transitionDirection) menu.transitionFrame++;
		if(menu.transitionFrame >= DEF_FRAMES_PER_TRANSITION) {
			menu.transitionFrame = 0;
			menu.transitionDirection = 0;
		}
		
		//DRAW
		sf2d_start_frame(GFX_TOP, GFX_LEFT);
		drawMainMenu(data, menu);
		sf2d_end_frame();
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		drawMainMenuBot(loaded, sf2d_get_fps(), showGetBGM, showLoadLevels);
		sf2d_end_frame();
		sf2d_swapbuffers();
	}
	return PROGRAM_QUIT;
}

//EXTERNAL
GameState doPlayGame(Level level, LiveLevel* gameOver, Track levelUp) {
	int delayFrame = 0; //tweening between side switches
	int flipFrame = FLIP_FRAMES_MAX; //amount of frames left until flip

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
	liveLevel.score = 0;

	//fetch some random starting patterns
	double distance = (double)SCREEN_TOP_DIAG_FROM_CENTER;
	for(int i = 0;  i < TOTAL_PATTERNS_AT_ONE_TIME;  i++) {
		liveLevel.patterns[i] = getLivePattern(level.patterns, level.numPatterns, distance);
		distance = getFurthestWallDistance(liveLevel.patterns[i]);
	}

	//set up the amount of sides the level should have.
	int lastSides = liveLevel.patterns[0].sides;
	int currentSides = liveLevel.patterns[0].sides;

	while(aptMainLoop()) {

		//current value of sides.
		double sides;

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

		//bring walls forward if we are not delaying
		//otherwise tween from one shape to another.
		if(delayFrame == 0) {
			sides = (double)currentSides;
			for(int i = 0;  i < TOTAL_PATTERNS_AT_ONE_TIME;  i++) {
				LivePattern pattern =  liveLevel.patterns[i];
				for(int iWall = 0; iWall < pattern.numWalls; iWall++) {
					pattern.walls[iWall].distance -= level.speedWall;
				}
			}
		} else {
			double percent = (double)(delayFrame) / (double)(FRAMES_PER_CHANGE_SIDE * abs(currentSides - lastSides));
			sides = linear((double)currentSides, (double)lastSides, percent);
			delayFrame--;
		}

		//shift patterns forward
		if(getFurthestWallDistance(liveLevel.patterns[0]) < DEF_HEX_FULL_LEN) {
			lastSides = liveLevel.patterns[0].sides;

			//free
			freeLivePattern(liveLevel.patterns[0]);
			liveLevel.patterns[0].numWalls = 0; //just in case?

			//shift
			for(int i = 1; i < TOTAL_PATTERNS_AT_ONE_TIME; i++) liveLevel.patterns[i - 1] = liveLevel.patterns[i];

			//generate a new pattern after the last pattern
			double distance = getFurthestWallDistance(liveLevel.patterns[TOTAL_PATTERNS_AT_ONE_TIME - 1]);
			liveLevel.patterns[TOTAL_PATTERNS_AT_ONE_TIME - 1] = getLivePattern(level.patterns, level.numPatterns, distance);
			currentSides = liveLevel.patterns[0].sides;

			//Delay the level if the shifted pattern does  not have the same sides as the last.
			if(lastSides != currentSides) {
				delayFrame = FRAMES_PER_CHANGE_SIDE * abs(currentSides - lastSides);
			}
		}

		//rotate level
		liveLevel.rotation += level.speedRotation;
		if(liveLevel.rotation >= TAU) liveLevel.rotation -= TAU;
		if(liveLevel.rotation < 0) liveLevel.rotation  += TAU;

		//flip level if needed
		flipFrame--;
		if(flipFrame == 0) {
			level.speedRotation *= -1.0;

			//reset the timer to somewhere between [FLIP_FRAMES_MIN, FLIP_FRAMES_MAX)
			flipFrame = FLIP_FRAMES_MIN + rand() % (FLIP_FRAMES_MAX - FLIP_FRAMES_MIN);
		}

		//button presses
		ButtonState press = getButton();

		//check collision
		MovementState collision = collisionLiveLevel(liveLevel, level.speedCursor);
		if(collision == DEAD) press = BACK; //go back if dead.

		//handle player
		switch(press) {
		case QUIT:
			return PROGRAM_QUIT;
		case BACK:
			memcpy(gameOver, &liveLevel, sizeof(LiveLevel)); //copy to game over screen
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
		char* lastScoreText = getScoreText(liveLevel.score);
		liveLevel.score++;
		if(lastScoreText != getScoreText(liveLevel.score)) {
			level.speedRotation *= DIFFICULTY_MULTIPLYER;
			audioPlay(&levelUp, ONCE);
		}

		//DRAW
		sf2d_start_frame(GFX_TOP, GFX_LEFT);
		drawPlayGame(level, liveLevel, 0, sides); //0 = no offset
		sf2d_end_frame();
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		drawPlayGameBot(level, liveLevel, sf2d_get_fps());
		sf2d_end_frame();
		sf2d_swapbuffers();

	}
	return PROGRAM_QUIT;
}

//EXTERNAL
GameState doGameOver(Level level, LiveLevel gameOver) {
	int frames = 0;
	double offsetDistance = 1.0;
	int sides = gameOver.patterns[0].sides;
	while(aptMainLoop()) {

		//LOGIC
		frames++;

		gameOver.rotation += GAME_OVER_ROT_SPEED;
		if(gameOver.rotation >= TAU) gameOver.rotation -= TAU;
		if(gameOver.rotation < 0) gameOver.rotation  += TAU;

		ButtonState press = getButton();
		if(press == QUIT) return PROGRAM_QUIT;

		if(frames <= FRAMES_PER_GAME_OVER) {
			offsetDistance *= GAME_OVER_ACCEL_RATE;
		}
		if(frames == FRAMES_PER_GAME_OVER - 1) {
			for(int i = 0; i < TOTAL_PATTERNS_AT_ONE_TIME; i++){
				freeLivePattern(gameOver.patterns[i]);
				gameOver.patterns[i].numWalls = 0;
			}
		}
		if(frames >= FRAMES_PER_GAME_OVER) {
			switch(press) {
			case SELECT:
				return PLAYING;
			case BACK:
				return MAIN_MENU;
			default:;
			}
		}

		//DRAW
		sf2d_start_frame(GFX_TOP, GFX_LEFT);
		drawPlayGame(level, gameOver, offsetDistance, (double)sides);
		sf2d_end_frame();
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		drawGameOverBot(gameOver.score, level.highScore, sf2d_get_fps(), frames, frames >= FRAMES_PER_GAME_OVER);
		sf2d_end_frame();
		sf2d_swapbuffers();
	}
	return PROGRAM_QUIT;
}

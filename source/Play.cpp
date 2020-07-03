#include "Play.h"

namespace SuperHaxagon {
	Play::Play(Game& game, Level& level) : game(game), platform(game.getPlatform()), level(level) {
		delayFrame = 0; //tweening between side switches
		flipFrame = FLIP_FRAMES_MAX; //amount of frames left until flip

		cursorPos = TAU/4.0 + (level.getSpeedCursor() / 2.0);
		rotation = 0.0;
		tweenFrame = 0;
		indexBG1 = 0;
		indexBG2 = 0;
		indexFG = 0;
		nextIndexBG1 = (level.getColorsBG1().size() > 1 ? 1 : 0);
		nextIndexBG2 = (level.getColorsBG2().size() > 1 ? 1 : 0);
		nextIndexFG = (level.getColorsFG().size() > 1 ? 1 : 0);
		score = 0;

		//fetch some random starting patterns
		auto distance = (double)game.getRenderDistance();
		for(int i = 0;  i < TOTAL_PATTERNS_AT_ONE_TIME;  i++) {
			liveLevel.patterns[i] = getLivePattern(level.patterns, level.numPatterns, distance);
			distance = getFurthestWallDistance(liveLevel.patterns[i]);
		}

		//set up the amount of sides the level should have.
		int lastSides = liveLevel.patterns[0].sides;
		int currentSides = liveLevel.patterns[0].sides;
	}

	std::unique_ptr<State> Play::update() {
		//current value of sides.
		double sides;

		//LOGIC
		//update color frame and clamp
		tweenFrame++;
		if(tweenFrame >= level.getSpeedPulse()) {
			tweenFrame = 0;
			indexBG1 = nextIndexBG1;
			indexBG2 = nextIndexBG2;
			indexFG = nextIndexFG;
			nextIndexBG1 = (indexBG1 + 1 < level.getColorsBG1().size() ? indexBG1 + 1 : 0);
			nextIndexBG2 = (indexBG2 + 1 < level.getColorsBG2().size() ? indexBG2 + 1 : 0);
			nextIndexFG = (indexFG + 1 < level.getColorsFG().size() ? indexFG + 1 : 0);
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
	}

	void Play::draw() {

	}
}
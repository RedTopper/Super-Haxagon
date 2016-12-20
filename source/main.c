#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>
#include <sf2d.h>
#include <time.h>

#include "types.h"
#include "util.h"
#include "levels.h"
#include "draw.h"
#include "logic.h"
#include "font.h"
#include "sound.h"

//Inside hexagon style
const double FULL_LEN = 24.0;
const double BORDER_LEN = 4.0;

//Human triangle style
const double HUMAN_WIDTH = TAU/30;
const double HUMAN_HEIGHT = 5.0;
const double HUMAN_PADDING = 5.0;

//Hexagon Constants
const int FRAMES_PER_ONE_SIDE_ROTATION = 12;
const int MIN_DISTANCE_FROM_LAST_PATTERN = 50;

//Overflow so you don't get glitchy lines between hexagons.
//This is really just some arbituary number so yeah...
const double OVERFLOW_OFFSET = TAU/900.0; 

//Screen diagnal (calculated with a physical calculator)
const int TOP_SCREEN_DIAG_CENTER = 257;

//Game over screen
const double GAME_OVER_ACCEL_RATE = 1.1;
const double GAME_OVER_ROT_SPEED = TAU/240.0;
const int FRAMES_PER_GAME_OVER = 60;

//Pulse color option
const double MAX_RANGE_COLOR = 10;
const double PULSES_PER_SPIN = 3; //may look weird if not int!

//Chance to flip the rotation in a new direction
const int CHANCE_OF_FLIP_MODULO = 5;

//The center of the screen
const Point CENTER = {TOP_WIDTH/2, SCREEN_HEIGHT/2};

int main() {
	//3ds init
	sf2d_init();
	sf2d_set_vblank_wait(1);
	romfsInit();
	ndspInit();
	ndspSetOutputMode(NDSP_OUTPUT_STEREO);
	
	//program init
	initSounds();
	srand(svcGetSystemTick());
	
	audioPlay(&g_hexagon, false);
	
	//Controller
	while (1) {
		break;
	}
	sf2d_fini();
	audioUnload();
	
	gfxExit();	
	romfsExit();	
	ndspExit();	
	return 0;
}

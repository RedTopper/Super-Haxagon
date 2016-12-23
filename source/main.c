#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>
#include <sf2d.h>
#include <time.h>

#include "types.h"
#include "util.h"
#include "levels.h"
#include "sound.h"
#include "logic.h"

//Minimum allowed distance from the last shown pattern
const int MIN_DISTANCE_FROM_LAST_PATTERN = 50;

//Game over screen
const double GAME_OVER_ACCEL_RATE = 1.1;
const double GAME_OVER_ROT_SPEED = TAU/240.0;

//Pulse color option
const double MAX_RANGE_COLOR = 10;
const double PULSES_PER_SPIN = 3; //may look weird if not int!

//Chance to flip the rotation in a new direction
const int CHANCE_OF_FLIP_MODULO = 5;

const char* PROJECT_FILE_NAME = "romfs:/internal.haxagon";

int main() {
	
	//3ds init
	sf2d_init();
	sf2d_set_vblank_wait(1);
	sf2d_set_clear_color(RGBA8(0xFF,0x00,0xFF,0xFF)); //obvious
	romfsInit();
	sdmcInit();
	ndspInit();
	ndspSetOutputMode(NDSP_OUTPUT_STEREO);
	FILE *file = fopen(PROJECT_FILE_NAME, "rb");
	check(!file, "Could not read internal pattern file!", DEF_DEBUG, 0);
	GlobalData data = getData(file);
	
	Track begin;
	Track hexagon; 
	Track over;
	Track select;
	Track mainMenu;
	audioLoad("romfs:/sound/begin.wav", &begin, 0);
	audioLoad("romfs:/sound/hexagon.wav", &hexagon, 1);
	audioLoad("romfs:/sound/over.wav", &over, 2);
	audioLoad("romfs:/sound/select.wav", &select, 3);
	audioLoad("romfs:/bgm/pamgaea.wav", &mainMenu, 4);
	
	//program init
	srand(svcGetSystemTick());
	
	//Controller
	GameState state = MAIN_MENU;
	while(1) {
		switch(state) {
		case MAIN_MENU:
			audioPlay(&hexagon, ONCE);
			audioPlay(&mainMenu, LOOP);
			state = doMainMenu(data, select);
			audioStop(&mainMenu);
			break;
		case PLAYING:
			break;
		case GAME_OVER:
			break;
		case PROGRAM_QUIT:
			break;
		}
		if(state == PROGRAM_QUIT) break;
	}
	
	audioFree(&begin);
	audioFree(&hexagon);
	audioFree(&over);
	audioFree(&select);
	audioFree(&mainMenu);
	
	//close GFX
	sf2d_fini();
	gfxExit();	
	romfsExit();	
	sdmcExit();
	ndspExit();	
	return 0;
}

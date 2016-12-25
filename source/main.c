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

//file location for built in levels
const char* PROJECT_FILE_NAME = "romfs:/levels.haxagon";

int main() {
	
	//3ds init
	sf2d_init();
	sf2d_set_vblank_wait(1);
	sf2d_set_clear_color(RGBA8(0xFF,0x00,0xFF,0xFF)); //obvious
	romfsInit();
	sdmcInit();
	ndspInit();
	ndspSetOutputMode(NDSP_OUTPUT_STEREO);
	
	//pattern loading
	FILE *file = fopen(PROJECT_FILE_NAME, "rb");
	check(!file, "Could not read internal pattern file!", DEF_DEBUG, 0);
	GlobalData data = getData(file);
	
	//program init
	srand(svcGetSystemTick());
	
	//audio loading
	Track begin;
	Track hexagon; 
	Track over;
	Track select;
	Track mainMenu;
	Track bgm;
	audioLoad("romfs:/sound/begin.wav", &begin, 0);
	audioLoad("romfs:/sound/hexagon.wav", &hexagon, 1);
	audioLoad("romfs:/sound/over.wav", &over, 2);
	audioLoad("romfs:/sound/select.wav", &select, 3);
	audioLoad("romfs:/bgm/pamgaea.wav", &mainMenu, 5);
	
	//level selection and game over
	int nlevel = 0;
	int nLastLevel = -1;
	Level level;
	LiveLevel gameOver;
	
	//Controller
	GameState state = MAIN_MENU;
	while(1) {
		switch(state) {
		case MAIN_MENU:
			audioStop(&bgm);
			audioPlay(&hexagon, ONCE);
			audioPlay(&mainMenu, LOOP);
			state = doMainMenu(data, select, &nlevel);
			level = data.levels[nlevel];
			audioStop(&mainMenu);
			if(nlevel != nLastLevel) {
				audioFree(&bgm);
				audioLoad(level.music.str, &bgm, 6);
				nLastLevel = nlevel;
			}
			audioPlay(&bgm, LOOP);
			break;
		case PLAYING:
			audioPlay(&begin, ONCE);
			state = doPlayGame(level, &gameOver);
			break;
		case GAME_OVER:
			audioPlay(&over, ONCE);
			state = doGameOver(level, gameOver);
			break;
		case PROGRAM_QUIT:
			break;
		}
		if(state == PROGRAM_QUIT) break;
	}
	
	//audio free
	audioFree(&begin);
	audioFree(&hexagon);
	audioFree(&over);
	audioFree(&select);
	audioFree(&mainMenu);
	audioFree(&bgm);
	
	//close GFX
	sf2d_fini();
	gfxExit();	
	romfsExit();	
	sdmcExit();
	ndspExit();	
	return 0;
}

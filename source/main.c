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
const char* SDMC_PROJECT_NAME = "sdmc:/3ds/data/haxagon/levels.haxagon";

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
	LoadedState loaded = NOT_LOADED;
	GlobalData data;
	data.loaded = 0;
	
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
	GameState state = SWITCH_LOAD_LOCATION;
	while(1) {
		switch(state) {
		case SWITCH_LOAD_LOCATION:;
			FILE* file;
			switch(loaded) {
			default:
			case NOT_LOADED:
			case SDMC:;
				file = fopen(PROJECT_FILE_NAME, "rb");
				check(!file, "NO INTERNAL FILE!", DEF_DEBUG, 0);
				loaded = ROMFS;
				break;
			case ROMFS:;
				file = fopen(SDMC_PROJECT_NAME, "rb");
				check(!file, "NO EXTERNAL FILE TO LOAD!", DEF_DEBUG, 0);
				loaded = SDMC;
				break;
			}
			freeData(data);
			data = getData(file);
			state = MAIN_MENU;
			nlevel = 0;
			nLastLevel = -1;
			break;
		case MAIN_MENU:
			audioStop(&bgm);
			audioPlay(&hexagon, ONCE);
			audioPlay(&mainMenu, LOOP);
			state = doMainMenu(data, loaded, select, &nlevel);
			level = data.levels[nlevel];
			audioStop(&mainMenu);
			if(state == PLAYING) {
				if(nlevel != nLastLevel) {
					audioFree(&bgm);
					audioLoad(level.music.str, &bgm, 6);
					nLastLevel = nlevel;
				}
				audioPlay(&bgm, LOOP);
			}
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
		default:;
		}
		if(state == PROGRAM_QUIT) break;
	}
	
	//level free
	freeData(data);
	
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

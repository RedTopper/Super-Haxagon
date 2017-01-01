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
#include "score.h"

//file location for built in levels
const char* NAME_ROMFS_PROJECT = "romfs:/levels.haxagon";
const char* NAME_ROMFS_SCORE = "sdmc:/3ds/data/haxagon/dataromfs.db";
const char* NAME_SDMC_PROJECT = "sdmc:/3ds/data/haxagon/levels.haxagon";
const char* NAME_SDMC_SCORE = "sdmc:/3ds/data/haxagon/datasdmc.db";

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
	FILE* file;
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
		case SWITCH_LOAD_LOCATION:
			switch(loaded) {
			default:
			case NOT_LOADED:
			case SDMC:;
				file = fopen(NAME_ROMFS_PROJECT, "rb");
				check(!file, "NO INTERNAL FILE!", "There was no internal file to load. \
				The game was likely compiled incorrectly.", DEF_DEBUG, 0);
				loaded = ROMFS;
				break;
			case ROMFS:;
				file = fopen(NAME_SDMC_PROJECT, "rb");
				check(!file, "NO EXTERNAL FILE TO LOAD!", "There was no external file to load. \
				You need to put external levels in the location defined in the README", DEF_DEBUG, 0);
				loaded = SDMC;
				break;
			}
			freeData(data);
			data = getData(file);
			fclose(file);
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
		case PROGRAM_QUIT:;
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

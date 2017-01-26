#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>
#include <sf2d.h>
#include <time.h>
#include <unistd.h>

#include "types.h"
#include "util.h"
#include "levels.h"
#include "scores.h"
#include "sound.h"
#include "logic.h"

//file locations for levels and scores
const char* NAME_ROMFS_PROJECT = "romfs:" FILE_PROJECT;
const char* NAME_SDMC_PROJECT = "sdmc:" DIR_3DS DIR_DATA DIR_HAXAGON FILE_PROJECT;
const char* NAME_ROMFS_SCORE = "sdmc:" DIR_3DS DIR_DATA DIR_HAXAGON FILE_SCORE_ROMFS;
const char* NAME_SDMC_SCORE = "sdmc:" DIR_3DS DIR_DATA DIR_HAXAGON FILE_SCORE_SDMC;

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
	GlobalData data = EMPTY_GLOBAL_DATA;
	FILE* fileData;
	const char*  scorePath;
	data.loaded = 0;

	//program init
	srand(svcGetSystemTick());

	//audio loading
	Track begin;
	Track hexagon;
	Track over;
	Track rotate;
	Track mainMenu;
	Track levelUp;
	Track bgm = EMPTY_TRACK;
	audioLoad("romfs:/sound/begin.wav", &begin, 0);
	audioLoad("romfs:/sound/hexagon.wav", &hexagon, 1);
	audioLoad("romfs:/sound/over.wav", &over, 2);
	audioLoad("romfs:/sound/select.wav", &rotate, 3);
	audioLoad("romfs:/sound/level.wav", &levelUp, 4);
	audioLoad("romfs:/bgm/pamgaea.wav", &mainMenu, 5);
	int channelBGM = 6; //Last channel + 1. Remember to update this!
	int showGetBGM = 1; //Used to hide the get BGM info after a button press.
    int showLoadLevels = 0; //Used to show option to load levels if external levels exist.
	if(access(NAME_SDMC_PROJECT, 0) != -1) showLoadLevels = 1; //Set true if accessible at startup.
	//Note: will still panic if file is ever removed during runtime when it needs it (by design?)

	//level selection and game over
	int nlevel = 0;
	int nLastLevel = -1;
	Level level = EMPTY_LEVEL;
	LiveLevel gameOver = EMPTY_LIVE_LEVEL;

	//Controller
	GameState state = SWITCH_LOAD_LOCATION;
	while(state != PROGRAM_QUIT) {
		switch(state) {
		case SWITCH_LOAD_LOCATION:
			freeData(data);
			data.loaded = 0;
			switch(loaded) {
			default:
			case NOT_LOADED:
			case SDMC:;
                fileData = fopen(NAME_ROMFS_PROJECT, "rb");
                if(!fileData) panic("NO INTERNAL FILE!", "There was no internal file to load. "
                        "The game was likely compiled incorrectly.", DEF_DEBUG, (int)fileData);
                scorePath = NAME_ROMFS_SCORE;
                loaded = ROMFS;
				break;
			case ROMFS:;
                fileData = fopen(NAME_SDMC_PROJECT, "rb");
                if(!fileData) panic("NO EXTERNAL FILE TO LOAD!", "There was no external file to load. "
                        "You need to put external levels in the location defined in the README", DEF_DEBUG, (int)fileData);
                scorePath = NAME_SDMC_SCORE;
                loaded = SDMC;
				break;
			}
			data = getData(fileData);
			getScores(scorePath, data);
			fclose(fileData);
			state = MAIN_MENU;
			nlevel = 0;
			nLastLevel = -1;
			break;
		case MAIN_MENU:
			audioStop(&bgm);
			audioPlay(&hexagon, ONCE);
			audioPlay(&mainMenu, LOOP);
			state = doMainMenu(data, loaded, rotate, &nlevel, showGetBGM, showLoadLevels);
			level = data.levels[nlevel];
			
			//load audio
			audioStop(&mainMenu);
			if(state == PLAYING) {
				if(nlevel != nLastLevel) {
					audioFree(&bgm);
					audioLoad(level.music.str, &bgm, channelBGM);
					nLastLevel = nlevel;
				}
				audioPlay(&bgm, LOOP);
			}

			//never show again.
			showGetBGM = 0;
			break;
		case PLAYING:
			audioPlay(&begin, ONCE);
			state = doPlayGame(level, &gameOver, levelUp);
			break;
		case GAME_OVER:
			audioPlay(&over, ONCE);
			state = doGameOver(level, gameOver);

			//replace and save high score if needed.
			if(gameOver.score > level.highScore) {
				level.highScore = gameOver.score;
				data.levels[nlevel].highScore = gameOver.score;
				putScores(scorePath, data);
			}
			break;
		case PROGRAM_QUIT:;
		}
	}

	//level free
	freeData(data);

	//audio free
	audioFree(&begin);
	audioFree(&hexagon);
	audioFree(&over);
	audioFree(&rotate);
	audioFree(&mainMenu);
	audioFree(&levelUp);
	audioFree(&bgm);

	//close GFX
	sf2d_fini();
	gfxExit();
	romfsExit();
	sdmcExit();
	ndspExit();
	return 0;
}

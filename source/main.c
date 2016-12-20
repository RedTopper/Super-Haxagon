#include "types.h"
#include "util.h"
#include "levels.h"
#include "draw.h"
#include "logic.h"
#include "font.h"
#include "sound.h"

int main() {
	//3ds init
	sf2d_init();
	sf2d_set_vblank_wait(1);
	romfsInit();
	ndspInit();
	ndspSetOutputMode(NDSP_OUTPUT_STEREO);
	
	//program init
	init(FULL_RESET);
	initPatterns();
	initLevels();
	initLevelData();
	initSounds();
	srand(svcGetSystemTick());
	
	audioPlay(&g_hexagon, false);
	
	//Controller
	while (1) {
		int level = -1;
		hidScanInput();
		u32 kDown = hidKeysDown();
		if(kDown & KEY_START) break;
		
		////DRAW MENU
		if(g_gameState == MAIN_MENU) {
			level = doMainMenu();
			if(level != -1) { //Ran when main menu exits.
				resetLevelData();
				init(PARTIAL_RESET);
				audioPlay(&g_begin, false);
			}
		} else
		
		////PLAY GAME
		if(g_gameState == PLAYING) {
			g_gameState = doPlayGame(); 
			if(g_gameState == GAME_OVER) {
				g_transition = true;
				audioPlay(&g_over, false);
			}
			if(g_gameState == MAIN_MENU) {
				audioStop(&g_bgm);
				audioPlay(&g_hexagon, false);
			}
		} else
		
		////GAME OVER
		if(g_gameState == GAME_OVER) {
			g_gameState = doGameOver(); 
			if(g_gameState == PLAYING) {
				resetLevelData();
				init(PARTIAL_RESET);
				audioPlay(&g_begin, false);
			}
			if(g_gameState == MAIN_MENU) {
				audioStop(&g_bgm);
				audioPlay(&g_hexagon, false);
			}
		}
		
		////DRAW LAGOMETER
		doLagometer(level);
		
		////FLUSH AND CALC LAGOMETER
		sf2d_swapbuffers();
		g_fps = sf2d_get_fps();
		
		////START GAME IF NEEDED
		if(level >= 0) {
			playLevelBGM(level);
			g_gameState = PLAYING;
		}
		g_renderedWalls = 0;
	}
	sf2d_fini();
	freePatterns();
	audioUnload();
	
	gfxExit();	
	romfsExit();	
	ndspExit();	
	return 0;
}

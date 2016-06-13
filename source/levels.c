#include "levels.h"

int g_patterns[NUM_PATTERNS][3][MAX_ELEMENTS_PER_PATTERN] =
  {{{200,200,230,230,260,260},
	{1	,4	,2	,5	,3	,6	},
	{15	,15	,15	,15	,15	,15	}}, //3 on each side, spiral like

   {{200,200,230,230,260,260},
	{1	,4	,2	,5	,3	,6	},
	{15	,15	,15	,15	,15	,15	}}}; //3 on each side, spiral like

void initLevelData() { 
	g_levelData[0].rotStep = TAU/240.0;
	g_levelData[0].rotStepHuman = TAU/60.0;
	g_levelData[0].pulseSpeed = 10.0;
}

void resetLevelData() {
	for(int i = 0; i < TOTAL_LEVELS; i++) {
		g_levelData[i].cursor = TAU/4.0;
		g_levelData[i].radians = 0;
	}
}
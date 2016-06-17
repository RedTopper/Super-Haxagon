#include "levels.h"

Pattern g_patterns[TOTAL_PATTERNS];

Pattern writePattern(int numberOfWalls, int* distanceFromCenter, int* side, int* length) {
	Pattern p;
	p.numberOfWalls = numberOfWalls;
	int dfc[numberOfWalls];
	int s[numberOfWalls];
	int l[numberOfWalls];
	for(int i = 0; i < numberOfWalls; i++) {
		dfc[i] = distanceFromCenter[i];
		s[i] = side[i];
		l[i] = length[i];
	} 
	p.distanceFromCenter = dfc;
	p.side = s;
	p.length = l;
	return p;
}
	
void initLevelData() { 
	//level 0
	g_levelData[0].rotStep = TAU/240.0;
	g_levelData[0].rotStepHuman = TAU/60.0;
	g_levelData[0].pulseSpeed = 10.0;
	
	//level 1
	g_levelData[1].rotStep = TAU/200.0;
	g_levelData[1].rotStepHuman = TAU/60.0;
	g_levelData[1].pulseSpeed = 10.0;
	
	//level 2
	g_levelData[2].rotStep = TAU/100.0;
	g_levelData[2].rotStepHuman = TAU/40.0;
	g_levelData[2].pulseSpeed = 10.0;
	
	//level 3
	g_levelData[3].rotStep = TAU/200.0;
	g_levelData[3].rotStepHuman = TAU/60.0;
	g_levelData[3].pulseSpeed = 10.0;
	
	//level 4
	g_levelData[4].rotStep = TAU/160.0;
	g_levelData[4].rotStepHuman = TAU/60.0;
	g_levelData[4].pulseSpeed = 10.0;
	
	//level 5
	g_levelData[5].rotStep = 0;
	g_levelData[5].rotStepHuman = TAU/60.0;
	g_levelData[5].pulseSpeed = 10.0;
}

void resetLevelData() {
	for(int i = 0; i < TOTAL_LEVELS; i++) {
		g_levelData[i].cursor = TAU/4.0;
		g_levelData[i].radians = 0;
	}
}
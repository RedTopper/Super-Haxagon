#include "levels.h"

/*
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
*/

bool readPatterns() {
	FILE *file = fopen("romfs:/patterns.leve", "rb");
	if(file == NULL){
		return false;
	}
	
	char sig[4];
	fread(sig, 1, 4, file);
	
	if(sig[0] != 'L' && sig[1] != 'E' && sig[2] != 'V' && sig[3] != 'E'){
		fclose(file);
		return false;
	}
	
	if(!(fread(&(g_patterns.numberOfPatterns), 4, 1, file))) {
		fclose(file);
		return false;
	}
	
	
	g_patterns.patterns = malloc(sizeof(Pattern *) * g_patterns.numberOfPatterns); //Alloc pointers to patterns
	if(!g_patterns.patterns) {
		fclose(file);
		return false;
	}
	
	for(int pattern = 0; pattern < g_patterns.numberOfPatterns; pattern++) {
		g_patterns.patterns[pattern] = malloc(sizeof(Pattern)); //Alloc atual pattern.
		if(!g_patterns.patterns[pattern]) {
			fclose(file);
			return false;
		}
		
		if(!(fread(&(g_patterns.patterns[pattern]->numberOfWalls), 4, 1, file))) {
			fclose(file);
			return false;
		}
		
		g_patterns.patterns[pattern]->walls = malloc(sizeof(Wall *) * g_patterns.patterns[pattern]->numberOfWalls); //Alloc pointers to wall
		if(!g_patterns.patterns[pattern]->walls) {
			fclose(file);
			return false;
		}
	
		for(int wall = 0; wall < g_patterns.patterns[pattern]->numberOfWalls; wall++) {
			g_patterns.patterns[pattern]->walls[wall] = malloc(sizeof(Wall)); //Alloc atual wall
			
			if(!(fread(&(g_patterns.patterns[pattern]->walls[wall]), 6, 1, file))) {
				fclose(file);
				return false;
			}
		}
	}
	return true; //I hate C arrays.
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
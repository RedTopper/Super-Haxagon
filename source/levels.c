#include "levels.h"

bool initPatterns() {
	g_patterns.loaded = false;
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
		g_patterns.patterns[pattern] = malloc(sizeof(Pattern)); //Alloc actual pattern.
		if(!g_patterns.patterns[pattern]) {
			fclose(file);
			freePatterns();
			return false;
		}
		
		if(!(fread(&(g_patterns.patterns[pattern]->numberOfWalls), 4, 1, file))) {
			fclose(file);
			freePatterns();
			return false;
		}
		
		g_patterns.patterns[pattern]->walls = malloc(sizeof(Wall *) * g_patterns.patterns[pattern]->numberOfWalls); //Alloc pointers to wall
		if(!g_patterns.patterns[pattern]->walls) {
			fclose(file);
			freePatterns();
			return false;
		}
	
		for(int wall = 0; wall < g_patterns.patterns[pattern]->numberOfWalls; wall++) {
			g_patterns.patterns[pattern]->walls[wall] = malloc(sizeof(Wall)); //Alloc actual wall
			if(!g_patterns.patterns[pattern]->walls[wall]) {
				fclose(file);
				freePatterns();
				return false;
			}
			
			if(!(fread(&(g_patterns.patterns[pattern]->walls[wall]->side), 2, 1, file))) {
				fclose(file);
				freePatterns();
				return false;
			}
			if(!(fread(&(g_patterns.patterns[pattern]->walls[wall]->distanceFromCenter), 2, 1, file))) {
				fclose(file);
				freePatterns();
				return false;
			}
			if(!(fread(&(g_patterns.patterns[pattern]->walls[wall]->length), 2, 1, file))) {
				fclose(file);
				freePatterns();
				return false;
			}
		}
	}
	g_patterns.loaded = true;
	return true; //I hate C arrays.
}

void freePatterns() {
	if(!g_patterns.patterns) {
		return; //Check to see if pattern pointers are alloced
	}
	for(int pattern = 0; pattern < g_patterns.numberOfPatterns; pattern++) {
		if(!g_patterns.patterns[pattern]) {
			continue; //Check to see if an individual pattern is alloced
		}
		if(!g_patterns.patterns[pattern]->walls) {
			continue; //Check to see if wall pointers of the pattern are alloced
		}
		for(int wall = 0; wall < g_patterns.patterns[pattern]->numberOfWalls; wall++) {
			if(!g_patterns.patterns[pattern]->walls[wall]) {
				continue; //Check to see if an individual wall of the individual pattern is alloced
			}
			free(g_patterns.patterns[pattern]->walls[wall]); //Free wall of pattern
		}
		free(g_patterns.patterns[pattern]->walls); //Free wall pointers of pattern
		free(g_patterns.patterns[pattern]); //Free pattern
	}
	free(g_patterns.patterns); //Free pattern pointers
}
	
void initLevelData() { 
	//level 0
	g_levelData[0].rotStep = TAU/240.0;
	g_levelData[0].rotStepHuman = TAU/60.0;
	g_levelData[0].pulseSpeed = 10.0;
	g_levelData[0].wallSpeed = 1;
	
	//level 1
	g_levelData[1].rotStep = TAU/200.0;
	g_levelData[1].rotStepHuman = TAU/38.0;
	g_levelData[1].pulseSpeed = 10.0;
	g_levelData[1].wallSpeed = 2;
	
	//level 2
	g_levelData[2].rotStep = TAU/160.0;
	g_levelData[2].rotStepHuman = TAU/32.0;
	g_levelData[2].pulseSpeed = 10.0;
	g_levelData[2].wallSpeed = 3;
	
	//level 3
	g_levelData[3].rotStep = TAU/120.0;
	g_levelData[3].rotStepHuman = TAU/36.0;
	g_levelData[3].pulseSpeed = 10.0;
	g_levelData[3].wallSpeed = 2;
	
	//level 4
	g_levelData[4].rotStep = TAU/110.0;
	g_levelData[4].rotStepHuman = TAU/30.0;
	g_levelData[4].pulseSpeed = 10.0;
	g_levelData[4].wallSpeed = 3;
	
	//level 5
	g_levelData[5].rotStep = 0;
	g_levelData[5].rotStepHuman = TAU/32.0;
	g_levelData[5].pulseSpeed = 10.0;
	g_levelData[5].wallSpeed = 3;
}

void resetLevelData() {
	for(int i = 0; i < TOTAL_LEVELS; i++) {
		g_levelData[i].cursor = TAU/4.0 + g_levelData[i].rotStepHuman/2.0; //So the player never looks between to walls.
		g_levelData[i].radians = 0;
	}
}
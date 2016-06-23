#include "levels.h"

void distributePatterns() {
	//calc how many patterns each level has
	for(int level = 0; level < TOTAL_LEVELS; level++) {
		g_patterns[level].numberOfPatterns = 0;
		g_patterns[level].loaded = false;
		for(int pattern = 0; pattern < g_allPatterns.numberOfPatterns; pattern++) {
			if(g_allPatterns.patterns[pattern]->levelFlags >> level & 1) {
				g_patterns[level].loaded = true;
				g_patterns[level].numberOfPatterns++;
			}
		}
		//Alloc pointers to patterns
		g_patterns[level].patterns = malloc(sizeof(Pattern *) * g_patterns[level].numberOfPatterns);
	}
	
	//allocate and set pointers.
	for(int level = 0; level < TOTAL_LEVELS; level++) {
		int i = 0;
		for(int pattern = 0; pattern < g_allPatterns.numberOfPatterns; pattern++) {
			if(g_allPatterns.patterns[pattern]->levelFlags >> level & 1) {
				g_patterns[level].patterns[i++] = g_allPatterns.patterns[pattern];
			}
		}	
	}
}

bool initPatterns() {
	g_allPatterns.loaded = false;
	FILE *file = fopen("romfs:/patterns.leve", "rb");
	if(file == NULL){
		return false;
	}
	
	char sig[4];
	fread(sig, 1, 4, file);
	if(!(sig[0] == 'L' && sig[1] == 'E' && sig[2] == 'V' && sig[3] == 'E') ||
	   !(fread(&(g_allPatterns.numberOfPatterns), 4, 1, file))){
		fclose(file);
		return false;
	}
	
	//Alloc pointers to patterns
	g_allPatterns.patterns = malloc(sizeof(Pattern *) * g_allPatterns.numberOfPatterns);
	if(!g_allPatterns.patterns) {
		fclose(file);
		return false;
	}
	
	for(int pattern = 0; pattern < g_allPatterns.numberOfPatterns; pattern++) {
		//Alloc actual pattern.
		g_allPatterns.patterns[pattern] = malloc(sizeof(Pattern)); 
		if(!(g_allPatterns.patterns[pattern]) ||
		   !(fread(&(g_allPatterns.patterns[pattern]->numberOfWalls), 4, 1, file)) ||
		   !(fread(&(g_allPatterns.patterns[pattern]->levelFlags), 1, 1, file))) {
			fclose(file);
			freePatterns();
			return false;
		}
		
		//Alloc pointers to wall
		g_allPatterns.patterns[pattern]->walls = malloc(sizeof(Wall *) * g_allPatterns.patterns[pattern]->numberOfWalls); 
		if(!(g_allPatterns.patterns[pattern]->walls)) {
			fclose(file);
			freePatterns();
			return false;
		}
	
		for(int wall = 0; wall < g_allPatterns.patterns[pattern]->numberOfWalls; wall++) {
			//Alloc actual wall
			g_allPatterns.patterns[pattern]->walls[wall] = malloc(sizeof(Wall));
			if(!(g_allPatterns.patterns[pattern]->walls[wall]) ||
			   !(fread(&(g_allPatterns.patterns[pattern]->walls[wall]->side), 2, 1, file)) ||
			   !(fread(&(g_allPatterns.patterns[pattern]->walls[wall]->distanceFromCenter), 2, 1, file)) ||
			   !(fread(&(g_allPatterns.patterns[pattern]->walls[wall]->length), 2, 1, file))) {
				fclose(file);
				freePatterns();
				return false;
			}
		}
	}
	distributePatterns();
	g_allPatterns.loaded = true;
	return true; //I hate C arrays.
}

void freePatterns() {
	if(!g_allPatterns.patterns) {
		return; //Check to see if pattern pointers are alloced
	}
	for(int pattern = 0; pattern < g_allPatterns.numberOfPatterns; pattern++) {
		if(!g_allPatterns.patterns[pattern]) {
			continue; //Check to see if an individual pattern is alloced
		}
		if(!g_allPatterns.patterns[pattern]->walls) {
			continue; //Check to see if wall pointers of the pattern are alloced
		}
		for(int wall = 0; wall < g_allPatterns.patterns[pattern]->numberOfWalls; wall++) {
			if(!g_allPatterns.patterns[pattern]->walls[wall]) {
				continue; //Check to see if an individual wall of the individual pattern is alloced
			}
			free(g_allPatterns.patterns[pattern]->walls[wall]); //Free wall of pattern
		}
		free(g_allPatterns.patterns[pattern]->walls); //Free wall pointers of pattern
		free(g_allPatterns.patterns[pattern]); //Free pattern
	}
	free(g_allPatterns.patterns); //Free pattern pointers
	
	for(int level = 0; level < TOTAL_LEVELS; level++) {
		free(g_patterns[level].patterns);
	}
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
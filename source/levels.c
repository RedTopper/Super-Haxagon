#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "util.h"
#include "levels.h"

const char* UNLOADED = "ERROR";
const char* PROJECT_HEADER = "HAXAGON1.0";
const char* PROJECT_FOOTER = "ENDHAXAGON";
const char* LEVEL_HEADER = "LEVEL2.0";
const char* LEVEL_FOOTER = "ENDLEVEL";
const char* PATTERN_HEADER = "PATTERN1.0";
const char* PATTERN_FOOTER = "ENDPATTERN";
const char* PROJECT_FILE_NAME = "sdmc:/3ds/data/haxagon/levels.haxagon";

const int MIN_WALL_HEIGHT = 8;
const int MIN_PATTERN_SIDES = 3;

int compare(FILE* file, const char* string) {
	int len = strlen(string);
	char* buff = malloc(sizeof(char) * (len + 1)); //for '/0'
	checkv(buff, "Cannot check file string!", ftell(file));
	fread(buff, sizeof(char), len, file); //no '/0' in file, only len
	buff[len] = '\0'; //add '/0'
	int result = strcmp(buff, string);
	free(buff);
	return result;
}

void* getMalloc(FILE* file, size_t size, int* length, int extra, char* error) {
	if(extra < 0) extra = 0;
	fread(length, sizeof(int), 1, file);
	if(*length > 300) check(0, "Huge alloc found!", ftell(file));
	void* address = malloc(size * (*length + extra));
	checkv(address, error, ftell(file));
	return address;
}

FileString getString(FILE* file) {
	FileString string;
	string.str = getMalloc(file, sizeof(char), &string.len, 1, "Cannot load string from file!");
	string.str[string.len] = '\0';
	return string;
}

Wall getWall(FILE* file, int maxSide) {
	Wall wall;
	
	//wall data
	fread(&wall.distance, sizeof(short), 1, file);
	fread(&wall.height, sizeof(short), 1, file);
	fread(&wall.side, sizeof(short), 1, file);
	
	//check wall data
	if(wall.height < MIN_WALL_HEIGHT) wall.height = MIN_WALL_HEIGHT;
	if(wall.side < 0) wall.side = 0;
	if(wall.side >= maxSide) wall.side = maxSide - 1;
	
	//exit
	return wall;
} 

Color getColor(FILE* file) {
	Color color;
	fread(&color.r, sizeof(short), 1, file);
	fread(&color.g, sizeof(short), 1, file);
	fread(&color.b, sizeof(short), 1, file);
	return color;
}

Pattern getPattern(FILE* file) {
	Pattern pattern;
	
	//pattern (file) name
	pattern.name = getString(file);
	
	//header
	check(compare(file, PATTERN_HEADER), "Pattern header incorrect!", ftell(file));
	
	//number of sides
	fread(&pattern.sides, sizeof(int), 1, file);
	if(pattern.sides < MIN_PATTERN_SIDES) pattern.sides = MIN_PATTERN_SIDES;
	
	//walls
	pattern.walls = getMalloc(file, sizeof(Wall), &pattern.numWalls, 0, "Cannot alloc walls!");
	check(pattern.numWalls, "Pattern must have at least one wall!", ftell(file));
	for(int i = 0; i < pattern.numWalls; i++) pattern.walls[i] = getWall(file, pattern.sides);
	
	//footer
	check(compare(file, PATTERN_FOOTER), "Pattern header incorrect!", ftell(file));
	
	//exit
	return pattern;
}

Pattern locatePattern(FILE* file, Pattern* patterns, int numPatterns) {
	FileString search = getString(file);
	int i = 0;
	for(i = 0; i < numPatterns; i++) {
		if(strcmp(patterns[i].name.str, search.str) == 0) {
			break;
		}
	}
	
	//could not find pattern
	if(i == numPatterns) check(0, "Could not locate pattern!", ftell(file));
	
	//copy pattern if located.
	Pattern located;
	memcpy(&located, &patterns[i], sizeof(Pattern));
	return located;
}

Level getLevel(FILE* file, Pattern* patterns, int numPatterns) {
	Level level;
	
	//header
	check(compare(file, LEVEL_HEADER), "Level header incorrect!", ftell(file));
	
	//strings
	level.name = getString(file);
	level.difficulty = getString(file);
	level.mode = getString(file);
	level.creator = getString(file);
	level.music = getString(file);
	
	//colors
	level.colorsBG1 = getMalloc(file, sizeof(Color), &level.numBG1, 0, "Cannot alloc BG1 colors!");
	check(level.numBG1, "Level must have at least one bg1 color!", ftell(file));
	for(int i = 0; i < level.numBG1; i++) level.colorsBG1[i] = getColor(file);
	level.colorsBG2 = getMalloc(file, sizeof(Color), &level.numBG2, 0, "Cannot alloc BG2 colors!");
	check(level.numBG2, "Level must have at least one bg2 color!", ftell(file));
	for(int i = 0; i < level.numBG2; i++) level.colorsBG2[i] = getColor(file);
	level.colorsFG = getMalloc(file, sizeof(Color), &level.numFG, 0, "Cannot alloc FG colors!");
	check(level.numFG, "Level must have at least one fg color!", ftell(file));
	for(int i = 0; i < level.numFG; i++) level.colorsFG[i] = getColor(file);
	
	//floats
	fread(&level.speedWall, sizeof(float), 1, file);
	fread(&level.speedRotation, sizeof(float), 1, file);
	fread(&level.speedHuman, sizeof(float), 1, file);
	
	//pulsieness
	fread(&level.speedPulse, sizeof(int), 1, file);
	
	//linked patterns (a copy of loaded patterns)
	level.patterns = getMalloc(file, sizeof(Pattern), &level.numPatterns, 0, "Cannot alloc patterns!");
	check(level.numPatterns, "Level must have at least one pattern!", ftell(file));
	for(int i = 0; i < level.numPatterns; i++) level.patterns[i] = locatePattern(file, patterns, numPatterns);
	
	//exit
	return level;
}


//FILE* file = fopen(PROJECT_FILE_NAME, "rb");
//if(!file) {
//	showError("Cannot open levels.haxagon!", 0x0);
//	return NULL;
//}

GlobalData getData(FILE* file) {
	GlobalData data;
	
	//header
	check(compare(file, PROJECT_HEADER), "Wrong file type/format/version!", ftell(file));
	
	//patterns
	data.patterns = getMalloc(file, sizeof(Pattern), &data.numPatterns, 0, "Cannot alloc patterns!");
	check(data.numPatterns, "Must load at least one pattern!", ftell(file));
	for(int i = 0; i < data.numPatterns; i++) data.patterns[i] = getPattern(file);
	
	//levels
	data.levels = getMalloc(file, sizeof(Level), &data.numLevels, 0, "Cannot alloc levels!");
	check(data.numLevels, "Must load at least one level!", ftell(file));
	for(int i = 0; i < data.numLevels; i++) data.levels[i] = getLevel(file, data.patterns, data.numPatterns);
	
	//footer
	check(compare(file, PROJECT_FOOTER), "Project footer incorrect!", ftell(file));
	return data;
}
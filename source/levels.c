#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "types.h"
#include "util.h"
#include "levels.h"

const char* UNLOADED = "ERROR";
const char* PROJECT_HEADER = "HAX1.1";
const char* PROJECT_FOOTER = "ENDHAX";
const char* LEVEL_HEADER = "LEV2.1";
const char* LEVEL_FOOTER = "ENDLEV";
const char* PATTERN_HEADER = "PTN1.1";
const char* PATTERN_FOOTER = "ENDPTN";
const char* BGM_PATH = "sdmc:/3ds/data/haxagon/";

const int MIN_WALL_HEIGHT = 8;
const int MIN_PATTERN_SIDES = 3;


/** INTERNAL
 * Gets a memory address with specific parameters.
 * FILE* file Pointer to a file stream
 * int size The size of the data type to allocate
 * int* length A pointer to a variable to hold the length of the objects read.
 * int extra A value to allocate extra memory
 * char* error A string to show the user if there is a problem allocating memory.
 *
 * This memory must be freed!
 */
void* getMalloc(FILE* file, int size, int* length, int extra, char* message) {
	if(extra < 0) extra = 0;
	fread(length, sizeof(int), 1, file);
	
	if(*length > 300 || *length < 1) check(0, "SIZE ERROR!", message, DEF_DEBUG, ftell(file));

	void* address = malloc(size * (*length + extra));
	check(!address, "MALLOC ERROR!", message, DEF_DEBUG, ftell(file));
	return address;
}

/** INTERNAL
 * Compares a fixed length string to an expected string in a file.
 * (useful for checking both headers and footers)
 */
int compare(FILE* file, const char* string) {
	int len = strlen(string);
	char* buff = malloc(sizeof(char) * (len + 1)); //for '/0'
	
	check(!buff, "NO MEM CHECK!", 
	"It is impossible to  allocate enough RAM to check \
	this string. This should never happen.", DEF_DEBUG, ftell(file));
	
	fread(buff, sizeof(char), len, file); //no '/0' in file, only len
	buff[len] = '\0'; //add '/0'
	int result = strcmp(buff, string);
	free(buff);
	return result;
}

/** INTERNAL
 * Gets a single string from the file. It reads an integer then reads
 * to the length of that integer. Always loads string null terminated, as
 * well as the length
 */
FileString getString(FILE* file) {
	FileString string = EMPTY_STRING;
	
	string.str = getMalloc(file, sizeof(char), &string.len, 1, 
	"Cannot load string from file! All strings must have at least one letter.");
	
	fread(string.str, sizeof(char), string.len, file);
	string.str[string.len] = '\0';
	return string;
}

/** INTERNAL
 * Similar to getString(...), this method obtains a string from a file, but
 * appends a constant string to the beginning of  the buffer. Usefull for
 * adding prefixes to things, such as  "DIFFICULTY: " or "CREATOR: ", 
 * or even a file path location.
 */
FileString getStringPrefix(const char* prefix, FILE* file) {
	FileString string = EMPTY_STRING;
	int prefixlen = strlen(prefix);
	
	string.str = getMalloc(file, sizeof(char), &string.len, prefixlen + 1, 
	"Cannot load string from file with prefix!");
	
	memcpy(string.str, prefix, prefixlen);
	fread(&(string.str[prefixlen]), sizeof(char), string.len, file); //potentially dangerous?
	string.len += prefixlen;
	string.str[string.len] = '\0';
	return string;
}

/** INTERNAL
 * Gets a wall from a file. Also does simple checks to make sure the wall
 * isn't too crazy.
 */
Wall getWall(FILE* file, int maxSide) {
	Wall wall = EMPTY_WALL;
	
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

/**  INTERNAL
 * Gets an RGB color from a file. The alpha is always solid (0xFF).
 */ 
Color getColor(FILE* file) {
	Color color = EMPTY_COLOR;
	fread(&color.r, sizeof(char), 1, file);
	fread(&color.g, sizeof(char), 1, file);
	fread(&color.b, sizeof(char), 1, file);
	color.a = 0xFF;
	return color;
}

/** INTERNAL
 * Gets a pattern from a file.
 */
Pattern getPattern(FILE* file) {
	Pattern pattern = EMPTY_PATTERN;
	
	//pattern (file) name
	pattern.name = getString(file);
	
	//header
	check(compare(file, PATTERN_HEADER), "WRONG PATTERN HEADER!", 
	"The pattern that was being loaded had the wrong header. \
	You probably need to re-export your levels in an updated \
	format to fix this error.", DEF_DEBUG, ftell(file));
	
	//number of sides
	fread(&pattern.sides, sizeof(int), 1, file);
	if(pattern.sides < MIN_PATTERN_SIDES) pattern.sides = MIN_PATTERN_SIDES;
	
	//walls
	pattern.walls = getMalloc(file, sizeof(Wall), &pattern.numWalls, 0, 
	"Cannot alloc walls! Check to see if all patterns have at least one wall.");
	for(int i = 0; i < pattern.numWalls; i++) pattern.walls[i] = getWall(file, pattern.sides);
	
	//footer
	check(compare(file, PATTERN_FOOTER), "WRONG PATTERN FOOTER!", 
	"The pattern that was being loaded had the wrong footer. \
	Try re-exporting your levels in an updated format \
	to fix this error.", DEF_DEBUG, ftell(file));
	
	//exit
	return pattern;
}

/** INTERNAL
 * Frees a pattern from memory
 */
void freePattern(Pattern pattern) {
	free(pattern.walls);
}

/**INTERNAL
 * Locates a pattern in a predefined and preallocated list of patterns based on name.
 * Returns a copy of the found pattern.
 */
Pattern getLoadedPattern(FILE* file, Pattern* patterns, int numPatterns) {
	FileString search = getString(file);
	int i = 0;
	for(i = 0; i < numPatterns; i++) {
		if(!strcmp(patterns[i].name.str, search.str)) {
			break;
		}
	}
	
	free(search.str);
	
	//could not find pattern
	check(i == numPatterns, "NO PATTERN LOCATED!", 
	"Could not locate the pattern that needed to be linked. \
	There was a level in the exported file that referenced a \
	pattern that was not found. Make sure all levels \
	have patterns that actually exist.", DEF_DEBUG, ftell(file));
	
	//copy pattern if located.
	Pattern located;
	memcpy(&located, &patterns[i], sizeof(Pattern));
	return located;
}

/** INTERNAL
 * Loads a level from a file based on the loaded patterns.
 */
Level getLevel(FILE* file, Pattern* patterns, int numPatterns) {
	Level level = EMPTY_LEVEL;
	
	//header
	check(compare(file, LEVEL_HEADER), "WRONG LEVEL HEADER!",  
	"The level that was being loaded had the wrong header. \
	Try re-exporting your levels in an updated format \
	to fix this error.", DEF_DEBUG, ftell(file));
	
	//strings
	level.name = getString(file);
	level.difficulty = getStringPrefix("DIFFICULTY: ", file);
	level.mode = getStringPrefix("MODE: ", file);
	level.creator = getStringPrefix("CREATOR: ", file);
	level.music = getStringPrefix(BGM_PATH, file);
	
	//colors
	level.colorsBG1 = getMalloc(file, sizeof(Color), &level.numBG1, 0, 
	"Cannot allocate BG1 colors! Check to see if all levels have at least 1 background 1 color.");
	for(int i = 0; i < level.numBG1; i++) level.colorsBG1[i] = getColor(file);
	
	level.colorsBG2 = getMalloc(file, sizeof(Color), &level.numBG2, 0, 
	"Cannot allocate BG2 colors! Check to see if all levels have at least 1 background 2 color.");
	for(int i = 0; i < level.numBG2; i++) level.colorsBG2[i] = getColor(file);
	
	level.colorsFG = getMalloc(file, sizeof(Color), &level.numFG, 0, 
	"Cannot allocate FG colors! Check to see if all levels have at least 1 foreground color.");
	for(int i = 0; i < level.numFG; i++) level.colorsFG[i] = getColor(file);
	
	//floats
	fread(&level.speedWall, sizeof(float), 1, file);
	fread(&level.speedRotation, sizeof(float), 1, file);
	fread(&level.speedCursor, sizeof(float), 1, file);
	
	//int
	fread(&level.speedPulse, sizeof(int), 1, file);
	
	//linked patterns (a copy of loaded patterns)
	level.patterns = getMalloc(file, sizeof(Pattern), &level.numPatterns, 0, 
	"Cannot alloc patterns! Check to see if all levels have at least 1 pattern.");
	for(int i = 0; i < level.numPatterns; i++) level.patterns[i] = getLoadedPattern(file, patterns, numPatterns);
	
	//footer
	check(compare(file, LEVEL_FOOTER), "WRONG LEVEL FOOTER!",  
	"The level that was being loaded had the wrong footer. \
	Try re-exporting your levels in an updated format \
	to fix this error.", DEF_DEBUG, ftell(file));
	
	//exit
	return level;
}

/** INTERNAL
 * Frees a level from memory
 */
void freeLevel(Level level) {
	free(level.name.str);
	free(level.difficulty.str);
	free(level.mode.str);
	free(level.creator.str);
	free(level.music.str);
	free(level.colorsBG1);
	free(level.colorsBG2);
	free(level.colorsFG);
	free(level.patterns);
}

//EXTERNAL
GlobalData getData(FILE* file) {
	GlobalData data = EMPTY_GLOBAL_DATA;
	data.loaded = 0;
	
	//header
	check(compare(file, PROJECT_HEADER), "WRONG PROJ HEADER!",
	"The project being loaded had the wrong file header. Either you tried to \
	load a file of the wrong format, or your  project file is out of date. Try \
	re-exporting all levels to fix this problem.", DEF_DEBUG, ftell(file));
	
	//patterns
	data.patterns = getMalloc(file, sizeof(Pattern), &data.numPatterns, 0, 
	"Cannot allocate patterns! You must load at least one pattern!");
	for(int i = 0; i < data.numPatterns; i++) data.patterns[i] = getPattern(file);
	
	//levels
	data.levels = getMalloc(file, sizeof(Level), &data.numLevels, 0, 
	"Cannot allocate levels! You must load at least one level!");
	for(int i = 0; i < data.numLevels; i++) data.levels[i] = getLevel(file, data.patterns, data.numPatterns);
	
	//footer
	check(compare(file, PROJECT_FOOTER), "WRONG PROJ FOOTER!",
	"The project being loaded had the wrong file footer. Your project file \
	is out of date or corrupted. Try re-exporting all levels to fix \
	this problem.", DEF_DEBUG, ftell(file));
	
	data.loaded = 1;
	return data;
}

//EXTERNAL
void freeData(GlobalData data) {
	if(!(data.loaded)) return;
	for(int i = 0; i < data.numPatterns; i++) freePattern(data.patterns[i]);
	free(data.patterns);
	for(int i = 0; i < data.numLevels; i++) freeLevel(data.levels[i]);
	free(data.levels);
}

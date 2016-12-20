#pragma once

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

/**
 * Compares a fixed length string to an expected string in a file.
 * (useful for checking both headers and footers)
 */
int compare(FILE* file, const char* string);

/**
 * Gets a memory address with specific parameters.
 * FILE* file Pointer to a file stream
 * size_t size The size of the struct/data type to allocate
 * int* length A pointer to a variable to hold the length of the objects read.
 * int extra A value to allocate extra memory
 * char* error A string to show the user if there is a problem allocating memory.
 */
void* getMalloc(FILE* file, size_t size, int* length, int extra, char* error);

/**
 * Gets a single string from the file. It reads an integer then reads
 * to the length of that integer. Always loads string null terminated, as
 * well as the length
 */
FileString getString(FILE* file);

/**
 * Gets a wall from a file. Also does simple checks to make sure the wall
 * isn't too crazy.
 */
Wall getWall(FILE* file, int maxSide);

/**
 * Gets an RGB color from a file.
 */ 
Color getColor(FILE* file);

/**
 * Gets a pattern from a file.
 */
Pattern getPattern(FILE* file);

/**
 * Locates a pattern in a predefined and preallocated list of patterns based on name.
 */
Pattern locatePattern(FILE* file, Pattern* patterns, int numPatterns);

/**
 * Loads a level from a file based on the loaded patterns.
 */
Level getLevel(FILE* file, Pattern* patterns, int numPatterns);

/**
 * Completely loads all information from a file.
 */
GlobalData getData(FILE* file);
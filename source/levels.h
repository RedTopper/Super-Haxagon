#pragma once

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
Pattern getLoadedPattern(FILE* file, Pattern* patterns, int numPatterns);

/**
 * Loads a level from a file based on the loaded patterns.
 */
Level getLevel(FILE* file, Pattern* patterns, int numPatterns);

/**
 * Completely loads all information from a file.
 */
GlobalData getData(FILE* file);
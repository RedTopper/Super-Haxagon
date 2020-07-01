#pragma once

/**
 * Gets a memory address with specific parameters.
 * FILE* file Pointer to a file stream
 * int size The size of the data type to allocate
 * int* length A pointer to a variable to hold the length of the objects read.
 * int extra A value to allocate extra memory
 * char* error A string to show the user if there is a problem allocating memory.
 *
 * This memory must be freed!
 */
void* getMalloc(FILE* file, int size, int* length, int extra, char* message);

/**
 * Compares a fixed length string to an expected string in a file.
 * (useful for checking both headers and footers)
 */
int compare(FILE* file, const char* string);

/**
 * Gets a single string from the file. It reads an integer then reads
 * to the length of that integer. Always loads string null terminated, as
 * well as the length
 */
FileString getString(FILE* file);

/**
 * Similar to getString(...), this method obtains a string from a file, but
 * appends a constant string to the beginning of  the buffer. Useful for
 * adding prefixes to things, such as  "DIFFICULTY: " or "CREATOR: ",
 * or even a file path location.
 */
FileString getStringPrefix(const char* prefix, FILE* file);

/**
 * Completely loads all information from a file.
 */
GlobalData getData(FILE* file);

/**
 * Completely frees all loaded information from memory.
 */
void freeData(GlobalData data);  

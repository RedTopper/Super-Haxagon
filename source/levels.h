#pragma once

/**
 * Completely loads all information from a file.
 */
GlobalData getData(FILE* file);

/**
 * Completely frees all loaded information from memory.
 */
void freeData(GlobalData data);  
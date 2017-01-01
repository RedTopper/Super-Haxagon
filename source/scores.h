#pragma once

/**
 * Obtains all of the scores from a file and loads it to
 * the global data.
 */
void getScores(char* path, GlobalData data);

/**
 * writes out all scores to a file. Will attempt
 * to create file if it does not exist.
 */
void putScores(char* path, GlobalData data);

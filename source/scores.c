#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "types.h"
#include "util.h"
#include "levels.h"
#include "scores.h"

#define SCOREDB_HEADER "SCDB1.0"
#define SCOREDB_FOOTER "ENDSCDB"

/** INTERNAL
 * Reads the database file.
 */
void createPath(void) {
	mkdir("sdmc:" DIR_3DS, 0777);
	mkdir("sdmc:" DIR_3DS DIR_DATA, 0777);
	mkdir("sdmc:" DIR_3DS DIR_DATA DIR_HAXAGON, 0777);
}

/** INTERNAL
 * matches the passed score information with the level data.
 */
void match(GlobalData data, FileString name, FileString difficulty, FileString mode, FileString creator, int score) {
	if(!(data.loaded)) return;
	for(int i = 0; i < data.numLevels;  i++) {
		Level level  = data.levels[i];
		if((strcmp(level.name.str, name.str) == 0)  &&
		   (strcmp(level.difficulty.str, difficulty.str) == 0) &&
		   (strcmp(level.mode.str, mode.str) == 0) &&
		   (strcmp(level.creator.str, creator.str) == 0)) {
			data.levels[i].highScore = score;
			break;
		}
	}
}

/** INTERNAL
 * Puts a string into a file.
 */
void putString(FileString string, FILE* file) {
	fwrite(&string.len, sizeof(int), 1, file);
	fwrite(string.str, sizeof(char), string.len, file);
}

//EXTERNAL
void putScores(const char* path, GlobalData data) {
	FILE* file = fopen(path, "wb");
	if(!file) {
		createPath();
		file = fopen(path, "wb");
		if(!file) panic("ERROR SAVING SCORES!", "Could not open the score database in write mode.", DEF_DEBUG, (int)file);
	}

	fwrite(SCOREDB_HEADER, sizeof(char), sizeof(SCOREDB_HEADER) - 1, file);
	fwrite(&data.numLevels, sizeof(int), 1, file);
	for(int i = 0; i < data.numLevels; i++) {
		Level level  = data.levels[i];
		putString(level.name, file);
		putString(level.difficulty, file);
		putString(level.mode, file);
		putString(level.creator, file);
		fwrite(&level.highScore, sizeof(int), 1, file);
	}
	fwrite(SCOREDB_FOOTER, sizeof(char), sizeof(SCOREDB_FOOTER) - 1, file);
	fclose(file);
}

//EXTERNAL
void getScores(const char* path, GlobalData data) {
	FILE* file = fopen(path, "rb");

	//We'll at least create the file  if  it does not exist, so the user
	//can always  save their scores.
	if(!file) {
		putScores(path, data);
		return;
	}

	if(compare(file, SCOREDB_HEADER) != 0) {
		warning("WRONG SCORE DB HEADER!", "Looks like your score database "
				"has the wrong file signature. You'll have to delete your score  database and let the game create a "
				"new one", DEF_DEBUG);
		return;
	}


	int numEntries = 0;
	fread(&numEntries, sizeof(int), 1, file);

	for(int i = 0; i < numEntries; i++) {
		int score = 0;
		FileString name = getString(file);
		FileString difficulty = getString(file);
		FileString mode = getString(file);
		FileString creator = getString(file);
		fread(&score, sizeof(int), 1, file);

		match(data, name, difficulty, mode, creator, score);

		free(name.str);
		free(difficulty.str);
		free(mode.str);
		free(creator.str);
	}

	if(compare(file, SCOREDB_FOOTER) != 0) {
		warning("WRONG SCORE DB FOOTER!", "Looks like your score database "
				"has the wrong file footer. You'll have to delete your score database and let the game create a "
				"new one", DEF_DEBUG);
		return;
	}

	fclose(file);
}

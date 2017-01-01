#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "types.h"
#include "util.h"
#include "levels.h"
#include "scores.h"

const char* SCOREDB_HEADER = "SCDB1.0";
const char* SCOREDB_FOOTER = "ENDSCDB";

/** INTERNAL
 * Reads the database file.
 */
void createDatabase(const char* path) {
	mkdir("sdmc:" DIR_3DS, 0777);
	mkdir("sdmc:" DIR_3DS DIR_DATA, 0777);
	mkdir("sdmc:" DIR_3DS DIR_DATA DIR_HAXAGON, 0777);
	FILE* create = fopen(path, "wb");
	if(!create) panic("SAVE ERROR!", "It is impossible to open the scores data for writing. "
			"You might need to change the permissions of the save data or create the save "
			"database manually.", DEF_DEBUG, 0xDEADF11E);
	fclose(create);
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
		warning(level.name.str, "test", DEF_DEBUG);
		warning(level.difficulty.str, "test", DEF_DEBUG);
		warning(level.mode.str, "test", DEF_DEBUG);
		warning(level.creator.str, "test", DEF_DEBUG);
	}
}

//EXTERNAL
void putScores(const char* path, GlobalData data) {
	FILE* file = fopen(path, "wb");
	if(!file) createDatabase(path);
}

//EXTERNAL
void getScores(const char* path, GlobalData data) {
	FILE* file = fopen(path, "rb");

	//We'll at least create the file  if  it does not exist, so the user
	//can always  save their scores.
	if(!file) {
		createDatabase(path);
		return;
	}

	if(compare(file, SCOREDB_HEADER) != 0) {
		warning("WRONG SCORE DB HEADER!", "Looks like your score database "
				"has the wrong file signature. You'll have to delete your score  database and let the game create a"
				"new one", DEF_DEBUG);
		return;
	}


	int numEntries = 0;
	fread(&numEntries, sizeof(int), 1, file);

	for(int i = 0; i < numEntries; i++) {
		int score = 0;
		FileString name = getString(file);
		FileString difficulty = getStringPrefix(PREFIX_DIFFICULTY, file);
		FileString mode = getStringPrefix(PREFIX_MODE, file);
		FileString creator = getStringPrefix(PREFIX_CREATOR, file);
		fread(&score, sizeof(int), 1, file);

		match(data, name, difficulty, mode, creator, score);

		free(name.str);
		free(difficulty.str);
		free(mode.str);
		free(creator.str);
	}

	if(compare(file, SCOREDB_FOOTER) != 0) {
		warning("WRONG SCORE DB FOOTER!", "Looks like your score database "
				"has the wrong file footer. You'll have to delete your score database and let the game create a"
				"new one", DEF_DEBUG);
		return;
	}

	fclose(file);
}

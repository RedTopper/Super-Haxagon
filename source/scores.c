#include <stdio.h>
#include <sys/stat.h>

#include "types.h"
#include "util.h"
#include "levels.h"
#include "scores.h"

static const char* SCORE_LOAD_ERROR = "There needs to be at least one score here!";

/** INTERNAL
 * Reads the database file.
 */
void createDatabase(char* path) {
	mkdir("sdmc:" DIR_3DS, 0777);
	mkdir("sdmc:" DIR_3DS DIR_DATA, 0777);
	mkdir("sdmc:" DIR_3DS DIR_DATA DIR_HAXAGON, 0777);
	FILE* create = fopen(path, "w");
	if(!create) panic("SAVE ERROR!", "It is impossible to open the scores data for writing. "
			"You might need to change the permissions of the save data or create the save "
			"database manually.", DEF_DEBUG, 0xDEADF11E);
	fclose(create);
}

//EXTERNAL
void getScores(char* path, GlobalData data) {
	FILE* file = fopen(path, "rb");

	//We'll at least create the file  if  it does not exist, so the user
	//can always  save their scores.
	if(!file) {
		createDatabase(path);
		return;
	}

	if(compare(file, "SCDB") != 0) {
		warning("WRONG SCORE DB HEADER!", "Looks like your score database "
				"has the wrong file signature. You'll have to delete your score  database and let the game create a"
				"new one", DEF_DEBUG);
		return;
	}


	int numNames = 0;
	int numDifficulties = 0;
	int numModes = 0;
	int numcreators = 0;
	int numScores = 0;

	//get names allocation
	FileString* names 			= getMalloc(file, sizeof(FileString), &numNames, 0, SCORE_LOAD_ERROR);
	FileString* difficulties	= getMalloc(file, sizeof(FileString), &numDifficulties, 0, SCORE_LOAD_ERROR);
	FileString* modes			= getMalloc(file, sizeof(FileString), &numModes, 0, SCORE_LOAD_ERROR);
	FileString* creators 		= getMalloc(file, sizeof(FileString), &numcreators, 0, SCORE_LOAD_ERROR);
	int* scores 				= getMalloc(file, sizeof(int), &numScores, 0, SCORE_LOAD_ERROR);
	if(!(numNames == numDifficulties && numDifficulties == numModes &&
			numModes == numcreators && numcreators == numScores)) {
		warning("WRONG MAGIC NUMBERS!", "The wrong magic numbers occur in the database file. "
				"Something went wrong when loading your scores.", DEF_DEBUG);
		return;
	}


	fclose(file);
}

#include "score.h"
#include "levels.h"

FILE *readScore_File;
FILE *writeScore_File;
FILE *isSaveThere;

char scores[6];
char path[100] = "/3ds/data/haxagon/savedata/";
char format[4] = ".txt";

char sentencescore[18];

struct stat st = {0};

void checkForSaveData(FileString string){   
	scores[0] = '\0';
	sentencescore[0] = '\0';
	path[0] = '\0';
	strncat(path, "/3ds/data/haxagon/savedata/", 100);
	strncat(path, string.str, 100);
	strncat(path, format, 4);
	
	//This method checks if the save data exists.
	//If it doesnt exist, it creates the directories and creates the save data.
	isSaveThere = fopen(path, "r");	
	if(isSaveThere == NULL) {
		fclose(isSaveThere);
		createSaveData();	
	}else
	{
		fclose(isSaveThere);
	}
	//Clear all strings so that when the high score is updated,
	//it can reload the record for each level.
	
	readScore_File = fopen(path, "r");

	fgets(scores, 9, readScore_File);			
	fclose(readScore_File);	

}
void createSaveData(){
	if(stat("/3ds", &st) == -1){
			mkdir("/3ds", 0700);
			mkdir("/3ds/data", 0700);
			mkdir("/3ds/data/haxagon", 0700);
			mkdir("/3ds/data/haxagon/savedata", 0700);			
	}else if(stat("/3ds/data", &st) == -1){
			mkdir("/3ds/data", 0700);
			mkdir("/3ds/data/haxagon", 0700);
			mkdir("/3ds/data/haxagon/savedata", 0700);	
	}else if(stat("/3ds/data/haxagon", &st) == -1){
			mkdir("/3ds/data/haxagon", 0700);
			mkdir("/3ds/data/haxagon/savedata", 0700);	
	}else if(stat("/3ds/data/haxagon/savedata", &st) == -1){
			mkdir("/3ds/data/haxagon/savedata", 0700);	
	}
	
	writeScore_File = fopen(path, "w");
		
	for(int i = 0; i < 6; i++){
		fputs("000:00", writeScore_File);
		fclose(writeScore_File);
	}
	
	
}
char* showSaveData(){   
		if(strlen(sentencescore) != 15){
			strncat(sentencescore, "SCORE: ", 9);
			strncat(sentencescore, scores,6);
			return sentencescore;
		}
		return NULL;
		
}
bool isCurrentScoreHigher(int level, int score, int decimalpart){
	char currentscore[6];
	char highscore[6];
	

	strcpy(highscore, scores);

	snprintf(currentscore, 6, "%03d:%02d", score, decimalpart);
	
	if(strcmp(highscore, currentscore) < 0){
		return true;
	}
	else if(strcmp(highscore, currentscore) > 0){
		return false;
	}
	return false;//It had to return something in case the above statements didnt.
}
void saveScore(int score, int decimalpart, bool firstCreation){	
		
	writeScore_File = fopen(path, "w+");	

	if(firstCreation == false){
		
		if(score >= 100){
			fprintf(writeScore_File, "%d", score);
			fputs(":", writeScore_File);
			if(decimalpart >= 10){
				fprintf(writeScore_File, "%d", decimalpart);
				fclose(writeScore_File);
			}else{
				fprintf(writeScore_File, "%d", 0);
				fprintf(writeScore_File, "%d", decimalpart);
				fclose(writeScore_File);
			}
		}
		else if(score >= 10){
			fprintf(writeScore_File, "%d", 0);
			fprintf(writeScore_File, "%d", score);
			fputs(":", writeScore_File);
			if(decimalpart >= 10){
				fprintf(writeScore_File, "%d", decimalpart);
				fclose(writeScore_File);
			}else{
				fprintf(writeScore_File, "%d", 0);
				fprintf(writeScore_File, "%d", decimalpart);
				fclose(writeScore_File);
			}
		}
		else if(score >= 0){
			fprintf(writeScore_File, "%d", 0);
			fprintf(writeScore_File, "%d", 0);
			fprintf(writeScore_File, "%d", score);
			fputs(":", writeScore_File);
			if(decimalpart >= 10){
				fprintf(writeScore_File, "%d", decimalpart);
				fclose(writeScore_File);
			}else{
				fprintf(writeScore_File, "%d", 0);
				fprintf(writeScore_File, "%d", decimalpart);
				fclose(writeScore_File);
			}
		}
	}
}
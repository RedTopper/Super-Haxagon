#include "score.h"
#include "levels.h"

FILE *readScore_File;
FILE *writeScore_File;
FILE *isSaveThere;

char scores[6];
char path[300];
char format[4] = ".txt";
char slash[1] = "/";
char sentencescore[13];

char difficultylevel[15];
char creator[30];
char levelmode[15];

struct stat st = {0};

void checkForSaveData(FileString levelname, FileString creatorName, FileString difficulty, FileString mode){   
	scores[0] = '\0';
	sentencescore[0] = '\0';
	path[0] = '\0';
	difficultylevel[0] = '\0';
	creator[0] = '\0';
	levelmode[0] = '\0';
	
	
	strncpy(creator, creatorName.str+9, 30);
	strncpy(difficultylevel, difficulty.str+12, 15);
	strncpy(levelmode, mode.str+6, 15);
	
	strncat(path, "/3ds/data/haxagon/savedata/", 100);
	strncat(path, creator, 100);
	strncat(path, slash, 100);
	strncat(path, difficultylevel, 100);
	strncat(path, slash, 100);
	strncat(path, levelmode, 100);
	strncat(path, slash, 100);
	strncat(path, levelname.str, 100);
	strncat(path, format, 4);
	

	createSaveData(levelname, creator, difficultylevel, levelmode);	

	//Clear all strings so that when the high score is updated,
	//it can reload the record for each level.
	
	readScore_File = fopen(path, "r");

	fgets(scores, 9, readScore_File);			
	fclose(readScore_File);	

}
void createSaveData(FileString levelname, char creatorName[], char difficulty[], char mode[]){
	path[0] = '\0';
	strncat(path, "/3ds/data/haxagon/savedata/", 100);

	
	
	if(stat("/3ds/", &st) == -1){
			mkdir("/3ds/", 0700);
	}if(stat("/3ds/data/", &st) == -1){
			mkdir("/3ds/data/", 0700);
	}if(stat("/3ds/data/haxagon/", &st) == -1){
			mkdir("/3ds/data/haxagon/", 0700);
	}if(stat("/3ds/data/haxagon/savedata/", &st) == -1){
			mkdir("/3ds/data/haxagon/savedata/", 0700);			
	}
	strncat(path, creatorName, 100);
	strncat(path, slash, 100);
	if(stat(path, &st) == -1){
			mkdir(path, 0700);			
	}
	strncat(path, difficulty, 100);
	strncat(path, slash, 100);
	if(stat(path, &st) == -1){
			mkdir(path, 0700);
	}
	strncat(path, mode, 100);
	strncat(path, slash, 100);
	if(stat(path, &st) == -1){
			mkdir(path, 0700);	
	}
	
	strncat(path, levelname.str, 100);
	strncat(path, format, 4);
	
	isSaveThere = fopen(path, "r");	
	if(isSaveThere == NULL) {
		writeScore_File = fopen(path, "w");
		fputs("000:00", writeScore_File);
		fclose(writeScore_File);
		fclose(isSaveThere);
	}else
	{
		fclose(isSaveThere);
	}
}
char* showSaveData(){
		sentencescore[0] = '\0';	
		if(strlen(sentencescore) != 13){
			strncat(sentencescore, "SCORE: ", 7);
			strncat(sentencescore, scores,6);
			return sentencescore;
		}
		return sentencescore;
		
}
bool isCurrentScoreHigher(int score, int decimalpart){
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
void saveScore(int score, int decimalpart){	
		
	writeScore_File = fopen(path, "w+");	
		
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
	fclose(writeScore_File);
}
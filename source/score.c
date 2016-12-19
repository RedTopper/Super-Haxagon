#include "score.h"

FILE *readScore_File[6];
FILE *writeScore_File[6];
FILE *isSaveThere;

char scores1[6];
char scores2[6];
char scores3[6];
char scores4[6];
char scores5[6];
char scores6[6];

char sentencescore1[6];
char sentencescore2[6];
char sentencescore3[6];
char sentencescore4[6];
char sentencescore5[6];
char sentencescore6[6];

char sentencehighscore1[6];
char sentencehighscore2[6];
char sentencehighscore3[6];
char sentencehighscore4[6];
char sentencehighscore5[6];
char sentencehighscore6[6];

struct stat st = {0};

void checkForSaveData(){
	isSaveThere = fopen("/3DS/SuperHaxagon/savedata/level1.txt", "r");	
	if(isSaveThere == NULL) {
		fclose(isSaveThere);
		createSaveData();	
	}			
		fclose(isSaveThere);
		
		scores1[0] = '\0';
		scores2[0] = '\0';
		scores3[0] = '\0';
		scores4[0] = '\0';
		scores5[0] = '\0';
		scores6[0] = '\0';
		 
		sentencescore1[0] = '\0';
		sentencescore2[0] = '\0';
		sentencescore3[0] = '\0';
		sentencescore4[0] = '\0';
		sentencescore5[0] = '\0';
		sentencescore6[0] = '\0';

		sentencehighscore1[0] = '\0';
		sentencehighscore2[0] = '\0';
		sentencehighscore3[0] = '\0';
		sentencehighscore4[0] = '\0';
		sentencehighscore5[0] = '\0';
		sentencehighscore6[0] = '\0';
		
		
		readScore_File[0] = fopen("/3DS/SuperHaxagon/savedata/level1.txt", "r");
		readScore_File[1] = fopen("/3DS/SuperHaxagon/savedata/level2.txt", "r");
		readScore_File[2] = fopen("/3DS/SuperHaxagon/savedata/level3.txt", "r");
		readScore_File[3] = fopen("/3DS/SuperHaxagon/savedata/level4.txt", "r");
		readScore_File[4] = fopen("/3DS/SuperHaxagon/savedata/level5.txt", "r");
		readScore_File[5] = fopen("/3DS/SuperHaxagon/savedata/level6.txt", "r");
		

		
		

		fgets(scores1, 9, readScore_File[0]);			
		fclose(readScore_File[0]);	
		fgets(scores2, 9, readScore_File[1]);			
		fclose(readScore_File[1]);	
		fgets(scores3, 9, readScore_File[2]);			
		fclose(readScore_File[2]);
		fgets(scores4, 9, readScore_File[3]);			
		fclose(readScore_File[3]);
		fgets(scores5, 9, readScore_File[4]);			
		fclose(readScore_File[4]);
		fgets(scores6, 9, readScore_File[5]);			
		fclose(readScore_File[5]);
}

void createSaveData(){
		if(stat("/3DS", &st) == -1){
			mkdir("/3DS", 0700);
			mkdir("/3DS/SuperHaxagon", 0700);
			mkdir("/3DS/SuperHaxagon/savedata", 0700);			
		}else if(stat("/3DS/SuperHaxagon", &st) == -1){
			mkdir("/3DS/SuperHaxagon", 0700);
			mkdir("/3DS/SuperHaxagon/savedata", 0700);
		}else if(stat("/3DS/SuperHaxagon/savedata", &st) == -1){
			mkdir("/3DS/SuperHaxagon/savedata", 0700);
		}
	defaultscores();
}

char* showSaveData(int onlevel, bool isMainMenu){   
	
if(isMainMenu == false){

	if(onlevel == 0) {
		if(strlen(sentencehighscore1) != 6){
			strncat(sentencehighscore1, scores1,6);	
		}		
		return 	sentencehighscore1;
	}
	else if(onlevel == 1) {
		if(strlen(sentencehighscore2) != 6){
			strncat(sentencehighscore2, scores2,6);		
		}			
		return 	sentencehighscore2;
	}
	else if(onlevel == 2) {
		if(strlen(sentencehighscore3) != 6){
			strncat(sentencehighscore3, scores3,6);	
		}			
		return 	sentencehighscore3;
	}
	else if(onlevel == 3) {
		if(strlen(sentencehighscore4) != 6){
			strncat(sentencehighscore4, scores4,6);	
		}		
		return 	sentencehighscore4;
	}
	else if(onlevel == 4) {
		if(strlen(sentencehighscore5) != 6){
			strncat(sentencehighscore5, scores5,6);	
		}		
		return 	sentencehighscore5;
	}
	else if(onlevel == 5) {
		if(strlen(sentencehighscore6) != 6){
			strncat(sentencehighscore6, scores6,6);	
		}		
		return 	sentencehighscore6;
	}
	return "";
  }
else if(isMainMenu == true){	 

	if(onlevel == 0) {
		if(strlen(sentencescore1) != 6){
			strncat(sentencescore1, scores1,6);	
		}		
		return 	sentencescore1;
	}
	else if(onlevel == 1) {
		if(strlen(sentencescore2) != 6){
			strncat(sentencescore2, scores2,6);		
		}			
		return 	sentencescore2;
	}
	else if(onlevel == 2) {
		if(strlen(sentencescore3) != 6){
			strncat(sentencescore3, scores3,6);	
		}			
		return 	sentencescore3;
	}
	else if(onlevel == 3) {
		if(strlen(sentencescore4) != 6){
			strncat(sentencescore4, scores4,6);	
		}		
		return 	sentencescore4;
	}
	else if(onlevel == 4) {
		if(strlen(sentencescore5) != 6){
			strncat(sentencescore5, scores5,6);	
		}		
		return 	sentencescore5;
	}
	else if(onlevel == 5) {
		if(strlen(sentencescore6) != 6){
			strncat(sentencescore6, scores6,6);	
		}		
		return 	sentencescore6;
	}
	return "";
  }
  
   return "";// A blank string is returned if it got past the statements since it had to return something
}
bool isCurrentScoreHigher(int level, int score, int decimalpart){
	char currentscore[6];
	char highscore[6];
	
	if (level == 0){
		strcpy(highscore, scores1);
	}
	else if (level == 1){
		strcpy(highscore, scores2);
	}
	else if (level == 2){
		strcpy(highscore, scores3);
	}
	else if (level == 3){
		strcpy(highscore, scores4);
	}
	else if (level == 4){
		strcpy(highscore, scores5);
	}
	else if (level == 5){
		strcpy(highscore, scores6);
	}
	snprintf(currentscore, 6, "%03d:%02d", score, decimalpart);
	
	if(strcmp(highscore, currentscore) < 0){
		return true;
	}
	else if(strcmp(highscore, currentscore) > 0){
		return false;
	}
}
void saveScore(int level, int score, int decimalpart, bool firstCreation){	
		
	if(level == 0){
		writeScore_File[0] = fopen("/3DS/SuperHaxagon/savedata/level1.txt", "w+");	
	}
	else if(level == 1){
		writeScore_File[1] = fopen("/3DS/SuperHaxagon/savedata/level2.txt", "w+");	
	}
	else if(level == 2){
		writeScore_File[2] = fopen("/3DS/SuperHaxagon/savedata/level3.txt", "w+");	
	}
	else if(level == 3){
		writeScore_File[3] = fopen("/3DS/SuperHaxagon/savedata/level4.txt", "w+");	
	}
	else if(level == 4){
		writeScore_File[4] = fopen("/3DS/SuperHaxagon/savedata/level5.txt", "w+");
	}
	else if(level == 5){
		writeScore_File[5] = fopen("/3DS/SuperHaxagon/savedata/level6.txt", "w+");		
	}	

	if(firstCreation == false){
		
		if(score >= 100){
			fprintf(writeScore_File[level], "%d", score);
			fputs(":", writeScore_File[level]);
			if(decimalpart >= 10){
				fprintf(writeScore_File[level], "%d", decimalpart);
				fclose(writeScore_File[level]);
			}else{
				fprintf(writeScore_File[level], "%d", 0);
				fprintf(writeScore_File[level], "%d", decimalpart);
				fclose(writeScore_File[level]);
			}
		}
		else if(score >= 10){
			fprintf(writeScore_File[level], "%d", 0);
			fprintf(writeScore_File[level], "%d", score);
			fputs(":", writeScore_File[level]);
			if(decimalpart >= 10){
				fprintf(writeScore_File[level], "%d", decimalpart);
				fclose(writeScore_File[level]);
			}else{
				fprintf(writeScore_File[level], "%d", 0);
				fprintf(writeScore_File[level], "%d", decimalpart);
				fclose(writeScore_File[level]);
			}
		}
		else if(score >= 0){
			fprintf(writeScore_File[level], "%d", 0);
			fprintf(writeScore_File[level], "%d", 0);
			fprintf(writeScore_File[level], "%d", score);
			fputs(":", writeScore_File[level]);
			if(decimalpart >= 10){
				fprintf(writeScore_File[level], "%d", decimalpart);
				fclose(writeScore_File[level]);
			}else{
				fprintf(writeScore_File[level], "%d", 0);
				fprintf(writeScore_File[level], "%d", decimalpart);
				fclose(writeScore_File[level]);
			}
		}
	}
}
void defaultscores(){
		writeScore_File[0] = fopen("/3DS/SuperHaxagon/savedata/level1.txt", "w");
		writeScore_File[1] = fopen("/3DS/SuperHaxagon/savedata/level2.txt", "w");	
		writeScore_File[2] = fopen("/3DS/SuperHaxagon/savedata/level3.txt", "w");
		writeScore_File[3] = fopen("/3DS/SuperHaxagon/savedata/level4.txt", "w");	
		writeScore_File[4] = fopen("/3DS/SuperHaxagon/savedata/level5.txt", "w");
		writeScore_File[5] = fopen("/3DS/SuperHaxagon/savedata/level6.txt", "w");	
		
		for(int i = 0; i < 6; i++){
			fputs("000:00", writeScore_File[i]);
			fclose(writeScore_File[i]);
		}
}
//Level up related content is below
void calculateLevelUp(Point p, double score){	
	
	if(score >= 60.0) {
		writeFont(p,"HEXAGON", false);
	}else
	if(score >= 45.0) {
		writeFont(p,"PENTAGON", false);
	}else
	if(score >= 30.0) {
		writeFont(p,"SQUARE", false);
	}else
	if(score >= 20.0) {
		writeFont(p,"TRIANGLE", false);
	}else
	if(score >= 10.0) {
		writeFont(p,"LINE", false);
	}else
	if(score < 10) {
		writeFont(p,"POINT", false);
	}
}
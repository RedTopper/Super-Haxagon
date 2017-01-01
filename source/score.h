#pragma once

#include <3ds.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "types.h"

void checkForSaveData(FileString levelname, FileString creatorName, FileString difficulty, FileString mode);
void createSaveData(FileString levelname, char creatorName[], char difficulty[], char mode[]);
char* showSaveData();
bool isCurrentScoreHigher(int score, int decimalpart);
void saveScore(int score, int decimalPart);
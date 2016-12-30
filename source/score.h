#pragma once

#include <3ds.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "types.h"

void checkForSaveData(FileString string);
void createSaveData();
char* showSaveData();
bool isCurrentScoreHigher();
void saveScore(int score, int decimalPart, bool firstCreation);
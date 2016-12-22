#pragma once

#include <3ds.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "triangle.h"
#include "font.h"

void saveScore(int level, int score, int decimalPart, bool firstCreation);
void createSaveData();
char* showSaveData(int level);
bool isCurrentScoreHigher();
void checkForSaveData();
void calculateLevelUp();
void defaultscores();
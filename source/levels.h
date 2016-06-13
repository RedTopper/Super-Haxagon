#pragma once

#include <3ds.h>

#define TAU 6.28318530718
#define TOTAL_LEVELS 6
#define MAX_ELEMENTS_PER_PATTERN 20
#define NUM_PATTERNS 2

//These defines define the lookups in the arrays to human form.
#define DISTANCE_FROM_CENTER 0
#define SIDE 1
#define LENGTH 2

typedef struct {
	double radians;
	double cursor;
	double rotStep;
	double rotStepHuman;
	double pulseSpeed;
} LevelData;

LevelData g_levelData[TOTAL_LEVELS];

void initLevelData();
void resetLevelData();
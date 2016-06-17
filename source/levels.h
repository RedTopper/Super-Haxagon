#pragma once

#include <3ds.h>

#define TAU 6.28318530718
#define TOTAL_LEVELS 6
#define TOTAL_PATTERNS 2

typedef struct {
	double radians;
	double cursor;
	double rotStep;
	double rotStepHuman;
	double pulseSpeed;
} LevelData;

typedef struct {
	int numberOfWalls;
	int* distanceFromCenter;
	int* side;
	int* length;
} Pattern;

LevelData g_levelData[TOTAL_LEVELS];

Pattern writePattern(int numberOfWalls, int* distanceFromCenter, int* side, int* length);
void initLevelData();
void resetLevelData();
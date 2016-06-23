#pragma once

#include <3ds.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define TAU 6.28318530718
#define TOTAL_LEVELS 6

typedef struct {
	double radians;
	double cursor;
	double rotStep;
	double rotStepHuman;
	double pulseSpeed;
	int wallSpeed;
} LevelData;

typedef struct {
	short side;
	short distanceFromCenter;
	short length;
} Wall;

typedef struct {
	int numberOfWalls;
	u8 levelFlags;
	Wall** walls;
} Pattern;

typedef struct {
	bool loaded;
	int numberOfPatterns;
	Pattern** patterns; 
} Patterns;

LevelData g_levelData[TOTAL_LEVELS];
Patterns g_allPatterns;
Patterns g_patterns[TOTAL_LEVELS];

bool initPatterns();
void freePatterns();
void initLevelData();
void resetLevelData();
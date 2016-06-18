#pragma once

#include <3ds.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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
	short side;
	short distanceFromCenter;
	short length;
} Wall;

typedef struct {
	int numberOfWalls;
	Wall** walls;
} Pattern;

typedef struct {
	bool loaded;
	int numberOfPatterns;
	Pattern** patterns; 
} Patterns;

LevelData g_levelData[TOTAL_LEVELS];
Patterns g_patterns;

bool readPatterns();
void freePatterns()
void initLevelData();
void resetLevelData();
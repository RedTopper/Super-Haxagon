#pragma once

#include <3ds.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define TAU 6.28318530718

const char[] UNLOADED = "ERROR";
const char[] PROJECT_HEADER = "HAXAGON1.0";
const char[] PROJECT_FOOTER = "ENDHAXAGON";
const char[] LEVEL_HEADER = "LEVEL2.0";
const char[] LEVEL_FOOTER = "ENDLEVEL";
const char[] PATTERN_HEADER = "PATTERN1.0";
const char[] PATTERN_FOOTER = "ENDPATTERN";
const char[] PROJECT_FILE_NAME = "sdmc:/3ds/data/haxagon/levels.haxagon";

typedef struct {
	char* str;
	unsigned char len;
} FileString;

typedef struct {
	char r;
	char g;
	char b;
} Color;

typedef struct {
	short distance;
	short height;
	short side;
} Wall;

typedef struct {
	FileString name;
	Wall* walls;
	unsigned int numWalls;
	unsigned char sides;
} Pattern;

typedef struct {
	FileString name;
	FileString difficulty;
	FileString mode;
	FileString creator;
	FileString music;
	Color* colorsBG1;
	Color* colorsBG2;
	Color* colorsFG;
	unsigned char numBG1;
	unsigned char numBG2;
	unsigned char numFG;
	float speedWall;
	float speedRotation;
	float speedHuman;
	float speedPulse;
	Pattern* patterns;
	int numPatterns; 
} Level;

typedef struct {
	Level* levels;
	Pattern* patterns;
	int numLevels;
	int numPatterns;
} GlobalData;

bool initPatterns();
void freePatterns();
void initLevelData();
void resetLevelData();
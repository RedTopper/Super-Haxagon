#pragma once

#define TAU 6.28318530718
#define SCREEN_HEIGHT 240
#define TOP_WIDTH  400
#define BOT_WIDTH  320
#define TOTAL_PATTERNS_AT_ONE_TIME 3

typedef struct {
	char* str;
	int len;
} FileString;

typedef struct {
	char r;
	char g;
	char b;
} Color;

typedef struct {
  int x;
  int y;
} Point;

typedef struct {
	short distance;
	short height;
	short side;
} Wall;

typedef struct {
	double distance;
	double height;
	int side;
} LiveWall;

typedef struct {
	FileString name;
	Wall* walls;
	int numWalls;
	int sides;
} Pattern;

typedef struct {
	LiveWall* walls;
	int numWalls;
	int sides;
} LivePattern;

typedef struct {
	FileString name;
	FileString difficulty;
	FileString mode;
	FileString creator;
	FileString music;
	Color* colorsBG1;
	Color* colorsBG2;
	Color* colorsFG;
	int numBG1;
	int numBG2;
	int numFG;
	float speedWall;
	float speedRotation;
	float speedHuman;
	int speedPulse;
	Pattern* patterns;
	int numPatterns; 
} Level;

typedef struct {
	double cursorPos;
	double rotation;
	double tweenPercent;
	Color currentBG1;
	Color currentBG2;
	Color currentFG;
	Color nextBG1;
	Color nextBG2;
	Color nextFG;
	LivePattern patterns[TOTAL_PATTERNS_AT_ONE_TIME];
} LiveLevel;

typedef struct {
	Level* levels;
	Pattern* patterns;
	int numLevels;
	int numPatterns;
} GlobalData;

typedef struct {
	int rotationFrame;
	int rotationDirection;
	int lastLevel;
	int level;
	int transitioning;
} MainMenu;

typedef enum {
	CAN_MOVE,
	CANNOT_MOVE_LEFT,
	CANNOT_MOVE_RIGHT,
	DEAD,
} MovementState;

typedef enum {
	TOO_CLOSE,
	TOO_FAR,
	RENDERED,
} RenderState;

typedef enum {
	MAIN_MENU,
	PLAYING,
	GAME_OVER,
} GameState;

typedef enum {
	FULL_RESET,
	PARTIAL_RESET,
} ResetTypeGame;

//Inside hexagon style
const double FULL_LEN = 24.0;
const double BORDER_LEN = 4.0;

//Human triangle style
const double HUMAN_WIDTH = TAU/30;
const double HUMAN_HEIGHT = 5.0;
const double HUMAN_PADDING = 5.0;

//Hexagon Constants
const int FRAMES_PER_ONE_SIDE_ROTATION = 12;
const int MIN_DISTANCE_FROM_LAST_PATTERN = 50;

//Overflow so you don't get glitchy lines between hexagons.
//This is really just some arbituary number so yeah...
const double OVERFLOW_OFFSET = TAU/900.0; 

//Screen diagnal (calculated with a physical calculator)
const int TOP_SCREEN_DIAG_CENTER = 257;

//Game over screen
const double GAME_OVER_ACCEL_RATE = 1.1;
const double GAME_OVER_ROT_SPEED = TAU/240.0;
const int FRAMES_PER_GAME_OVER = 60;

//Pulse color option
const double MAX_RANGE_COLOR = 10;
const double PULSES_PER_SPIN = 3; //may look weird if not int!

//Chance to flip the rotation in a new direction
const int CHANCE_OF_FLIP_MODULO = 5;

//The center of the screen
const Point CENTER = {TOP_WIDTH/2, SCREEN_HEIGHT/2};

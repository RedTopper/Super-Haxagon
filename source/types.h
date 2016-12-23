#pragma once

#define TAU 6.28318530718
#define SCREEN_TOP_DIAG_FROM_CENTER 320
#define SCREEN_HEIGHT 240
#define TOP_WIDTH  400
#define BOT_WIDTH  320
#define TOTAL_PATTERNS_AT_ONE_TIME 3

//Inside hexagon style
#define DEF_HEX_FULL_LEN 24.0
#define DEF_HEX_BORDER_LEN 4.0

//Human triangle style
#define DEF_HUMAN_WIDTH TAU/30
#define DEF_HUMAN_HEIGHT 5.0
#define DEF_HUMAN_PADDING 5.0

//Main menu rotation constraints
#define DEF_FRAMES_PER_TRANSITION 12

//debug utils
#define DEF_DEBUG __FILE__, __func__, __LINE__

/**
 * A color but as a struct instead of a long.
 */
typedef struct {
	char r;
	char g;
	char b;
	char a;
} Color;

/**
 * A simple integer representation of an X and Y point.
 */
typedef struct {
	int x;
	int y;
} Point;

/**
 * A string as it appears in a project file.
 */
typedef struct {
	char* str;
	int len;
} FileString;

/**
 * A wall as it appears in the project file.
 */
typedef struct {
	short distance;
	short height;
	short side;
} Wall;

/**
 * A pattern as it appears in a project file.
 */
typedef struct {
	FileString name;
	Wall* walls;
	int numWalls;
	int sides;
} Pattern;

/**
 * Level data as it appears in a file.
 */
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

/**
 * The current state of the main menu.
 */
typedef struct {
	int transitionFrame;
	int transitionDirection;
	int transitioning;
	int lastLevel;
	int level;
} MainMenu;

/**
 * A wall as it appears during runtime.
 */
typedef struct {
	double distance;
	double height;
	int side;
} LiveWall;

/**
 * A pattern as it appears during runtime.
 */
typedef struct {
	LiveWall* walls;
	int numWalls;
	int sides;
} LivePattern;

/**
 * A level as it appears during runtime.
 */
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
	PROGRAM_QUIT,
} GameState;

typedef enum {
	SELECT,
	DIR_LEFT,
	DIR_RIGHT,
	BACK,
	QUIT,
	NOTHING
} ButtonState;
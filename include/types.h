#pragma once

#define TAU 6.28318530718
#define SCREEN_TOP_DIAG_FROM_CENTER 320
#define SCREEN_HEIGHT 240
#define TOP_WIDTH  400
#define BOT_WIDTH  320
#define TOTAL_PATTERNS_AT_ONE_TIME 4

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

//Overflow so you don't get glitchy lines between hexagons.
//This is really just some arbitrary number so yeah...
#define OVERFLOW_OFFSET TAU/900.0

//all paths
#define DIR_3DS "/3ds"
#define DIR_DATA "/data"
#define DIR_HAXAGON "/haxagon"
#define FILE_SCORE_SDMC "/datasdmc.db"
#define FILE_SCORE_ROMFS "/dataromfs.db"
#define FILE_PROJECT "/levels.haxagon"

//prefixes
#define PREFIX_DIFFICULTY "DIFF: "
#define PREFIX_MODE "MODE: "
#define PREFIX_CREATOR "CREATOR: "

/**
 * A color but as a structure instead of a long.
 */
typedef struct {
	char r;
	char g;
	char b;
	char a;
} Color;
extern const Color EMPTY_COLOR;

/**
 * A simple integer representation of an X and Y point.
 */
typedef struct {
	int x;
	int y;
} Point;
extern const Point EMPTY_POINT;

/**
 * A string as it appears in a project file.
 */
typedef struct {
	char* str;
	int len;
} FileString;
extern const FileString EMPTY_STRING;

/**
 * A wall as it appears in the project file.
 */
typedef struct {
	short distance;
	short height;
	short side;
} Wall;
extern const Wall EMPTY_WALL;

/**
 * A pattern as it appears in a project file.
 */
typedef struct {
	FileString name;
	Wall* walls;
	int numWalls;
	int sides;
} Pattern;
extern const Pattern EMPTY_PATTERN;

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
	float speedCursor;
	int speedPulse;
	int highScore;
	Pattern* patterns;
	int numPatterns; 
} Level;
extern const Level EMPTY_LEVEL;

/**
 * The current state of the main menu.
 */
typedef struct {
	int transitionFrame;
	int transitionDirection;
	int lastLevel;
	int level;
} MainMenu;
extern const MainMenu EMPTY_MAIN_MENU;

/**
 * A wall as it appears during runtime.
 */
typedef struct {
	double distance;
	double height;
	int side;
} LiveWall;
extern const LiveWall EMPTY_LIVE_WALL;

/**
 * A pattern as it appears during runtime.
 */
typedef struct {
	LiveWall* walls;
	int numWalls;
	int sides;
} LivePattern;
extern const LivePattern EMPTY_LIVE_PATTERN;

/**
 * A level as it appears during runtime.
 */
typedef struct {
	double cursorPos;
	double rotation;
	int tweenFrame; //tweening colors
	int indexBG1;
	int indexBG2;
	int indexFG;
	int nextIndexBG1;
	int nextIndexBG2;
	int nextIndexFG;
	int score;
	LivePattern patterns[TOTAL_PATTERNS_AT_ONE_TIME];
} LiveLevel;
extern const LiveLevel EMPTY_LIVE_LEVEL;

/**
 * All of the data loaded, ever.
 */
typedef struct {
	int loaded;
	Level* levels;
	Pattern* patterns;
	int numLevels;
	int numPatterns;
} GlobalData;
extern const GlobalData EMPTY_GLOBAL_DATA;

//States
typedef enum {
	CAN_MOVE,
	CANNOT_MOVE_LEFT,
	CANNOT_MOVE_RIGHT,
	DEAD,
} MovementState;

typedef enum {
	MAIN_MENU,
	PLAYING,
	GAME_OVER,
	PROGRAM_QUIT,
	SWITCH_LOAD_LOCATION
} GameState;

typedef enum {
	SELECT,
	DIR_LEFT,
	DIR_RIGHT,
	BACK,
	QUIT,
	NOTHING
} ButtonState;

typedef enum {
	SDMC,
	ROMFS,
	NOT_LOADED
} LoadedState;

#pragma once

#include "header.h"

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
const double OVERFLOW_OFFSET = TAU/900.0; //This is really just some arbituary number so yeah...

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

/**
 * Checks a result and, if it's bad, will hang the program in a loop and
 * display an error to the user with a file offset.
 */
int check(int result, char* message, int offset);

/**
 * Linearlizes two numbers based on a percent between two colors
 */
double linear(double start, double end, double percent);

/**
 * Full linear interpolation of a color.
 */
Color interpolateColor(Color one, Color two, double percent);

/**
 * Calculates a point (usually of a wall) based on the running level's rotation, some offset
 * so it renders correctly, some distance the point should be from the center, the  side the point
 * should be rendered on, based on a total number of sides the shape has.
 */
Point calcPoint(LiveLevel live, double offset, double distance, int side, int numSides);
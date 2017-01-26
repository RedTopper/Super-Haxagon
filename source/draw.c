#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <3ds.h>
#include <sf2d.h>

#include "types.h"
#include "util.h"
#include "font.h"
#include "draw.h"

const Color SHADOW = {0, 0, 0, 0xC0};
const Color BLACK =  {0, 0, 0, 0xFF};
const Color TRANSP = {0, 0, 0, 0xA0};
const Color GREY = {0xA0, 0xA0, 0xA0, 0xFF};
const Color WHITE =  {0xFF, 0xFF, 0xFF, 0xFF};

//Shadow settings
const int SHADOW_X = 4;
const int SHADOW_Y = -4;

//The center of the screen
const Point SCREEN_CENTER = {TOP_WIDTH/2, SCREEN_HEIGHT/2};

//Record pulsing settings
const Color PULSE_LOW = {0xFF, 0xFF, 0xFF, 0x7F};
const Color PULSE_HIGH = {0xFF, 0xFF, 0xFF, 0xFF};
const int PULSE_TIME = 75;
const double PULSE_TIMES = 2.0;

/** INTERNAL
 * Draws a simple triangle using an array of points and a color.
 * The array must have 3 points.
 */
void drawTriangle(Color color, Point points[3]) {
	long paint = RGBA8(color.r,color.g,color.b,color.a);

	//draws a triangle on the correct axis
	sf2d_draw_triangle(
		points[0].x, SCREEN_HEIGHT - 1 - points[0].y,
		points[1].x, SCREEN_HEIGHT - 1 - points[1].y,
		points[2].x, SCREEN_HEIGHT - 1 - points[2].y,
		paint);
}

/** INTERNAL
 * Draws a trapezoid using an array of points and a color.
 * The array must have 4 points.
 */
void drawTrap(Color color, Point points[4]) {
	Point triangle[3];
	triangle[0] = points[0];
	triangle[1] = points[1];
	triangle[2] = points[2];
	drawTriangle(color, triangle);
	triangle[1] = points[2];
	triangle[2] = points[3];
	drawTriangle(color, triangle);
}

/** INTERNAL
 * Draws a rectangle using the super fast 2d library and
 * the color/point data type.
 */
void drawRect(Color color, Point position, Point size) {
	long paint = RGBA8(color.r,color.g,color.b,color.a);
	sf2d_draw_rectangle(position.x, position.y, size.x, size.y, paint);
}

/** INTERNAL
 * Draws a single moving wall based on a live wall, a color, some rotational value, and the total
 * amount of sides that appears.
 */
void drawMovingWall(Color color, Point focus, LiveWall wall, double rotation, double sides) {
	double distance = wall.distance;
	double height = wall.height;
	if(distance + height < DEF_HEX_FULL_LEN) return; //TOO_CLOSE;
	if(distance > SCREEN_TOP_DIAG_FROM_CENTER) return; //TOO_FAR;
	if(wall.side >= sides) return; //NOT_IN_RANGE

	if(distance < DEF_HEX_FULL_LEN - 2.0) {//so the distance is never negative as it enters.
		height -= DEF_HEX_FULL_LEN - 2.0 - distance;
		distance = DEF_HEX_FULL_LEN - 2.0; //Should never be 0!!!
	}

	Point edges[4] = {0};
	edges[0] = calcPointWall(focus, rotation, OVERFLOW_OFFSET, distance, wall.side + 1, sides);
	edges[1] = calcPointWall(focus, rotation, OVERFLOW_OFFSET, distance + height, wall.side + 1, sides);
	edges[2] = calcPointWall(focus, rotation, -OVERFLOW_OFFSET, distance + height, wall.side, sides);
	edges[3] = calcPointWall(focus, rotation, -OVERFLOW_OFFSET, distance, wall.side, sides);
	drawTrap(color, edges);
	return; //RENDERED;
}

/** INTERNAL
 * Completely draws all patterns in a live level. Can also be used to create
 * an "Explosion" effect if you use "offset". (for game overs)
 */
void drawMovingPatterns(Color color, Point focus, LiveLevel live, double offset, double sides) {

	//for all patterns
	for(int iPattern = 0; iPattern < TOTAL_PATTERNS_AT_ONE_TIME; iPattern++) {
		LivePattern pattern = live.patterns[iPattern];

		//draw all walls
		for(int iWall = 0; iWall < pattern.numWalls; iWall++) {
			LiveWall wall = pattern.walls[iWall];
			wall.distance += offset;
			drawMovingWall(color, focus, wall, live.rotation, sides);
		}
	}
}

/** INTERNAL
 * Draws a regular polygon at some point focus. Useful for generating
 * the regular polygon in the center of the screen.
 */
void drawRegular(Color color, Point focus, int height, double rotation, double sides) {
	int exactSides = (int)(sides + 0.99999);

	Point* edges = malloc(sizeof(Point) * exactSides);
	if(!edges) panic("POLYGON ERROR!", "There was an error allocating memory for "
			"a regular polygon. This should never happen.", DEF_DEBUG, 0x0000DEAD);

	//calculate the triangle backwards so it overlaps correctly.
	for(int i = 0; i < exactSides; i++) {
		edges[i].x = (int)(height * cos(rotation + (double)i * TAU/sides) + (double)(focus.x) + 0.5);
		edges[i].y = (int)(height * sin(rotation + (double)i * TAU/sides) + (double)(focus.y) + 0.5);
	}

	Point triangle[3];
	triangle[0] = focus;

	//connect last triangle edge to first
	triangle[1] = edges[exactSides - 1];
	triangle[2] = edges[0];
	drawTriangle(color, triangle);

	//draw rest of regular polygon
	for(int i = 0; i < exactSides - 1; i++) {
		triangle[1] = edges[i];
		triangle[2] = edges[i + 1];
		drawTriangle(color, triangle);
	}

	free(edges);
}

/** INTERNAL
 * Draws the background of the game. It's very colorful.
 */
void drawBackground(Color color1, Color color2, Point focus, double height, double rotation, double sides) {
	int exactSides = (int)(sides + 0.99999);

	//solid background.
	Point position = {0,0};
	Point size = {TOP_WIDTH, SCREEN_HEIGHT};
	drawRect(color1, position, size);

	//This draws the main background.
	Point* edges = malloc(sizeof(Point) * exactSides);
	if(!edges) panic("BG ERROR!", "There was an error allocating memory for "
			"the background. This should never happen.", DEF_DEBUG, 0x0000DEAD);

	for(int i = 0; i < exactSides; i++) {
		edges[i].x = (int)(height * cos(rotation + (double)i * TAU/sides) + (double)(focus.x) + 0.5);
		edges[i].y = (int)(height * sin(rotation + (double)i * TAU/sides) + (double)(focus.y) + 0.5);
	}

	Point triangle[3];
	triangle[0] = focus;

	//if the sides is odd we need to "make up a color" to put in the gap between the last and first color
	if(exactSides % 2) {
		triangle[1] = edges[exactSides - 1];
		triangle[2] = edges[0];
		drawTriangle(interpolateColor(color1, color2, 0.5f), triangle);
	}

	//Draw the rest of the triangles
	for(int i = 0; i < exactSides - 1; i = i + 2) {
		triangle[1] = edges[i];
		triangle[2] = edges[i + 1];
		drawTriangle(color2, triangle);
	}

	free(edges);
}

/** INTERNAL
 * Draws the little cursor in the center of the screen controlled by a human.
 */
void drawHumanCursor(Color color, Point focus, double cursor, double rotation) {
	Point humanTriangle[3];
	humanTriangle[0] = calcPoint(focus, cursor + rotation, 0.0, DEF_HEX_FULL_LEN + DEF_HUMAN_PADDING + DEF_HUMAN_HEIGHT);
	humanTriangle[1] = calcPoint(focus, cursor + rotation, DEF_HUMAN_WIDTH/2, DEF_HEX_FULL_LEN + DEF_HUMAN_PADDING);
	humanTriangle[2] = calcPoint(focus, cursor + rotation, -DEF_HUMAN_WIDTH/2, DEF_HEX_FULL_LEN + DEF_HUMAN_PADDING);
	drawTriangle(color, humanTriangle);
}

/** INTERNAL
 * Draws  the frame rate (passed as a double).
 */
void drawFramerate(double fps) {
	char framerate[12 + 1];
	Point position = {4,SCREEN_HEIGHT - 20};
	snprintf(framerate, 12 + 1, "%.2f FPS", fps);
	writeFont(WHITE, position, framerate, FONT16, ALIGN_LEFT_C);
}

/** INTERNAL
 * Draws a black bock for the background.
 */
void drawBlack(void) {
	Point topLeft = {0,0};
	Point screenSize = {TOP_WIDTH, SCREEN_HEIGHT};
	drawRect(BLACK, topLeft, screenSize);
}

//EXTERNAL
void drawMainMenu(GlobalData data, MainMenu menu) {
	double percentRotated = (double)(menu.transitionFrame) / (double)DEF_FRAMES_PER_TRANSITION;
	double rotation = percentRotated * TAU/6.0;
	if(menu.transitionDirection == -1) { //if the user is going to the left, flip the radians so the animation plays backwards.
		rotation *= -1.0;
	}

	//Colors
	Color FG;
	Color BG1;
	Color BG2;
	Color BG3;
	Level lastLevel = data.levels[menu.lastLevel];
	Level level = data.levels[menu.level];
	if(menu.transitionDirection) {
		FG = interpolateColor(lastLevel.colorsFG[0], level.colorsFG[0], percentRotated);
		BG1 = interpolateColor(lastLevel.colorsBG1[0], level.colorsBG2[0], percentRotated);
		BG2 = interpolateColor(lastLevel.colorsBG2[0], level.colorsBG1[0], percentRotated);
		BG3 = interpolateColor(lastLevel.colorsBG2[0], level.colorsBG2[0], percentRotated); //Real BG2 transition
	} else {
		FG = level.colorsFG[0];
		BG1 = level.colorsBG1[0];
		BG2 = level.colorsBG2[0];
		BG3 = level.colorsBG2[0]; //same as BG2
	}

	Point focus = {TOP_WIDTH/2, SCREEN_HEIGHT/2 - 60};
	Point offsetFocus = {focus.x + SHADOW_X, focus.y + SHADOW_Y};

	//home screen always has 6 sides.
	drawBackground(BG1, BG2, focus, SCREEN_TOP_DIAG_FROM_CENTER, rotation, 6.0);

	//shadows
	drawRegular(SHADOW, offsetFocus, DEF_HEX_FULL_LEN, rotation, 6.0);
	drawHumanCursor(SHADOW, offsetFocus, TAU/4.0, 0);

	//geometry
	drawRegular(FG, focus, DEF_HEX_FULL_LEN, rotation, 6.0);
	drawRegular(BG3, focus, DEF_HEX_FULL_LEN - DEF_HEX_BORDER_LEN, rotation, 6.0);
	drawHumanCursor(FG, focus, TAU/4.0, 0); //Draw cursor fixed quarter circle, no movement.

	//top rectangle and triangle
	int TRIANGLE_WIDTH = 70;
	int GAP_FROM_RIGHT_SIDE = 20;

	//text positions
	Point posTitle = {4, 2};
	Point posDifficulty = {4, posTitle.y + 32 + 1};
	Point posMode = {4, posDifficulty.y + 16 + 1};
	Point posCreator = {4, posMode.y + 16 + 1};
	Point posTime = {4, SCREEN_HEIGHT - 18};

	Point infoPos = {0, 0};
	Point infoSize = {TOP_WIDTH - TRIANGLE_WIDTH - GAP_FROM_RIGHT_SIDE, posCreator.y + 16 + 3};
	Point infoTriangle[3] = {
		{infoSize.x, SCREEN_HEIGHT - 1 - infoSize.y},
		{infoSize.x, SCREEN_HEIGHT - 1},
		{infoSize.x + TRIANGLE_WIDTH, SCREEN_HEIGHT - 1}};
	drawRect(TRANSP, infoPos, infoSize);
	drawTriangle(TRANSP, infoTriangle);

	//score block with triangle
	Point timePos = {0, posTime.y - 3};
	Point timeSize = {10/*chars?*/ * 16 + 4, 16 + 7};
	Point timeTriangle[3] = {
		{timeSize.x, timeSize.y - 3},
		{timeSize.x, - 1}, //why does this have to be -1?
		{timeSize.x + 18, -1}}; //I mean, it works...
	drawRect(TRANSP, timePos, timeSize);
	drawTriangle(TRANSP, timeTriangle);

	//actual text
	char* scoreTime  = getBestTime(level.highScore);
	writeFont(WHITE, posTitle, level.name.str, FONT32, ALIGN_LEFT_C);
	writeFont(GREY, posDifficulty, level.difficulty.str, FONT16, ALIGN_LEFT_C);
	writeFont(GREY, posMode, level.mode.str, FONT16, ALIGN_LEFT_C);
	writeFont(GREY, posCreator, level.creator.str, FONT16, ALIGN_LEFT_C);
	writeFont(WHITE, posTime, scoreTime, FONT16, ALIGN_LEFT_C);
	free(scoreTime);
}

//EXTERNAL
void drawPlayGame(Level level, LiveLevel liveLevel, double offset, double sides) {

	//calculate colors
	double percentTween = (double)(liveLevel.tweenFrame) / (double)(level.speedPulse);
	Color FG = interpolateColor(level.colorsFG[liveLevel.indexFG], level.colorsFG[liveLevel.nextIndexFG], percentTween);
	Color BG1 = interpolateColor(level.colorsBG1[liveLevel.indexBG1], level.colorsBG1[liveLevel.nextIndexBG1], percentTween);
	Color BG2 = interpolateColor(level.colorsBG2[liveLevel.indexBG2], level.colorsBG2[liveLevel.nextIndexBG2], percentTween);

	//fix for triangle levels
	int diagonal = (sides >= 3 && sides < 4 ? SCREEN_TOP_DIAG_FROM_CENTER * 2 : SCREEN_TOP_DIAG_FROM_CENTER);

	drawBackground(BG1, BG2, SCREEN_CENTER, diagonal, liveLevel.rotation, sides);

	//draw shadows
	Point offsetFocus = {SCREEN_CENTER.x + SHADOW_X, SCREEN_CENTER.y + SHADOW_Y};
	drawMovingPatterns(SHADOW, offsetFocus, liveLevel, offset, sides);
	drawRegular(SHADOW, offsetFocus, DEF_HEX_FULL_LEN, liveLevel.rotation, sides);
	drawHumanCursor(SHADOW, offsetFocus, liveLevel.cursorPos, liveLevel.rotation);

	//draw real thing
	drawMovingPatterns(FG, SCREEN_CENTER, liveLevel, offset, sides);
	drawRegular(FG, SCREEN_CENTER, DEF_HEX_FULL_LEN, liveLevel.rotation, sides);
	drawRegular(BG2, SCREEN_CENTER, DEF_HEX_FULL_LEN - DEF_HEX_BORDER_LEN, liveLevel.rotation, sides);
	drawHumanCursor(FG, SCREEN_CENTER, liveLevel.cursorPos, liveLevel.rotation);
}

//EXTERNAL
void drawPanic(const char* message, const char* file, const char* function, int line, int error) {
	Point posAwwSnap = {TOP_WIDTH / 2, 4};
	Point posCrash = {TOP_WIDTH / 2, posAwwSnap.y + 32 + 4};
	Point posInfo = {TOP_WIDTH / 2, posCrash.y + 16 + 2};
	Point posMessage = {TOP_WIDTH / 2, posInfo.y + 32 + 2};
	Point posFile = {TOP_WIDTH / 2, posMessage.y + 16 + 2};
	Point posFunct = {TOP_WIDTH / 2, posFile.y + 16 + 2};
	Point posCheck = {TOP_WIDTH / 2, posFunct.y + 48 + 2};
	Point posButtons = {TOP_WIDTH / 2, posCheck.y + 16 + 2};

	drawBlack();

	writeFont(WHITE, posAwwSnap, "AWW SNAP!", FONT32, ALIGN_CENTER_C);
	writeFont(WHITE, posCrash, "LOOKS LIKE THE GAME CRASHED!", FONT16, ALIGN_CENTER_C);
	writeFont(WHITE, posInfo, "HERE'S SOME INFORMATION:", FONT16, ALIGN_CENTER_C);

	//WARNING: PROGRAM WILL BE MAD IF writeFont(...) EVER MESSES WITH THE STRING!
	writeFont(WHITE, posMessage, (char*)message, FONT16, ALIGN_CENTER_C);
	writeFont(WHITE, posFile, (char*)file, FONT16, ALIGN_CENTER_C);
	writeFont(WHITE, posFunct, (char*)function, FONT16, ALIGN_CENTER_C);

	writeFont(WHITE, posCheck, "CHECK HAXAPANIC.TXT FOR INFO", FONT16, ALIGN_CENTER_C);
	writeFont(WHITE, posButtons, "PRESS START OR HOME TO QUIT", FONT16, ALIGN_CENTER_C);
}

//EXTERNAL
void drawWarning(const char* message, const char* file, const char* function, int line) {
	Point posAwwSnap = {TOP_WIDTH / 2, 4};
	Point posCrash = {TOP_WIDTH / 2, posAwwSnap.y + 32 + 4};
	Point posInfo = {TOP_WIDTH / 2, posCrash.y + 16 + 2};
	Point posMessage = {TOP_WIDTH / 2, posInfo.y + 32 + 2};
	Point posFile = {TOP_WIDTH / 2, posMessage.y + 16 + 2};
	Point posFunct = {TOP_WIDTH / 2, posFile.y + 16 + 2};
	Point posCheck = {TOP_WIDTH / 2, posFunct.y + 48 + 2};
	Point posButtons = {TOP_WIDTH / 2, posCheck.y + 16 + 2};

	drawBlack();

	writeFont(WHITE, posAwwSnap, "WARNING!", FONT32, ALIGN_CENTER_C);
	writeFont(WHITE, posCrash, "THERE SEEMS TO BE A PROBLEM!", FONT16, ALIGN_CENTER_C);
	writeFont(WHITE, posInfo, "HERE'S SOME INFORMATION:", FONT16, ALIGN_CENTER_C);

	//WARNING: PROGRAM WILL BE MAD IF writeFont(...) EVER MESSES WITH THE STRING!
	writeFont(WHITE, posMessage, (char*)message, FONT16, ALIGN_CENTER_C);
	writeFont(WHITE, posFile, (char*)file, FONT16, ALIGN_CENTER_C);
	writeFont(WHITE, posFunct, (char*)function, FONT16, ALIGN_CENTER_C);

	writeFont(WHITE, posCheck, "CHECK HAXAPANIC.TXT FOR INFO", FONT16, ALIGN_CENTER_C);
	writeFont(WHITE, posButtons, "PRESS A TO CONTINUE", FONT16, ALIGN_CENTER_C);
}

//EXTERNAL
void drawBlackBot(void) {
	Point topLeft = {0,0};
	Point screenSize = {BOT_WIDTH, SCREEN_HEIGHT};
	drawRect(BLACK, topLeft, screenSize);
}

//EXTERNAL
void drawMainMenuBot(LoadedState loaded, double fps, int showGetBGM, int showLoadLevels) {
	Point posButton = {4, 4};
	Point posLocation = {210, 4};
	Point posLevels = {4, posLocation.y + 16 + 1};
	Point posDownload = {4, posLevels.y + 32 + 1};
	Point posDownload2 = {4, posDownload.y + 16 + 1};
	Point posDownload3 = {4, posDownload2.y + 16 + 1};

	Point posDownloadHigh = {4, posButton.y};
	Point posDownloadHigh2 = {4, posDownloadHigh.y + 16 + 1};
	Point posDownloadHigh3 = {4, posDownloadHigh2.y + 16 + 1};

	drawBlackBot();
	if(showLoadLevels){
		writeFont(WHITE, posButton, "PRESS B TO LOAD", FONT16, ALIGN_LEFT_C);
		if(loaded == ROMFS) writeFont(WHITE, posLocation, "SDMC", FONT16, ALIGN_LEFT_C);
		if(loaded == SDMC) writeFont(WHITE, posLocation, "ROMFS", FONT16, ALIGN_LEFT_C);
		writeFont(WHITE, posLevels, "LEVELS", FONT16, ALIGN_LEFT_C);
	}
	if(showGetBGM) {
		if(showLoadLevels) {
			writeFont(WHITE, posDownload, "GET BGM FROM GITHUB: ", FONT16, ALIGN_LEFT_C);
			writeFont(WHITE, posDownload2, "REDINQUISITIVE/", FONT16, ALIGN_LEFT_C);
			writeFont(WHITE, posDownload3, "SUPER-HAXAGON!", FONT16, ALIGN_LEFT_C);
		} else {
			writeFont(WHITE, posDownloadHigh, "GET BGM FROM GITHUB: ", FONT16, ALIGN_LEFT_C);
			writeFont(WHITE, posDownloadHigh2, "REDINQUISITIVE/", FONT16, ALIGN_LEFT_C);
			writeFont(WHITE, posDownloadHigh3, "SUPER-HAXAGON!", FONT16, ALIGN_LEFT_C);
		}
	}

	drawFramerate(fps);
}

//EXTERNAL
void drawPlayGameBot(Level level, LiveLevel liveLevel, double fps) {
	Point posLevelUp = {4,4};
	Point posScore = {BOT_WIDTH - 4, 4};
	Point posBest = {BOT_WIDTH - 4, 20};

	drawBlackBot();

	char* scoreTime = getScoreTime(liveLevel.score);
	writeFont(WHITE, posScore, scoreTime, FONT16, ALIGN_RIGHT_C);
	writeFont(WHITE, posLevelUp, getScoreText(liveLevel.score), FONT16, ALIGN_LEFT_C);
	free(scoreTime);

	if(level.highScore > 0 && liveLevel.score > level.highScore) {
		Color textColor = WHITE;
		if (liveLevel.score - level.highScore <= PULSE_TIMES * PULSE_TIME) {
			double percent = getPulse(liveLevel.score, PULSE_TIME, level.highScore);
			textColor = interpolateColor(PULSE_LOW, PULSE_HIGH, percent);
		}
		writeFont(textColor, posBest, "NEW RECORD!", FONT16, ALIGN_RIGHT_C);
	} else {
		char* bestTime = getBestTime(level.highScore);
		writeFont(WHITE, posBest, bestTime, FONT16, ALIGN_RIGHT_C);
		free(bestTime);
	}

	drawFramerate(fps);
}

//EXTERNAL
void drawGameOverBot(int score, int highScore, double fps, int frames, int showText) {
	Point posGameOver = {BOT_WIDTH / 2, 4};
	Point posTime = {BOT_WIDTH / 2, 40};
	Point posBest = {BOT_WIDTH / 2, 56};
	Point posA = {BOT_WIDTH / 2, 78};
	Point posB = {BOT_WIDTH / 2, 94};

	drawBlackBot();

	char* scoreTime  = getScoreTime(score);
	writeFont(WHITE, posGameOver, "GAME OVER", FONT32, ALIGN_CENTER_C);
	writeFont(WHITE, posTime, scoreTime, FONT16, ALIGN_CENTER_C);
	free(scoreTime);

	if(score > highScore) {
		double percent = getPulse(frames, PULSE_TIME, 0);
		Color pulse = interpolateColor(PULSE_LOW, PULSE_HIGH, percent);
		writeFont(pulse, posBest, "NEW RECORD!", FONT16, ALIGN_CENTER_C);
	} else {
		char* bestTime = getBestTime(highScore);
		writeFont(WHITE, posBest, bestTime, FONT16, ALIGN_CENTER_C);
		free(bestTime);
	}

	if(showText) {
		writeFont(WHITE, posA, "PRESS A TO PLAY", FONT16, ALIGN_CENTER_C);
		writeFont(WHITE, posB, "PRESS B TO QUIT", FONT16, ALIGN_CENTER_C);
	}

	drawFramerate(fps);
}

//EXTERNAL
void drawPanicBot(void) {
	Point posCanada = {BOT_WIDTH / 2, SCREEN_HEIGHT / 2 - 16};
	drawBlackBot();
	writeFont(WHITE, posCanada, "SORRY", FONT32, ALIGN_CENTER_C);
}

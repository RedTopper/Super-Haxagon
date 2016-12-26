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
 * Draws a trapizoid using an array of points and a color.
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
 * the color/point datatype.
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
 * Draws a regular polygon at some point focus. Usefull for generating
 * the regular polygon in the center of the screen.
 */
void drawRegular(Color color, Point focus, int height, double rotation, double sides) {
	int exactSides = (int)(sides + 0.99999);
	
	Point* edges = malloc(sizeof(Point) * exactSides);
	check(!edges, "Error drawing regular polygon!", DEF_DEBUG, 0x0);
	
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
	check(!edges, "Error drawing background!", DEF_DEBUG, 0x0);
	
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
 * Draws  the framerate (passed as a double).
 */
void drawFramerate(double fps) {
	char framerate[12 + 1];
	Point position = {4,SCREEN_HEIGHT - 20};
	snprintf(framerate, 12 + 1, "%.2f FPS", fps);
	writeFont(WHITE, position, framerate, FONT16, ALIGN_LEFT_C);
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
	if(menu.transitioning) {
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

	//text positions
	Point title = {4, 4};
	Point difficulty = {4, 40};
	Point mode = {4, 56};
	Point creator = {4, 72};
	Point time = {4, SCREEN_HEIGHT - 18};
	
	//top rectangle and triangle
	int triangleWidth = 70;
	int distanceFromRightSide = 30;
	
	Point infoPos = {0, 0};
	Point infoSize = {TOP_WIDTH - triangleWidth - distanceFromRightSide, creator.y + 16 + 2};
	drawRect(TRANSP, infoPos, infoSize);
	Point triangle1[3] = {
		{infoSize.x, SCREEN_HEIGHT - 1 - infoSize.y},
		{infoSize.x, SCREEN_HEIGHT - 1},
		{infoSize.x + triangleWidth, SCREEN_HEIGHT - 1}};
	drawTriangle(TRANSP, triangle1);
	
	//score block with triangle
	Point timePos = {0, time.y - 4};
	Point timeSize = {11/*chars*/ * 16, 16 + 8};
	drawRect(TRANSP, timePos, timeSize);
	Point triangle2[3] = {
		{timeSize.x, timeSize.y - 2}, //Why -2?
		{timeSize.x, - 1}, //why does this have to be -1?
		{timeSize.x + 18, -1}}; //I mean, it works...
	drawTriangle(TRANSP, triangle2);

	//actual text
	writeFont(WHITE, title, level.name.str, FONT32, ALIGN_LEFT_C);
	writeFont(GREY, difficulty, level.difficulty.str, FONT16, ALIGN_LEFT_C);
	writeFont(GREY, mode, level.mode.str, FONT16, ALIGN_LEFT_C);
	writeFont(GREY, creator, level.creator.str, FONT16, ALIGN_LEFT_C);
	writeFont(WHITE, time, "SCORE: ??????", FONT16, ALIGN_LEFT_C);
}

//EXTERNAL
void drawMainMenuBot(LoadedState loaded, double fps) {
	Point topLeft = {0,0};
	Point screenSize = {BOT_WIDTH, SCREEN_HEIGHT};
	drawRect(BLACK, topLeft, screenSize);
	
	Point location = {4, 4};
	writeFont(WHITE, location, "PRESS B TO LOAD", FONT16, ALIGN_LEFT_C);
	
	Point state = {194, 4};
	if(loaded == ROMFS) writeFont(WHITE, state, "SDMC", FONT16, ALIGN_LEFT_C);
	if(loaded == SDMC) writeFont(WHITE, state, "ROMFS", FONT16, ALIGN_LEFT_C);
	
	drawFramerate(fps);
}

//EXTERNAL
void drawPlayGame(Level level, LiveLevel liveLevel, double offset, double sides) {
	
	//calculate colors
	double percentTween = (double)(liveLevel.tweenFrame) / (double)(level.speedPulse);
	Color FG = interpolateColor(level.colorsFG[liveLevel.indexFG], level.colorsFG[liveLevel.nextIndexFG], percentTween);
	Color BG1 = interpolateColor(level.colorsBG1[liveLevel.indexBG1], level.colorsBG1[liveLevel.nextIndexBG1], percentTween);
	Color BG2 = interpolateColor(level.colorsBG2[liveLevel.indexBG2], level.colorsBG2[liveLevel.nextIndexBG2], percentTween);
	
	//fix for triangle levels 
	int diagnal = (sides >= 3 && sides < 4 ? SCREEN_TOP_DIAG_FROM_CENTER * 2 : SCREEN_TOP_DIAG_FROM_CENTER);
	
	drawBackground(BG1, BG2, SCREEN_CENTER, diagnal * 2, liveLevel.rotation, sides);
	
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
void drawPlayGameBot(FileString name, int score, double fps) {
	Point topLeft = {0,0};
	Point screenSize = {BOT_WIDTH, SCREEN_HEIGHT};
	drawRect(BLACK, topLeft, screenSize);
	
	Point levelUpPosition = {4,4};
	writeFont(WHITE, levelUpPosition, getScoreText(score), FONT16, ALIGN_LEFT_C);
	
	Point scorePosition = {BOT_WIDTH - 4, 4};
	char buffer[6 + 1]; //null term
	int scoreInt = (int)((double)score/60.0);
	int decimalPart = (int)(((double)score/60.0 - (double)scoreInt) * 100.0);
	snprintf(buffer, 6 + 1, "%03d:%02d", scoreInt, decimalPart); //Emergency stack overflow prevention
	writeFont(WHITE, scorePosition, buffer, FONT16, ALIGN_RIGHT_C);
	
	drawFramerate(fps);
}

//EXTERNAL
void drawGameOverBot(int score, double fps, int frame) {
	Point topLeft = {0,0};
	Point screenSize = {BOT_WIDTH, SCREEN_HEIGHT};
	drawRect(BLACK, topLeft, screenSize);

	Point gameOverPosition = {BOT_WIDTH / 2, 4};
	writeFont(WHITE, gameOverPosition, "GAME OVER", FONT32, ALIGN_CENTER_C);
	
	Point timePosition = {BOT_WIDTH / 2, 40};
	char buffer[12+1];
	int scoreInt = (int)((double)score/60.0);
	int decimalPart = (int)(((double)score/60.0 - (double)scoreInt) * 100.0);
	snprintf(buffer, 12+1, "TIME: %03d:%02d", scoreInt, decimalPart);
	writeFont(WHITE, timePosition, buffer, FONT16, ALIGN_CENTER_C);
	
	if(frame == 0) {
		Point aPosition = {BOT_WIDTH / 2, 70};
		writeFont(WHITE, aPosition, "PRESS A TO PLAY", FONT16, ALIGN_CENTER_C);
		Point bPosition = {BOT_WIDTH / 2, 86};
		writeFont(WHITE, bPosition, "PRESS B TO QUIT", FONT16, ALIGN_CENTER_C);
	}
	
	drawFramerate(fps);
}

//EXTERNAL
void drawPanic(const char* message, const char* file, const char* function, int line, int error) {
	Point topLeft = {0,0};
	Point screenSize = {TOP_WIDTH, SCREEN_HEIGHT};
	drawRect(BLACK, topLeft, screenSize);
	
	Point awwsnap = {TOP_WIDTH / 2, 4};
	writeFont(WHITE, awwsnap, "AWW SNAP!", FONT32, ALIGN_CENTER_C);
	
	Point crash = {TOP_WIDTH / 2, awwsnap.y + 32 + 4};
	writeFont(WHITE, crash, "LOOKS LIKE THE GAME CRASHED!", FONT16, ALIGN_CENTER_C);
	
	Point info = {TOP_WIDTH / 2, crash.y + 16 + 2};
	writeFont(WHITE, info, "HERE'S SOME INFORMATION:", FONT16, ALIGN_CENTER_C);
	
	//WARNING: PROGRAM WILL SHIT ITSELF IF writeFont(...) EVER MESSES WITH THE STRING! 
	Point msg = {TOP_WIDTH / 2, info.y + 32 + 2};
	writeFont(WHITE, msg, (char*)message, FONT16, ALIGN_CENTER_C);
	Point fl = {TOP_WIDTH / 2, msg.y + 16 + 2};
	writeFont(WHITE, fl, (char*)file, FONT16, ALIGN_CENTER_C);
	Point fc = {TOP_WIDTH / 2, fl.y + 16 + 2};
	writeFont(WHITE, fc, (char*)function, FONT16, ALIGN_CENTER_C);
	
	Point check = {TOP_WIDTH / 2, fc.y + 48 + 2};
	writeFont(WHITE, check, "CHECK HAXAPANIC.TXT FOR INFO", FONT16, ALIGN_CENTER_C);
	Point buttons = {TOP_WIDTH / 2, check.y + 16 + 2};
	writeFont(WHITE, buttons, "PRESS START OR HOME TO QUIT", FONT16, ALIGN_CENTER_C);
}

//EXTERNAL
void drawPanicBot() {
	Point topLeft = {0,0};
	Point screenSize = {BOT_WIDTH, SCREEN_HEIGHT};
	drawRect(BLACK, topLeft, screenSize);
	
	Point frown = {BOT_WIDTH / 2, SCREEN_HEIGHT / 2 - 16};
	writeFont(WHITE, frown, "SORRY", FONT32, ALIGN_CENTER_C);
}
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <3ds.h>
#include <sf2d.h>

#include "types.h"
#include "util.h"
#include "draw.h"
#include "sound.h"



//EXTERNAL
void panic(const char* title, const char* message, const char* file, const char* function, int line, int error) {
	FILE* panic = fopen("sdmc:/haxapanic.txt", "a");
	if(panic) {
		fprintf(panic, "Compilation date: %s %s\n", __DATE__, __TIME__);
		fprintf(panic, "Sorry! There was a problem during runtime.\n");
		fprintf(panic, "Title: %s\n", title);
		fprintf(panic, "...with message: %s\n", message);
		fprintf(panic, "...in file: %s.\n", file);
		fprintf(panic, "...in function: %s.\n", function);
		fprintf(panic, "...on line: %d.\n", line);
		fprintf(panic, "...with error code: 0x%08x.\n", error);
		fprintf(panic, "For more information, see https://github.com/RedInquisitive/Super-Haxagon\n");
		fprintf(panic, "If a file failed to load, see https://github.com/RedInquisitive/Super-Haxagon/tree/master/sdmc\n");
		fprintf(panic, "The game has quit.\n\n");
		fclose(panic);
	}
	while(aptMainLoop()) {
		if(getButton() == QUIT) break;
		sf2d_start_frame(GFX_TOP, GFX_LEFT);
		drawPanic(title, file, function, line, error);
		sf2d_end_frame();
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		drawPanicBot();
		sf2d_end_frame();
		sf2d_swapbuffers();
	}
	sf2d_fini();
	gfxExit();
	romfsExit();
	sdmcExit();
	ndspExit();
	exit(1);
}

//EXTERNAL
void warning(const char* title, const char* message, const char* file, const char* function, int line) {
	FILE* panic = fopen("sdmc:/haxapanic.txt", "a");
	if(panic) {
		fprintf(panic, "Compilation date: %s %s\n", __DATE__, __TIME__);
		fprintf(panic, "The game output a warning for you.\n");
		fprintf(panic, "Title: %s\n", title);
		fprintf(panic, "...with message: %s\n", message);
		fprintf(panic, "...in file: %s.\n", file);
		fprintf(panic, "...in function: %s.\n", function);
		fprintf(panic, "...on line: %d.\n", line);
		fprintf(panic, "For more information, see https://github.com/RedInquisitive/Super-Haxagon\n");
		fprintf(panic, "If a file failed to load, see https://github.com/RedInquisitive/Super-Haxagon/tree/master/sdmc\n");
		fprintf(panic, "This warning did not crash the game. It was logged here for your record.\n\n");
		fclose(panic);
	}
	while(aptMainLoop()) {
		if(getButton() == SELECT) break;
		sf2d_start_frame(GFX_TOP, GFX_LEFT);
		drawWarning(title, file, function, line);
		sf2d_end_frame();
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		drawBlackBot();
		sf2d_end_frame();
		sf2d_swapbuffers();
	}
}

//EXTERNAL
double linear(double start, double end, double percent) {
	return (end - start) * percent + start;
}

//EXTERNAL
Color interpolateColor(Color one, Color two, double percent) {
	Color new;
	new.r = (int)linear((double)one.r, (double)two.r, percent);
	new.g = (int)linear((double)one.g, (double)two.g, percent);
	new.b = (int)linear((double)one.b, (double)two.b, percent);
	new.a = (int)linear((double)one.a, (double)two.a, percent);
	return new;
}

//EXTERNAL
Point calcPointWall(Point focus, double rotation, double offset, double distance, int side, double sides)  {
	Point point = {0,0};
	double width = (double)side * TAU/sides + offset;
	if(width > TAU + OVERFLOW_OFFSET) width = TAU + OVERFLOW_OFFSET;
	point.x = (int)((distance * cos(rotation + width) + (double)(focus.x)) + 0.5);
	point.y = (int)((distance * sin(rotation + width) + (double)(focus.y)) + 0.5);
	return point;
}

//EXTERNAL
Point calcPoint(Point focus, double rotation, double offset, double distance)  {
	Point point = {0,0};
	point.x = (int)(distance * cos(rotation + offset) + focus.x + 0.5);
	point.y = (int)(distance * sin(rotation + offset) + focus.y + 0.5);
	return point;
}

//EXTERNAL
ButtonState getButton(void) {
	hidScanInput();
	u32 kDown = hidKeysDown();
	u32 kHold = hidKeysHeld();
	if(kDown & KEY_A ) {
		return SELECT;
	}
	if(kDown & KEY_START ) {
		return QUIT;
	}
	if(kDown & KEY_B ) {
		return BACK;
	}
	if(kHold & (KEY_R | KEY_ZR | KEY_CSTICK_RIGHT | KEY_CPAD_RIGHT | KEY_DRIGHT | KEY_X)) {
		return DIR_RIGHT;
	}
	if(kHold & (KEY_L | KEY_ZL | KEY_CSTICK_LEFT | KEY_CPAD_LEFT | KEY_DLEFT | KEY_Y)) {
		return DIR_LEFT;
	}
	return NOTHING;
}

//EXTERNAL
char* getScoreText(int score) {
	if(score < 10 * 60) return "SPACE";
	if(score < 20 * 60) return "POINT";
	if(score < 30 * 60) return "LINE";
	if(score < 40 * 60) return "TRIANGLE";
	if(score < 50 * 60) return "SQUARE";
	if(score < 60 * 60) return "PENTAGON";
	return "HEXAGON";
}

//EXTERNAL
char* getScoreTime(int score) {
	char* buffer = malloc(sizeof(char) * 12 + 1);
	int scoreInt = (int)((double)score/60.0);
	int decimalPart = (int)(((double)score/60.0 - (double)scoreInt) * 100.0);
	snprintf(buffer, 12+1, "TIME: %03d:%02d", scoreInt, decimalPart);
	return buffer;
}

//EXTERNAL
char* getBestTime(int score) {
	char* buffer = malloc(sizeof(char) * 12 + 1);
	int scoreInt = (int)((double)score/60.0);
	int decimalPart = (int)(((double)score/60.0 - (double)scoreInt) * 100.0);
	snprintf(buffer, 12+1, "BEST: %03d:%02d", scoreInt, decimalPart);
	return buffer;
}

//EXTERNAL
double getPulse(int frame, int range, int start) {
	frame -= start;
	// Alternate algorithm:
	//double percent = sin((double)frame * <speed>) / 2.0 + 0.5;
	return fabs(((double)(frame % range * 2) - (double)range)
							  / (double)range);
}

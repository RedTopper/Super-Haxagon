#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <3ds.h>
#include <sf2d.h>

#include "types.h"
#include "util.h"
#include "sound.h"

const Point CENTER = {TOP_WIDTH/2, SCREEN_HEIGHT/2};

//real version will stick in loop until home is pressed
void panic(const char* message, const char* file, const char* function, int line, int error) {
	FILE* panic = fopen("sdmc:/haxapanic.txt", "a");
	if(panic) {
		fprintf(panic, "Compilation date: %s %s\n", __DATE__, __TIME__);
		fprintf(panic, "Sorry! There was a problem during runtime.\n");
		fprintf(panic, "Message: %s\n", message);
		fprintf(panic, "...in file: %s.\n", file);
		fprintf(panic, "...in function: %s.\n", function);
		fprintf(panic, "...on line: %d.\n", line);
		fprintf(panic, "...with error code: 0x%08x.\n", error);
		fprintf(panic, "The game has quit.\n\n");
		fclose(panic);
	}
	sf2d_fini();
	gfxExit();	
	romfsExit();	
	sdmcExit();
	ndspExit();	
	exit(1);
}

int check(int result, const char* message, const char* file, const char* function, int line, int error) {
	if(result) panic(message, file, function, line, error);
	return result;
}

double linear(double start, double end, double percent) {
    return (end - start) * percent + start;
}

Color interpolateColor(Color one, Color two, double percent) {
	Color new;
	new.r = (int)linear((double)one.r, (double)two.r, percent);
	new.g = (int)linear((double)one.g, (double)two.g, percent);
	new.b = (int)linear((double)one.b, (double)two.b, percent);
	new.a = (int)linear((double)one.a, (double)two.a, percent);
	return new;
}

Point calcPoint(double rotation, double offset, double distance, int side, int numSides)  {
	Point point = {0,0};
	point.x = (int)((distance * cos(rotation + (double)side * TAU/(double)numSides + offset) + (double)(CENTER.x)));
	point.y = (int)((distance * sin(rotation + (double)side * TAU/(double)numSides + offset) + (double)(CENTER.y)));
	return point;
}

ButtonState getButton() {
	hidScanInput();
	u32 kDown = hidKeysDown();
	u32 kHold = hidKeysHeld();
	if(kDown & KEY_A ) {
		return SELECT;
	} 
	if(kHold & (KEY_R | KEY_ZR | KEY_CSTICK_RIGHT | KEY_CPAD_RIGHT | KEY_DRIGHT | KEY_X)) {
		return DIR_RIGHT;
	} 
	if(kHold & (KEY_L | KEY_ZL | KEY_CSTICK_LEFT | KEY_CPAD_LEFT | KEY_DLEFT | KEY_Y)) {
		return DIR_LEFT;
	} 
	if(kDown & KEY_START ) {
		return QUIT;
	} 
	return NOTHING;
}
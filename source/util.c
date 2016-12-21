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
void panic(const char* message, int offset) {
	FILE* panic = fopen("sdmc:/haxapanic.txt", "a");
	if(panic) {
		fprintf(panic, "Sorry! There was a problem during runtime.\nMessage: %s At (file) offset: %d", message, offset);
		fclose(panic);
	}
	sf2d_fini();
	gfxExit();	
	romfsExit();	
	sdmcExit();
	ndspExit();	
	exit(1);
}

int check(int result, const char* message, int offset) {
	if(result) panic(message, offset);
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
	return new;
}

Point calcPoint(LiveLevel live, double offset, double distance, int side, int numSides)  {
	Point point = {0,0};
	point.x = (int)((distance * cos(live.rotation + (double)side * TAU/(double)numSides + offset) + (double)(CENTER.x)));
	point.y = (int)((distance * sin(live.rotation + (double)side * TAU/(double)numSides + offset) + (double)(CENTER.y)));
	return point;
}
#include "util.h"

int check(int result, char* message, int offset) {
	if(!result) {
		exit(1);
		//real version will stick in loop until home is pressed
	}
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
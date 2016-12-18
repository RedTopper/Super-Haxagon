#include "font.h"

BmpFont font16;
BmpFont font32;

void writeFont(Color color, Point point, char* string, int large) {
	long paint = RGBA8(color.r,color.b,color.g,0xFF);
	
	if(!font16) {
		font16.load("romfs:/font16.bff"); //Load font file
	}
	if(!font32) {
		font32.load("romfs:/font32.bff"); //Load font file
	}
	if(large) {
		font32.drawStr(string, point.x, point.y, paint);
	} else {
		font16.drawStr(string, point.x, point.y, paint);
	}
}

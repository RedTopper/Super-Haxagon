#include "font.h"

BmpFont font16;
BmpFont font32;

void writeFont(Point p, const char* s, bool large) {
	if(!font16) {
		font16.load("romfs:/font16.bff"); //Load font file
	}
	if(!font32) {
		font32.load("romfs:/font32.bff"); //Load font file
	}
	if(large) {
		font32.drawStr(s, p.x, p.y, p.color);
	} else {
		font16.drawStr(s, p.x, p.y, p.color);
	}
}

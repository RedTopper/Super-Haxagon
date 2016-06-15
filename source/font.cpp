#include "font.h"

BmpFont *font16 = NULL;
BmpFont *font32 = NULL;

void writeFont(Point p, const char* s, bool large) {
	if(font16 == NULL) {
		font16 = new BmpFont("romfs:/font16.bff"); //Load font file
	}
	if(font32 == NULL) {
		font32 = new BmpFont("romfs:/font32.bff"); //Load font file
	}
	if(large) {
		font32->drawStr(s, p.x, p.y, p.color);
	} else {
		font16->drawStr(s, p.x, p.y, p.color);
	}
}

void freeFonts() {
	font16->free();
	font32->free();
}
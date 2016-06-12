#include "font.h"

BmpFont *font = NULL;

void writeFont(Point p, const char* s) {
	if(font == NULL) {
		font = new BmpFont("font16.bff"); //Load font file
	}
	font->drawStr("Hello, world!", 8, 8, RGBA8(0x00, 0xFF, 0xFF, 0xFF));
}
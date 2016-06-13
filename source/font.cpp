#include "font.h"

BmpFont *font16 = NULL;
BmpFont *font32 = NULL;

//Used for compiling sdmc access instead of ROMFS. Might go unused.
const bool LOAD_SDMC = false;

void writeFont(Point p, const char* s, bool large) {
	if(font16 == NULL) {
		if(LOAD_SDMC) {
			font16 = new BmpFont("font16.bff"); //Load font file
		} else {
			font16 = new BmpFont("romfs:/font16.bff"); //Load font file
		}

	}
	if(font32 == NULL) {
		if(LOAD_SDMC) {
			font32 = new BmpFont("font32.bff"); //Load font file
		} else {
			font32 = new BmpFont("romfs:/font32.bff"); //Load font file
		}
	}
	if(large) {
		font32->drawStr(s, p.x, p.y, RGBA8(p.r, p.g, p.b, 0xFF));
	} else {
		font16->drawStr(s, p.x, p.y, RGBA8(p.r, p.g, p.b, 0xFF));
	}
}
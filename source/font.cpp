#include "font.h"

BmpFont font ("font16.bff"); //Load font file

void writeFont(Point p, const char* s) {
	sf2d_start_frame(GFX_TOP, GFX_LEFT);
	font.drawStr("Hello, world!", 8, 8, RGBA8(0x00, 0xFF, 0xFF, 0xFF));
	sf2d_end_frame();
}

#include <3ds.h>
#include <sf2d.h>

#include "types.h"
#include "font.h"

BmpFont font16;
BmpFont font32;

//EXTERNAL
void writeFont(Color color, Point point, char* string, FontSize size) {
	long paint = RGBA8(color.r,color.b,color.g,color.a);
	
	if(!font16)	font16.load("romfs:/font16.bff"); //small file
	if(!font32)	font32.load("romfs:/font32.bff"); //large file
	
	switch(size) {
	case FONT16:
		font16.drawStr(string, point.x, point.y, paint);
		break;
	case FONT32:
		font32.drawStr(string, point.x, point.y, paint);
		break;
	}
}

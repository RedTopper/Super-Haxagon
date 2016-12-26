#pragma once

typedef enum {
	FONT16,
	FONT32
} FontSize;

typedef enum { 
	ALIGN_LEFT_C, 
	ALIGN_CENTER_C, 
	ALIGN_RIGHT_C 
} TextAlignmentC;

#ifdef __cplusplus
#include "font/source/BmpFont.h"
extern "C" {
#endif

/**
 * Draws a font using BmpFont based on a color, a position, 
 * the string, and if it is large or not.
 */
void writeFont(Color color, Point point, char* string, FontSize size, TextAlignmentC align);

#ifdef __cplusplus
}
#endif

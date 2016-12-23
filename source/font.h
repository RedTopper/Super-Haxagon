#pragma once

typedef enum {
	FONT16,
	FONT32
} FontSize;

#ifdef __cplusplus
#include "font/source/BmpFont.h"
extern "C" {
#endif

/**
 * Draws a font using BmpFont based on a color, a position, 
 * the string, and if it is large or not.
 */
void writeFont(Color color, Point point, char* string, FontSize size);

#ifdef __cplusplus
}
#endif

#ifndef SUPER_HAXAGON_STRUCTS_H
#define SUPER_HAXAGON_STRUCTS_H

#include <cstdint>

namespace SuperHaxagon {
	struct Color {
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;
	};

	struct Point {
		int x;
		int y;
	};

	static constexpr double TAU = M_PI * 2;
	static const Color COLOR_SHADOW = {0, 0, 0, 0xC0};
	static const Color COLOR_TRANSPARENT = {0, 0, 0, 0xA0};
	static const Color COLOR_GREY = {0xA0, 0xA0, 0xA0, 0xFF};
	static const Color COLOR_WHITE =  {0xFF, 0xFF, 0xFF, 0xFF};
	static const Color COLOR_BLACK =  {0, 0, 0, 0xFF};
}

#endif //SUPER_HAXAGON_STRUCTS_H

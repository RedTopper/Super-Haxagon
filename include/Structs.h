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
}

#endif //SUPER_HAXAGON_STRUCTS_H

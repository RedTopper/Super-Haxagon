#ifndef SUPER_HAXAGON_GAME_H
#define SUPER_HAXAGON_GAME_H

#include "Driver/Platform.h"

namespace SuperHaxagon {
	class Game {
	public:
		int run(const std::unique_ptr<Platform>& driver);
	};
}

#endif //SUPER_HAXAGON_GAME_H

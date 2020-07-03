#ifndef SUPER_HAXAGON_LEVEL_H
#define SUPER_HAXAGON_LEVEL_H

#include <vector>

#include "Game.h"

namespace SuperHaxagon {
	class Level {
	public:
		const std::vector<Color>& getColorsFG() const { return colorsFG; }
		const std::vector<Color>& getColorsBG1() const { return colorsBG1; }
		const std::vector<Color>& getColorsBG2() const { return colorsBG2; }

	private:
		std::vector<Color> colorsFG;
		std::vector<Color> colorsBG1;
		std::vector<Color> colorsBG2;
	};
}

#endif //SUPER_HAXAGON_LEVEL_H

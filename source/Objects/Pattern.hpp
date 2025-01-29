#ifndef SUPER_HAXAGON_PATTERN_HPP
#define SUPER_HAXAGON_PATTERN_HPP

#include "Objects/Wall.hpp"

#include <vector>

namespace SuperHaxagon {
	class Twist;
	class Pattern {
	public:
		Pattern(std::vector<Wall>& walls, int sides);

		const std::vector<Wall>& getWalls() const {return _walls;}
		int getSides() const {return _sides;}

		float getFurthestWallDistance() const;
		float getClosestWallDistance() const;
		void advance(float speed);

	private:
		std::vector<Wall> _walls;
		int _sides;
	};
}

#endif //SUPER_HAXAGON_PATTERN_HPP
